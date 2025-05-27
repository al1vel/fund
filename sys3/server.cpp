#include <iostream>
#include <string>
#include <exception>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <thread>
#include <csignal>
#include <fstream>
#include "Process_Compiler.h"
#include <sys/sem.h>
#include "SharedMemory.h"
#include "DualSemaphore.h"
#include <sys/stat.h>
#include "MsgQueue.h"
#include "Process_Game.h"
#include <memory>
#include <ctime>
#include <iomanip>
#include "Logger.h"

long get_file_size(const char* filename) {
    struct stat st{};
    if (stat(filename, &st) != 0) {
        perror("stat failed");
        return -1;
    }
    return st.st_size;
}

class Server {
private:
    int server_socket;
    int port;
    int max_clients;
    sockaddr_in address{};
    std::shared_ptr<Logger> logger;

public:
    Server(int port, int max_clients, std::shared_ptr<Logger> logger) : port(port), max_clients(max_clients) {
        this->logger = logger;
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == -1) {
            logger->error("[SERVER]: Failed to create server socket.");
            throw std::runtime_error("Error creating server socket.");
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        logger->info("[SERVER]: Server started.");
    }

    void bind() {
        if (::bind(server_socket, (struct sockaddr*)&address, sizeof(address)) < 0) {
            logger->error("[SERVER]: Failed to bind server socket.");
            throw std::runtime_error("Bind failed.");
        }
        logger->info("[SERVER]: Socket binded.");
    }

    void listen() const {
        if(::listen(server_socket, max_clients) < 0){
            logger->error("[SERVER]: Failed to listen on socket.");
            throw std::runtime_error("Listen failed.");
        }
    }

    std::pair<int, sockaddr_in> accept() {
        sockaddr_in client_address {};
        socklen_t addr_len = sizeof(client_address);
        int client_socket;
        if((client_socket = ::accept(server_socket, (struct sockaddr*)&client_address, &addr_len)) == -1){
            logger->error("[SERVER]: Accept failed.");
            throw std::runtime_error("Accept failed.");
        }
        return std::make_pair(client_socket, client_address);
    }

    ssize_t send_bytes(int client_socket, const void *buff, size_t size) {
        ssize_t sent_bytes = send(client_socket, &size, sizeof(size), 0);
        if (sent_bytes == -1) {
            logger->error("[SERVER]: Send size failed.");
            throw std::runtime_error("Send 1 failed.");
        }
        if ((sent_bytes = send(client_socket, buff, size, 0)) == -1) {
            logger->error("[SERVER]: Send content failed.");
            throw std::runtime_error("Send 2 failed.");
        }
        return sent_bytes;
    }

    std::vector<char> receive_bytes(int client_socket){
        size_t len = 0;
        ssize_t rcv_bytes = recv(client_socket, &len, sizeof(len), 0);
        if (rcv_bytes  == -1) {
            logger->error("[SERVER]: Receive size failed.");
            throw std::runtime_error("Receive 1 failed.");
        }

        std::vector<char> buf(len);
        rcv_bytes = recv(client_socket, buf.data(), len, 0);
        if (rcv_bytes == -1){
            logger->error("[SERVER]: Receive content failed.");
            throw std::runtime_error("Receive 2 failed.");
        }
        return buf;
    }

    ssize_t send_string(int client_socket, const std::string& str) {
        return send_bytes(client_socket, str.c_str(), str.length());
    }

    std::string receive_string(int client_socket) {
        std::vector<char> ret = receive_bytes(client_socket);
        std::string answer(ret.begin(), ret.end());
        return answer;
    }

    void stop(){
        if(server_socket != -1){
            close(server_socket);
            server_socket = -1;
            logger->info("[SERVER]: Server socket closed.");
        }
        else {
            logger->error("[SERVER]: Close failed.");
            throw std::runtime_error("Close failed.");
        }
    }

    virtual ~Server(){
        if(server_socket != -1){
            close(server_socket);
        }
        logger->info("[SERVER]: Server stopped.");
    }

    bool socket_available() const {
        if (server_socket != -1) {
            return true;
        }
        return false;
    }

    void shutdown() const {
        ::shutdown(server_socket, SHUT_RDWR);
    }
};

