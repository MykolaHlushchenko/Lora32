#include <Arduino.h>
#include <RadioLib.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Lora32.h>

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

#define PACKET_SIZE 64

// =====================================================
// Setup
// =====================================================

void setup() {

  Serial.begin(115200);
  delay(2000);

  Serial.println();
  Serial.println("========================================");
  Serial.println("           FSK RX TEST");
  Serial.println("========================================");

  // ===================================================
  // OLED
  // ===================================================

  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C
      )) {

    Serial.println("[OLED] Not found");

  } else {

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    display.println("FSK RX");
    display.println();
    display.println("BR: 15.2 kbps");
    display.println("DEV: 15.2 kHz");
    display.println("915 MHz");
    display.println();
    display.println("Initializing...");

    display.display();
  }

  // ===================================================
  // FSK initialization
  // ===================================================

  Serial.println("[RX] Initializing FSK...");

  int state = radio.beginFSK(
    FSK_FREQUENCY,
    FSK_BITRATE,
    FSK_DEVIATION,
    FSK_BANDWIDTH,
    FSK_POWER,
    FSK_PREAMBLE
  );

  Serial.print("[RX] beginFSK result: ");
  Serial.println(state);

  if (state != RADIOLIB_ERR_NONE) {

    Serial.print("[RX] FSK initialization error: ");
    Serial.println(state);

    display.clearDisplay();
    display.setCursor(0, 0);

    display.println("FSK ERROR!");

    display.print("Code: ");
    display.println(state);

    display.display();

    while (true) {
      delay(1000);
    }
  }

  // ===================================================
  // Fixed packet length = 64 bytes
  // ===================================================

  state = radio.fixedPacketLengthMode(PACKET_SIZE);

  Serial.print("[RX] fixedPacketLengthMode: ");
  Serial.println(state);

  if (state != RADIOLIB_ERR_NONE) {

    display.clearDisplay();
    display.setCursor(0, 0);

    display.println("PACKET ERROR!");

    display.print("Code: ");
    display.println(state);

    display.display();

    while (true) {
      delay(1000);
    }
  }

  Serial.println("[RX] FSK initialized successfully.");
  Serial.println("[RX] Waiting for packet...");

  // ===================================================
  // OLED ready
  // ===================================================

  display.clearDisplay();
  display.setCursor(0, 0);

  display.println("FSK RX READY");
  display.println();

  display.println("BR: 15.2 kbps");
  display.println("DEV: 15.2 kHz");
  display.println("915 MHz");

  display.println();
  display.println("Waiting...");

  display.display();
}

// =====================================================
// Loop
// =====================================================

void loop() {

  uint8_t receivedData[PACKET_SIZE];

  Serial.println("[RX] Waiting...");

  int state = radio.receive(
    receivedData,
    PACKET_SIZE
  );

  if (state == RADIOLIB_ERR_NONE) {

    Serial.println("[RX] RX OK");

    // -----------------------------------------------
    // Show text
    // -----------------------------------------------

    Serial.print("[RX] Received ");
    Serial.print(PACKET_SIZE);
    Serial.println(" bytes:");

    Serial.print("[RX] TEXT: ");

    for (int i = 0; i < PACKET_SIZE; i++) {
      Serial.print((char)receivedData[i]);
    }

    Serial.println();

    // -----------------------------------------------
    // OLED
    // -----------------------------------------------

    display.clearDisplay();
    display.setCursor(0, 0);

    display.println("FSK RX OK");
    display.println("----------------");

    // show first ~40 symbols
    
    display.print("RX: ");

    for (int i = 0; i < 40 && i < PACKET_SIZE; i++) {
      display.print((char)receivedData[i]);
    }

    display.println();

    display.println();

    display.print("Size: ");
    display.print(PACKET_SIZE);
    display.println(" B");

    display.print("RSSI: ");
    display.print(radio.getRSSI());
    display.println(" dBm");

    display.display();

  }
  else {

    if (state != RADIOLIB_ERR_RX_TIMEOUT) {

      Serial.print("[RX] Error: ");
      Serial.println(state);

      display.clearDisplay();
      display.setCursor(0, 0);

      display.println("RX ERROR!");

      display.print("Code: ");
      display.println(state);

      display.display();
    }
  }
}