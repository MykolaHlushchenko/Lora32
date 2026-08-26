#ifndef LORA32_H
#define LORA32_H

// =====================================================
// OLED
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define OLED_SDA 21
#define OLED_SCL 22

// =====================================================
// Radio pins
// =====================================================

#define RADIO_NSS   18
#define RADIO_RST   23
#define RADIO_DIO0  26
#define RADIO_BUSY  -1

// =====================================================
// LoRa configuration
// =====================================================

#define LORA_FREQUENCY 915.0
#define LORA_BANDWIDTH 500.0
#define LORA_SF        6
#define LORA_CR        5
#define LORA_SYNC_WORD 0x12
#define LORA_POWER     20

// =====================================================
// FSK configuration
// =====================================================

#define FSK_FREQUENCY  915.0
#define FSK_BITRATE    15.2
#define FSK_DEVIATION  15.2
#define FSK_BANDWIDTH  125.0
#define FSK_POWER      10
#define FSK_PREAMBLE   16

#endif
