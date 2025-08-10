# BQ4050 Utils Conditional Compilation

The BQ4050 library supports optional inclusion of utility functions. By default, only core functionality is included to minimize code size. Utils can be optionally enabled for human-readable formatting.

## How to Control Utils Inclusion

### Exclude Utils (Default - Size Optimized)
```cpp
// By default, BQ4050_INCLUDE_UTILS is not defined
#include <BQ4050.h>
// Only core BQ4050 functionality, raw values only
```

### Include Utils (Optional)
```cpp
// Method 1: In platformio.ini (Recommended)
build_flags = -DBQ4050_INCLUDE_UTILS

// Method 2: Before including header
#define BQ4050_INCLUDE_UTILS
#include <BQ4050.h>
```

## Code Size Impact

**Without Utils (Default):**
- Core BQ4050 functionality only
- Raw numeric values (no formatting)
- No string processing overhead  
- ~4KB base code size

**With Utils (Optional):**
- Full BQ4050Utils.h functionality
- Human-readable status parsing
- Formatted output functions
- Safety flag descriptions
- ~12KB total code size (+8KB)

## Example Usage Patterns

### With Utils Disabled (Default)
```cpp
// No need to define anything - this is the default
#include <BQ4050.h>

void loop() {
  float voltage = bq4050.getVoltage();
  Serial.println(voltage); // Raw float: 12.345
  
  uint16_t status = bq4050.getBatteryStatus();
  Serial.println(status); // Raw value: 128 (format manually if needed)
}
```

### With Utils Enabled
```cpp
// In platformio.ini: build_flags = -DBQ4050_INCLUDE_UTILS
#include <BQ4050.h>

void loop() {
  float voltage = bq4050.getVoltage();
  Serial.println(BQ4050Utils::formatVoltage(voltage)); // "12.345V"
  
  uint16_t status = bq4050.getBatteryStatus();
  Serial.println(BQ4050Utils::parseBatteryStatus(status)); // "Battery OK"
}
```

## Automatic Example Adaptation

The ESP32 example automatically adapts based on the macro setting:
- Uses formatting macros that expand to either BQ4050Utils calls or raw values
- Complex status parsing is conditionally compiled  
- Raw mode outputs numeric values for maximum efficiency
- Formatted mode provides human-readable strings

## When to Use Each Mode

**Use Without Utils (Default) When:**
- Memory-constrained embedded systems
- Production firmware where size matters
- Simple monitoring applications
- Custom formatting is preferred
- Minimal code footprint is required

**Use With Utils When:**
- Development and debugging
- User-facing applications needing readable output  
- Diagnostic tools
- Code size is not critical
- Human-readable output is desired