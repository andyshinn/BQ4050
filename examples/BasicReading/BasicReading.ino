/*
  BQ4050 Basic Reading Example
  
  This example demonstrates basic battery monitoring using the BQ4050 library.
  It reads and displays essential battery parameters like voltage, current, 
  temperature, and state of charge.
  
  Hardware Requirements:
  - Arduino (any platform supported by the library)
  - BQ4050 battery monitoring IC connected via I2C
  - Pull-up resistors on SDA and SCL lines (typically 4.7kΩ)
  
  Circuit:
  - VCC to 3.3V or 5V (depending on your Arduino)
  - GND to GND
  - SDA to A4 (Uno) or SDA pin (other boards)
  - SCL to A5 (Uno) or SCL pin (other boards)
  
  Author: Andy Shinn
  Date: 2024
*/

#include <BQ4050.h>

// Create BQ4050 instance with default I2C address (0x0B)
BQ4050 battery;

void setup() {
  Serial.begin(115200);
  Serial.println("BQ4050 Basic Reading Example");
  Serial.println("============================");
  
  // Initialize the library
  if (!battery.begin()) {
    Serial.println("Failed to initialize BQ4050!");
    Serial.println("Check connections and I2C address.");
    while (1) {
      delay(1000);
    }
  }
  
  Serial.println("BQ4050 initialized successfully");
  
  // Print device information
  Serial.println("\nDevice Information:");
  Serial.print("Device Type: 0x");
  Serial.println(battery.getDeviceType(), HEX);
  Serial.print("Firmware Version: 0x");
  Serial.println(battery.getFirmwareVersion(), HEX);
  Serial.print("Hardware Version: 0x");
  Serial.println(battery.getHardwareVersion(), HEX);
  Serial.println();
}

void loop() {
  Serial.println("Battery Status:");
  Serial.println("==============");
  
  // Read basic battery parameters
  float voltage = battery.getVoltage();
  float current = battery.getCurrent();
  float temperature = battery.getTemperature();
  int soc = battery.getRelativeStateOfCharge();
  
  // Print results
  Serial.print("Voltage: ");
  Serial.print(voltage, 3);
  Serial.println(" V");
  
  Serial.print("Current: ");
  Serial.print(current, 3);
  Serial.println(" A");
  
  Serial.print("Temperature: ");
  Serial.print(temperature, 1);
  Serial.println(" °C");
  
  Serial.print("State of Charge: ");
  Serial.print(soc);
  Serial.println(" %");
  
  // Read capacity information
  int remainingCapacity = battery.getRemainingCapacity();
  int fullCapacity = battery.getFullChargeCapacity();
  int cycleCount = battery.getCycleCount();
  
  Serial.print("Remaining Capacity: ");
  Serial.print(remainingCapacity);
  Serial.println(" mAh");
  
  Serial.print("Full Charge Capacity: ");
  Serial.print(fullCapacity);
  Serial.println(" mAh");
  
  Serial.print("Cycle Count: ");
  Serial.println(cycleCount);
  
  // Check charging status
  if (battery.isCharging()) {
    Serial.println("Status: CHARGING");
  } else if (battery.isDischarging()) {
    Serial.println("Status: DISCHARGING");
  } else {
    Serial.println("Status: IDLE");
  }
  
  // Check battery health
  if (battery.isBatteryHealthy()) {
    Serial.println("Health: GOOD");
  } else {
    Serial.println("Health: WARNING - Check safety status!");
  }
  
  Serial.println("==============\n");
  
  delay(2000); // Update every 2 seconds
}