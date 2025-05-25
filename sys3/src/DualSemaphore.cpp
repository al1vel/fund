#include "DualSemaphore.h"
#include <fstream>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>

DualSemaphore::DualSemaphore(int id, unsigned short first_val, unsigned short second_val) {
    path = "dual_sem_" + std::to_string(id) + ".key";
    std::ofstream(path.c_str()).put('\n');

    key_t key = ftok(path.c_str(), id);
    sem_id = semget(key, 2, 0666 | IPC_CREAT);
    if (sem_id == -1) {
        throw std::runtime_error("Failed to create semaphore");
    }

    semun arg;
    unsigned short values[2] = {first_val, second_val};
    arg.array = values;

    if (semctl(sem_id, 0, SETALL, arg) == -1) {
        throw std::runtime_error("Failed to set semaphore");
    }
}

void DualSemaphore::detach() const {
    remove(path.c_str());
    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        throw std::runtime_error("Failed to remove semaphore");
    }
}

DualSemaphore::~DualSemaphore() {
    if (sem_id != -1) {
        detach();
    }
}

void DualSemaphore::wait_for(int expect0, int expect1, int max_retries, int delay_ms) const {
    for (int i = 0; i < max_retries; ++i) {
        int v0 = semctl(sem_id, 0, GETVAL);
        int v1 = semctl(sem_id, 1, GETVAL);
        if (v0 == -1 || v1 == -1)
            throw std::runtime_error("semctl GETVAL failed");

        if (v0 == expect0 && v1 == expect1)
            return;

        usleep(delay_ms * 1000);
    }
    throw std::runtime_error("Timeout waiting for semaphore values");
}

void DualSemaphore::up(int index) const {
    struct sembuf op = { static_cast<unsigned short>(index), +1, 0 };
    if (semop(sem_id, &op, 1) == -1) {
        throw std::runtime_error("semop up failed");
    }
}

void DualSemaphore::down(int index) const {
    struct sembuf op = { static_cast<unsigned short>(index), -1, 0 };
    if (semop(sem_id, &op, 1) == -1) {
        throw std::runtime_error("semop down failed");
    }
}