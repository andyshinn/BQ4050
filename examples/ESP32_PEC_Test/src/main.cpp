/*
 * BQ4050 ESP32 PEC (Packet Error Checking) Test Example
 * 
 * This example tests PEC functionality with all BQ4050 commands
 * to identify which commands support PEC and which ones fail.
 * 
 * This is a minimal example without utility functions - just raw
 * command testing to isolate PEC behavior patterns.
 */

#include <Arduino.h>
#include <Wire.h>
#include <BQ4050.h>

// I2C pins for ESP32
#define SDA_PIN 21
#define SCL_PIN 22

// BQ4050 instance
BQ4050 bq4050;

// Structure to track command results
struct CommandTest {
  String name;
  bool pecSuccess;
  bool executed;
  String errorMsg;
};

// Array to store test results
CommandTest commands[] = {
  {"getVoltage()", false, false, ""},
  {"getCurrent()", false, false, ""},
  {"getTemperature()", false, false, ""},
  {"getRelativeStateOfCharge()", false, false, ""},
  {"getAbsoluteStateOfCharge()", false, false, ""},
  {"getRemainingCapacity()", false, false, ""},
  {"getFullChargeCapacity()", false, false, ""},
  {"getDesignCapacity()", false, false, ""},
  {"getDesignVoltage()", false, false, ""},
  {"getBatteryStatus()", false, false, ""},
  {"getCycleCount()", false, false, ""},
  {"getManufacturerDate()", false, false, ""},
  {"getSerialNumber()", false, false, ""},
  {"getCellVoltage1()", false, false, ""},
  {"getCellVoltage2()", false, false, ""},
  {"getCellVoltage3()", false, false, ""},
  {"getCellVoltage4()", false, false, ""},
  {"getSafetyAlert()", false, false, ""},
  {"getSafetyStatus()", false, false, ""},
  {"getPFAlert()", false, false, ""},
  {"getPFStatus()", false, false, ""},
  {"getOperationStatus()", false, false, ""},
  {"getChargingStatus()", false, false, ""},
  {"getGaugingStatus()", false, false, ""},
  {"getManufacturingStatus()", false, false, ""},
  {"getDeviceType()", false, false, ""},
  {"getFirmwareVersion()", false, false, ""},
  {"getHardwareVersion()", false, false, ""},
  {"getManufacturerName()", false, false, ""},
  {"getDeviceName()", false, false, ""},
  {"getDeviceChemistry()", false, false, ""},
  // Extended SBS Commands
  {"getAFERegister()", false, false, ""},
  {"getLifeTimeDataBlock1()", false, false, ""},
  {"getLifeTimeDataBlock2()", false, false, ""},
  {"getLifeTimeDataBlock3()", false, false, ""},
  {"getLifeTimeDataBlock4()", false, false, ""},
  {"getLifeTimeDataBlock5()", false, false, ""},
  {"getManufacturerInfo()", false, false, ""},
  {"getDAStatus1()", false, false, ""},
  {"getDAStatus2()", false, false, ""}
};

const int numCommands = sizeof(commands) / sizeof(commands[0]);

