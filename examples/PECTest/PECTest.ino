/*
  BQ4050 PEC (Packet Error Check) Test Example
  
  This example demonstrates how to use PEC functionality for enhanced
  communication reliability with the BQ4050 battery monitoring IC.
  
  PEC provides error detection for SMBus communications by calculating
  and verifying CRC-8 checksums for each packet.
  
  Hardware Requirements:
  - Arduino (any platform supported by the library)
  - BQ4050 battery monitoring IC
  - Pull-up resistors on SDA and SCL lines (typically 4.7kΩ)
  
  Author: Andy Shinn
  Date: 2024
*/

#include <BQ4050.h>

BQ4050 battery;

void setup() {
  Serial.begin(115200);
  Serial.println("BQ4050 PEC Test Example");
  Serial.println("========================");
  
  if (!battery.begin()) {
    Serial.println("Failed to initialize BQ4050!");
    while (1) delay(1000);
  }
  
  Serial.println("BQ4050 initialized successfully");
  
  // Enable PEC for enhanced communication reliability
  battery.setPECEnabled(true);
  Serial.print("PEC enabled: ");
  Serial.println(battery.isPECEnabled() ? "Yes" : "No");
  
  // Enable debug mode to see communication details
  battery.setDebugMode(true);
  
  Serial.println();
}

void loop() {
  Serial.println("Testing basic communication with PEC enabled:");
  Serial.println("============================================");
  
  // Test basic voltage reading with PEC
  float voltage = battery.getVoltage();
  if (battery.getLastError() == BQ4050_ERROR_NONE) {
    Serial.print("✓ Voltage reading successful: ");
    Serial.print(voltage, 3);
    Serial.println(" V");
  } else {
    Serial.print("✗ Voltage reading failed: ");
    Serial.println(battery.getErrorString(battery.getLastError()));
  }
  
  // Test current reading with PEC
  float current = battery.getCurrent();
  if (battery.getLastError() == BQ4050_ERROR_NONE) {
    Serial.print("✓ Current reading successful: ");
    Serial.print(current, 3);
    Serial.println(" A");
  } else {
    Serial.print("✗ Current reading failed: ");
    Serial.println(battery.getErrorString(battery.getLastError()));
  }
  
  // Test temperature reading with PEC
  float temperature = battery.getTemperature();
  if (battery.getLastError() == BQ4050_ERROR_NONE) {
    Serial.print("✓ Temperature reading successful: ");
    Serial.print(temperature, 1);
    Serial.println(" °C");
  } else {
    Serial.print("✗ Temperature reading failed: ");
    Serial.println(battery.getErrorString(battery.getLastError()));
  }
  
  // Test State of Charge reading with PEC
  uint8_t soc = battery.getRelativeStateOfCharge();
  if (battery.getLastError() == BQ4050_ERROR_NONE) {
    Serial.print("✓ SOC reading successful: ");
    Serial.print(soc);
    Serial.println(" %");
  } else {
    Serial.print("✗ SOC reading failed: ");
    Serial.println(battery.getErrorString(battery.getLastError()));
  }
  
  Serial.println();
  
  // Test PEC calculation manually (demonstration)
  Serial.println("Manual PEC Calculation Test:");
  Serial.println("============================");
  
  uint8_t testData[] = {0x16, 0x01, 0x02, 0x03}; // Sample data
  uint8_t expectedPEC = 0x5A; // This would normally come from the device
  
  Serial.print("Test data: ");
  for (int i = 0; i < sizeof(testData); i++) {
    Serial.print("0x");
    Serial.print(testData[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  // Note: We can't directly call calculatePEC since it's private
  // This is just for demonstration of how PEC would work
  Serial.print("Expected PEC: 0x");
  Serial.println(expectedPEC, HEX);
  
  // Test with PEC disabled
  Serial.println();
  Serial.println("Testing with PEC disabled:");
  Serial.println("===========================");
  
  battery.setPECEnabled(false);
  Serial.print("PEC enabled: ");
  Serial.println(battery.isPECEnabled() ? "Yes" : "No");
  
  // Re-test voltage reading without PEC
  voltage = battery.getVoltage();
  if (battery.getLastError() == BQ4050_ERROR_NONE) {
    Serial.print("✓ Voltage reading (no PEC): ");
    Serial.print(voltage, 3);
    Serial.println(" V");
  } else {
    Serial.print("✗ Voltage reading failed: ");
    Serial.println(battery.getErrorString(battery.getLastError()));
  }
  
  // Re-enable PEC for next cycle
  battery.setPECEnabled(true);
  
  Serial.println("=====================================");
  Serial.println();
  
  delay(5000); // Wait 5 seconds before next test cycle
}