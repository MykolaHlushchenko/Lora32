#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

struct DeviceConfig {
    float frequency;
    float bandwidth;
    uint8_t spreadingFactor;
    uint8_t codingRate;
    uint8_t syncWord;
};

class Config {
public:
    bool begin();
    bool save();
    bool reset();
    bool set(const char* key, const char* value);
    void print() const;

    const DeviceConfig& get() const {
        return data;
    }

    static constexpr uint8_t CURRENT_VERSION = 1;

private:
    DeviceConfig data;

    void setDefaults();
    bool migrate(uint8_t oldVersion);
};

extern Config config;

#endif
