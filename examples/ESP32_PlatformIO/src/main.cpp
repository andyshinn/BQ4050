/*
 * BQ4050 ESP32 PlatformIO Example with Shared I2C Bus Support
 * 
 * This example demonstrates reliable communication with the BQ4050 battery gauge
 * when the I2C bus is shared with other devices or masters.
 * 
 * Key improvements made for shared I2C environments:
 * 1. Disabled PEC (Packet Error Checking) initially to avoid communication issues
 * 2. Reduced I2C clock speed to 50kHz for more reliable communication
 * 3. Added I2C timeout configuration
 * 4. Added device presence check before initialization
 * 5. Added small delays between I2C transactions to prevent bus conflicts
 * 6. Added comprehensive error reporting for failed reads
 * 7. Added human-readable decoding for device type, firmware, and hardware versions
 */

#include <Arduino.h>
#include <Wire.h>
#include <BQ4050.h>
// BQ4050Utils.h is conditionally included by BQ4050.h based on BQ4050_INCLUDE_UTILS

// Utility macros for conditional formatting vs raw values
#ifdef BQ4050_INCLUDE_UTILS
  #define FORMAT_VOLTAGE(v) BQ4050Utils::formatVoltage(v)
  #define FORMAT_CURRENT(c) BQ4050Utils::formatCurrent(c)
  #define FORMAT_TEMPERATURE(t) BQ4050Utils::formatTemperature(t)
  #define FORMAT_PERCENTAGE(p) BQ4050Utils::formatPercentage(p)
  #define FORMAT_CAPACITY(c) BQ4050Utils::formatCapacity(c)
  #define FORMAT_CYCLE_COUNT(c) BQ4050Utils::formatCycleCount(c)
  #define FORMAT_MFG_DATE(d) BQ4050Utils::formatManufacturerDate(d)
  #define FORMAT_SERIAL(s) BQ4050Utils::formatSerialNumber(s)
  #define PARSE_BATTERY_STATUS(s) BQ4050Utils::parseBatteryStatus(s)
  #define GET_HEALTH_SUMMARY(ss, sa, bs) BQ4050Utils::getBatteryHealthSummary(ss, sa, bs)
  #define GET_SECURITY_DESC(s) BQ4050Utils::getSecurityModeDescription(s)
  #define IS_VALID_CYCLE_COUNT(c) BQ4050Utils::isValidCycleCount(c)
#else
  #define FORMAT_VOLTAGE(v) (v)
  #define FORMAT_CURRENT(c) (c)
  #define FORMAT_TEMPERATURE(t) (t)
  #define FORMAT_PERCENTAGE(p) (p)
  #define FORMAT_CAPACITY(c) (c)
  #define FORMAT_CYCLE_COUNT(c) (c)
  #define FORMAT_MFG_DATE(d) (d)
  #define FORMAT_SERIAL(s) (s)
  #define PARSE_BATTERY_STATUS(s) (s)
  #define GET_HEALTH_SUMMARY(ss, sa, bs) (ss)
  #define GET_SECURITY_DESC(s) (s)
  #define IS_VALID_CYCLE_COUNT(c) (c != 0xFFFF)
#endif

// I2C pins for ESP32 (can be customized)
#define SDA_PIN 21
#define SCL_PIN 22

