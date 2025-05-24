#include <iostream>
#include <string>
#include <exception>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <thread>
#include <map>
#include <csignal>
#include <atomic>


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
        socklen_t addr_len;
        int client_socket;
        if((client_socket = ::accept(server_socket, (struct sockaddr*)&client_address, &addr_len)) == -1){
            throw std::runtime_error("Accept failed.");
        }
        return std::make_pair(client_socket, client_address);
    }

    static ssize_t send_bytes(int client_socket, const void *buff, size_t size) {
        ssize_t sent_bytes = send(client_socket, &size, sizeof(size), 0);
        if (sent_bytes == -1) {
            throw std::runtime_error("Send 1 failed.");
        }
        if ((sent_bytes = send(client_socket, buff, size, 0)) == -1) {
            throw std::runtime_error("Send 2 failed.");
        }
        return sent_bytes;
    }

    static std::vector<char> receive_bytes(int client_socket){
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

    void shutdown() {
        ::shutdown(server_socket, SHUT_RDWR);
    }
};

void process_client(Server& server, std::pair<int, sockaddr_in> client) {

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

    std::thread cmdThread(commandListener, std::ref(server));
    std::vector<std::thread> threads;

    while (true) {
        try {
            if (!server.socket_available()) {
                break;
            }
            auto clt = server.accept();
            if (clt.first > 0) {
                threads.emplace_back(process_client, std::ref(server), clt);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error on accept: " << e.what() << std::endl;
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

    std::cout << "Server threads finished successfully.\nFinishing with exit code 0." << std::endl;
    return 0;
}