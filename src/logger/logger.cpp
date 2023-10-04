#include "logger.hpp"

void Logger::Log(const char* logger, const std::string &msg, LogLevel log_level) {

    const std::time_t now = std::time(nullptr) ; // get the current time point

    const std::tm calendar_time = *std::localtime( std::addressof(now) ) ;

    switch (log_level) {
        case LogLevel::INFO:
            std::cout << "\033[32m";
            break;
        case LogLevel::WARNING:
            std::cout << "\033[31m";
            break;
        case LogLevel::DANGER:
            std::cout << "\033[31m";
            break;
    }

    std::cout << "[" <<  logger << " | "; 
    std::cout << calendar_time.tm_hour << ":"; 
    std::cout << calendar_time.tm_min << ":"; 
    std::cout << calendar_time.tm_sec << "]:\n"; 

    std::cout << msg << "\033[0m\n";
}
