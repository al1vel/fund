#include "Process_Game.h"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <map>
#include <MsgQueue.h>
#include <utility>
#include <random>

ProcessGame::ProcessGame(std::string name, std::shared_ptr<Logger> logger) : name(std::move(name)) {
    this->logger = logger;
}

ProcessGame::~ProcessGame() = default;

void ProcessGame::start() {
    pid = fork();
    if (pid < 0) {
        throw std::runtime_error("Compiler fork failed.");
        logger->error("[PLAY]: Fork failed.");
    }

    if (pid == 0) {
        logger->info("[PLAY]: Started process.");
        MsgQueue msg_queue_in(1, false, logger);
        MsgQueue msg_queue_out(2, false, logger);

        std::map<long, unsigned int> data;

        while (true) {
            std::pair<long, std::string> packet = msg_queue_in.receive_message(0);
            if (packet.second[0] == 'S') {
                logger->info("[PLAY]: Client " + std::to_string(packet.first) + " started the game.");
                data[packet.first] = 21;
                msg_queue_out.send_message(packet.first, "G");

            } else if (packet.second[0] == 'E') {
                unsigned int n = atoi(&packet.second[2]);
                if (n > data[packet.first]) {
                    n = data[packet.first];
                }
                data[packet.first] -= n;

                if (data[packet.first] == 0) {
                    msg_queue_out.send_message(packet.first, "W");
                    logger->info("[PLAY]: Client " + std::to_string(packet.first) + " won the game.");
                } else {
                    msg_queue_out.send_message(packet.first, "O|" + std::to_string(data[packet.first]));
                    logger->debug("[PLAY]: Client " + std::to_string(packet.first)
                        + " took " + std::to_string(n) + " in the game "
                        + std::to_string(data[packet.first]) + " left.");

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
                        logger->info("[PLAY]: Client " + std::to_string(packet.first) + " lost the game.");
                        data.erase(packet.first);

                    } else {
                        msg_queue_out.send_message(packet.first, "T|" + std::to_string(number));
                        logger->debug("[PLAY]: Client " + std::to_string(packet.first) + ": server took " + std::to_string(number));
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
    logger->info("[PLAY]: Process stopped.");
}
