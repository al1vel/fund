#include "DualSemaphore.h"
#include <fstream>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>

DualSemaphore::DualSemaphore(int id, unsigned short first_val, unsigned short second_val, bool create, const std::shared_ptr<Logger> &logger) {
    this->logger = logger;
    path = "dual_sem_" + std::to_string(id) + ".key";
    if (create) {
        std::ofstream(path.c_str()).put('\n');
        logger->debug("[SEM]: Created file " + path + " for sem " + std::to_string(id));
    }

    key_t key = ftok(path.c_str(), id);
    if (key == -1) {
        throw std::runtime_error("Failed to generate key");
        logger->error("[SEM]: Failed to generate key for sem " + std::to_string(id));
    }

    int flags = 0666 | (create ? IPC_CREAT : 0);
    sem_id = semget(key, 2, flags);
    if (sem_id == -1) {
        throw std::runtime_error("Failed to get semaphore");
        logger->error("[SEM]: Failed to get semaphore");
    }
    logger->debug("[SEM]: Created semaphore " + std::to_string(sem_id));

    if (create) {
        semun arg;
        unsigned short values[2] = {first_val, second_val};
        arg.array = values;
        if (semctl(sem_id, 0, SETALL, arg) == -1) {
            throw std::runtime_error("Failed to initialize semaphore values");
            logger->error("[SEM]: Failed to initialize semaphore values");
        }
        logger->debug("[SEM]: Initialized semaphore " + std::to_string(sem_id) + " values");
    }
}

void DualSemaphore::detach(bool remove = false) const {
    if (remove) {
        std::remove(path.c_str());
        if (semctl(sem_id, 0, IPC_RMID) == -1) {
            throw std::runtime_error("Failed to remove semaphore");
            logger->error("[SEM]: Failed to remove semaphore");
        }
        logger->debug("[SEM]: Removed semaphore " + std::to_string(sem_id));
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
            logger->error("[SEM]: sem_id " + std::to_string(sem_id) + " semctl GETVAL failed");
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
        logger->error("[SEM]: sem_id " + std::to_string(sem_id) + " up failed");
    }
    logger->debug("[SEM]: sem_id " + std::to_string(sem_id) + " increased sem #" + std::to_string(index));
}

void DualSemaphore::down(int index) const {
    struct sembuf op = { static_cast<unsigned short>(index), -1, 0 };
    if (semop(sem_id, &op, 1) == -1) {
        throw std::runtime_error("semop down failed");
        logger->error("[SEM]: sem_id " + std::to_string(sem_id) + " down failed");
    }
    logger->debug("[SEM]: sem_id " + std::to_string(sem_id) + " decreased sem #" + std::to_string(index));
}