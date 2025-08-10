# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an Arduino library for the Texas Instruments BQ4050 Smart Battery System IC. The library provides I2C/SMBus communication with comprehensive battery monitoring, safety status parsing, and device control functions.

## Code Architecture

### Core Library Structure
- **BQ4050.h/cpp**: Main library class with I2C communication, SBS commands, manufacturer access commands, and PEC support
- **BQ4050Utils.h/cpp**: Optional utility functions for human-readable output, status parsing, and bitfield unions (conditionally compiled)

### Size-Optimized Design
The library uses conditional compilation for size optimization:
- **Default**: ~4KB core functionality with raw values only
- **With Utils**: ~12KB total with formatted output and parsing functions
- Control via `BQ4050_INCLUDE_UTILS` macro

### Communication Patterns
- **Smart PEC Management**: Automatic PEC (Packet Error Checking) enable/disable based on register compatibility
- **Register-Specific Logic**: 0x50-0x57 registers don't support PEC; library handles this automatically
- **Sealed Mode Support**: Most read commands work in sealed mode; manufacturer access commands may require unsealed mode

### Key Features
1. **Flexible Initialization**: Multiple `begin()` overloads for different platforms (ESP32 pins, custom I2C frequency)
2. **Enhanced Debug System**: Memory-efficient debug macros with Flash string support
3. **Bitfield Unions**: Type-safe status parsing with individual bit access
4. **Buffer Protection**: Comprehensive overflow protection for string reads
5. **Robust I2C**: Enhanced timeout handling and error reporting

## Build and Development

### Arduino IDE
- Standard Arduino library installation in `libraries/` folder
- Include: `#include <BQ4050.h>`
- Optional utils: `#define BQ4050_INCLUDE_UTILS` before include

### PlatformIO
```ini
; Basic usage
lib_deps = BQ4050=file://path/to/library

; Enable utilities (adds ~8KB)
build_flags = -DBQ4050_INCLUDE_UTILS

; Enable debug output
build_flags = -DBQ4050_DEBUG
```

### Code Quality
```bash
# Static analysis with suppressions
cppcheck --enable=all --inconclusive --check-level=exhaustive --suppressions-list=cppcheck-suppressions --std=c++11 --platform=avr8 src
```

## Configuration Macros

### BQ4050_INCLUDE_UTILS
- **Purpose**: Include utility functions for formatted output and status parsing
- **Default**: Undefined (core-only, ~4KB)
- **When enabled**: Full utilities (~12KB total, +8KB)
- **Usage**: Define in platformio.ini build_flags or before #include

### BQ4050_DEBUG
- **Purpose**: Enable comprehensive debug output with memory-efficient Flash strings
- **Macros**: `BQ4050_DEBUG_PRINT()`, `BQ4050_DEBUG_PRINTF()`, `BQ4050_DEBUG_HEX()`
- **Usage**: Define in build_flags for development builds

## Key Implementation Details

### PEC (Packet Error Checking)
- Automatically managed per register compatibility
- Registers 0x50-0x57 have PEC disabled due to hardware limitations
- Smart PEC macro handles enable/disable transparently

### Bitfield Unions
```cpp
// Efficient status parsing
BatteryStatusBits statusBits = BQ4050Utils::parseBatteryStatusBits(raw);
if (statusBits.bits.oca) { /* Over Charge Alarm active */ }
```

### Error Handling
- All functions set `_lastError` for debugging
- Use `getLastError()` and `getErrorString()` for diagnostics
- I2C timeouts and NACK conditions properly handled

### Security Modes
- **Sealed**: Most read commands available, limited write access
- **Unsealed**: Full access to manufacturer commands
- Library detects mode automatically and adjusts available functions

## Examples Structure

- **BasicReading**: Simple voltage/current/temperature monitoring
- **BitfieldExample**: Demonstrates status bitfield union usage  
- **ESP32_PlatformIO**: Full-featured example with conditional compilation
- **ESP32_PEC_Test**: Isolated PEC testing for troubleshooting
- **Configuration**: Advanced CEDV and FET control examples

## Testing Commands

The library includes comprehensive command testing:
- Standard SBS commands (0x01-0x5F) 
- Manufacturer access commands (0x0001-0x007A)
- PEC compatibility testing per register
- All examples include error checking and reporting

## Important Notes

- Library was developed with Claude Code assistance - review code carefully
- Uses Wire library dependency (Arduino I2C)
- Supports multiple platforms: AVR, ESP32, ESP8266, STM32, Teensy, Nordic
- MIT licensed for commercial and personal use
- The cppcheck command is `cppcheck --enable=all --inconclusive --check-level=exhaustive --suppressions-list=cppcheck-suppressions --std=c++11 --platform=avr8 src`