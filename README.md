# Lora32
## Version 1.0.0 — Initial Configuration Version

### Overview

The first version of the LoRa32 configuration system introduces persistent storage of device configuration using ESP32 NVS (Non-Volatile Storage).

### Configuration Parameters

Version 1 stores the following parameters:

| Parameter        | NVS Key     | Description                      |
| ---------------- | ----------- | -------------------------------- |
| Frequency        | `frequency` | LoRa operating frequency in MHz  |
| Bandwidth        | `bandwidth` | LoRa signal bandwidth in kHz     |
| Spreading Factor | `sf`        | LoRa spreading factor (SF6–SF12) |
| Coding Rate      | `cr`        | LoRa coding rate                 |
| Sync Word        | `sync`      | LoRa synchronization word        |

The configuration version is stored under the `cfg_version` key and set to `1`.

### Features

* Persistent configuration storage using ESP32 NVS.
* Default configuration initialization when no configuration exists.
* Configuration parameter validation.
* Runtime configuration updates.
* Configuration reset to default values.
* Serial commands for displaying and modifying configuration.
* Power and other parameters can be added in future configuration versions.

### Validation

The following checks were completed:

* NVS configuration initialization: **PASS**
* OLED initialization: **PASS**
* LoRa radio initialization: **PASS**
* ADC sensor initialization: **PASS**

POST result: `0x0F` — all four checks passed.

### Migration

Version 1 is the baseline for future configuration migrations. Version 2 will introduce the `txPower` parameter while preserving existing configuration values.

### Release

* Git tag: `v1.0.0`
* Configuration schema: `1`


