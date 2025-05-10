#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>
#include <iomanip>

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

int main() {
    Logger* my_logger = LoggerBuilder()
                            .set_level(Logger::WARNING)
                            .add_handler(std::cout)
                            .add_handler(std::make_unique<std::ofstream>("/home/begemot/fund/sys2/log.txt"))
                            .add_handler(std::make_unique<std::ofstream>("/home/begemot/fund/sys2/log2.txt"))
                            .make_object();

    my_logger->critical("Process 101 died.");
    my_logger->error("Division by zero.");
    my_logger->warning("File a.txt is not closed.");
    my_logger->info("Process 101 finished successfully.");
    my_logger->debug("Func <add>: (5, 10).");

    delete my_logger;
    return 0;
}