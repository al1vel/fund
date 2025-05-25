#include "SharedMemory.h"

#include <cstring>

SharedMemory::SharedMemory(int id, size_t size) {
    this->sz = size;
    path = "memory_" + std::to_string(id) + ".key";
    std::ofstream(path.c_str()).put('\n');

    key_t key = ftok(path.c_str(), id);
    if (key == -1) {
        throw std::runtime_error("Error creating shared memory key");
    }

    shm_id = shmget(key, sz, 0666 | IPC_CREAT);
    if (shm_id == -1) {
        throw std::runtime_error("Error getting shared memory");
    }

    data = static_cast<char*>(shmat(shm_id, nullptr, 0));
    if (data == (char*)(-1)) {
        throw std::runtime_error("Error attaching shared memory");
    }
}

SharedMemory::~SharedMemory() {
    this->detach();
}

void SharedMemory::detach() const {
    if (data != nullptr) {
        shmdt(data);
    }
    if (shm_id != -1) {
        shmctl(shm_id, IPC_RMID, nullptr);
        remove(path.c_str());
    }
}

void SharedMemory::write(const std::string& message) const {
    strcpy(data, message.c_str());
}

std::string SharedMemory::read() const {
    std::string ret(data);
    return ret;
}