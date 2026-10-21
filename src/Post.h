#pragma once
#include <Arduino.h>
#include <RadioLib.h>
#include <Adafruit_SSD1306.h>
#include "Config.h"
#include "Logger.h"

// Bit mask (Power-On Self-Test)
const uint8_t POST_NVS_BIT   = (1 << 0); // 0x01
const uint8_t POST_OLED_BIT  = (1 << 1); // 0x02
const uint8_t POST_LORA_BIT  = (1 << 2); // 0x04
const uint8_t POST_ADC_BIT   = (1 << 3); // 0x08

const uint8_t POST_ALL_OK = (POST_NVS_BIT | POST_OLED_BIT | POST_LORA_BIT | POST_ADC_BIT);

extern uint8_t postResultMask;

void runDevicePOST(SX1276& radio, Adafruit_SSD1306& display);

void printPostStatus();