// BQ4050 instance
BQ4050 bq4050;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("BQ4050 ESP32 PlatformIO Test");
  Serial.println("=============================");
  
  // Initialize I2C with custom pins
  // Note: If I2C is already initialized by another master, this will show a warning but still work
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(50000);  // Use slower 50kHz for more reliable communication
  Wire.setTimeout(1000); // 1 second timeout
  
  // Check if BQ4050 is present on I2C bus
  Serial.print("Scanning for BQ4050 at address 0x0B... ");
  Wire.beginTransmission(0x0B);
  if (Wire.endTransmission() == 0) {
    Serial.println("Found!");
  } else {
    Serial.println("Not found!");
    Serial.println("Check wiring and I2C address");
    return;
  }
  
  // Start with PEC disabled for initial communication
  bq4050.setPECEnabled(false);
  
  // Initialize BQ4050
  if (!bq4050.begin()) {
    Serial.println("ERROR: Failed to initialize BQ4050!");
    Serial.print("Last error: ");
    Serial.println(BQ4050::getErrorString(bq4050.getLastError()));
    Serial.println("Check connections and I2C address");
    return;
  }
  
  Serial.println("BQ4050 initialized successfully!");
  Serial.println();
  
  // Test device identification with retries
  Serial.println("=== Device Information ===");
  
  // Add extra delays for manufacturer access commands
  delay(100);
  uint16_t deviceType = bq4050.getDeviceType();
  delay(50);
  
  // Check if we got a valid device type, retry if not
  if (deviceType == 0xFFA5 || deviceType == 0xFFFF || deviceType == 0x0000) {
    Serial.println("Retrying device type read...");
    delay(100);
    deviceType = bq4050.getDeviceType();
    delay(50);
  }
  
  uint16_t fwVersion = bq4050.getFirmwareVersion();
  delay(50);
  
  // Check if we got a valid firmware version, retry if not
  if (fwVersion == 0xFFA5 || fwVersion == 0xFFFF || fwVersion == 0x0000) {
    Serial.println("Retrying firmware version read...");
    delay(100);
    fwVersion = bq4050.getFirmwareVersion();
    delay(50);
  }
  
  uint16_t hwVersion = bq4050.getHardwareVersion();
  delay(50);
  
  Serial.print("Device Type: 0x");
  Serial.println(deviceType, HEX);
  Serial.print("Firmware Version: 0x");
  Serial.println(fwVersion, HEX);
  Serial.print("Hardware Version: 0x");
  Serial.println(hwVersion, HEX);
  Serial.print("Full Device Info: ");
#ifdef BQ4050_INCLUDE_UTILS
  Serial.println(BQ4050Utils::getFullDeviceInfo(deviceType, fwVersion, hwVersion));
#else
  Serial.print("Device: 0x"); Serial.print(deviceType, HEX);
  Serial.print(" (FW: 0x"); Serial.print(fwVersion, HEX);
  Serial.print(", HW: 0x"); Serial.print(hwVersion, HEX); Serial.println(")");
