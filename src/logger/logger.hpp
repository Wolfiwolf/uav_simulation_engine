#pragma once

#include <iostream>
#include <string>
#include <ctime>

enum LogLevel {
    INFO = 0,
    WARNING,
    DANGER
};

class Logger {
    public:
        static void Log(const char* logger, const std::string &msg, LogLevel log_level = LogLevel::INFO);

};
