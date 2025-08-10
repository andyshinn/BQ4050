# BQ4050 Arduino Library

Arduino library for interfacing with the Texas Instruments BQ4050 Smart Battery System IC.

## Claude Code

This library was written with the help of Claude Code. I am not a C++ expert and while I have reviewed and tested the code there may be mistakes. Please open an issue if you find any problems.

## Basic Usage

```cpp
#include <BQ4050.h>

BQ4050 bq4050;

void setup() {
  Serial.begin(115200);

  if (!bq4050.begin()) {
    Serial.println("Failed to initialize BQ4050");
    return;
  }

  Serial.println("BQ4050 Ready");
}

void loop() {
  // Read basic battery information
  float voltage = bq4050.getVoltage();
  float current = bq4050.getCurrent();
  float temperature = bq4050.getTemperature();
  uint8_t soc = bq4050.getRelativeStateOfCharge();

  Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
  Serial.print("Current: "); Serial.print(current); Serial.println("A");
  Serial.print("Temperature: "); Serial.print(temperature); Serial.println("C");
  Serial.print("State of Charge: "); Serial.print(soc); Serial.println("%");

  delay(2000);
}
```

## Enhanced Features

### Formatted Output

Enable utility functions for human-readable output by adding to your build flags:
```
-DBQ4050_INCLUDE_UTILS
```

```cpp
#define BQ4050_INCLUDE_UTILS
#include <BQ4050.h>

// Now you can use formatted output
Serial.println(BQ4050Utils::formatVoltage(voltage));  // "12.345V"
Serial.println(BQ4050Utils::formatCurrent(current));  // "1.250A"
```

### Flexible Initialization

```cpp
// Default I2C
bq4050.begin();

// Custom I2C pins (ESP32)
bq4050.begin(21, 22);

// Custom frequency
bq4050.begin(21, 22, 100000);
```

### Debug Output

Enable debug output during development:
```
-DBQ4050_DEBUG
```

## Key Functions

### Battery Monitoring
- `getVoltage()` - Battery pack voltage
- `getCurrent()` - Charge/discharge current
- `getTemperature()` - Battery temperature
- `getRelativeStateOfCharge()` - State of charge percentage
- `getRemainingCapacity()` - Remaining capacity in mAh
- `getCycleCount()` - Charge cycle count

### Cell Information
- `getCellVoltage1()` through `getCellVoltage4()` - Individual cell voltages

### Status and Safety
- `getBatteryStatus()` - Battery status flags
- `getSafetyStatus()` - Safety status flags
- `isCharging()` - Charging state
- `isBatteryHealthy()` - Overall health check

### Device Information
- `getDeviceType()` - Device type identification
- `getFirmwareVersion()` - Firmware version
- `getManufacturerName()` - Manufacturer string
- `getDeviceName()` - Device name string

## Error Handling

```cpp
BQ4050_Error error = bq4050.getLastError();
if (error != BQ4050_ERROR_NONE) {
  Serial.println(BQ4050::getErrorString(error));
}
```

## License

MIT License - see [LICENSE](LICENSE) file for details.
