#ifndef PROCESS_H
#define PROCESS_H

#include <Logger.h>
#include <memory>

class ProcessCompiler {
private:
    pid_t pid = -1;
    std::string name;
    std::shared_ptr<Logger> logger;

public:
    explicit ProcessCompiler(std::string name, const std::shared_ptr<Logger> &logger);
    ~ProcessCompiler();

    void start();
    bool wait() const;
    pid_t getPid() const;
    std::string get_name() const;
    void stop() const;
};

#endif //PROCESS_H
