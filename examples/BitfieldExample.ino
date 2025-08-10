/*
 * BQ4050 Bitfield Union Example
 * 
 * This example demonstrates how to use the bitfield unions for efficient
 * status parsing and individual bit access.
 * 
 * Requires: BQ4050_INCLUDE_UTILS to be defined for bitfield functions
 */

#define BQ4050_INCLUDE_UTILS  // Enable utility functions including bitfields
#include <BQ4050.h>

BQ4050 bq4050;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  if (!bq4050.begin()) {
    Serial.println("Failed to initialize BQ4050!");
    return;
  }
  
  Serial.println("BQ4050 Bitfield Union Example");
  Serial.println("==============================");
}

void loop() {
  // Read battery status using traditional method
  uint16_t batteryStatusRaw = bq4050.getBatteryStatus();
  Serial.print("Battery Status (raw): 0x");
  Serial.println(batteryStatusRaw, HEX);
  
  // Parse using bitfield union
  BatteryStatusBits batteryBits = BQ4050Utils::parseBatteryStatusBits(batteryStatusRaw);
  
  // Access individual bits easily
  Serial.println("Battery Status Analysis:");
  if (batteryBits.bits.oca) Serial.println("  ⚠ Over Charged Alarm");
  if (batteryBits.bits.tca) Serial.println("  ⚠ Terminate Charge Alarm");
  if (batteryBits.bits.ota) Serial.println("  ⚠ Over Temperature Alarm");
  if (batteryBits.bits.tda) Serial.println("  ⚠ Terminate Discharge Alarm");
  if (batteryBits.bits.rca) Serial.println("  ⚠ Remaining Capacity Alarm");
  if (batteryBits.bits.rta) Serial.println("  ⚠ Remaining Time Alarm");
  
  if (batteryStatusRaw == 0) {
    Serial.println("  ✓ All battery status flags are clear");
  }
  
  // Get formatted description
  Serial.print("Battery Status Summary: ");
  Serial.println(BQ4050Utils::describeBatteryStatusBits(batteryBits));
  
  // Demonstrate safety status bitfield
  uint16_t safetyStatusRaw = bq4050.getSafetyStatus();
  if (safetyStatusRaw != 0) {
    Serial.print("Safety Status (raw): 0x");
    Serial.println(safetyStatusRaw, HEX);
    
    // Convert to 32-bit for full status and parse with bitfield
    uint32_t extendedSafety = (uint32_t)safetyStatusRaw;
    SafetyStatusBits safetyBits = BQ4050Utils::parseSafetyStatusBits(extendedSafety);
    
    Serial.println("Safety Status Analysis:");
    if (safetyBits.bits.cuv) Serial.println("  ⚠ Cell Undervoltage");
    if (safetyBits.bits.cov) Serial.println("  ⚠ Cell Overvoltage");
    if (safetyBits.bits.occ1) Serial.println("  ⚠ Overcurrent Charge 1");
    if (safetyBits.bits.ocd1) Serial.println("  ⚠ Overcurrent Discharge 1");
    if (safetyBits.bits.otc) Serial.println("  ⚠ Overtemperature Charge");
    if (safetyBits.bits.otd) Serial.println("  ⚠ Overtemperature Discharge");
    
    Serial.print("Safety Status Summary: ");
    Serial.println(BQ4050Utils::describeSafetyStatusBits(safetyBits));
  }
  
  Serial.println("--------------------------------");
  delay(5000);
}