void process_client(Server& server, std::pair<int, sockaddr_in> client, SharedMemory& shm, DualSemaphore& sem,
    MsgQueue& queue_in, MsgQueue& queue_out, std::shared_ptr<Logger> logger) {

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client.second.sin_addr), ip_str, INET_ADDRSTRLEN);
    std::string client_identificator(ip_str);
    unsigned short port = ntohs(client.second.sin_port);
    client_identificator += ":" + std::to_string(port);
    logger->info("[SERVER]: Client " + client_identificator + " connected. Socket: " + std::to_string(client.first));

    while (true) {
        std::string command = server.receive_string(client.first);
        logger->debug("[SERVER]: Got command <" + command + "> from client " + std::to_string(client.second.sin_addr.s_addr));

        std::string file_name = std::to_string(client.second.sin_addr.s_addr) + "_in.cpp";

        if (command == "compile") {
            std::string file_size = server.receive_string(client.first);
            char buffer[1024];
            FILE* file_cpp = fopen(file_name.c_str(), "wb");
            if (file_cpp == nullptr) {
                logger->error("[SERVER]: Temp file could not be opened.");
                throw std::runtime_error("Temp file could not be opened.");
            }
            int sz = atoi(file_size.c_str());
            for (int i = 0; i < sz; ++i) {
                std::vector<char> buf = server.receive_bytes(client.first);
                std::copy(buf.begin(), buf.end(), buffer);
                fwrite(buffer, sizeof(char), buf.size(), file_cpp);
            }
            fclose(file_cpp);
            logger->info("[SERVER]: Got file from client " + std::to_string(client.second.sin_addr.s_addr));

            sem.wait_for(0, 1);
            sem.down(1);
            shm.write(file_name.c_str());
            sem.up(0);

            sem.wait_for(1, 1);
            std::string ret = shm.read();

            if (ret == "fail") {
                logger->error("[SERVER]: Compilation for client " + std::to_string(client.second.sin_addr.s_addr) + "failed.");
                server.send_string(client.first, "failed");
                std::remove(file_name.c_str());
            } else {
                logger->info("[SERVER]: Compilation for client " + std::to_string(client.second.sin_addr.s_addr) + " successfully done.");
                long out_size = get_file_size(ret.c_str());
                if (out_size == -1) {
                    logger->error("[SERVER]: Output file size could not be read.");
                    throw std::runtime_error("Output file size could not be read.");
                }
                if (out_size % 1024 != 0) {
                    out_size = out_size / 1024 + 1;
                } else {
                    out_size = out_size / 1024;
                }
                FILE* out = fopen(ret.c_str(), "rb");
                if (out == nullptr) {
                    logger->error("[SERVER]: Temp out file could not be opened.");
                    throw std::runtime_error("Temp out file could not be opened.");
                }

                server.send_string(client.first, std::to_string(out_size));
                size_t bytes_read;
                while ((bytes_read = fread(buffer, 1, sizeof(buffer), out)) > 0) {
                    server.send_bytes(client.first, buffer, bytes_read);
                }
                fclose(out);
                std::remove(ret.c_str());
                std::remove(file_name.c_str());
                logger->info("[SERVER]: File sent to client " + std::to_string(client.second.sin_addr.s_addr));
            }
            sem.down(0);

        } else if (command == "play") {
            //long client_id = client.second.sin_addr.s_addr;
            long client_id = client.first;
            queue_in.send_message(client_id, "S");
            std::pair<long, std::string> ret = queue_out.receive_message(client_id);
            if (ret.second[0] != 'G') {
                throw std::runtime_error("Receive didn't get G.");
            }
            server.send_string(client.first, "ready");

            while (true) {
                std::string n_str = server.receive_string(client.first);
                queue_in.send_message(client_id, "E|" + n_str);

                ret = queue_out.receive_message(client_id);
                if (ret.second[0] == 'W') {
                    server.send_string(client.first, "0|0");
                    break;
                }
                if (ret.second[0] == 'O') {
                    int cnt = std::stoi(ret.second.substr(2));
                    ret = queue_out.receive_message(client_id);
                    if (ret.second[0] == 'L') {
                        int took = std::stoi(ret.second.substr(2));
                        std::string pck(std::to_string(took) + "|0");
                        server.send_string(client.first, pck);
                        break;
                    }
                    if (ret.second[0] == 'T') {
                        int took = std::stoi(ret.second.substr(2));
                        cnt -= took;
                        std::string pck(std::to_string(took) + "|" + std::to_string(cnt));
                        server.send_string(client.first, pck);
                    }
                }
            }


        } else if (command == "exit") {
            break;
        }
    }
    close(client.first);
    logger->info("[SERVER]: Client " + client_identificator + " disconnected.");
}

void commandListener(Server &server) {
    std::string line;
    while (true) {
        std::getline(std::cin, line);
        if (line == "close") {
            server.shutdown();
            server.stop();
            break;
        }
    }
}

int main() {
    auto logger = LoggerBuilder()
        .set_level(Logger::INFO)
        .add_handler(std::cout)
        .add_handler(std::make_unique<std::ofstream>("/home/begemot/fund/sys3/log.txt"))
        .make_object();
    logger->info("Logger initialized");

    Server server(5000, 10, logger);
    server.bind();
    server.listen();

    DualSemaphore compile_semaphore(1, 0, 1, true, logger);
    SharedMemory compile_shm(1, 256, true, logger);
    MsgQueue play_queue_in(1, true, logger); // data to game_process
    MsgQueue play_queue_out(2, true, logger); // data from game_process

    ProcessCompiler compiler("compiler", logger);
    compiler.start();

    ProcessGame game("game", logger);
    game.start();

    std::thread cmdThread(commandListener, std::ref(server));
    std::vector<std::thread> threads;

    while (server.socket_available()) {
        try {
            auto clt = server.accept();
            if (clt.first > 0) {
                threads.emplace_back(process_client, std::ref(server), clt, std::ref(compile_shm),
                    std::ref(compile_semaphore), std::ref(play_queue_in), std::ref(play_queue_out), logger);
            }
        } catch (const std::exception& e) {
            if (server.socket_available()) {
                std::cerr << "Error on accept: " << e.what() << std::endl;
                logger->error("[SERVER]: Error on accept.");
            } else {
                logger->info("[SERVER]: Stopping accept loop.");
                break;
            }
        }
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    if (cmdThread.joinable()) {
        cmdThread.join();
    }
    compiler.stop();
    game.stop();

    compile_shm.detach(true);
    compile_semaphore.detach(true);
    play_queue_in.detach(true);
    play_queue_out.detach(true);

    logger->info("[SERVER]: Finishing with exit code 0.");
    return 0;
}