#endif
  
  // Test enhanced manufacturer access functions
  Serial.println();
  Serial.println("=== Enhanced Manufacturer Access Data ===");
  delay(50);
  String deviceTypeBlock = bq4050.getDeviceTypeBlock();
  delay(50);
  String firmwareBlock = bq4050.getFirmwareVersionBlock();  
  delay(50);
  String hardwareBlock = bq4050.getHardwareVersionBlock();
  
  Serial.print("Device Type Block: ");
  Serial.println(deviceTypeBlock);
  Serial.print("Firmware Version Block: ");
  Serial.println(firmwareBlock);
  Serial.print("Hardware Version Block: ");
  Serial.println(hardwareBlock);
  
  // Test additional sealed-mode commands
  Serial.println();
  Serial.println("=== Additional Sealed-Mode Commands ===");
  delay(50);
  uint16_t ifChecksum = bq4050.getIFChecksum();
  delay(50);
  uint16_t staticDFSig = bq4050.getStaticDFSignature();
  delay(50);
  uint16_t allDFSig = bq4050.getAllDFSignature();
  
  Serial.print("IF Checksum: 0x");
  Serial.println(ifChecksum, HEX);
  Serial.print("Static DF Signature: 0x");
  Serial.println(staticDFSig, HEX);
  Serial.print("All DF Signature: 0x");
  Serial.println(allDFSig, HEX);
  
  Serial.println();
  
  // Get security mode
  String securityMode = bq4050.getSecurityModeString();
  Serial.print("Security Mode: ");
  Serial.println(securityMode);
  Serial.print("Security Details: ");
  Serial.println(GET_SECURITY_DESC(securityMode));
  
  // Test cycle count reading
  delay(50);
  uint16_t cycleCount = bq4050.getCycleCount();
  delay(10);
  
  Serial.print("Cycle Count: ");
  Serial.println(FORMAT_CYCLE_COUNT(cycleCount));
  
  // Get manufacturer date and serial number
  uint16_t mfgDate = bq4050.getManufacturerDate();
  delay(10);
  uint16_t serialNumber = bq4050.getSerialNumber();
  delay(10);
  
  Serial.print("Manufacturer Date: ");
  Serial.println(FORMAT_MFG_DATE(mfgDate));
  Serial.print("Serial Number: ");
  Serial.println(FORMAT_SERIAL(serialNumber));
  
  // Get manufacturer name with retry
  Serial.print("Manufacturer: ");
  delay(50);
  String manufacturer = bq4050.getManufacturerName();
  if (manufacturer.length() == 0) {
    Serial.print("(retrying...) ");
    delay(100);
    manufacturer = bq4050.getManufacturerName();
  }
  Serial.println(manufacturer);
  
  // Get device name with retry
  Serial.print("Device Name: ");
  delay(50);
  String deviceName = bq4050.getDeviceName();
  if (deviceName.length() == 0) {
    Serial.print("(retrying...) ");
    delay(100);
    deviceName = bq4050.getDeviceName();
  }
  Serial.println(deviceName);
  
  // Get chemistry with retry
  delay(50);
  String chemistry = bq4050.getDeviceChemistry();
  if (chemistry.length() == 0) {
    Serial.print("Chemistry: (retrying...) ");
    delay(100);
    chemistry = bq4050.getDeviceChemistry();
  }
  Serial.print("Chemistry: ");
  Serial.println(chemistry);
  Serial.println();
  
  // Test PEC functionality once basic communication is working
  Serial.println("=== Testing PEC (Packet Error Checking) ===");
  Serial.print("PEC Status: ");
  Serial.println(bq4050.isPECEnabled() ? "Enabled" : "Disabled");
  Serial.println("Note: Library now uses smart PEC management");
  Serial.println("- PEC enabled for standard SBS commands (0x01-0x4F)");
  Serial.println("- PEC disabled for status registers (0x50-0x57) due to device limitations");
  Serial.println("- PEC disabled for extended commands (0x58+) until tested");
  
  // Try enabling PEC and test multiple commands
  Serial.println("Enabling PEC and testing communication...");
  bq4050.setPECEnabled(true);
  delay(10);
  
  // Test PEC with multiple command types
  int pecSuccessCount = 0;
  int pecTestCount = 0;
  
  // Test 1: Voltage (SBS command)
  pecTestCount++;
  float pecTestVoltage = bq4050.getVoltage();
  if (bq4050.getLastError() == BQ4050_ERROR_NONE) {
    pecSuccessCount++;
    Serial.println("✓ PEC voltage read successful");
  } else {
    Serial.println("✗ PEC voltage read failed");
  }
  
  // Test 2: Battery Status (SBS command)
  pecTestCount++;
  uint16_t pecTestStatus = bq4050.getBatteryStatus();
  if (bq4050.getLastError() == BQ4050_ERROR_NONE) {
    pecSuccessCount++;
    Serial.println("✓ PEC status read successful");
  } else {
    Serial.println("✗ PEC status read failed");
  }
  
  // Test 3: State of Charge (SBS command)
  pecTestCount++;
  uint8_t pecTestSOC = bq4050.getRelativeStateOfCharge();
  if (bq4050.getLastError() == BQ4050_ERROR_NONE) {
    pecSuccessCount++;
    Serial.println("✓ PEC SOC read successful");
  } else {
    Serial.println("✗ PEC SOC read failed");
  }
  
  // Evaluate PEC test results
  Serial.print("PEC Test Results: ");
  Serial.print(pecSuccessCount);
  Serial.print("/");
  Serial.print(pecTestCount);
  Serial.println(" commands successful");
  
  if (pecSuccessCount == pecTestCount) {
    Serial.println("✓ All PEC tests passed - keeping PEC enabled");
    Serial.print("PEC-validated voltage: ");
    Serial.println(FORMAT_VOLTAGE(pecTestVoltage));
  } else if (pecSuccessCount > 0) {
    Serial.println("⚠ Partial PEC support detected - keeping enabled with monitoring");
    Serial.println("  Will auto-disable if errors persist during operation");
  } else {
    Serial.println("✗ No PEC support detected - disabling for compatibility");
    bq4050.setPECEnabled(false);
  }
  
  Serial.print("Final PEC Status: ");
  Serial.println(bq4050.isPECEnabled() ? "Enabled" : "Disabled");
  Serial.println();
  
  // Add troubleshooting information for missing string data or invalid cycle count
  if (manufacturer.length() == 0 || chemistry.length() == 0 || 
      !IS_VALID_CYCLE_COUNT(cycleCount)) {
    Serial.println();
    Serial.println("=== Troubleshooting Information ===");
    Serial.println("String data (manufacturer/chemistry) could not be read. This may be due to:");
    if (securityMode == "Sealed") {
      Serial.println("1. *** Device is SEALED *** - This explains missing manufacturer strings");
      Serial.println("   - Sealed devices restrict access to manufacturer access commands");
      Serial.println("   - This is normal behavior for secured battery packs");
    } else {
      Serial.println("1. I2C communication timing issues on shared bus");
    }
    Serial.println("2. Device may not support all standard SBS string commands");
    Serial.println("3. Manufacturer may use custom implementation for string data");
    Serial.print("4. PEC (Packet Error Checking) status: ");
    Serial.println(bq4050.isPECEnabled() ? "Enabled" : "Disabled");
    if (!bq4050.isPECEnabled()) {
      Serial.println("   - PEC was disabled due to compatibility issues");
      Serial.println("   - This is normal for some battery pack implementations");
    }
    Serial.println();
    if (!IS_VALID_CYCLE_COUNT(cycleCount)) {
      Serial.println();
      Serial.println("Cycle Count Issue:");
      Serial.print("- SBS Command 0x17 returned: ");
      Serial.println(cycleCount);
      Serial.println("- 65535 (0xFFFF) typically indicates:");
      Serial.println("  * Communication error with I2C");
      Serial.println("  * Uninitialized battery data");
      Serial.println("  * Battery may never have been properly cycled");
      Serial.println("  * Some battery packs ship with 0xFFFF as default");
      Serial.println("- This is a standard SBS command that should work in sealed mode");
      Serial.println("- If other readings work, the cycle count data may be genuinely uninitialized");
    }
    Serial.println();
    Serial.println("Note: 0xFFA5 device type/firmware values are likely manufacturer-specific,");
    Serial.println("not communication errors. Your device appears to be working correctly!");
  }
  Serial.println();
}

