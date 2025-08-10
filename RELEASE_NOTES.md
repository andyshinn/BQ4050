# Release Notes

## v1.0.0 (2024-12-27)

Initial release of the BQ4050 Arduino library for Texas Instruments BQ4050 Smart Battery System IC.

### Features

**Core Library**
- Full Smart Battery System (SBS) command support
- Comprehensive manufacturer access commands with sealed/unsealed mode detection
- Smart PEC (Packet Error Checking) management with automatic register-specific handling
- Robust I2C communication with enhanced timeout and error handling
- Buffer overflow protection for safe string operations

**Size Optimization**
- Conditional compilation system for size optimization
- Core library: ~4KB (raw values only)
- Full library: ~12KB with utilities (+8KB for formatted output)
- Controlled via `BQ4050_INCLUDE_UTILS` macro

**Platform Support**
- Flexible initialization methods for different platforms
- ESP32 custom pin support with frequency control
- Multiple Arduino platforms: AVR, ESP32, ESP8266, STM32, Teensy, Nordic NRF52

**Advanced Features**
- Bitfield unions for type-safe status parsing
- Enhanced debug system with Flash string support for memory efficiency
- Comprehensive error handling and reporting
- Individual cell voltage monitoring (up to 4 cells)
- Battery safety and protection status monitoring

**Developer Experience**
- Professional documentation with examples
- Multiple example sketches including PlatformIO integration
- Static code analysis configuration with cppcheck
- MIT license for commercial and personal use

### API Highlights

**Basic Monitoring**
- `getVoltage()`, `getCurrent()`, `getTemperature()`
- `getRelativeStateOfCharge()`, `getRemainingCapacity()`
- `getCellVoltage1()` through `getCellVoltage4()`

**Status and Safety**
- `getBatteryStatus()`, `getSafetyStatus()`, `getSafetyAlert()`
- `isCharging()`, `isDischarging()`, `isBatteryHealthy()`
- Bitfield unions for efficient status bit access

**Device Information**
- `getDeviceType()`, `getFirmwareVersion()`, `getHardwareVersion()`
- `getManufacturerName()`, `getDeviceName()`, `getDeviceChemistry()`
- Security mode detection and sealed/unsealed operation

**Utility Functions** (with BQ4050_INCLUDE_UTILS)
- Human-readable formatting: `formatVoltage()`, `formatCurrent()`
- Status parsing: `parseBatteryStatus()`, `parseSafetyFlags()`
- Enhanced flag descriptions and summaries

### Examples Included

- `BasicReading` - Simple battery monitoring
- `BitfieldExample` - Demonstrates status bitfield usage
- `ESP32_PlatformIO` - Full-featured PlatformIO example with conditional compilation
- `ESP32_PEC_Test` - PEC testing and validation
- `CellMonitoring` - Individual cell voltage monitoring
- `Configuration` - Advanced CEDV and FET control

### Technical Notes

- Smart PEC management automatically disables PEC for registers 0x50-0x57 due to hardware limitations
- Library supports both sealed and unsealed device modes
- Comprehensive manufacturer access command documentation with sealed mode availability
- Built with Claude Code assistance - code has been reviewed and tested but users should verify for their specific applications

### Dependencies

- Arduino Wire library (I2C communication)

### Supported Platforms

Arduino IDE and PlatformIO support for:
- Atmel AVR (Arduino Uno, Mega, etc.)
- Atmel SAM (Arduino Zero, Due, etc.)
- Espressif ESP8266
- Espressif ESP32
- Teensy
- STMicroelectronics STM32
- Nordic nRF52