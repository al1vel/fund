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
#include <cstring>
#include <sys/wait.h>
#include <sys/stat.h>

long get_file_size(const char* filename) {
    struct stat st{};
    if (stat(filename, &st) != 0) {
        perror("stat failed");
        return -1;
    }
    return st.st_size;
}

ssize_t send_bytes(int server_socket, const void *buff, size_t size) {
    ssize_t sent_bytes = send(server_socket, &size, sizeof(size), 0);
    if (sent_bytes == -1) {
        throw std::runtime_error("Send 1 failed.");
    }
    if ((sent_bytes = send(server_socket, buff, size, 0)) == -1) {
        throw std::runtime_error("Send 2 failed.");
    }
    return sent_bytes;
}

std::vector<char> receive_bytes(int server_socket){
    size_t len = 0;
    ssize_t rcv_bytes = recv(server_socket, &len, sizeof(len), 0);
    if (rcv_bytes  == -1) {
        throw std::runtime_error("Receive 1 failed.");
    }

    std::vector<char> buf(len);
    rcv_bytes = recv(server_socket, buf.data(), len, 0);
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

int main() {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        throw std::runtime_error("Socket open failed.");
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        return 1;
    }

    if (connect(client_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed");
        return 1;
    }

    std::cout << "Connected to server.\nAvailable commands:\n  1. play\n  2. compile\n  3. exit\n" << std::endl;
    std::string command;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);

        if (command == "exit") {
            send_string(client_socket, "exit");
            break;
        }
        if (command == "play") {
            send_string(client_socket, "play");
            std::string ret = receive_string(client_socket);
            if (ret != "ready") {
                std::cout << "Something bad happened on server. Try later..." << std::endl;
                break;
            }

            std::cout << "There are 21 stones. Your turn: ";
            std::string answer;
            int turn = 0;

            while (true) {
                std::getline(std::cin, answer);
                turn = std::stoi(answer);
                if (turn <= 3 && turn >= 1) {
                    break;
                }
                std::cout << "Enter a number between 1 and 3: ";
            }
            send_string(client_socket, answer);

            while (true) {
                answer = receive_string(client_socket);
                if (answer == "0|0") {
                    std::cout << "You won!" << std::endl;
                    break;
                }

                size_t pos = answer.find('|');
                std::string comp_took_str = answer.substr(0, pos);
                std::string left_str = answer.substr(pos + 1);

                int comp_took = std::stoi(comp_took_str);
                int stones_left = std::stoi(left_str);

                if (stones_left == 0) {
                    std::cout << "Computer took " << comp_took << " stones.\nYou lost!" << std::endl;
                    break;
                }

                std::cout << "Computer took " << comp_took << " stones." << std::endl;
                std::cout << "There are " << stones_left << " stones. Your turn: ";
                while (true) {
                    std::getline(std::cin, answer);
                    turn = std::stoi(answer);
                    if (turn <= 3 && turn >= 1) {
                        break;
                    }
                    std::cout << "Enter a number between 1 and 3: ";
                }
                send_string(client_socket, answer);
            }

        } else if (command == "compile") {
            std::cout << "Enter path to .cpp file: ";
            std::string file_path;
            FILE* file = nullptr;

            while (true) {
                std::getline(std::cin, file_path);
                file = fopen(file_path.c_str(), "rb");
                if (file == nullptr) {
                    std::cout << "Unable to open file <" << file_path << ">.\nTry again: " << std::endl;
                } else {
                    break;
                }
            }

            send_string(client_socket, "compile");
            size_t file_size = get_file_size(file_path.c_str());

            size_t total_sent = 0, total_bytes = file_size;

            if (file_size % 1024 != 0) {
                file_size = file_size / 1024 + 1;
            } else {
                file_size = file_size / 1024;
            }
            send_string(client_socket, std::to_string(file_size));

            char buffer[1024];
            size_t bytes_read;
            while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
                send_bytes(client_socket, buffer, bytes_read);
                total_sent += bytes_read;
                int percent = static_cast<int>((100.0 * total_sent) / total_bytes);
                std::cout << "\rSending: [" << std::string(percent / 2, '#') << std::string(50 - percent / 2, ' ') << "] " << percent << "%" << std::flush;
            }
            std::cout << std::endl;
            fclose(file);

            std::string size = receive_string(client_socket);
            if (size == "failed") {
                std::cout << "Compilation failed." << std::endl;

            } else {
                std::string compiled_file_path = file_path.substr(0, file_path.find_last_of('/')) +
                file_path.substr(file_path.find_last_of('/'), file_path.find('.') - file_path.find_last_of('/')) + "_compiled";

                FILE* compiled = fopen(compiled_file_path.c_str(), "wb");
                if (compiled == nullptr) {
                    throw std::runtime_error("Unable to open file <compiled>.");
                }

                int sz = atoi(size.c_str());
                for (int i = 0; i < sz; ++i) {
                    std::vector<char> buf = receive_bytes(client_socket);
                    std::copy(buf.begin(), buf.end(), buffer);
                    fwrite(buffer, sizeof(char), buf.size(), compiled);

                    int percent = static_cast<int>((100.0 * (i + 1)) / sz);
                    std::cout << "\rReceiving: [" << std::string(percent / 2, '#') << std::string(50 - percent / 2, ' ') << "] " << percent << "%" << std::flush;
                }
                std::cout << std::endl;
                fclose(compiled);
                std::string cmd = "chmod +x " + compiled_file_path;
                system(cmd.c_str());
                std::cout << "Got compiled file.\nSaved to: " << compiled_file_path << std::endl;
            }
        }
    }

    close(client_socket);
    return 0;
}