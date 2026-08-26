#include <Arduino.h>
#include <RadioLib.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Lora32.h>

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

// 64 ASCII-symbols = 64 bytes
const char* message =
   "Hello LoRa!!!! This is a 64-byte message for radio transmission.";

const size_t PAYLOAD_SIZE = 64;

// Setup
void setup() {

  Serial.begin(115200);
  delay(2000);

  Serial.println();
  Serial.println("========================================");
  Serial.println("           FSK TX TEST");
  Serial.println("========================================");


  // OLED initialization
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("[OLED] Not found");
  } else {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    display.println("FSK TX");
    display.println();

    display.print("BR: ");
    display.print(FSK_BITRATE, 1);
    display.println("kbps");

    display.print("DEV: ");
    display.print(FSK_DEVIATION, 1);
    display.println(" kHz");

    display.println("915 MHz");
    display.println();
    display.println("Initializing...");

    display.display();
  }

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

  // FSK initialization
  Serial.println("[TX] Initializing FSK...");

  int state = radio.beginFSK(
    FSK_FREQUENCY,
    FSK_BITRATE,
    FSK_DEVIATION,
    FSK_BANDWIDTH,
    FSK_POWER,
    FSK_PREAMBLE
  );

  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("[TX] FSK initialization error: ");
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

  // Fixed packet length = 64 bytes for FSK mode
  state = radio.fixedPacketLengthMode(PAYLOAD_SIZE);

  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("[TX] fixedPacketLengthMode error: ");
    Serial.println(state);

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

  Serial.println("[TX] FSK initialized successfully.");

  Serial.println();
  Serial.println("========================================");
  Serial.println("Configuration:");

  Serial.println("Frequency: ");
  Serial.println(FSK_FREQUENCY, 1);
  Serial.println(" MHz");

  Serial.println("Bitrate: ");
  Serial.println(FSK_BITRATE, 1);
  Serial.println(" kbps");

  Serial.println("Deviation: ");
  Serial.println(FSK_DEVIATION, 1);
  Serial.println(" kHz");

  Serial.println("Bandwidth:");
  Serial.println(FSK_BANDWIDTH, 1);
  Serial.println(" kHz");

  Serial.println("Payload   : 64 bytes");
  Serial.println("========================================");
  Serial.println();

  // OLED ready screen
  display.clearDisplay();
  display.setCursor(0, 0);

  display.println("FSK TX");
  display.println();

  display.print("BR: ");
  display.print(FSK_BITRATE, 1);
  display.println(" kbps");

  display.print("DEV: ");
  display.print(FSK_DEVIATION, 1);
  display.println(" kHz");

  display.println("Freq: ");
  display.println(FSK_FREQUENCY, 1);
  display.println(" MHz");

  display.println();
  display.println("Payload: 64 B");
  display.display();

  delay(2000);
}


// Loop
void loop() {
  Serial.println();
  Serial.println("----------------------------------------");
  Serial.println("[TX] Sending 64-byte packet...");

  // Start measurement time
  unsigned long startTime = micros();

  int state = radio.transmit(
    message,
    PAYLOAD_SIZE
  );

  unsigned long endTime = micros();

  // Calculate transmission time
  unsigned long transmissionTime = endTime - startTime;

  float transmissionTimeMs = transmissionTime / 1000.0;

  // Result
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("[TX] Transmission successful!");

    Serial.print("[TX] Payload: ");
    Serial.print(PAYLOAD_SIZE);
    Serial.println(" bytes");

    Serial.print("[TX] Bitrate: ");
    Serial.print(FSK_BITRATE);
    Serial.println(" kbps");

    Serial.print("[TX] Deviation: ");
    Serial.print(FSK_DEVIATION);
    Serial.println(" kHz");

    Serial.print("[TX] Transmission time: ");
    Serial.print(transmissionTimeMs, 3);
    Serial.println(" ms");

    // Effective bitrate
    float effectiveBitrate = (PAYLOAD_SIZE * 8.0) / (transmissionTime / 1000000.0);

    Serial.print("[TX] Effective bitrate: ");
    Serial.print(effectiveBitrate, 2);
    Serial.println(" bit/s");

    // OLED
    display.clearDisplay();
    display.setCursor(0, 0);

    display.println("FSK TX");

    display.print("BR: ");
    display.print(FSK_BITRATE, 1);
    display.println(" kbps");

    display.print("DEV: ");
    display.print(FSK_DEVIATION, 1);
    display.println(" kHz");

    display.print("Size: ");
    display.print(PAYLOAD_SIZE);
    display.println(" B");

    display.println();

    display.print("Time: ");
    display.print(transmissionTimeMs, 2);
    display.println(" ms");
    display.display();

  } else {

    Serial.print("[TX] Transmission error: ");
    Serial.println(state);

    display.clearDisplay();
    display.setCursor(0, 0);

    display.println("TX ERROR!");

    display.print("Code: ");
    display.println(state);
    display.display();
  }

  // Wait before next packet
  Serial.println("[TX] Waiting 5 seconds...");

  delay(5000);
}
