#include "SerialCommands.h"

void SerialCommands::begin(CommandHandler handler) {
    commandHandler = handler;
    inputBuffer.reserve(128);
}

void SerialCommands::update() {
    while (Serial.available() > 0) {
        char c = (char)Serial.read();

        if (c == '\n' || c == '\r') {
            inputBuffer.trim();

            if (inputBuffer.length() > 0) {
                if (commandHandler != nullptr) {
                    commandHandler(inputBuffer);
                }

                inputBuffer = "";
            }
        } else {
            if (inputBuffer.length() < 128) {
                inputBuffer += c;
            }
        }
    }
}
