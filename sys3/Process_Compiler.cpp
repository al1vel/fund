#include "Process_Compiler.h"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
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
        _exit(0);
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