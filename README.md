# LoRa32

## Version 2.0.0 — Configuration Migration

### Overview

Version 2.0.0 extends the LoRa32 configuration system by introducing the `txPower` parameter and migrating the configuration schema from version 1 to version 2.

Configuration data is stored persistently in ESP32 NVS (Non-Volatile Storage). During firmware upgrade, existing configuration values are preserved, and the new parameter is initialized with its default value.

### Configuration Parameters

Version 2 stores the following parameters:

| Parameter        | NVS Key     | Description                      |
| ---------------- | ----------- | -------------------------------- |
| Frequency        | `frequency` | LoRa operating frequency in MHz  |
| Bandwidth        | `bandwidth` | LoRa signal bandwidth in kHz     |
| Spreading Factor | `sf`        | LoRa spreading factor (SF6–SF12) |
| Coding Rate      | `cr`        | LoRa coding rate                 |
| Sync Word        | `sync`      | LoRa synchronization word        |
| Transmit Power   | `power`     | LoRa transmission power in dBm   |

The configuration version is stored under the `cfg_version` key and set to `2`.

### Features

* Persistent configuration storage using ESP32 NVS.
* Default configuration initialization when no configuration exists.
* Configuration parameter validation.
* Runtime configuration updates through Serial commands.
* Configuration reset to default values.
* Configuration display through the Serial interface.
* Firmware version and build hash reporting.
* Automatic configuration migration from version 1 to version 2.
* Preservation of existing configuration values during migration.
* Initialization of the new transmit power parameter using the default `LORA_POWER` value.

### Configuration Migration: v1 → v2

Version 2 introduces the `power` NVS key, which was not present in version 1.

During migration:

1. The firmware reads the existing configuration version.
2. If `cfg_version` is `1`, the migration procedure checks whether the `power` key exists.
3. If the key is missing, it is created using the default `LORA_POWER` value.
4. The configuration version is updated to `2` after the new parameter is saved successfully.
5. Existing configuration parameters are preserved.

The migration does not require erasing NVS or manually resetting the configuration.

### Validation

The following checks were completed:

* NVS configuration initialization: **PASS**
* OLED initialization: **PASS**
* LoRa radio initialization: **PASS**
* ADC sensor initialization: **PASS**
* Configuration migration from v1 to v2: **PASS**

POST result: `0x0F` — all four system checks passed.

The migration test confirmed that the device loaded configuration version `2`, preserved the existing LoRa parameters, and initialized the new transmit power parameter.

### Verified Configuration

```text
cfg_version: 2
frequency: 915.0 MHz
bandwidth: 125.00 kHz
sf: 6
cr: 4/5
sync: 0x12
tx_power: 20 dBm
```

### Release

* Firmware version: `2.0.0`
* Git tag: `v2.0.0`
* Configuration schema: `2`
* Previous configuration schema: `1`
