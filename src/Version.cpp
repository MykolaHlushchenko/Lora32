#include "Version.h"
#include <Arduino.h>

const char* const APP_VERSION = "2.0.0";
const char* const GIT_HASH = BUILD_HASH; // includes in platformio.ini

void printVersion() {
    Serial.println("========== VERSION INFO ==========");
    Serial.print("Firmware Version: "); Serial.println(APP_VERSION);
    Serial.print("Build Hash      : "); Serial.println(GIT_HASH);
    Serial.println("==================================");
}
