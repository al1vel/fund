#include "SharedMemory.h"

#include <cstring>

SharedMemory::SharedMemory(int id, size_t size, bool create, const std::shared_ptr<Logger> &logger) {
    this->logger = logger;
    this->sz = size;
    path = "memory_" + std::to_string(id) + ".key";

    if (create) {
        std::ofstream(path.c_str()).put('\n');
        logger->debug("[SHM]: Created file " + path);
    }

    key_t key = ftok(path.c_str(), id);
    if (key == -1) {
        throw std::runtime_error("Error creating shared memory key");
        logger->error("[SHM]: Failed to generate key for shm " + std::to_string(id));
    }

    int shm_flags = 0666 | (create ? IPC_CREAT : 0);
    shm_id = shmget(key, sz, shm_flags);
    if (shm_id == -1) {
        throw std::runtime_error("Error getting shared memory");
        logger->error("[SHM]: Failed to get shared memory " + std::to_string(id));
    }
    logger->debug("[SHM]: Created shared memory " + std::to_string(shm_id));

    data = static_cast<char*>(shmat(shm_id, nullptr, 0));
    if (data == (char*)(-1)) {
        throw std::runtime_error("Error attaching shared memory");
        logger->error("[SHM]: Failed to attach shared memory " + std::to_string(shm_id));
    }
    logger->debug("[SHM]: Attached shared memory " + std::to_string(shm_id));
}

SharedMemory::~SharedMemory() {
    this->detach(false);
}

void SharedMemory::detach(bool remove = false) const {
    if (data != nullptr) {
        shmdt(data);
        logger->debug("[SHM]: Detached shared memory " + std::to_string(shm_id));
    }
    if (remove && shm_id != -1) {
        shmctl(shm_id, IPC_RMID, nullptr);
        std::remove(path.c_str());
        logger->debug("[SHM]: Closed shared memory " + std::to_string(shm_id));
    }
}

void SharedMemory::write(const std::string& message) const {
    strcpy(data, message.c_str());
    logger->debug("[SHM]: Sending message <" + message + "> to shm " + std::to_string(shm_id));
}

std::string SharedMemory::read() const {
    std::string ret(data);
    logger->debug("[SHM]: Received message <" + ret + "> from shm " + std::to_string(shm_id));
    return ret;
}