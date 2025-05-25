#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include <vector>

class ProcessCompiler {
private:
    pid_t pid = -1;
    std::string name;

public:
    explicit ProcessCompiler(std::string name);
    ~ProcessCompiler();

    void start();
    bool wait() const;
    pid_t getPid() const;
    std::string get_name() const;
    void stop() const;
};

#endif //PROCESS_H
