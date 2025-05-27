#include "Logger.h"
#include <iostream>

Logger::Logger() {
    log_level = INFO;
}

std::string Logger::timestamp() const {
    std::time_t now = std::time(nullptr);
    std::tm* tm_ptr = std::localtime(&now);

    std::ostringstream oss;
    oss << std::put_time(tm_ptr, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void Logger::log(unsigned int level, const std::string& label, const std::string& msg) const {
    if (level > log_level) {
        return;
    }
    for (auto&& out : handlers) {
        (*out) << timestamp() + " --> " << label << ": " << msg << std::endl;
    }
}

Logger::~Logger() {
    std::cout << "Logger destroyed." << std::endl << "All own handlers closed." << std::endl;
}

Logger::Logger(Logger&& other) noexcept {
    std::cout << "Logger move constructed." << std::endl;
    log_level = other.log_level;
    handlers = std::move(other.handlers);
    own_handlers = std::move(other.own_handlers);
}

Logger& Logger::operator=(Logger&& other) noexcept {
    std::cout << "Logger move assigned." << std::endl;
    if (this != &other) {
        log_level = other.log_level;
        handlers = std::move(other.handlers);
        own_handlers = std::move(other.own_handlers);
    }
    return *this;
}

Logger &Logger::operator=(const Logger &) {
    std::cout << "Logger assigned." << std::endl;
    return *this;
}

Logger::Logger(const Logger &) {
    std::cout << "Logger copy constructed." << std::endl;
}

void Logger::critical(const std::string& msg) const {
    Logger::log(CRITICAL, "CRITICAL", msg);
}
void Logger::error(const std::string& msg) const {
    Logger::log(ERROR, "ERROR", msg);
}
void Logger::warning(const std::string& msg) const {
    Logger::log(WARNING, "WARNING", msg);
}
void Logger::info(const std::string& msg) const {
    Logger::log(INFO, "INFO", msg);
}
void Logger::debug(const std::string& msg) const {
    Logger::log(DEBUG, "DEBUG", msg);
}


LoggerBuilder::LoggerBuilder() {
    this->logger = std::make_shared<Logger>();
}

LoggerBuilder& LoggerBuilder::set_level(unsigned int log_level) {
    this->logger->log_level = log_level;
    if (log_level > Logger::DEBUG) {
        std::cout << "Provided log level is doesn't exist!\nDEBUG mode was used." << std::endl;
        this->logger->log_level = Logger::DEBUG;
    }
    return *this;
}

LoggerBuilder& LoggerBuilder::add_handler(std::ostream& stream) {
    this->logger->handlers.push_back(&stream);
    return *this;
}

LoggerBuilder& LoggerBuilder::add_handler(std::unique_ptr<std::ostream> stream) {
    this->logger->handlers.push_back(stream.get());
    this->logger->own_handlers.push_back(std::move(stream));
    return *this;
}

std::shared_ptr<Logger> LoggerBuilder::make_object() {
    return std::move(this->logger);
}