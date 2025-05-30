#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include <fstream>
#include <iostream>
#include <Logger.h>
#include <memory>
#include <sys/ipc.h>
#include <sys/shm.h>

class SharedMemory {
private:
    int shm_id;
    std::string path;
    size_t sz;
    char *data;
    std::shared_ptr<Logger> logger;

public:
    SharedMemory(int id, size_t size, bool create, const std::shared_ptr<Logger> &logger);

    ~SharedMemory();

    void detach(bool remove) const;

    void write(const std::string& str) const;

    [[nodiscard]] std::string read() const;
};

#endif //SHAREDMEMORY_H
