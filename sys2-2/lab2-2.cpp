#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <netinet/in.h>
#include <streambuf>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/sem.h>

struct tcp_traffic_pkg {
    in_addr_t src_addr;
    in_port_t src_port;
    in_addr_t dst_addr;
    in_port_t dst_port;
    const size_t sz;
};

struct stat_pkg {
    int command;
    std::string ip;
    size_t send_sz;
    size_t recv_sz;
    size_t connect_cnt;
};

class LoggerBuilder;

class Logger {
private:
    friend class LoggerBuilder;
    std::vector<std::ostream*> handlers;
    std::vector<std::unique_ptr<std::ostream>> own_handlers;
    unsigned int log_level;

    Logger() {
        log_level = CRITICAL;
    }

    std::string timestamp() {
        std::time_t now = std::time(nullptr);
        std::tm* tm_ptr = std::localtime(&now);

        std::ostringstream oss;
        oss << std::put_time(tm_ptr, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

public:
    enum {
        CRITICAL = 0,
        ERROR = 1,
        WARNING = 2,
        INFO = 3,
        DEBUG = 4
    };

    ~Logger() {
        std::cout << "Logger destroyed." << std::endl << "All own handlers closed." << std::endl;
    }

    void log(unsigned int level, const std::string& label, const std::string& msg) {
        if (level > log_level) {
            return;
        }
        for (auto&& out : handlers) {
            (*out) << timestamp() + " --> " << label << ": " << msg << std::endl;
        }
    }

    void critical(const std::string& msg) {
        log(CRITICAL, "CRITICAL", msg);
    }
    void error(const std::string& msg) {
        log(ERROR, "ERROR", msg);
    }
    void warning(const std::string& msg) {
        log(WARNING, "WARNING", msg);
    }
    void info(const std::string& msg) {
        log(INFO, "INFO", msg);
    }
    void debug(const std::string& msg) {
        log(DEBUG, "DEBUG", msg);
    }
};

class LoggerBuilder {
private:
    Logger* logger = nullptr;

public:
    LoggerBuilder() {
        this->logger = new Logger();
    }

    LoggerBuilder& set_level(unsigned int log_level) {
        this->logger->log_level = log_level;
        if (log_level > Logger::DEBUG) {
            std::cout << "Provided log level is doesn't exist!\nDEBUG mode was used." << std::endl;
            this->logger->log_level = Logger::DEBUG;
        }
        return *this;
    }

    LoggerBuilder& add_handler(std::ostream& stream) {
        this->logger->handlers.push_back(&stream);
        return *this;
    }

    LoggerBuilder& add_handler(std::unique_ptr<std::ostream> stream) {
        this->logger->handlers.push_back(stream.get());
        this->logger->own_handlers.push_back(std::move(stream));
        return *this;
    }

    [[nodiscard]] Logger* make_object() const {
        return this->logger;
    }
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void sem_wait(int sem_id) {
    sembuf op = {0, -1, 0};
    if (semop(sem_id, &op, 1) == -1) {
        perror("sem_wait");
    }
}

void sem_signal(int sem_id) {
    sembuf op = {0, 1, 0};
    if (semop(sem_id, &op, 1) == -1) {
        perror("sem_signal");
    }
}

void sem_set(int sem_id, int value) {
    semun arg{};
    arg.val = value;
    if (semctl(sem_id, 0, SETVAL, arg) == -1) {
        perror("sem_set");
    }
}

enum {
    begin,
    statistic,
    stop,
    cont,
    quit
};

int main() {
    pid_t pid = fork();
    if (pid == -1) {
        throw std::runtime_error("Fork failed.");
    }

    if (pid == 0) {
        pid_t pid2 = fork();
        if (pid2 == -1) {
            throw std::runtime_error("Fork failed.");
        }

        if (pid2 == 0) {
            //Generator
            key_t gen_key = ftok("connect_generator", 1);
            int gen_shm_id = shmget(gen_key, 256, 0666 | IPC_CREAT);
            auto* shm_ptr = static_cast<stat_pkg*>(shmat(gen_shm_id, nullptr, 0));

            key_t sem_key = ftok("connect_generator", 2);
            int sem_id = semget(sem_key, 1, 0666 | IPC_CREAT);

            while (true) {
                sem_wait(sem_id);


                std::cout << "Generator got:\n";
                std::cout << "command = " << shm_ptr->command << "\n";
                std::cout << "connect_cnt = " << shm_ptr->connect_cnt << "\n";
                std::cout << "recv_sz = " << shm_ptr->recv_sz << "\n";
                std::cout << "send_sz = " << shm_ptr->send_sz << "\n";

                if (shm_ptr->command == quit) {
                    break;
                }
            }
            shmdt(shm_ptr);

        } else {
            //Analyzer
            key_t an_key = ftok("connect_analyzer", 1);
            int an_shm_id = shmget(an_key, 256, 0666 | IPC_CREAT);
            auto* shm_ptr = static_cast<stat_pkg*>(shmat(an_shm_id, nullptr, 0));

            key_t sem_key = ftok("connect_analyzer", 2);
            int sem_id = semget(sem_key, 1, 0666 | IPC_CREAT);

            while (true) {
                sem_wait(sem_id);


                std::cout << "Generator got:\n";
                std::cout << "command = " << shm_ptr->command << "\n";
                std::cout << "connect_cnt = " << shm_ptr->connect_cnt << "\n";
                std::cout << "recv_sz = " << shm_ptr->recv_sz << "\n";
                std::cout << "send_sz = " << shm_ptr->send_sz << "\n";

                if (shm_ptr->command == quit) {
                    break;
                }
            }
            shmdt(shm_ptr);
        }

    } else {
        //Manager
        std::cout << "Manager process started." << std::endl;

        key_t gen_key = ftok("connect_analyzer", 1);
        int gen_shm_id = shmget(gen_key, 256, 0666 | IPC_CREAT);
        auto *gen_buf = static_cast<stat_pkg*>(shmat(gen_shm_id, nullptr, 0));

        key_t an_key = ftok("connect_generator", 1);
        int an_shm_id = shmget(an_key, 256, 0666 | IPC_CREAT);
        auto *an_buf = static_cast<stat_pkg*>(shmat(an_shm_id, nullptr, 0));

        key_t gen_sem_key = ftok("connect_generator", 2);
        int gen_sem_id = semget(gen_sem_key, 1, 0666 | IPC_CREAT);
        sem_set(gen_sem_id, 0);

        key_t an_sem_key = ftok("connect_analyzer", 2);
        int an_sem_id = semget(an_sem_key, 1, 0666 | IPC_CREAT);
        sem_set(an_sem_id, 0);

        std::cout << "Available commands:\n1.begin\n1.stat <ip>\n2.stop\n3.continue\n4.quit\n" << std::endl;
        std::string command;
        while (true) {
            if (!std::getline(std::cin, command)) {
                break;
            }
            if (command == "quit") {
                stat_pkg pkg {quit, "", 0, 0, 0};
                *gen_buf = pkg;
                sem_signal(gen_sem_id);

                *an_buf = pkg;
                sem_signal(an_sem_id);
                break;
            }
            if (command == "begin") {
                stat_pkg pkg {begin, "", 0, 0, 0};
                *gen_buf = pkg;
                sem_signal(gen_sem_id);
            }
            if (command.substr(0, 4) == "stat") {
                std::string ip = command.substr(4);
                stat_pkg pkg {statistic, "", 0, 0, 0};
                *an_buf = pkg;
                sem_signal(an_sem_id);
            }
        }
        shmdt(gen_buf);
        shmdt(an_buf);
        shmctl(gen_shm_id, IPC_RMID, nullptr);
        shmctl(an_shm_id, IPC_RMID, nullptr);
        semctl(gen_sem_id, 0, IPC_RMID);
        semctl(an_sem_id, 0, IPC_RMID);
    }

    return 0;
}

