#include "Process_Compiler.h"
#include <DualSemaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <SharedMemory.h>
#include <utility>

ProcessCompiler::ProcessCompiler(std::string name) : name(std::move(name)) {}

ProcessCompiler::~ProcessCompiler() = default;

void ProcessCompiler::start() {
    pid = fork();
    if (pid < 0) {
        throw std::runtime_error("Compiler fork failed.");
    }

    if (pid == 0) {
        std::cout << "Process " << name << " started" << std::endl;
        SharedMemory shm(1, 256, false);
        DualSemaphore sem(1, 0, 0, false);

        while (true) {
            sem.wait_for(1, 0);
            std::string msg = shm.read();
            std::cout << "Compiler got: " << msg << std::endl;

            std::string command = "g++ " + msg + " -o " + msg.substr(0, msg.find('.'));
            //std::cout << "<" << command << ">" << std::endl;

            int result = system(command.c_str());
            if (result == 0) {
                shm.write(msg.substr(0, msg.find('.')));
            } else {
                shm.write("fail");
            }
            sem.up(1);
        }
    }
}

bool ProcessCompiler::wait() const {
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

pid_t ProcessCompiler::getPid() const {
    return pid;
}

void ProcessCompiler::stop() const {
    if (pid > 0) {
        if (kill(pid, SIGTERM) == -1) {
            perror("kill");
        }
    }
    std::cout << "Process " << name << " stopped." << std::endl;
}