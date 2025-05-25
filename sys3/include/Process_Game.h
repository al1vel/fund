#ifndef PROCESS_GAME_H
#define PROCESS_GAME_H
#include <iostream>

class ProcessGame {
private:
    pid_t pid = -1;
    std::string name;

public:
    explicit ProcessGame(std::string name);
    ~ProcessGame();

    void start();
    bool wait() const;
    pid_t getPid() const;
    std::string get_name() const;
    void stop() const;
};

#endif //PROCESS_GAME_H
