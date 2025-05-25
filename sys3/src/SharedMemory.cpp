#include "SharedMemory.h"

#include <cstring>

SharedMemory::SharedMemory(int id, size_t size, bool create) {
    this->sz = size;
    path = "memory_" + std::to_string(id) + ".key";

    if (create) {
        std::ofstream(path.c_str()).put('\n');
    }

    key_t key = ftok(path.c_str(), id);
    if (key == -1) {
        throw std::runtime_error("Error creating shared memory key");
    }

    int shm_flags = 0666 | (create ? IPC_CREAT : 0);
    shm_id = shmget(key, sz, shm_flags);
    if (shm_id == -1) {
        throw std::runtime_error("Error getting shared memory");
    }

    data = static_cast<char*>(shmat(shm_id, nullptr, 0));
    if (data == (char*)(-1)) {
        throw std::runtime_error("Error attaching shared memory");
    }
}

SharedMemory::~SharedMemory() {
    this->detach(false);
}

void SharedMemory::detach(bool remove = false) const {
    if (data != nullptr) {
        shmdt(data);
    }
    if (remove && shm_id != -1) {
        shmctl(shm_id, IPC_RMID, nullptr);
        std::remove(path.c_str());
    }
}

void SharedMemory::write(const std::string& message) const {
    strcpy(data, message.c_str());
}

std::string SharedMemory::read() const {
    std::string ret(data);
    return ret;
}