// Helper function to check and report PEC status after each command
void checkPECAfterCommand(const String& commandName, BQ4050_Error error) {
  static bool verbosePECLogging = true; // Set to false to reduce output after first few cycles
  static int loopCount = 0;
  
  if (error == BQ4050_ERROR_PEC_MISMATCH && verbosePECLogging) {
    Serial.print("  ⚠ PEC MISMATCH in ");
    Serial.println(commandName);
  } else if (error != BQ4050_ERROR_NONE && error != BQ4050_ERROR_PEC_MISMATCH && verbosePECLogging) {
    Serial.print("  ✗ ERROR in ");
    Serial.print(commandName);
    Serial.print(": ");
    Serial.println(BQ4050::getErrorString(error));
  } else if (error == BQ4050_ERROR_NONE && verbosePECLogging && bq4050.isPECEnabled()) {
    Serial.print("  ✓ PEC OK: ");
    Serial.println(commandName);
  }
  
  // Reduce verbosity after 3 loops to avoid spam
  loopCount++;
  if (loopCount >= 3) {
    verbosePECLogging = false;
  }
}

void loop() {
  Serial.println("=== Battery Status ===");
  
  // Add small delay before starting communication
  delay(50);
  
  // Track PEC status for this loop iteration
  bool pecEnabled = bq4050.isPECEnabled();
  if (pecEnabled) {
    Serial.println("(PEC monitoring active - showing command-level results)");
  }
  
  // Basic battery information with detailed PEC checking
  float voltage = bq4050.getVoltage();
  delay(10);
  BQ4050_Error error = bq4050.getLastError();
  checkPECAfterCommand("getVoltage()", error);
  if (error != BQ4050_ERROR_NONE && error != BQ4050_ERROR_PEC_MISMATCH) {
    Serial.print("Voltage read error: ");
    Serial.println(BQ4050::getErrorString(error));
  }
  
  float current = bq4050.getCurrent();
  delay(10);
  error = bq4050.getLastError();
  checkPECAfterCommand("getCurrent()", error);
  if (error != BQ4050_ERROR_NONE && error != BQ4050_ERROR_PEC_MISMATCH) {
    Serial.print("Current read error: ");
    Serial.println(BQ4050::getErrorString(error));
  }
  
  float temperature = bq4050.getTemperature();
  delay(10);
  error = bq4050.getLastError();
  checkPECAfterCommand("getTemperature()", error);
  if (error != BQ4050_ERROR_NONE && error != BQ4050_ERROR_PEC_MISMATCH) {
    Serial.print("Temperature read error: ");
    Serial.println(BQ4050::getErrorString(error));
  }
  
  uint8_t soc = bq4050.getRelativeStateOfCharge();
  delay(10);
  error = bq4050.getLastError();
  checkPECAfterCommand("getRelativeStateOfCharge()", error);
  if (error != BQ4050_ERROR_NONE && error != BQ4050_ERROR_PEC_MISMATCH) {
    Serial.print("SOC read error: ");
    Serial.println(BQ4050::getErrorString(error));
  }
  
  Serial.print("Voltage: ");
  Serial.println(FORMAT_VOLTAGE(voltage));
  
  Serial.print("Current: ");
  Serial.println(FORMAT_CURRENT(current));
  
  Serial.print("Temperature: ");
  Serial.println(FORMAT_TEMPERATURE(temperature));
  
  Serial.print("State of Charge: ");
  Serial.println(FORMAT_PERCENTAGE(soc));
  
  // Capacity information with PEC monitoring
  uint16_t remainingCapacity = bq4050.getRemainingCapacity();
  delay(10);
  checkPECAfterCommand("getRemainingCapacity()", bq4050.getLastError());
  
  uint16_t fullCapacity = bq4050.getFullChargeCapacity();
  delay(10);
  checkPECAfterCommand("getFullChargeCapacity()", bq4050.getLastError());
  
  uint16_t designCapacity = bq4050.getDesignCapacity();
  delay(10);
  checkPECAfterCommand("getDesignCapacity()", bq4050.getLastError());
  
  Serial.print("Remaining Capacity: ");
  Serial.println(FORMAT_CAPACITY(remainingCapacity));
  
  Serial.print("Full Charge Capacity: ");
  Serial.println(FORMAT_CAPACITY(fullCapacity));
  
  Serial.print("Design Capacity: ");
  Serial.println(FORMAT_CAPACITY(designCapacity));
  
  // Cell voltages
  Serial.println("\n=== Cell Voltages ===");
  
  float cell1V = bq4050.getCellVoltage1();
  delay(10);
  checkPECAfterCommand("getCellVoltage1()", bq4050.getLastError());
  Serial.print("Cell 1: ");
  Serial.println(FORMAT_VOLTAGE(cell1V));
  
  float cell2V = bq4050.getCellVoltage2();
  delay(10);
  checkPECAfterCommand("getCellVoltage2()", bq4050.getLastError());
  Serial.print("Cell 2: ");
  Serial.println(FORMAT_VOLTAGE(cell2V));
  
  float cell3V = bq4050.getCellVoltage3();
  delay(10);
  checkPECAfterCommand("getCellVoltage3()", bq4050.getLastError());
  Serial.print("Cell 3: ");
  Serial.println(FORMAT_VOLTAGE(cell3V));
  
  float cell4V = bq4050.getCellVoltage4();
  delay(10);
  checkPECAfterCommand("getCellVoltage4()", bq4050.getLastError());
  Serial.print("Cell 4: ");
  Serial.println(FORMAT_VOLTAGE(cell4V));
  
  // Battery status and safety with PEC monitoring
  Serial.println("\n=== Status Information ===");
  uint16_t batteryStatus = bq4050.getBatteryStatus();
  delay(10);
  checkPECAfterCommand("getBatteryStatus()", bq4050.getLastError());
  Serial.print("Battery Status: ");
  Serial.println(PARSE_BATTERY_STATUS(batteryStatus));
  
#ifdef BQ4050_INCLUDE_UTILS
  // Demonstrate bitfield union usage
  BatteryStatusBits statusBits = BQ4050Utils::parseBatteryStatusBits(batteryStatus);
  Serial.print("Battery Status (bitfield): ");
  Serial.println(BQ4050Utils::describeBatteryStatusBits(statusBits));
  if (statusBits.bits.oca) Serial.println("  - Over Charged Alarm active");
  if (statusBits.bits.tca) Serial.println("  - Terminate Charge Alarm active");
#endif
  
  bool charging = bq4050.isCharging();
  delay(10);
  checkPECAfterCommand("isCharging()", bq4050.getLastError());
  Serial.print("Charging: ");
  Serial.println(charging ? "Yes" : "No");
  
  bool discharging = bq4050.isDischarging();
  delay(10);
  checkPECAfterCommand("isDischarging()", bq4050.getLastError());
  Serial.print("Discharging: ");
  Serial.println(discharging ? "Yes" : "No");
  
  bool healthy = bq4050.isBatteryHealthy();
  delay(10);
  checkPECAfterCommand("isBatteryHealthy()", bq4050.getLastError());
  Serial.print("Battery Healthy: ");
  Serial.println(healthy ? "Yes" : "No");
  
  // Safety status with enhanced 32-bit flag parsing and PEC monitoring
  uint16_t safetyAlert = bq4050.getSafetyAlert();
  delay(10);
  checkPECAfterCommand("getSafetyAlert()", bq4050.getLastError());
  
  uint16_t safetyStatus = bq4050.getSafetyStatus();
  delay(10);
  checkPECAfterCommand("getSafetyStatus()", bq4050.getLastError());
  
  // Overall health summary
  Serial.print("Health Summary: ");
  Serial.println(GET_HEALTH_SUMMARY(safetyStatus, safetyAlert, batteryStatus));
  
  if (safetyAlert != 0) {
    Serial.print("SAFETY ALERT: ");
#ifdef BQ4050_INCLUDE_UTILS
    Serial.println(BQ4050Utils::parseSafetyAlert(safetyAlert));
    
    // Demonstrate enhanced safety flag utilities with codes and descriptions
    Serial.println("\\n=== Enhanced Safety Alert Analysis ===");
    uint32_t extendedAlert = (uint32_t)safetyAlert; // Convert to 32-bit for full flag parsing
    Serial.println("Active Alert Flags:");
    Serial.println(BQ4050Utils::listAllActiveSafetyFlags(extendedAlert));
#else
    Serial.print("0x"); Serial.println(safetyAlert, HEX);
#endif
  }
  
  if (safetyStatus != 0) {
    Serial.print("SAFETY STATUS: ");
#ifdef BQ4050_INCLUDE_UTILS
    Serial.println(BQ4050Utils::parseSafetyStatus(safetyStatus));
    
    // Demonstrate bitfield union usage for safety status
    uint32_t extendedStatus = (uint32_t)safetyStatus;
    SafetyStatusBits safetyBits = BQ4050Utils::parseSafetyStatusBits(extendedStatus);
    Serial.print("Safety Status (bitfield): ");
    Serial.println(BQ4050Utils::describeSafetyStatusBits(safetyBits));
    
    // Individual bit access examples
    if (safetyBits.bits.cuv) Serial.println("  - Cell Undervoltage detected");
    if (safetyBits.bits.cov) Serial.println("  - Cell Overvoltage detected");
    if (safetyBits.bits.otc) Serial.println("  - Overtemperature during charge");
    if (safetyBits.bits.otd) Serial.println("  - Overtemperature during discharge");
    
    // Demonstrate enhanced safety flag utilities with codes and descriptions
    Serial.println("\\n=== Enhanced Safety Status Analysis ===");
    Serial.println("Active Status Flags:");
    Serial.println(BQ4050Utils::listAllActiveSafetyFlags(extendedStatus));
    
    // Demonstrate individual flag information access
    Serial.println("\\nDetailed Flag Information:");
    if (extendedStatus & SAFETY_CUV) {
      SafetyFlagInfo info = BQ4050Utils::getSafetyFlagInfo(SAFETY_CUV);
      Serial.println("- " + info.code + ": " + info.description);
    }
    if (extendedStatus & SAFETY_COV) {
      SafetyFlagInfo info = BQ4050Utils::getSafetyFlagInfo(SAFETY_COV);
      Serial.println("- " + info.code + ": " + info.description);
    }
    if (extendedStatus & SAFETY_OTC) {
      SafetyFlagInfo info = BQ4050Utils::getSafetyFlagInfo(SAFETY_OTC);
      Serial.println("- " + info.code + ": " + info.description);
    }
    if (extendedStatus & SAFETY_OTD) {
      SafetyFlagInfo info = BQ4050Utils::getSafetyFlagInfo(SAFETY_OTD);
      Serial.println("- " + info.code + ": " + info.description);
    }
#else
    Serial.print("0x"); Serial.println(safetyStatus, HEX);
#endif
  }
  
  // Demonstrate additional flag utilities with PEC monitoring
  uint16_t pfAlert = bq4050.getPFAlert();
  delay(10);
  checkPECAfterCommand("getPFAlert()", bq4050.getLastError());
  
  uint16_t pfStatus = bq4050.getPFStatus();
  delay(10);
  checkPECAfterCommand("getPFStatus()", bq4050.getLastError());
  
  uint16_t operationStatus = bq4050.getOperationStatus();
  delay(10);
  checkPECAfterCommand("getOperationStatus()", bq4050.getLastError());
  
  uint16_t chargingStatus = bq4050.getChargingStatus();
  delay(10);
  checkPECAfterCommand("getChargingStatus()", bq4050.getLastError());
  
  if (pfAlert != 0) {
    Serial.println("\\n=== PF Alert Analysis ===");
    Serial.print("PF ALERT: ");
#ifdef BQ4050_INCLUDE_UTILS
    Serial.println(BQ4050Utils::parsePFFlags((uint32_t)pfAlert, true));
    Serial.println("Active PF Alert Flags:");
    Serial.println(BQ4050Utils::listAllActivePFFlags((uint32_t)pfAlert));
#else
    Serial.print("0x"); Serial.println(pfAlert, HEX);
#endif
  }
  
  if (pfStatus != 0) {
    Serial.println("\\n=== PF Status Analysis ===");
    Serial.print("PF STATUS: ");
#ifdef BQ4050_INCLUDE_UTILS
    Serial.println(BQ4050Utils::parsePFFlags((uint32_t)pfStatus, true));
    Serial.println("Active PF Status Flags:");
    Serial.println(BQ4050Utils::listAllActivePFFlags((uint32_t)pfStatus));
#else
    Serial.print("0x"); Serial.println(pfStatus, HEX);
#endif
  }
  
  if (operationStatus != 0) {
    Serial.println("\\n=== Operation Status Analysis ===");
    Serial.print("OPERATION STATUS: ");
#ifdef BQ4050_INCLUDE_UTILS
    Serial.println(BQ4050Utils::parseOperationFlags((uint32_t)operationStatus, true));
    Serial.println("Active Operation Flags:");
    Serial.println(BQ4050Utils::listAllActiveOperationFlags((uint32_t)operationStatus));
#else
    Serial.print("0x"); Serial.println(operationStatus, HEX);
#endif
  }
  
  if (chargingStatus != 0) {
    Serial.println("\\n=== Charging Status Analysis ===");
    Serial.print("CHARGING STATUS: ");
#ifdef BQ4050_INCLUDE_UTILS
    Serial.println(BQ4050Utils::parseChargingFlags(chargingStatus, true));
    Serial.println("Active Charging Flags:");
    Serial.println(BQ4050Utils::listAllActiveChargingFlags(chargingStatus));
#else
    Serial.print("0x"); Serial.println(chargingStatus, HEX);
#endif
  }
  
  // Error checking with smart PEC management
  BQ4050_Error lastError = bq4050.getLastError();
  if (lastError != BQ4050_ERROR_NONE) {
    Serial.print("Last Error: ");
    Serial.println(BQ4050::getErrorString(lastError));
    
    // Note: PEC errors should now be rare due to smart PEC management
    // The library automatically disables PEC for problematic registers (0x50-0x57)
    if (lastError == BQ4050_ERROR_PEC_MISMATCH) {
      Serial.println("⚠ Unexpected PEC mismatch - this should be rare with smart PEC management");
    }
  }
  
  // PEC status monitoring
  static bool lastPECStatus = false;
  bool currentPECStatus = bq4050.isPECEnabled();
  if (currentPECStatus != lastPECStatus) {
    Serial.print("PEC Status Changed: ");
    Serial.println(currentPECStatus ? "Enabled → Enhanced data integrity" : "Disabled → Basic compatibility mode");
    lastPECStatus = currentPECStatus;
  }
  
  Serial.println("\n----------------------------------------");
  delay(5000); // Update every 5 seconds
}