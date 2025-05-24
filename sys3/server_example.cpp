#include <iostream>
#include <string>
#include <exception>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <thread>
#include <map>

class Server{
    private:
        std::string ip;
        int port, sk, maximumListen;
        sockaddr_in serverAddr, socketDescr;
        std::map<std::string, int> clients;

    public:

        Server(std::string ip = "0.0.0.0", int port = 5000, int maximumListen = 10) : ip(ip), port(port), maximumListen(maximumListen) {
            sk = socket(AF_INET, SOCK_STREAM, 0);

            serverAddr.sin_family = AF_INET;
            serverAddr.sin_addr.s_addr = INADDR_ANY;
            serverAddr.sin_port = htons(port);

            if(!inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr.s_addr)){
                throw std::exception();
            }
        }

        bool userExists(std::string name){
            if(clients.find(name) == clients.end()){
                return false;
            }
            return true;
        }

        bool userAdd(std::string name, int cl){
            if(userExists(name)){
                return false;
            }
            clients[name] = cl;
            return true;
        }

        bool userRemove(std::string name){
            if(userExists(name)){
                clients.erase(name);
                return true;
            }
            return false;
        }

        int userGetSocket(std::string name){
            if(userExists(name)){
                return clients[name];
            }
            return -1;
        }

        void bind() {
            if(::bind(sk, (struct sockaddr*)&serverAddr, (socklen_t)sizeof(struct sockaddr_in)) < 0){
                throw std::exception();
            }
        }

        void listen(){
            if(::listen(sk, maximumListen) < 0){
                throw std::exception();
            }
        }

        std::pair<int, sockaddr_in> accept(){
            sockaddr_in clientAddr;
            socklen_t addrSize;
            int clientDescr;
            if((clientDescr = ::accept(sk, (struct sockaddr*)&clientAddr, &addrSize)) == -1){
                throw std::exception();
            }
            return std::make_pair(clientDescr, clientAddr);
        }

        std::string receiveString(int clientDescr) {
            std::vector<char> ret = receivedBytes(clientDescr);
            return std::string(ret.begin(), ret.end());
        }

        ssize_t sendBytes(int clientSk, const void *buff, size_t size) {
            ssize_t sendedBytes = send(clientSk, &size, sizeof(size), 0);
            if (sendedBytes == -1)
            {
                throw std::exception();
            }

            if ((sendedBytes = send(clientSk, buff, size, 0)) == -1)
            {
                throw std::exception();
            }
            return sendedBytes;
        }

        std::vector<char> receivedBytes(int clientDescr){
            size_t len = 0;
            ssize_t rcv_bytes = 0;
            if((rcv_bytes = recv(clientDescr, &len, sizeof(len), 0)) == -1){
                throw std::exception();
            }

            std::vector<char> buf(len);

            if((rcv_bytes = recv(clientDescr, buf.data(), len, 0)) == -1){
                throw std::exception();
            }
            return buf;
        }

        ssize_t sendString(int clientSk, const std::string& str){
            return sendBytes(clientSk, str.c_str(), str.length());
        }

        void stop(){
            if(sk != -1){
                close(sk);
                sk = -1;
            }
            else{
                throw std::exception();
            }
        }

        virtual ~Server(){
            if(sk != -1){
                close(sk);
            }
        }

};

void processClient(Server& server, std::pair<int, sockaddr_in> pr){
    std::cout << "Client " << pr.second.sin_addr.s_addr << " connected to the server" << std::endl;
    std::string senderName;
    try {

        while(true){
            senderName = server.receiveString(pr.first);
            if(senderName != "exit" && server.userExists(senderName)){
                server.sendString(pr.first, "NO");
            }
            else{
                server.userAdd(senderName, pr.first);
                server.sendString(pr.first, "OK");
                break;
            }
        }

        while(true) {
            int receiverSocket;
            std::string receiverName;
            while(true){
                receiverName = server.receiveString(pr.first);
                if(receiverName != "exit" && !server.userExists(receiverName)){
                    server.sendString(pr.first, "NO");
                }
                else{
                    server.sendString(pr.first, "OK");
                    if (receiverName == "exit") {
                        break;
                    }
                    receiverSocket = server.userGetSocket(receiverName);
                    break;
                }
            }

            std::string text = server.receiveString(pr.first);
            server.sendString(receiverSocket, senderName + " : "+ text);

        }
        close(pr.first);
    }
    catch(...) {}
    server.userRemove(senderName);
}

int main() {
    Server server{};
    server.bind();
    server.listen();

    std::vector<std::thread> threads;

    while (true) {
        auto clt = server.accept();
        if(clt.first > 0){
            threads.emplace_back(processClient, std::ref(server), clt);
        }
        else{
            std::cout << "Error!" << std::endl;
        }
    }

}