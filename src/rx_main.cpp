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

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Initialisation I2C for OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 0x3C
    Serial.println(F("OLED not found"));
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("LoRa Resiver");
  display.display();

  Serial.println(F("[RX] Initialisation..."));
  // Налаштування на ту саму частоту 915.0 МГц!
  //int state = radio.begin(915.0, 125.0, 6, 5);
int state = radio.begin(LORA_FREQUENCY, LORA_BANDWIDTH, LORA_SF, LORA_CR);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("[RX] Redy 915Mz!"));
    display.println("Waiting...");
    display.display();
  } else {
    display.println("Radio Error!");
    display.display();
    while (true);
  }

  // SF6 → implicit header
  state = radio.implicitHeader(64);

  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("[RX] implicitHeader error: ");
    Serial.println(state);
    while (true);
  }
}

void loop() {
  String receivedData;
  int state = radio.receive(receivedData);

  if (state == RADIOLIB_ERR_NONE) {
    // Очищаємо екран та виводимо нові дані
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
    delay(2000);

    // Show in Serial Monitor
    Serial.print(F("[RX] resived: "));
    Serial.println(receivedData);
   } 
  //  else if (state == RADIOLIB_ERR_RX_TIMEOUT) {

  //   // ================================
  //   // Пакет не прийшов
  //   // ================================

  //   Serial.println("[RX] NO SIGNAL / TIMEOUT");

  //   display.clearDisplay();
  //   display.setCursor(0, 0);

  //   display.println("RX: NO SIGNAL");
  //   display.println();
  //   display.println("Waiting for");
  //   display.println("packet...");

  //   display.display();
  // }
   else {
    display.println("[RX] PACKET NOT RECEIVED, CODE: ");
    display.println(state);
  }
//  delay(5000);
}