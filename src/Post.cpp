#include "Post.h"

uint8_t postResultMask = 0;

// Run ADC self method test
static bool runAdcSelfTest() {
    int rawValue = analogRead(34);
    return (rawValue >= 0 && rawValue <= 4095);
}

// Run Device Post
void runDevicePOST(SX1276& radio, Adafruit_SSD1306& display) {
    postResultMask = 0;

    logger.log(LogLevel::INFO_LEVEL, "--- STARTING POWER-ON SELF-TEST (POST) ---");

    // Config NVS test
    if (config.begin()) {
        postResultMask |= POST_NVS_BIT;
        logger.log(LogLevel::INFO_LEVEL, "[POST] NVS: OK");
    } else {
        logger.log(LogLevel::ERROR_LEVEL, "[POST] NVS: FAILED");
    }

    const DeviceConfig& cfg = config.get();

    // OLED display test
    static bool wireInitialized = false;
    if (!wireInitialized) {
        Wire.begin(OLED_SDA, OLED_SCL);
        Wire.setTimeOut(100);
        wireInitialized = true;
    }

    if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        postResultMask |= POST_OLED_BIT;
        logger.log(LogLevel::INFO_LEVEL, "[POST] OLED: OK");

        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println("POST Running...");
        display.display();
    } else {
        logger.log(LogLevel::ERROR_LEVEL, "[POST] OLED: FAILED");
    }

    // LoRa module test
    logger.log(LogLevel::INFO_LEVEL, "[POST] Initializing LoRa module...");

    int loraState = radio.begin(
        cfg.frequency,
        cfg.bandwidth,
        cfg.spreadingFactor,
        cfg.codingRate,
        cfg.syncWord
    );

    if (loraState == RADIOLIB_ERR_NONE) {
        radio.implicitHeader(64);
        postResultMask |= POST_LORA_BIT;
        logger.log(LogLevel::INFO_LEVEL, "[POST] LoRa: OK");
    } else {
        logger.log(LogLevel::ERROR_LEVEL, "[POST] LoRa: FAILED (Code: %d)", loraState);
    }

    // ADC self test
    if (runAdcSelfTest()) {
        postResultMask |= POST_ADC_BIT;
        logger.log(LogLevel::INFO_LEVEL, "[POST] ADC/Power: OK");
    } else {
        logger.log(LogLevel::ERROR_LEVEL, "[POST] ADC/Power: FAILED");
    }

    logger.log(LogLevel::INFO_LEVEL, "[POST] Finished. Mask: 0x%02X", postResultMask);

    // --- Show on display ---
    if (postResultMask & POST_OLED_BIT) {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("POST RESULTS:");
        display.printf("Mask: 0x%02X\n", postResultMask);
        display.println("--------------------");
        display.printf("NVS:  %s\n", (postResultMask & POST_NVS_BIT)  ? "OK" : "FAIL");
        display.printf("LoRa: %s\n", (postResultMask & POST_LORA_BIT) ? "OK" : "FAIL");
        display.printf("ADC:  %s\n", (postResultMask & POST_ADC_BIT)  ? "OK" : "FAIL");

        if (postResultMask == POST_ALL_OK) {
            display.println("\nSTATUS: ALL OPERATIONAL");
        } else {
            display.println("\nSTATUS: DEGRADED");
        }

        display.display();
    }
}

// Print post status
void printPostStatus() {
    Serial.println("--- POST Detailed Report ---");
    Serial.printf("NVS Config : %s\n", (postResultMask & POST_NVS_BIT)  ? "[ OK ]" : "[FAIL]");
    Serial.printf("OLED Screen: %s\n", (postResultMask & POST_OLED_BIT) ? "[ OK ]" : "[FAIL]");
    Serial.printf("LoRa Radio : %s\n", (postResultMask & POST_LORA_BIT) ? "[ OK ]" : "[FAIL]");
    Serial.printf("ADC Sensor : %s\n", (postResultMask & POST_ADC_BIT)  ? "[ OK ]" : "[FAIL]");
    Serial.printf("Final Mask : 0x%02X\n", postResultMask);
    Serial.println("----------------------------");
}
