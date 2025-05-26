#ifndef PROCESS_GAME_H
#define PROCESS_GAME_H
#include <iostream>
#include <Logger.h>
#include <memory>

class ProcessGame {
private:
    pid_t pid = -1;
    std::string name;
    std::shared_ptr<Logger> logger;

public:
    explicit ProcessGame(std::string name, std::shared_ptr<Logger> logger);
    ~ProcessGame();

    void start();
    bool wait() const;
    pid_t getPid() const;
    std::string get_name() const;
    void stop() const;
};

#endif //PROCESS_GAME_H
