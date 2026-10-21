#pragma once

#include <Arduino.h>

using CommandHandler = void (*)(const String& command);

class SerialCommands {
public:
    void begin(CommandHandler handler);
    void update();

private:
    String inputBuffer;
    CommandHandler commandHandler = nullptr;
};
