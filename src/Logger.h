#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

enum LogLevel {
    DEBUG_LEVEL = 0,
    INFO_LEVEL,
    WARN_LEVEL,
    ERROR_LEVEL
};

struct LogEntry {
    unsigned long timestamp;
    LogLevel level;
    char message[96];
};

class Logger {
    public:
        static const uint8_t CAPACITY = 20;

        void setLevel(LogLevel level);
        void log(LogLevel level, const char* message, ...);
        void dump();
        void clear();

    private:
        LogEntry entries[CAPACITY] = {};
        uint8_t next = 0;
        uint8_t count = 0;
        LogLevel currentLevel = INFO_LEVEL;
};

extern Logger logger;

#endif
