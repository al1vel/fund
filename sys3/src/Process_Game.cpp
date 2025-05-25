#include "Process_Game.h"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <map>
#include <MsgQueue.h>
#include <utility>
#include <random>

ProcessGame::ProcessGame(std::string name) : name(std::move(name)) {}

ProcessGame::~ProcessGame() = default;

void ProcessGame::start() {
    pid = fork();
    if (pid < 0) {
        throw std::runtime_error("Compiler fork failed.");
    }

    if (pid == 0) {
        std::cout << "Process " << name << " started" << std::endl;
        MsgQueue msg_queue_in(1, false);
        MsgQueue msg_queue_out(2, false);

        std::map<long, unsigned int> data;

        while (true) {
            std::pair<long, std::string> packet = msg_queue_in.receive_message(0);
            if (packet.second[0] == 'S') {
                std::cout << "Client " << packet.first << " started the game." << std::endl;
                data[packet.first] = 21;
                msg_queue_out.send_message(packet.first, "G");

            } else if (packet.second[0] == 'E') {
                int n = atoi(&packet.second[2]);
                if (n > data[packet.first]) {
                    n = data[packet.first];
                }
                data[packet.first] -= n;

                if (data[packet.first] <= 0) {
                    msg_queue_out.send_message(packet.first, "W");
                    std::cout << "Client " << packet.first << " won the game." << std::endl;
                } else {
                    msg_queue_out.send_message(packet.first, "O|" + std::to_string(data[packet.first]));
                    std::cout << "Client " << packet.first << " took " << n << " in the game " << std::to_string(data[packet.first]) << " left." << std::endl;

                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> dist(1, 3);
                    unsigned int number = dist(gen);
                    if (number > data[packet.first]) {
                        number = data[packet.first];
                    }

                    data[packet.first] -= number;
                    if (data[packet.first] <= 0) {
                        msg_queue_out.send_message(packet.first, "L|" + std::to_string(number));
                        std::cout << "Client " << packet.first << " lost the game." << std::endl;
                        data.erase(packet.first);

                    } else {
                        msg_queue_out.send_message(packet.first, "T|" + std::to_string(number));
                        std::cout << "Client " << packet.first << ": server took " << number << std::endl;
                    }
                }

            }
        }
    }
}

bool ProcessGame::wait() const {
    if (pid > 0) {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return false;
        }
        return WIFEXITED(status);
    }
    return false;
}

pid_t ProcessGame::getPid() const {
    return pid;
}

void ProcessGame::stop() const {
    if (pid > 0) {
        if (kill(pid, SIGTERM) == -1) {
            perror("kill");
        }
    }
    std::cout << "Process " << name << " stopped." << std::endl;
}
