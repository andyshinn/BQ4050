/*
  BQ4050 CEDV (Compensated End-of-Discharge Voltage) Monitoring Example
  
  This example demonstrates how to monitor CEDV status and thresholds.
  CEDV is an advanced feature that provides more accurate SOC estimation
  by dynamically adjusting end-of-discharge voltage thresholds based on
  battery conditions.
  
  Hardware Requirements:
  - Arduino (any platform supported by the library)
  - BQ4050 battery monitoring IC with CEDV enabled firmware
  - Pull-up resistors on SDA and SCL lines (typically 4.7kΩ)
  
  Author: Andy Shinn
  Date: 2024
*/

#include <BQ4050.h>

BQ4050 battery;

void setup() {
  Serial.begin(115200);
  Serial.println("BQ4050 CEDV Monitoring Example");
  Serial.println("===============================");
  
  if (!battery.begin()) {
    Serial.println("Failed to initialize BQ4050!");
    while (1) delay(1000);
  }
  
  Serial.println("BQ4050 initialized successfully");
  
  // Check if CEDV compensation is enabled
  if (battery.isEDVCompensationEnabled()) {
    Serial.println("CEDV compensation is ENABLED");
  } else {
    Serial.println("CEDV compensation is DISABLED (using fixed thresholds)");
  }
  
  Serial.println();
}

void loop() {
  // Get complete CEDV information using convenience method
  CEDVInfo cedvInfo = battery.getCompleteCEDVInfo();
  
  Serial.println("CEDV Status and Information:");
  Serial.println("============================");
  
  // Display EDV threshold status
  Serial.println("EDV Threshold Status:");
  if (cedvInfo.status.edv0Reached) {
    Serial.println("  🔴 EDV0 (0% SOC) - REACHED");
  } else {
    Serial.println("  ⚫ EDV0 (0% SOC) - Not reached");
  }
  
  if (cedvInfo.status.edv1Reached) {
    Serial.println("  🟠 EDV1 (3% SOC) - REACHED");
  } else {
    Serial.println("  ⚫ EDV1 (3% SOC) - Not reached");
  }
  
  if (cedvInfo.status.edv2Reached) {
    Serial.println("  🟡 EDV2 (Battery Low) - REACHED");
  } else {
    Serial.println("  ⚫ EDV2 (Battery Low) - Not reached");
  }
  
  Serial.println();
  
  // Display current EDV thresholds
  Serial.println("Current EDV Thresholds:");
  Serial.print("  EDV0: ");
  Serial.print(cedvInfo.currentEDV0, 3);
  Serial.println(" V");
  Serial.print("  EDV1: ");
  Serial.print(cedvInfo.currentEDV1, 3);
  Serial.println(" V");
  Serial.print("  EDV2: ");
  Serial.print(cedvInfo.currentEDV2, 3);
  Serial.println(" V");
  
  Serial.println();
  
  // Display CEDV operation status
  Serial.println("CEDV Operation Status:");
  Serial.print("  Compensation: ");
  Serial.println(cedvInfo.compensationEnabled ? "ENABLED" : "DISABLED");
  Serial.print("  Smoothing: ");
  Serial.println(cedvInfo.smoothingActive ? "ACTIVE" : "INACTIVE");
  Serial.print("  FCC Updated: ");
  Serial.println(cedvInfo.status.fccUpdated ? "YES" : "NO");
  Serial.print("  Qualified Discharge: ");
  Serial.println(cedvInfo.status.qualifiedDischarge ? "YES" : "NO");
  Serial.print("  VDQ (Voltage Delta Qualified): ");
  Serial.println(cedvInfo.status.vdq ? "YES" : "NO");
  
  Serial.println();
  
  // Display remaining capacity to each threshold
  Serial.println("Estimated Capacity Remaining to Thresholds:");
  Serial.print("  To EDV2: ");
  Serial.print(cedvInfo.remainingToEDV2);
  Serial.println(" mAh");
  Serial.print("  To EDV1: ");
  Serial.print(cedvInfo.remainingToEDV1);
  Serial.println(" mAh");
  Serial.print("  To EDV0: ");
  Serial.print(cedvInfo.remainingToEDV0);
  Serial.println(" mAh");
  
  Serial.println();
  
  // Display learning status
  Serial.println("CEDV Learning Status:");
  if (battery.isLearningDischarge()) {
    Serial.println("  📈 Currently in learning discharge");
  } else {
    Serial.println("  📊 Not currently learning");
  }
  
  uint16_t qualifiedCount = battery.getQualifiedDischargeCount();
  Serial.print("  Qualified discharge cycles: ");
  Serial.println(qualifiedCount);
  
  Serial.println();
  
  // Display current battery state for context
  Serial.println("Current Battery State (for context):");
  float voltage = battery.getVoltage();
  float current = battery.getCurrent();
  int soc = battery.getRelativeStateOfCharge();
  
  Serial.print("  Voltage: ");
  Serial.print(voltage, 3);
  Serial.println(" V");
  Serial.print("  Current: ");
  Serial.print(current, 3);
  Serial.println(" A");
  Serial.print("  SOC: ");
  Serial.print(soc);
  Serial.println(" %");
  
  // Warning messages based on EDV status
  if (cedvInfo.status.edv0Reached) {
    Serial.println("🚨 CRITICAL: Battery at 0% SOC - Shutdown imminent!");
  } else if (cedvInfo.status.edv1Reached) {
    Serial.println("⚠️  WARNING: Battery at 3% SOC - Low power mode recommended");
  } else if (cedvInfo.status.edv2Reached) {
    Serial.println("⚡ NOTICE: Battery Low threshold reached");
  }
  
  Serial.println("============================\n");
  
  delay(5000); // Update every 5 seconds
}