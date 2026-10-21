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

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

SX1276 radio = new Module(
  RADIO_NSS,
  RADIO_DIO0,
  RADIO_RST,
  RADIO_BUSY
);

SerialCommands serialCommands;

// Handle RX Command
void handleRXCommand(const String& command) {
    if (command.equalsIgnoreCase("version")) {
        printVersion();

    } else if (command.equalsIgnoreCase("log")) {
        logger.dump();

    } else if (command.equalsIgnoreCase("clear_logs")) {
        logger.clear();
        Serial.println("[LOG] Buffer cleared.");

    } else if (command.equalsIgnoreCase("config_show")) {
        config.print();

    } else if (command.equalsIgnoreCase("self_test")) {
        printPostStatus();

    } else {
        Serial.printf("Unknown RX command: %s\n", command.c_str());
    }
}

// Setup
void setup() {
  Serial.begin(115200);
  delay(2000);

  serialCommands.begin(handleRXCommand);

  // Self post diagnostic full system
  runDevicePOST(radio, display);

  logger.log(LogLevel::INFO_LEVEL, "Setup Log RX");

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("LoRa RX");
  display.display();
}

// Loop
void loop() {
  String receivedData;

  serialCommands.update();

  int state = radio.receive(receivedData);

  if (state == RADIOLIB_ERR_NONE) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("Package resived ");
    display.println("---------------------");

    // Text message
    display.setTextSize(1);
    display.print("TXT: ");
    display.println(receivedData);

    // Quality signal
    display.print("RSSI: ");
    display.print(radio.getRSSI());
    display.println(" dBm");

    float snr = radio.getSNR();
    display.print("SNR: ");
    display.print(snr, 1);
    display.println(" dB");
    display.display();
   } else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("RX: NO SIGNAL");
    display.println();
    display.println("Waiting for");
    display.println("packet...");
    display.display();
  } else {
    display.println("[RX] PACKET NOT RECEIVED, CODE: ");
    display.println(state);
  }
}
