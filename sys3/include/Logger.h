#ifndef LOGGER_H
#define LOGGER_H
#include <memory>
#include <ostream>
#include <vector>
#include <string>
#include <iomanip>

class LoggerBuilder;

class Logger {
private:
    friend class LoggerBuilder;
    std::vector<std::ostream*> handlers;
    std::vector<std::unique_ptr<std::ostream>> own_handlers;
    unsigned int log_level;

    std::string timestamp() const;

    void log(unsigned int level, const std::string& label, const std::string& msg) const;

public:
    Logger();

    enum {
        CRITICAL = 0,
        ERROR = 1,
        WARNING = 2,
        INFO = 3,
        DEBUG = 4
    };

    ~Logger();

    Logger(const Logger&);
    Logger& operator=(const Logger&);

    Logger(Logger&&) noexcept;
    Logger& operator=(Logger&&) noexcept;

    void critical(const std::string& msg) const;
    void error(const std::string& msg) const;
    void warning(const std::string& msg) const;
    void info(const std::string& msg) const;
    void debug(const std::string& msg) const;
};

class LoggerBuilder {
private:
    //Logger logger;
    std::shared_ptr<Logger> logger;

public:
    LoggerBuilder();

    LoggerBuilder& set_level(unsigned int log_level);

    LoggerBuilder& add_handler(std::ostream& stream);

    LoggerBuilder& add_handler(std::unique_ptr<std::ostream> stream);

    [[nodiscard]] std::shared_ptr<Logger> make_object();
};

#endif //LOGGER_H
