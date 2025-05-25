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


class Server {
private:
    int server_socket;
    int port;
    int max_clients;
    sockaddr_in address{};

public:
    Server(int port, int max_clients) : port(port), max_clients(max_clients) {
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == -1) {
            throw std::runtime_error("Error creating server socket.");
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
    }

    void bind() {
        if (::bind(server_socket, (struct sockaddr*)&address, sizeof(address)) < 0) {
            throw std::runtime_error("Bind failed.");
        }
    }

    void listen() const {
        if(::listen(server_socket, max_clients) < 0){
            throw std::runtime_error("Listen failed.");
        }
    }

    std::pair<int, sockaddr_in> accept() {
        sockaddr_in client_address {};
        socklen_t addr_len = sizeof(client_address);
        int client_socket;
        if((client_socket = ::accept(server_socket, (struct sockaddr*)&client_address, &addr_len)) == -1){
            throw std::runtime_error("Accept failed.");
        }
        return std::make_pair(client_socket, client_address);
    }

    ssize_t send_bytes(int client_socket, const void *buff, size_t size) {
        ssize_t sent_bytes = send(client_socket, &size, sizeof(size), 0);
        if (sent_bytes == -1) {
            throw std::runtime_error("Send 1 failed.");
        }
        if ((sent_bytes = send(client_socket, buff, size, 0)) == -1) {
            throw std::runtime_error("Send 2 failed.");
        }
        return sent_bytes;
    }

    std::vector<char> receive_bytes(int client_socket){
        size_t len = 0;
        ssize_t rcv_bytes = recv(client_socket, &len, sizeof(len), 0);
        if (rcv_bytes  == -1) {
            throw std::runtime_error("Receive 1 failed.");
        }

        std::vector<char> buf(len);
        rcv_bytes = recv(client_socket, buf.data(), len, 0);
        if (rcv_bytes == -1){
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
        }
        else{
            throw std::runtime_error("Close failed.");
        }
    }

    virtual ~Server(){
        if(server_socket != -1){
            close(server_socket);
        }
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

void process_client(Server& server, std::pair<int, sockaddr_in> client, SharedMemory& shm, DualSemaphore& sem) {
    std::cout << "Client " << client.second.sin_addr.s_addr << " connected to the server." << std::endl;

    while (true) {
        std::string command = server.receive_string(client.first);
        std::cout << "Command: " << command << std::endl;

        std::string file_name = std::to_string(client.second.sin_addr.s_addr) + "_in.cpp";

        if (command == "compile") {
            std::string file_size = server.receive_string(client.first);
            char buffer[1024];
            FILE* file_cpp = fopen(file_name.c_str(), "wb");
            if (file_cpp == nullptr) {
                throw std::runtime_error("Temp file could not be opened.");
            }
            int sz = atoi(file_size.c_str());
            for (int i = 0; i < sz; ++i) {
                std::vector<char> buf = server.receive_bytes(client.first);
                std::copy(buf.begin(), buf.end(), buffer);
                fwrite(buffer, sizeof(char), buf.size(), file_cpp);
            }
            fclose(file_cpp);
            std::cout << "Got file!" << std::endl;

            sem.wait_for(0, 1);
            sem.down(1);
            shm.write(file_name.c_str());
            sem.up(0);

            sem.wait_for(1, 1);
            std::string ret = shm.read();
            std::cout << ret << std::endl;
            sem.down(0);

        } else if (command == "play") {

        } else if (command == "exit") {
            break;
        }
    }
    close(client.first);
    std::cout << "Client " << client.second.sin_addr.s_addr << " disconnected." << std::endl;
}

void commandListener(Server &server) {
    std::string line;
    while (true) {
        std::getline(std::cin, line);
        if (line == "close") {
            server.shutdown();
            server.stop();
            std::cout << "Stopping server by command CLOSE" << std::endl;
            break;
        }
    }
}

int main() {
    Server server(5000, 10);
    server.bind();
    server.listen();

    DualSemaphore compile_semaphore(1, 0, 1, true);
    SharedMemory compile_shm(1, 256, true);

    ProcessCompiler compiler("compiler");
    compiler.start();

    std::thread cmdThread(commandListener, std::ref(server));
    std::vector<std::thread> threads;

    while (server.socket_available()) {
        try {
            auto clt = server.accept();
            if (clt.first > 0) {
                threads.emplace_back(process_client, std::ref(server), clt, std::ref(compile_shm), std::ref(compile_semaphore));
            }
        } catch (const std::exception& e) {
            if (server.socket_available()) {
                std::cerr << "Error on accept: " << e.what() << std::endl;
            } else {
                std::cout << "Server socket closed, stopping accept loop" << std::endl;
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
    if (!compiler.wait()) {
        std::cerr << "Compiler process didn't finish properly." << std::endl;
    }

    compile_shm.detach(true);
    compile_semaphore.detach(true);

    std::cout << "Server threads finished successfully.\nFinishing with exit code 0." << std::endl;
    return 0;
}