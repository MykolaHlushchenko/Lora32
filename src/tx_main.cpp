#include <Arduino.h>
#include <RadioLib.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Lora32.h>
#include "Logger.h"
#include "Version.h"
#include "Config.h"
#include "Post.h"
#include "SerialCommands.h"

SX1276 radio = new Module(
  RADIO_NSS,
  RADIO_DIO0,
  RADIO_RST,
  RADIO_BUSY
);

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);


// 64 ASCII - symbols = 64 bytes
const char* message =
  "Hello LoRa!!!! This is a 64-byte message for radio transmission.";

const size_t PAYLOAD_SIZE = 64;

unsigned long lastTxTime = 0;
const unsigned long TX_INTERVAL = 5000;

SerialCommands serialCommands;

// Handle TX Command
void handleTXCommand(const String& command) {
    if (command.equalsIgnoreCase("version")) {
        printVersion();

    } else if (command.equalsIgnoreCase("log")) {
        logger.dump();

    } else if (command.equalsIgnoreCase("clear_logs")) {
        logger.clear();
        Serial.println("[LOG] Buffer cleared.");

    } else if (command.equalsIgnoreCase("config_show")) {
        config.print();

    } else if (command.equalsIgnoreCase("config_reset")) {
        if (config.reset()) {
            Serial.println("[CFG] Config reset to defaults.");

            // Apply default configuration
            const DeviceConfig& cfg = config.get();

            int state = radio.begin(
                cfg.frequency,
                cfg.bandwidth,
                cfg.spreadingFactor,
                cfg.codingRate,
                cfg.syncWord
            );

            if (state == RADIOLIB_ERR_NONE) {
                state = radio.implicitHeader(PAYLOAD_SIZE);
            }

            if (state == RADIOLIB_ERR_NONE) {
                Serial.println("[RADIO] Re-initialized successfully.");
            } else {
                Serial.printf("[RADIO] Initialization failed: %d\n", state);
            }
        } else {
            Serial.println("[CFG] Failed to reset configuration.");
        }

    } else if (command.equalsIgnoreCase("self_test")) {
        printPostStatus();

    } else if (command.startsWith("set ")) {
        // Format: set <parameter> <value>
        String args = command.substring(4);
        args.trim();

        int separator = args.indexOf(' ');

        if (separator < 0) {
            Serial.println("Usage: set <freq|bw|sf|cr|sync> <value>");
            return;
        }

        String key = args.substring(0, separator);
        String value = args.substring(separator + 1);

        key.trim();
        value.trim();

        if (key.equalsIgnoreCase("freq")) {
            key = "frequency";
        } else if (key.equalsIgnoreCase("bw")) {
            key = "bandwidth";
        } else {
            key.toLowerCase();
        }

        if (!config.set(key.c_str(), value.c_str())) {
            Serial.printf("[CFG] Invalid value or save failed: %s = %s\n", key.c_str(), value.c_str());
            return;
        }

        Serial.printf("[CFG] Saved: %s = %s\n", key.c_str(), value.c_str());

        const DeviceConfig& cfg = config.get();

        int state = radio.begin(
            cfg.frequency,
            cfg.bandwidth,
            cfg.spreadingFactor,
            cfg.codingRate,
            cfg.syncWord
        );

        if (state == RADIOLIB_ERR_NONE) {
            state = radio.implicitHeader(PAYLOAD_SIZE);
        }

        if (state == RADIOLIB_ERR_NONE) {
            logger.log(INFO_LEVEL, "LoRa re-initialized successfully!");
            Serial.println("[RADIO] Re-initialized successfully.");
        } else {
            logger.log(ERROR_LEVEL, "LoRa re-init failed with code: %d", state);
            Serial.printf("[RADIO] Re-initialization failed: %d\n", state);
        }

    } else {
        Serial.printf("Unknown TX command: %s\n", command.c_str());
    }
}

// Setup
void setup() {
  Serial.begin(115200);
  delay(2000);
  serialCommands.begin(handleTXCommand);

  // Self post diagnostic full system
  runDevicePOST(radio, display);

  lastTxTime = millis();
  logger.log(LogLevel::INFO_LEVEL, "Setup Log TX");

  Serial.println();
  Serial.println("========== CONFIG ==========");
  config.print();
  Serial.println("============================");

  // Check payload size
  size_t actualSize = strlen(message);

  Serial.print("[TX] Payload size: ");
  Serial.print(actualSize);
  Serial.println(" bytes");

  if (actualSize != PAYLOAD_SIZE) {
    Serial.println();
    Serial.println("ERROR!");
    Serial.println("Payload must be exactly 64 bytes!");

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("PAYLOAD ERROR!");
    display.println();
    display.print("Size: ");
    display.println(actualSize);
    display.println("Expected: 64 B");
    display.display();

    while (true) {
      delay(1000);
    }
  }

  // LoRa initialization
  Serial.println("[TX] Initializing LoRa...");
  const DeviceConfig& cfg = config.get();

  Serial.println("[TX] LoRa initialized successfully.");

  Serial.println();
  Serial.println("========================================");
  Serial.println("Configuration:");
  Serial.print("Frequency: ");
  Serial.print(LORA_FREQUENCY, 1);
  Serial.println(" MHz");

  Serial.print("Bandwidth:");
  Serial.print(LORA_BANDWIDTH, 1);
  Serial.println(" kHz");

  Serial.print("SF: ");
  Serial.println(LORA_SF);

  Serial.println("CR        : 4/5");
  Serial.println("Payload   : 64 bytes");
  Serial.println("========================================");
  Serial.println();

  display.clearDisplay();
  display.setCursor(0, 0);

  display.println("LoRa TX");
  display.println();

  display.print("SF: ");
  display.println(LORA_SF);

  display.print("BW: ");
  display.print(LORA_BANDWIDTH, 1);
  display.println(" kHz");

  display.print("Freq: ");
  display.print(LORA_FREQUENCY, 1);
  display.println(" MHz");

  display.println();
  display.println("Payload: 64 B");
  display.display();

  delay(2000);
}

// Loop
void loop() {
  serialCommands.update();

  if (millis() - lastTxTime >= TX_INTERVAL) {
    lastTxTime = millis();

    const DeviceConfig& cfg = config.get();

    // Start measurement
    unsigned long startTime = micros();
    int state = radio.transmit(message);
    unsigned long endTime = micros();

    // Calculate transmission time
    unsigned long transmissionTime = endTime - startTime;
    float transmissionTimeMs = transmissionTime / 1000.0;

    // Result
    if (state == RADIOLIB_ERR_NONE) {
      // Approximate bitrate
      float bitrate = (PAYLOAD_SIZE * 8.0) / (transmissionTime / 1000000.0);

      //logger.log(LogLevel::INFO_LEVEL, "TX Success! Time: %.2f ms, Bitrate: %.2f b/s", transmissionTimeMs, bitrate);

      // OLED
      display.clearDisplay();
      display.setCursor(0, 0);
      display.printf("\nLoRa TX | %.1f MHz\n", cfg.frequency);
      display.printf("\nSF: %u | BW: %.0f\n", cfg.spreadingFactor, cfg.bandwidth);
      display.printf("Time: %.1f ms\n", transmissionTimeMs);
      display.display();

    } else {
      logger.log(LogLevel::ERROR_LEVEL, "Transmission error code: %d", state);

      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("TX ERROR!");
      display.printf("\nCode: %d\n", state);
      display.display();
    }
  }
}