void testCommand(int index) {
  commands[index].executed = true;
  BQ4050_Error error = BQ4050_ERROR_NONE;
  
  // Execute the specific command based on index
  switch (index) {
    case 0: bq4050.getVoltage(); break;
    case 1: bq4050.getCurrent(); break;
    case 2: bq4050.getTemperature(); break;
    case 3: bq4050.getRelativeStateOfCharge(); break;
    case 4: bq4050.getAbsoluteStateOfCharge(); break;
    case 5: bq4050.getRemainingCapacity(); break;
    case 6: bq4050.getFullChargeCapacity(); break;
    case 7: bq4050.getDesignCapacity(); break;
    case 8: bq4050.getDesignVoltage(); break;
    case 9: bq4050.getBatteryStatus(); break;
    case 10: bq4050.getCycleCount(); break;
    case 11: bq4050.getManufacturerDate(); break;
    case 12: bq4050.getSerialNumber(); break;
    case 13: bq4050.getCellVoltage1(); break;
    case 14: bq4050.getCellVoltage2(); break;
    case 15: bq4050.getCellVoltage3(); break;
    case 16: bq4050.getCellVoltage4(); break;
    case 17: bq4050.getSafetyAlert(); break;
    case 18: bq4050.getSafetyStatus(); break;
    case 19: bq4050.getPFAlert(); break;
    case 20: bq4050.getPFStatus(); break;
    case 21: bq4050.getOperationStatus(); break;
    case 22: bq4050.getChargingStatus(); break;
    case 23: bq4050.getGaugingStatus(); break;
    case 24: bq4050.getManufacturingStatus(); break;
    case 25: bq4050.getDeviceType(); break;
    case 26: bq4050.getFirmwareVersion(); break;
    case 27: bq4050.getHardwareVersion(); break;
    case 28: bq4050.getManufacturerName(); break;
    case 29: bq4050.getDeviceName(); break;
    case 30: bq4050.getDeviceChemistry(); break;
    // Extended SBS Commands
    case 31: bq4050.getAFERegister(); break;
    case 32: bq4050.getLifeTimeDataBlock1(); break;
    case 33: bq4050.getLifeTimeDataBlock2(); break;
    case 34: bq4050.getLifeTimeDataBlock3(); break;
    case 35: bq4050.getLifeTimeDataBlock4(); break;
    case 36: bq4050.getLifeTimeDataBlock5(); break;
    case 37: bq4050.getManufacturerInfo(); break;
    case 38: bq4050.getDAStatus1(); break;
    case 39: bq4050.getDAStatus2(); break;
    default: return;
  }
  
  error = bq4050.getLastError();
  
  if (error == BQ4050_ERROR_NONE) {
    commands[index].pecSuccess = true;
    commands[index].errorMsg = "OK";
  } else {
    commands[index].pecSuccess = false;
    commands[index].errorMsg = BQ4050::getErrorString(error);
  }
  
  delay(10); // Small delay between commands
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("BQ4050 ESP32 PEC Test");
  Serial.println("====================");
  
  // Initialize I2C
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(50000);  // 50kHz for reliable communication
  Wire.setTimeout(1000);
  
  // Check if BQ4050 is present
  Serial.print("Scanning for BQ4050 at address 0x0B... ");
  Wire.beginTransmission(0x0B);
  if (Wire.endTransmission() == 0) {
    Serial.println("Found!");
  } else {
    Serial.println("Not found!");
    Serial.println("Check wiring and I2C address");
    return;
  }
  
  // Initialize BQ4050 with PEC disabled first
  bq4050.setPECEnabled(false);
  if (!bq4050.begin()) {
    Serial.println("ERROR: Failed to initialize BQ4050!");
    return;
  }
  
  Serial.println("BQ4050 initialized successfully!");
  Serial.println();
  
  // Test all commands WITHOUT PEC first
  Serial.println("=== Testing Commands WITHOUT PEC ===");
  bq4050.setPECEnabled(false);
  
  int noPecSuccess = 0;
  for (int i = 0; i < numCommands; i++) {
    testCommand(i);
    if (commands[i].pecSuccess) {
      noPecSuccess++;
    }
    Serial.print(commands[i].name);
    Serial.print(": ");
    Serial.println(commands[i].errorMsg);
  }
  
  Serial.print("Without PEC: ");
  Serial.print(noPecSuccess);
  Serial.print("/");
  Serial.print(numCommands);
  Serial.println(" commands successful");
  Serial.println();
  
  // Reset command results for PEC test
  for (int i = 0; i < numCommands; i++) {
    commands[i].pecSuccess = false;
    commands[i].executed = false;
    commands[i].errorMsg = "";
  }
  
  // Test all commands WITH PEC enabled
  Serial.println("=== Testing Commands WITH PEC ===");
  bq4050.setPECEnabled(true);
  
  int pecSuccess = 0;
  int pecMismatch = 0;
  int otherErrors = 0;
  
  for (int i = 0; i < numCommands; i++) {
    testCommand(i);
    
    BQ4050_Error lastError = bq4050.getLastError();
    if (lastError == BQ4050_ERROR_NONE) {
      pecSuccess++;
      Serial.print("✓ ");
    } else if (lastError == BQ4050_ERROR_PEC_MISMATCH) {
      pecMismatch++;
      Serial.print("⚠ ");
    } else {
      otherErrors++;
      Serial.print("✗ ");
    }
    
    Serial.print(commands[i].name);
    Serial.print(": ");
    Serial.println(commands[i].errorMsg);
  }
  
  Serial.println();
  Serial.println("=== PEC Test Summary ===");
  Serial.print("✓ PEC Success: ");
  Serial.print(pecSuccess);
  Serial.print("/");
  Serial.println(numCommands);
  
  Serial.print("⚠ PEC Mismatch: ");
  Serial.print(pecMismatch);
  Serial.print("/");
  Serial.println(numCommands);
  
  Serial.print("✗ Other Errors: ");
  Serial.print(otherErrors);
  Serial.print("/");
  Serial.println(numCommands);
  
  float pecSuccessRate = (float)pecSuccess / numCommands * 100.0;
  Serial.print("PEC Success Rate: ");
  Serial.print(pecSuccessRate, 1);
  Serial.println("%");
  
  Serial.println();
  if (pecSuccessRate >= 90.0) {
    Serial.println("→ Excellent PEC support - device fully compatible");
  } else if (pecSuccessRate >= 50.0) {
    Serial.println("→ Partial PEC support - some commands work with PEC");
  } else if (pecSuccessRate > 0.0) {
    Serial.println("→ Limited PEC support - only few commands work with PEC");
  } else {
    Serial.println("→ No PEC support detected - use without PEC for compatibility");
  }
  
  // List commands that work with PEC
  if (pecSuccess > 0) {
    Serial.println();
    Serial.println("Commands that work WITH PEC:");
    for (int i = 0; i < numCommands; i++) {
      if (commands[i].pecSuccess) {
        Serial.print("  ✓ ");
        Serial.println(commands[i].name);
      }
    }
  }
  
  // List commands that fail with PEC mismatch
  if (pecMismatch > 0) {
    Serial.println();
    Serial.println("Commands that fail with PEC MISMATCH:");
    for (int i = 0; i < numCommands; i++) {
      if (commands[i].executed && !commands[i].pecSuccess && 
          commands[i].errorMsg.indexOf("PEC") >= 0) {
        Serial.print("  ⚠ ");
        Serial.println(commands[i].name);
      }
    }
  }
  
  Serial.println();
  Serial.println("Test complete! Results show which commands support PEC.");
}

void loop() {
  // This example runs once in setup() - no continuous loop needed
  delay(10000);
}