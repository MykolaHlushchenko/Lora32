#include "Config.h"
#include <Preferences.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <Lora32.h>

static Preferences prefs;

Config config;

// Set defaults config
void Config::setDefaults() {
    data.frequency = LORA_FREQUENCY;
    data.bandwidth = LORA_BANDWIDTH;
    data.spreadingFactor = LORA_SF;
    data.codingRate = LORA_CR;
    data.syncWord = LORA_SYNC_WORD;
    data.txPower = LORA_POWER;
}

// Begin
bool Config::begin() {
    if (!prefs.begin("lora_cfg", false)) {
        return false;
    }

    uint8_t version = prefs.getUChar("cfg_version", 0);

    if (version > CURRENT_VERSION) {
        return false;
    }

    // Set defaults data
    setDefaults();

    // Run migration in  NVS
    if (version > 0 && version < CURRENT_VERSION) {
        if (!migrate(version)) {
            return false;
        }
         version = CURRENT_VERSION;
    }

    if (version == 0) {
        return save();
    }

    data.frequency = prefs.getFloat("frequency", data.frequency);
    data.bandwidth = prefs.getFloat("bandwidth", data.bandwidth);
    data.spreadingFactor = prefs.getUChar("sf", data.spreadingFactor);
    data.codingRate = prefs.getUChar("cr", data.codingRate);
    data.syncWord = prefs.getUChar("sync", data.syncWord);
    data.txPower = prefs.getChar("power", data.txPower);

    // Validate data
    if (!isfinite(data.frequency) || data.frequency < 137.0f || data.frequency > 1020.0f ||
        !isfinite(data.bandwidth) || data.bandwidth < 7.8f || data.bandwidth > 500.0f ||
        data.spreadingFactor < 6 || data.spreadingFactor > 12 ||
        data.codingRate < 5 || data.codingRate > 8 ||
        data.txPower < 2 || data.txPower > 20) {

        Serial.println("[Config] Data not correct, reset to default...");
        setDefaults();
        return save();
    }

    return true;
}

// Migrate config
bool Config::migrate(uint8_t oldVersion) {
    if (oldVersion == 1) {
        Serial.println("[Config] Run migration v1 -> v2...");

        if (prefs.isKey("freq") && !prefs.isKey("frequency")) {
            prefs.putFloat("frequency", prefs.getFloat("freq", 868.0f));
            prefs.remove("freq");
        }

        if (prefs.isKey("tx_power") && !prefs.isKey("power")) {
            prefs.putChar("power", prefs.getChar("tx_power", 17));
            prefs.remove("tx_power");
        }

        // Recording new parameters that did not previously exist in v1
        if (!prefs.isKey("bandwidth")) {
            prefs.putFloat("bandwidth", 125.0f);
        }
        if (!prefs.isKey("sf")) {
            prefs.putUChar("sf", 9);
        }
        if (!prefs.isKey("cr")) {
            prefs.putUChar("cr", 5);
        }
        if (!prefs.isKey("sync")) {
            prefs.putUChar("sync", 0x12);
        }

        // Update version in NVS
        prefs.putUChar("cfg_version", CURRENT_VERSION);
        Serial.println("[Config] Migration Finished!");
        return true;
    }

    return false;
}

bool Config::save() {
    bool ok = true;

    ok &= prefs.putFloat("frequency", data.frequency) > 0;
    ok &= prefs.putFloat("bandwidth", data.bandwidth) > 0;
    ok &= prefs.putUChar("sf", data.spreadingFactor) > 0;
    ok &= prefs.putUChar("cr", data.codingRate) > 0;
    ok &= prefs.putUChar("sync", data.syncWord) > 0;
    ok &= prefs.putChar("power", data.txPower) > 0;

    if (ok) {
        ok = prefs.putUChar("cfg_version", CURRENT_VERSION) > 0;
    }

    return ok;
}

// Reset config
bool Config::reset() {
    setDefaults();
    return save();
}

// Set config
bool Config::set(const char* key, const char* value) {
    if (!key || !value || !*value) {
        return false;
    }

    char* end = nullptr;
    float number = strtof(value, &end);

    if (end == value || *end != '\0' || !isfinite(number)) {
        return false;
    }

    DeviceConfig old = data;

    if (strcmp(key, "frequency") == 0) {
        if (number < 137.0f || number > 1020.0f) return false;
        data.frequency = number;
    } else if (strcmp(key, "bandwidth") == 0) {
        if (number != 7.8f && number != 10.4f &&
            number != 15.6f && number != 20.8f &&
            number != 31.25f && number != 41.7f &&
            number != 62.5f && number != 125.0f &&
            number != 250.0f && number != 500.0f) {
            return false;
        }
        data.bandwidth = number;
    } else if (strcmp(key, "sf") == 0) {
        if (number < 6 || number > 12 || floorf(number) != number)
            return false;
        data.spreadingFactor = static_cast<uint8_t>(number);
    } else if (strcmp(key, "cr") == 0) {
        if (number < 5 || number > 8 || floorf(number) != number)
            return false;
        data.codingRate = static_cast<uint8_t>(number);
    } else if (strcmp(key, "sync") == 0) {
        if (number < 0 || number > 255 || floorf(number) != number)
            return false;
        data.syncWord = static_cast<uint8_t>(number);
    } else if (strcmp(key, "tx_power") == 0 || strcmp(key, "power") == 0) {
        if (number < 2 || number > 20 || floorf(number) != number)
            return false;
        data.txPower = static_cast<int8_t>(number);
    } else {
        return false;
    }

    if (!save()) {
        data = old;
        return false;
    }

    return true;
}

// Print current version
void Config::print() const {
    Serial.println("--- Current configuration ---");
    Serial.printf("cfg_version: %u\n", CURRENT_VERSION);
    Serial.printf("frequency: %.1f MHz\n", data.frequency);
    Serial.printf("bandwidth: %.2f kHz\n", data.bandwidth);
    Serial.printf("sf: %u\n", data.spreadingFactor);
    Serial.printf("cr: 4/%u\n", data.codingRate);
    Serial.printf("sync: 0x%02X\n", data.syncWord);
    Serial.printf("tx_power: %d dBm\n", data.txPower);
}
