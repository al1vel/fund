#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <netinet/in.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <random>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <streambuf>

class VectorStreamBuf : public std::streambuf {
private:
    std::vector<std::string>& buffer;
    std::string current_line;

protected:
    int overflow(int ch) override {
        if (ch == traits_type::eof()) return ch;

        if (ch == '\n') {
            buffer.push_back(std::move(current_line));
            current_line.clear();
        } else {
            current_line += static_cast<char>(ch);
        }

        return ch;
    }

    int sync() override {
        if (!current_line.empty()) {
            buffer.push_back(std::move(current_line));
            current_line.clear();
        }
        return 0;
    }

public:
    explicit VectorStreamBuf(std::vector<std::string>& buf) : buffer(buf) {}
};

struct tcp_traffic_pkg {
    in_addr_t src_addr;
    in_port_t src_port;
    in_addr_t dst_addr;
    in_port_t dst_port;
    const size_t sz;
};

struct stat_pkg {
    int command;
    size_t send_sz;
    size_t recv_sz;
    size_t connect_cnt;
    uint8_t ip[4];
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

std::string create_log() {
    std::string ips[10] = {
        "1.2.3.4",
        "127.0.0.1",
        "128.165.0.1",
        "110.250.1.0",
        "0.0.0.0",
        "10.81.50.23",
        "52.0.52.1",
        "100.111.34.53",
        "255.255.255.0",
        "116.3.123.1"
    };

    std::string op_types[4] {"CON", "SND", "REC", "DIS"};

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 1000);
    int random_number = dist(gen);
    std::string log;
    if ((random_number % 4 == 0) || (random_number % 4 == 3)) {
        log = op_types[random_number % 4] + "|" + ips[random_number % 10] + "|" + ips[random_number % 10];
    } else {
        log = op_types[random_number % 4] + "|" + ips[random_number % 10] + "|" + ips[random_number % 10] + "|" + std::to_string(random_number);
    }
    return log;
}

bool spam_on = false;
bool finish = false;

void make_logs(Logger& logger, int i) {
    while (true) {
        if (spam_on == true) {
            // std::cout << i << ": Hello world!" << std::endl;
            logger.info(create_log());
        } else {
            if (finish == true) {
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    //Manager
    std::cout << "Manager process started." << std::endl;

    std::ofstream("connect_generator.key").put('\n');
    std::ofstream("connect_analyzer.key").put('\n');

    key_t gen_key = ftok("connect_generator.key", 1);
    int gen_shm_id = shmget(gen_key, 256, 0666 | IPC_CREAT);
    auto *gen_buf = static_cast<stat_pkg*>(shmat(gen_shm_id, nullptr, 0));

    key_t an_key = ftok("connect_analyzer.key", 2);
    int an_shm_id = shmget(an_key, 256, 0666 | IPC_CREAT);
    auto *an_buf = static_cast<stat_pkg*>(shmat(an_shm_id, nullptr, 0));

    key_t gen_sem_key = ftok("connect_generator.key", 3);
    int gen_sem_id = semget(gen_sem_key, 1, 0666 | IPC_CREAT);
    sem_set(gen_sem_id, 0);

    key_t an_sem_key = ftok("connect_analyzer.key", 4);
    int an_sem_id = semget(an_sem_key, 1, 0666 | IPC_CREAT);
    sem_set(an_sem_id, 0);

    std::vector<std::string> log_buffer;
    auto* vsb = new VectorStreamBuf(log_buffer);
    auto* vector_stream = new std::ostream(vsb);
    // std::unique_ptr<VectorStreamBuf> vsb(new VectorStreamBuf(log_buffer));
    // std::unique_ptr<std::ostream> vector_stream(new std::ostream(vsb.get()));

    pid_t generator_pid = fork();
    if (generator_pid == 0) {
        //Generator
        std::vector<std::thread> threads;
        // Logger* logger = LoggerBuilder().set_level(Logger::INFO).add_handler(std::move(vector_stream)).make_object();
        Logger* logger = LoggerBuilder().set_level(Logger::INFO).add_handler(*vector_stream).make_object();

        for (int i = 0; i < 4; ++i) {
            threads.emplace_back(make_logs, std::ref(*logger), i);
        }

        while (true) {
            sem_wait(gen_sem_id);
            std::cout << "Generator got command: " << gen_buf->command << std::endl;
            if (gen_buf->command == quit) {
                spam_on = false;
                finish = true;
                break;
            }
            if (gen_buf->command == begin) {
                spam_on = true;
            }
            if (gen_buf->command == stop) {
                for (auto &log : log_buffer) {
                    std::cout << log << std::endl;
                }
                spam_on = false;
            }
            if (gen_buf->command == cont) {
                spam_on = true;
            }
        }

        for (auto& t : threads) {
            t.join();
        }
        delete logger;
        shmdt(gen_buf);
        delete vsb;
        delete vector_stream;
        return 0;
    }
    if (generator_pid == -1) {
        perror("fork generator failed");
        return 1;
    }

    pid_t analyzer_pid = fork();
    if (analyzer_pid == 0) {
        //Analyzer
        while (true) {
            sem_wait(an_sem_id);
            std::cout << "Analyzer got command: " << an_buf->command << std::endl;
            if (an_buf->command == quit) {
                break;
            }
        }
        shmdt(an_buf);
        return 0;
    }
    if (analyzer_pid == -1) {
        perror("fork analyzer failed");
        return 1;
    }

    std::cout << "Available commands:\n1.begin\n1.stat <ip>\n2.stop\n3.continue\n4.quit\n" << std::endl;
    std::string command;
    while (true) {
        if (!std::getline(std::cin, command)) {
            break;
        }
        if (command == "quit") {
            stat_pkg pkg {quit, 0, 0, 0, {0, 0, 0, 0}};
            *gen_buf = pkg;
            sem_signal(gen_sem_id);

            *an_buf = pkg;
            sem_signal(an_sem_id);
            break;
        }
        if (command == "begin") {
            stat_pkg pkg {begin, 0, 0, 0, {0, 0, 0, 0}};
            *gen_buf = pkg;
            sem_signal(gen_sem_id);
        }
        if (command.substr(0, 4) == "stat") {
            std::string ip_str = command.substr(5);
            uint8_t ip[4];
            if (inet_pton(AF_INET, ip_str.c_str(), ip) != 1) {
                std::cout << "Invalid IP address." << std::endl;
                continue;
            }
            stat_pkg pkg {statistic, 0, 0, 0};
            std::copy(ip, ip + 4, pkg.ip);
            *an_buf = pkg;
            sem_signal(an_sem_id);
        }
        if (command == "stop") {
            stat_pkg pkg {stop, 0, 0, 0, {0, 0, 0, 0}};
            *gen_buf = pkg;
            sem_signal(gen_sem_id);
        }
        if (command == "continue") {
            stat_pkg pkg {cont, 0, 0, 0, {0, 0, 0, 0}};
            *gen_buf = pkg;
            sem_signal(gen_sem_id);
        }
    }
    waitpid(analyzer_pid, nullptr, 0);
    waitpid(generator_pid, nullptr, 0);
    shmdt(gen_buf);
    shmdt(an_buf);
    shmctl(gen_shm_id, IPC_RMID, nullptr);
    shmctl(an_shm_id, IPC_RMID, nullptr);
    semctl(gen_sem_id, 0, IPC_RMID);
    semctl(an_sem_id, 0, IPC_RMID);
    remove("connect_generator.key");
    remove("connect_analyzer.key");

    std::cout << "Manager: All child processes finished." << std::endl;
    return 0;
}