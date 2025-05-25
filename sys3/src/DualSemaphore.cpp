#include "DualSemaphore.h"
#include <fstream>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>

DualSemaphore::DualSemaphore(int id, unsigned short first_val, unsigned short second_val, bool create) {
    path = "dual_sem_" + std::to_string(id) + ".key";
    if (create) {
        std::ofstream(path.c_str()).put('\n');
    }

    key_t key = ftok(path.c_str(), id);
    if (key == -1) {
        throw std::runtime_error("Failed to generate key");
    }

    int flags = 0666 | (create ? IPC_CREAT : 0);
    sem_id = semget(key, 2, flags);
    if (sem_id == -1) {
        throw std::runtime_error("Failed to get semaphore");
    }

    if (create) {
        semun arg;
        unsigned short values[2] = {first_val, second_val};
        arg.array = values;
        if (semctl(sem_id, 0, SETALL, arg) == -1) {
            throw std::runtime_error("Failed to initialize semaphore values");
        }
    }
}

void DualSemaphore::detach(bool remove = false) const {
    if (remove) {
        std::remove(path.c_str());
        if (semctl(sem_id, 0, IPC_RMID) == -1) {
            throw std::runtime_error("Failed to remove semaphore");
        }
    }
}

DualSemaphore::~DualSemaphore() {
    if (sem_id != -1) {
        detach(false);
    }
}

void DualSemaphore::wait_for(int expect0, int expect1, int delay_ms) const {
    while(true) {
        int v0 = semctl(sem_id, 0, GETVAL);
        int v1 = semctl(sem_id, 1, GETVAL);
        if (v0 == -1 || v1 == -1) {
            throw std::runtime_error("semctl GETVAL failed");
        }

        if (v0 == expect0 && v1 == expect1) {
            return;
        }

        usleep(delay_ms * 1000);
    }
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