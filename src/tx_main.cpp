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

// 64 ASCII - symbols = 64 bytes
const char* message =
  "Hello LoRa!!!! This is a 64-byte message for radio transmission.";

const size_t PAYLOAD_SIZE = 64;

// Setup
void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println();
  Serial.println("========================================");
  Serial.println("          LoRa TX TEST");
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

    display.println("LoRa TX");
    display.println();
    display.print("SF: ");
    display.println(LORA_SF);

    display.print("BW: ");
    display.print(LORA_BANDWIDTH, 1);
    display.println(" kHz");

    display.print(LORA_FREQUENCY, 1);
    display.println(" MHz");
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

  // LoRa initialization
  Serial.println("[TX] Initializing LoRa...");

  int state = radio.begin(
    LORA_FREQUENCY,
    LORA_BANDWIDTH,
    LORA_SF,
    LORA_CR,
    LORA_SYNC_WORD,
    LORA_POWER
  );

  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("[TX] LoRa initialization error: ");
    Serial.println(state);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("LoRa ERROR!");
    display.print("Code: ");
    display.println(state);
    display.display();

    while (true) {
      delay(1000);
    }
  }

  // Implicit header used for LoRa mode
  state = radio.implicitHeader(64);

  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("[TX] implicitHeader error: ");
    Serial.println(state);
    while (true);
  }

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
  Serial.println();
  Serial.println("----------------------------------------");
  Serial.println("[TX] Sending 64-byte packet...");

  // Start measurement
  unsigned long startTime = micros();
  int state = radio.transmit(message);
  unsigned long endTime = micros();

  // Calculate transmission time
  unsigned long transmissionTime = endTime - startTime;
  float transmissionTimeMs = transmissionTime / 1000.0;

  // Result
  if (state == RADIOLIB_ERR_NONE) {
     // Serial monitor
    Serial.println("[TX] Transmission successful!");

    Serial.print("[TX] Payload: ");
    Serial.print(PAYLOAD_SIZE);
    Serial.println(" bytes");

    Serial.print("[TX] SF: ");
    Serial.println(LORA_SF);

    Serial.print("[TX] Frequency: ");
    Serial.print(LORA_FREQUENCY, 1);
    Serial.println(" MHz");

    Serial.print("[TX] BW: ");
    Serial.print(LORA_BANDWIDTH, 1);
    Serial.println(" kHz");

    Serial.print("[TX] Transmission time: ");
    Serial.print(transmissionTimeMs, 3);
    Serial.println(" ms");

    // Approximate bitrate
    float bitrate = (PAYLOAD_SIZE * 8.0) / (transmissionTime / 1000000.0);

    Serial.print("[TX] Effective bitrate: ");
    Serial.print(bitrate, 2);
    Serial.println(" bit/s");

    // OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("LoRa TX");

    display.print("SF: ");
    display.println(LORA_SF);

    display.print("BW: ");
    display.print(LORA_BANDWIDTH, 1);
    display.println(" kHz");

    display.print("Size: ");
    display.print(PAYLOAD_SIZE);
    display.println(" B");

    display.println();
    display.print("Time: ");
    display.print(transmissionTimeMs, 2);
    display.println(" ms.");
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
