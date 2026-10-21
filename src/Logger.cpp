#include "Logger.h"
#include <string.h>

Logger logger;

// Logger set level
void Logger::setLevel(LogLevel level) {
    currentLevel = level;
}

// Logger log
void Logger::log(LogLevel level, const char* message, ...) {
    if (level < currentLevel || message == nullptr) {
        return;
    }

    LogEntry& entry = entries[next];

    entry.timestamp = millis();
    entry.level = level;

    va_list args;
    va_start(args, message);
    vsnprintf(entry.message, sizeof(entry.message), message, args);
    va_end(args);

    next = (next + 1) % CAPACITY;

    if (count < CAPACITY) {
        count++;
    }
}

// Logger dump
void Logger::dump() {
    uint8_t start = (next + CAPACITY - count) % CAPACITY;

    for (uint8_t i = 0; i < count; i++) {
        const LogEntry& entry = entries[(start + i) % CAPACITY];
        Serial.printf("[%lu] [%d] %s\n", entry.timestamp, static_cast<int>(entry.level), entry.message);
    }
}

// Logger clear
void Logger::clear() {
    next = 0;
    count = 0;
    memset(entries, 0, sizeof(entries));
}
