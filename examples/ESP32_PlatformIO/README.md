# BQ4050 ESP32 PlatformIO Example

This example demonstrates how to use the BQ4050 library with an ESP32 using PlatformIO.

## Hardware Setup

1. **ESP32 Development Board**
2. **BQ4050 Battery Monitor IC** (or development board with BQ4050)
3. **Connections:**
   - SDA: GPIO 21 (configurable in main.cpp)
   - SCL: GPIO 22 (configurable in main.cpp)
   - VCC: 3.3V
   - GND: Ground

## Software Setup

1. Install PlatformIO extension in VS Code or use PlatformIO CLI
2. Open this directory as a PlatformIO project
3. Build and upload to your ESP32

## Usage

1. Connect your ESP32 to the BQ4050 via I2C
2. Open Serial Monitor at 115200 baud
3. The example will display:
   - Device information (type, firmware, hardware versions)
   - Real-time battery data (voltage, current, temperature, SOC)
   - Cell voltages (for multi-cell packs)
   - Safety status and alerts
   - Charging/discharging status

## Configuration

- **I2C Pins:** Modify `SDA_PIN` and `SCL_PIN` in main.cpp
- **I2C Address:** Default is 0x0B (modify in BQ4050 constructor if different)
- **PEC:** Packet Error Checking is enabled for enhanced reliability
- **Debug:** Debug mode is enabled for detailed output

## Features Demonstrated

- Device identification and information
- Basic battery monitoring (voltage, current, temperature, SOC)
- Cell voltage monitoring
- Safety status monitoring
- PEC (Packet Error Check) validation
- Error handling and reporting

## Troubleshooting

- **"Failed to initialize BQ4050":** Check I2C connections and address
- **No data or errors:** Verify power supply and ground connections
- **Incorrect readings:** Ensure proper pull-up resistors on I2C lines (4.7kΩ)