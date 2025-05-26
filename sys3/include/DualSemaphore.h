#ifndef DUALSEMAPHORE_H
#define DUALSEMAPHORE_H
#include <Logger.h>
#include <memory>
#include <string>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

class DualSemaphore {
private:
    std::string path;
    int sem_id = -1;
    std::shared_ptr<Logger> logger;

public:
    DualSemaphore(int id, unsigned short first_val, unsigned short second_val, bool create, const std::shared_ptr<Logger> &logger);

    ~DualSemaphore();

    void detach(bool remove) const;

    void wait_for(int expect0, int expect1, int delay_ms = 100) const;

    void up(int index) const;

    void down(int index) const;
};

#endif //DUALSEMAPHORE_H
