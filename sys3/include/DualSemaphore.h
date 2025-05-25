#ifndef DUALSEMAPHORE_H
#define DUALSEMAPHORE_H
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

public:
    DualSemaphore(int id, unsigned short first_val, unsigned short second_val, bool create);

    ~DualSemaphore();

    void detach(bool remove) const;

    void wait_for(int expect0, int expect1, int delay_ms = 100) const;

    void up(int index) const;

    void down(int index) const;
};

#endif //DUALSEMAPHORE_H
