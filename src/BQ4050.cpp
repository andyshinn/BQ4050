#include "BQ4050.h"

BQ4050::BQ4050(uint8_t address, TwoWire& wire)
  : _address(address), _wire(&wire), _lastError(BQ4050_ERROR_NONE), _pecEnabled(false) {
}

bool BQ4050::begin() {
  BQ4050_DEBUG_BEGIN();
  _wire->begin();

  // Test communication by reading device type
  uint16_t deviceType = getDeviceType();
  if (_lastError != BQ4050_ERROR_NONE) {
    BQ4050_DEBUG_PRINTF("Initialization failed: %s", getErrorString(_lastError).c_str());
    return false;
  }

  BQ4050_DEBUG_HEX("Device Type", deviceType);
  BQ4050_DEBUG_PRINT("Initialization successful");
  return true;
}

bool BQ4050::begin(TwoWire& wire) {
  _wire = &wire;
  return begin();
}

bool BQ4050::begin(int sda, int scl) {
  _wire->begin(sda, scl);
  
  // Test communication by reading device type
  uint16_t deviceType = getDeviceType();
  if (_lastError != BQ4050_ERROR_NONE) {
    return false;
  }

  BQ4050_DEBUG_HEX("Device Type", deviceType);
  return true;
}

bool BQ4050::begin(int sda, int scl, uint32_t frequency) {
  _wire->begin(sda, scl);
  _wire->setClock(frequency);
  
  // Test communication by reading device type  
  uint16_t deviceType = getDeviceType();
  if (_lastError != BQ4050_ERROR_NONE) {
    return false;
  }

  BQ4050_DEBUG_PRINTF("Initialized with %uHz", frequency);
  BQ4050_DEBUG_HEX("Device Type", deviceType);
  return true;
}

// Enhanced I2C helper methods
bool BQ4050::safeBeginTransmission(uint8_t reg) {
  _wire->beginTransmission(_address);
  _wire->write(reg);
  
  uint8_t result = _wire->endTransmission(false);
  if (result != 0) {
    BQ4050_DEBUG_PRINTF("I2C transmission failed: %d", result);
    setError(BQ4050_ERROR_I2C_NACK);
    return false;
  }
  
  // Add response delay for device to prepare data
  delayMicroseconds(I2C_RESPONSE_DELAY_US);
  return true;
}

uint8_t BQ4050::safeRequestData(uint8_t bytesToRead) {
  uint8_t bytesReceived = _wire->requestFrom(_address, bytesToRead);
  if (bytesReceived != bytesToRead) {
    BQ4050_DEBUG_PRINTF("I2C request failed: wanted %d, got %d", bytesToRead, bytesReceived);
    setError(BQ4050_ERROR_I2C_TIMEOUT);
    return 0;
  }
  return bytesReceived;
}

// Private I2C Communication Methods
uint8_t BQ4050::readRegister8(uint8_t reg) {
  if (!safeBeginTransmission(reg)) {
    return 0;
  }

  uint8_t bytesToRead = _pecEnabled ? 2 : 1; // +1 for PEC if enabled
  if (safeRequestData(bytesToRead) == 0) {
    return 0;
  }

  uint8_t data = _wire->read();

  // Validate PEC if enabled
  if (_pecEnabled) {
    uint8_t receivedPEC = _wire->read();
    uint8_t packet[] = {(uint8_t)(_address << 1), reg, (uint8_t)((_address << 1) | 1), data};
    if (!validatePEC(packet, 4, receivedPEC)) {
      return 0; // Error already set by validatePEC
    }
  }

  setError(BQ4050_ERROR_NONE);
  return data;
}

uint16_t BQ4050::readRegister16(uint8_t reg) {
  _wire->beginTransmission(_address);
  _wire->write(reg);

  if (_wire->endTransmission(false) != 0) {
    setError(BQ4050_ERROR_I2C_NACK);
    return 0;
  }

  uint8_t bytesToRead = _pecEnabled ? 3 : 2; // +1 for PEC if enabled
  if (_wire->requestFrom(_address, bytesToRead) != bytesToRead) {
    setError(BQ4050_ERROR_I2C_TIMEOUT);
    return 0;
  }

  uint8_t lsb = _wire->read();
  uint8_t msb = _wire->read();

  // Validate PEC if enabled
  if (_pecEnabled) {
    uint8_t receivedPEC = _wire->read();
    uint8_t packet[] = {(uint8_t)(_address << 1), reg, (uint8_t)((_address << 1) | 1), lsb, msb};
    if (!validatePEC(packet, 5, receivedPEC)) {
      return 0; // Error already set by validatePEC
    }
  }

  setError(BQ4050_ERROR_NONE);
  return (msb << 8) | lsb;
}

uint32_t BQ4050::readRegister32(uint8_t reg) {
  _wire->beginTransmission(_address);
  _wire->write(reg);

  if (_wire->endTransmission(false) != 0) {
    setError(BQ4050_ERROR_I2C_NACK);
    return 0;
  }

  uint8_t bytesToRead = _pecEnabled ? 5 : 4; // +1 for PEC if enabled
  if (_wire->requestFrom(_address, bytesToRead) != bytesToRead) {
    setError(BQ4050_ERROR_I2C_TIMEOUT);
    return 0;
  }

  uint8_t data[4];
  uint32_t result = 0;
  for (int i = 0; i < 4; i++) {
    data[i] = _wire->read();
    result |= ((uint32_t)data[i]) << (i * 8);
  }

  // Validate PEC if enabled
  if (_pecEnabled) {
    uint8_t receivedPEC = _wire->read();
    uint8_t packet[] = {(uint8_t)(_address << 1), reg, (uint8_t)((_address << 1) | 1), data[0], data[1], data[2], data[3]};
    if (!validatePEC(packet, 7, receivedPEC)) {
      return 0; // Error already set by validatePEC
    }
  }

  setError(BQ4050_ERROR_NONE);
  return result;
}

bool BQ4050::writeRegister8(uint8_t reg, uint8_t value) {
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->write(value);

  if (_wire->endTransmission() != 0) {
    setError(BQ4050_ERROR_I2C_NACK);
    return false;
  }

  setError(BQ4050_ERROR_NONE);
  return true;
}

bool BQ4050::writeRegister16(uint8_t reg, uint16_t value) {
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->write(value & 0xFF);        // LSB
  _wire->write((value >> 8) & 0xFF); // MSB

  if (_wire->endTransmission() != 0) {
    setError(BQ4050_ERROR_I2C_NACK);
    return false;
  }

  setError(BQ4050_ERROR_NONE);
  return true;
}

// Manufacturer Access Methods
uint16_t BQ4050::manufacturerAccess16(uint16_t command) {
  if (!writeRegister16(0x00, command)) {
    return 0;
  }
  delay(5); // Allow processing time
  return readRegister16(0x00);
}

uint32_t BQ4050::manufacturerAccess32(uint16_t command) {
  if (!writeRegister16(0x00, command)) {
    return 0;
  }
  delay(5); // Allow processing time
  return readRegister32(0x00);
}

bool BQ4050::manufacturerAccessWrite(uint16_t command, uint16_t data) {
  _wire->beginTransmission(_address);
  _wire->write(0x00);
  _wire->write(command & 0xFF);
  _wire->write((command >> 8) & 0xFF);
  _wire->write(data & 0xFF);
  _wire->write((data >> 8) & 0xFF);

  if (_wire->endTransmission() != 0) {
    setError(BQ4050_ERROR_I2C_NACK);
    return false;
  }

  setError(BQ4050_ERROR_NONE);
  return true;
}

// Smart PEC Management
bool BQ4050::shouldUsePECForRegister(uint8_t reg) const {
  // Based on testing, registers 0x50-0x57 (status/flag registers) don't support PEC
  // For extended commands (0x58+), we'll be conservative and disable PEC until tested
  // All standard SBS registers (0x01-0x4F) work fine with PEC when enabled
  if (reg >= 0x50 && reg <= 0x57) {
    return false;  // Status registers confirmed to not support PEC
  } else if (reg >= 0x58) {
    return false;  // Extended commands - disable PEC until tested
  }
  return _pecEnabled;  // Use global PEC setting for standard SBS registers
}

// Helper macro to reduce duplication in smart PEC functions
#define SMART_PEC_READ(reg, readCall) \
  do { \
    bool originalPECEnabled = _pecEnabled; \
    bool shouldUsePEC = shouldUsePECForRegister(reg); \
    if (_pecEnabled != shouldUsePEC) { \
      _pecEnabled = shouldUsePEC; \
    } \
    auto result = readCall; \
    _pecEnabled = originalPECEnabled; \
    return result; \
  } while(0)

uint16_t BQ4050::readRegister16WithSmartPEC(uint8_t reg) {
  SMART_PEC_READ(reg, readRegister16(reg));
}

uint32_t BQ4050::readRegister32WithSmartPEC(uint8_t reg) {
  SMART_PEC_READ(reg, readRegister32(reg));
}

String BQ4050::readSBSStringWithSmartPEC(uint8_t reg) {
  SMART_PEC_READ(reg, readSBSString(reg));
}

// Utility Methods
float BQ4050::convertTemperature(uint16_t rawTemp) {
  // Temperature in 0.1K units, convert to Celsius
  return (rawTemp / 10.0) - 273.15;
}

float BQ4050::convertVoltage(uint16_t rawVoltage) {
  // Voltage in mV units, convert to V
  return rawVoltage / 1000.0;
}

float BQ4050::convertCurrent(int16_t rawCurrent) {
  // Current in mA units, convert to A
  return rawCurrent / 1000.0;
}

// Basic SBS Commands Implementation
uint16_t BQ4050::getRemainingCapacityAlarm() {
  return readRegister16(BQ4050_CMD_REMAINING_CAPACITY_ALARM);
}

uint16_t BQ4050::getRemainingTimeAlarm() {
  return readRegister16(BQ4050_CMD_REMAINING_TIME_ALARM);
}

uint16_t BQ4050::getBatteryMode() {
  return readRegister16(BQ4050_CMD_BATTERY_MODE);
}

bool BQ4050::setBatteryMode(uint16_t mode) {
  return writeRegister16(BQ4050_CMD_BATTERY_MODE, mode);
}

float BQ4050::getTemperature() {
  uint16_t rawTemp = readRegister16(BQ4050_CMD_TEMPERATURE);
  if (_lastError != BQ4050_ERROR_NONE) return 0.0;
  return convertTemperature(rawTemp);
}

float BQ4050::getVoltage() {
  uint16_t rawVoltage = readRegister16(BQ4050_CMD_VOLTAGE);
  if (_lastError != BQ4050_ERROR_NONE) return 0.0;
  return convertVoltage(rawVoltage);
}

float BQ4050::getCurrent() {
  int16_t rawCurrent = (int16_t)readRegister16(BQ4050_CMD_CURRENT);
  if (_lastError != BQ4050_ERROR_NONE) return 0.0;
  return convertCurrent(rawCurrent);
}

float BQ4050::getAverageCurrent() {
  int16_t rawCurrent = (int16_t)readRegister16(BQ4050_CMD_AVERAGE_CURRENT);
  if (_lastError != BQ4050_ERROR_NONE) return 0.0;
  return convertCurrent(rawCurrent);
}

uint8_t BQ4050::getRelativeStateOfCharge() {
  return readRegister16(BQ4050_CMD_RELATIVE_STATE_OF_CHARGE) & 0xFF;
}

uint8_t BQ4050::getAbsoluteStateOfCharge() {
  return readRegister16(BQ4050_CMD_ABSOLUTE_STATE_OF_CHARGE) & 0xFF;
}

uint16_t BQ4050::getRemainingCapacity() {
  return readRegister16(BQ4050_CMD_REMAINING_CAPACITY);
}

uint16_t BQ4050::getFullChargeCapacity() {
  return readRegister16(BQ4050_CMD_FULL_CHARGE_CAPACITY);
}

uint16_t BQ4050::getBatteryStatus() {
  return readRegister16(BQ4050_CMD_BATTERY_STATUS);
}

uint16_t BQ4050::getCycleCount() {
  return readRegister16(BQ4050_CMD_CYCLE_COUNT);
}

uint16_t BQ4050::getDesignCapacity() {
  return readRegister16(BQ4050_CMD_DESIGN_CAPACITY);
}

uint16_t BQ4050::getDesignVoltage() {
  return readRegister16(BQ4050_CMD_DESIGN_VOLTAGE);
}

uint16_t BQ4050::getManufacturerDate() {
  return readRegister16(BQ4050_CMD_MANUFACTURER_DATE);
}

uint16_t BQ4050::getSerialNumber() {
  return readRegister16(BQ4050_CMD_SERIAL_NUMBER);
}

// Cell Voltages
float BQ4050::getCellVoltage1() {
  uint16_t rawVoltage = readRegister16(BQ4050_CMD_CELL_VOLTAGE_1);
  if (_lastError != BQ4050_ERROR_NONE) return 0.0;
  return convertVoltage(rawVoltage);
}

float BQ4050::getCellVoltage2() {
  uint16_t rawVoltage = readRegister16(BQ4050_CMD_CELL_VOLTAGE_2);
  if (_lastError != BQ4050_ERROR_NONE) return 0.0;
  return convertVoltage(rawVoltage);
}

float BQ4050::getCellVoltage3() {
  uint16_t rawVoltage = readRegister16(BQ4050_CMD_CELL_VOLTAGE_3);
  if (_lastError != BQ4050_ERROR_NONE) return 0.0;
  return convertVoltage(rawVoltage);
}

float BQ4050::getCellVoltage4() {
  uint16_t rawVoltage = readRegister16(BQ4050_CMD_CELL_VOLTAGE_4);
  if (_lastError != BQ4050_ERROR_NONE) return 0.0;
  return convertVoltage(rawVoltage);
}

// Status and Alerts
uint16_t BQ4050::getSafetyAlert() {
  return readRegister16WithSmartPEC(BQ4050_CMD_SAFETY_ALERT);
}

uint16_t BQ4050::getSafetyStatus() {
  return readRegister16WithSmartPEC(BQ4050_CMD_SAFETY_STATUS);
}

uint16_t BQ4050::getPFAlert() {
  return readRegister16WithSmartPEC(BQ4050_CMD_PF_ALERT);
}

uint16_t BQ4050::getPFStatus() {
  return readRegister16WithSmartPEC(BQ4050_CMD_PF_STATUS);
}

uint16_t BQ4050::getOperationStatus() {
  return readRegister16WithSmartPEC(BQ4050_CMD_OPERATION_STATUS);
}

uint16_t BQ4050::getChargingStatus() {
  return readRegister16WithSmartPEC(BQ4050_CMD_CHARGING_STATUS);
}

uint16_t BQ4050::getGaugingStatus() {
  return readRegister16WithSmartPEC(BQ4050_CMD_GAUGING_STATUS);
}

uint16_t BQ4050::getManufacturingStatus() {
  return readRegister16WithSmartPEC(BQ4050_CMD_MANUFACTURING_STATUS);
}

// Extended SBS Commands
uint16_t BQ4050::getAFERegister() {
  return readRegister16WithSmartPEC(BQ4050_CMD_AFE_REGISTER);
}

uint32_t BQ4050::getLifeTimeDataBlock1() {
  return readRegister32WithSmartPEC(BQ4050_CMD_LIFETIME_DATA_BLOCK_1);
}

uint32_t BQ4050::getLifeTimeDataBlock2() {
  return readRegister32WithSmartPEC(BQ4050_CMD_LIFETIME_DATA_BLOCK_2);
}

uint32_t BQ4050::getLifeTimeDataBlock3() {
  return readRegister32WithSmartPEC(BQ4050_CMD_LIFETIME_DATA_BLOCK_3);
}

uint32_t BQ4050::getLifeTimeDataBlock4() {
  return readRegister32WithSmartPEC(BQ4050_CMD_LIFETIME_DATA_BLOCK_4);
}

uint32_t BQ4050::getLifeTimeDataBlock5() {
  return readRegister32WithSmartPEC(BQ4050_CMD_LIFETIME_DATA_BLOCK_5);
}

String BQ4050::getManufacturerInfo() {
  return readSBSStringWithSmartPEC(BQ4050_CMD_MANUFACTURER_INFO);
}

String BQ4050::getDAStatus1() {
  return readSBSStringWithSmartPEC(BQ4050_CMD_DA_STATUS_1);
}

String BQ4050::getDAStatus2() {
  return readSBSStringWithSmartPEC(BQ4050_CMD_DA_STATUS_2);
}

// Device Identification Commands
uint16_t BQ4050::getDeviceType() {
  return manufacturerAccess16(BQ4050_MAC_DEVICE_TYPE);  // Manufacturer Access 0x0001
}

uint16_t BQ4050::getFirmwareVersion() {
  return manufacturerAccess16(BQ4050_MAC_FIRMWARE_VERSION);  // Manufacturer Access 0x0002
}

uint16_t BQ4050::getHardwareVersion() {
  return manufacturerAccess16(BQ4050_MAC_HARDWARE_VERSION);  // Manufacturer Access 0x0003
}

uint16_t BQ4050::getIFChecksum() {
  return manufacturerAccess16(BQ4050_MAC_IF_CHECKSUM);  // [SEALED] Manufacturer Access 0x0004
}

uint16_t BQ4050::getStaticDFSignature() {
  return manufacturerAccess16(BQ4050_MAC_STATIC_DF_SIGNATURE);  // [SEALED] Manufacturer Access 0x0005
}

uint16_t BQ4050::getAllDFSignature() {
  return manufacturerAccess16(BQ4050_MAC_ALL_DF_SIGNATURE);  // [SEALED] Manufacturer Access 0x0009
}

// Enhanced manufacturer access functions that return full data blocks
String BQ4050::getDeviceTypeBlock() {
  // Send manufacturer access command for device type
  if (!writeRegister16(0x00, BQ4050_MAC_DEVICE_TYPE)) {
    return "Error: Failed to send command";
  }
  delay(5);
  
  // Read the full data block from ManufacturerData (0x23)
  return readSBSString(BQ4050_CMD_MANUFACTURER_DATA);
}

String BQ4050::getFirmwareVersionBlock() {
  // Send manufacturer access command for firmware version
  if (!writeRegister16(0x00, BQ4050_MAC_FIRMWARE_VERSION)) {
    return "Error: Failed to send command";
  }
  delay(5);
  
  // Read the full data block from ManufacturerData (0x23)
  String rawData = readSBSString(BQ4050_CMD_MANUFACTURER_DATA);
  
  // Parse the firmware version format: ddDDvvVVbbBBTTzzZZRREE
  if (rawData.length() >= 10) {
    String result = "Raw: " + rawData + " | ";
    // Add basic parsing if the data is in the expected format
    if (rawData.length() >= 10) {
      uint16_t deviceNum = (rawData[1] << 8) | rawData[0];  // DDdd
      uint16_t version = (rawData[3] << 8) | rawData[2];    // VVvv
      uint16_t build = (rawData[5] << 8) | rawData[4];      // BBbb
      uint8_t fwType = rawData[6];                          // TT
      result += "Device:" + String(deviceNum, HEX) + " Ver:" + String(version, HEX) + 
               " Build:" + String(build, HEX) + " Type:" + String(fwType, HEX);
    }
    return result;
  }
  
  return rawData; // Return raw data if parsing fails
}

String BQ4050::getHardwareVersionBlock() {
  // Send manufacturer access command for hardware version
  if (!writeRegister16(0x00, BQ4050_MAC_HARDWARE_VERSION)) {
    return "Error: Failed to send command";
  }
  delay(5);
  
  // Read the full data block from ManufacturerData (0x23)
  return readSBSString(BQ4050_CMD_MANUFACTURER_DATA);
}

String BQ4050::getManufacturerName() {
  return readSBSString(BQ4050_CMD_MANUFACTURER_NAME);  // Regular SBS command 0x20
}

String BQ4050::getDeviceName() {
  return readSBSString(BQ4050_CMD_DEVICE_NAME);  // Regular SBS command 0x21
}

String BQ4050::getDeviceChemistry() {
  return readSBSString(BQ4050_CMD_DEVICE_CHEMISTRY);  // Regular SBS command 0x22
}

uint32_t BQ4050::getLifetimeDataBlock1() {
  return manufacturerAccess32(BQ4050_MAC_LIFETIME_DATA_BLOCK_1);
}

uint32_t BQ4050::getLifetimeDataBlock2() {
  return manufacturerAccess32(BQ4050_MAC_LIFETIME_DATA_BLOCK_2);
}

uint32_t BQ4050::getLifetimeDataBlock3() {
  return manufacturerAccess32(BQ4050_MAC_LIFETIME_DATA_BLOCK_3);
}

// FET Control
bool BQ4050::enableChargeFET() {
  return manufacturerAccessWrite(BQ4050_MAC_CHARGE_FET_CONTROL, 0x0001);
}

bool BQ4050::disableChargeFET() {
  return manufacturerAccessWrite(BQ4050_MAC_CHARGE_FET_CONTROL, 0x0000);
}

bool BQ4050::enableDischargeFET() {
  return manufacturerAccessWrite(BQ4050_MAC_DISCHARGE_FET_CONTROL, 0x0001);
}

bool BQ4050::disableDischargeFET() {
  return manufacturerAccessWrite(BQ4050_MAC_DISCHARGE_FET_CONTROL, 0x0000);
}

bool BQ4050::enablePrechargeFET() {
  return manufacturerAccessWrite(BQ4050_MAC_PRECHARGE_FET_CONTROL, 0x0001);
}

bool BQ4050::disablePrechargeFET() {
  return manufacturerAccessWrite(BQ4050_MAC_PRECHARGE_FET_CONTROL, 0x0000);
}

bool BQ4050::setFETControl(uint8_t control) {
  return manufacturerAccessWrite(BQ4050_MAC_FET_CONTROL, control);
}

// Device Control
bool BQ4050::enterCalibrationMode() {
  return manufacturerAccessWrite(BQ4050_MAC_CALIBRATION_MODE, 0x0001);
}

bool BQ4050::sealDevice() {
  return manufacturerAccessWrite(BQ4050_MAC_SEAL_DEVICE, 0x0000);
}

bool BQ4050::resetDevice() {
  return manufacturerAccessWrite(BQ4050_MAC_RESET_DEVICE, 0x0000);
}

bool BQ4050::enterSleepMode() {
  return manufacturerAccessWrite(BQ4050_MAC_SLEEP_MODE, 0x0000);
}

bool BQ4050::enterShutdownMode() {
  return manufacturerAccessWrite(BQ4050_MAC_SHUTDOWN_MODE, 0x0000);
}

// Security Mode Detection
BQ4050_SecurityMode BQ4050::getSecurityMode() {
  // Get the Manufacturing Status register (0x57)
  uint16_t mfgStatus = getManufacturingStatus();
  
  if (_lastError != BQ4050_ERROR_NONE) {
    return BQ4050_SECURITY_UNKNOWN;
  }
  
  // Check security status bits in Manufacturing Status
  // Bit 13: SEAL (0 = Sealed, 1 = Unsealed/Full Access)
  // Bit 14: Full Access (0 = No, 1 = Yes)
  bool sealed = (mfgStatus & 0x2000) == 0;        // Bit 13
  bool fullAccess = (mfgStatus & 0x4000) != 0;    // Bit 14
  
  if (sealed) {
    return BQ4050_SECURITY_SEALED;
  } else if (fullAccess) {
    return BQ4050_SECURITY_FULL_ACCESS;
  } else {
    return BQ4050_SECURITY_UNSEALED;
  }
}

String BQ4050::getSecurityModeString() {
  BQ4050_SecurityMode mode = getSecurityMode();
  switch (mode) {
    case BQ4050_SECURITY_SEALED: return "Sealed";
    case BQ4050_SECURITY_UNSEALED: return "Unsealed";
    case BQ4050_SECURITY_FULL_ACCESS: return "Full Access";
    default: return "Unknown";
  }
}

bool BQ4050::isSealed() {
  return getSecurityMode() == BQ4050_SECURITY_SEALED;
}

bool BQ4050::isUnsealed() {
  BQ4050_SecurityMode mode = getSecurityMode();
  return (mode == BQ4050_SECURITY_UNSEALED || mode == BQ4050_SECURITY_FULL_ACCESS);
}

bool BQ4050::hasFullAccess() {
  return getSecurityMode() == BQ4050_SECURITY_FULL_ACCESS;
}

// Data Flash Access
uint8_t BQ4050::readDataFlash(uint16_t address) {
  if (address < BQ4050_DATA_FLASH_START || address > BQ4050_DATA_FLASH_END) {
    setError(BQ4050_ERROR_INVALID_PARAMETER);
    return 0;
  }

  if (!manufacturerAccessWrite(0x44, address)) {
    return 0;
  }
  delay(5);

  return readRegister8(0x40);
}

bool BQ4050::writeDataFlash(uint16_t address, uint8_t data) {
  if (address < BQ4050_DATA_FLASH_START || address > BQ4050_DATA_FLASH_END) {
    setError(BQ4050_ERROR_INVALID_PARAMETER);
    return false;
  }

  if (!manufacturerAccessWrite(0x44, address)) {
    return false;
  }

  if (!writeRegister8(0x40, data)) {
    return false;
  }

  return true;
}

// Simple Status Methods
bool BQ4050::isCharging() {
  uint16_t status = getBatteryStatus();
  return (status & 0x0002) != 0; // CHARGE bit
}

bool BQ4050::isDischarging() {
  uint16_t status = getBatteryStatus();
  return (status & 0x0001) != 0; // DISCHARGE bit
}

bool BQ4050::isBatteryHealthy() {
  uint16_t safetyStatus = getSafetyStatus();
  uint16_t pfStatus = getPFStatus();
  return (safetyStatus == 0) && (pfStatus == 0);
}

uint16_t BQ4050::getEstimatedRuntime() {
  return readRegister16(0x11); // RunTimeToEmpty
}

uint16_t BQ4050::getEstimatedChargeTime() {
  return readRegister16(0x13); // AverageTimeToFull
}

uint8_t BQ4050::getStateOfHealth() {
  uint16_t fullCapacity = getFullChargeCapacity();
  uint16_t designCapacity = getDesignCapacity();
  if (designCapacity == 0) return 0;
  return (fullCapacity * 100) / designCapacity;
}

// Error Handling
BQ4050_Error BQ4050::getLastError() const {
  return _lastError;
}

String BQ4050::getErrorString(BQ4050_Error error) {
  switch (error) {
    case BQ4050_ERROR_NONE:
      return "No error";
    case BQ4050_ERROR_I2C_TIMEOUT:
      return "I2C timeout";
    case BQ4050_ERROR_I2C_NACK:
      return "I2C NACK";
    case BQ4050_ERROR_INVALID_PARAMETER:
      return "Invalid parameter";
    case BQ4050_ERROR_CRC_MISMATCH:
      return "CRC mismatch";
    case BQ4050_ERROR_PEC_MISMATCH:
      return "PEC mismatch";
    case BQ4050_ERROR_DEVICE_NOT_FOUND:
      return "Device not found";
    default:
      return "Unknown error";
  }
}


void BQ4050::setPECEnabled(bool enable) {
  _pecEnabled = enable;
  BQ4050_DEBUG_PRINT("PEC " + String(enable ? "enabled" : "disabled"));
}

bool BQ4050::isPECEnabled() const {
  return _pecEnabled;
}


// Private Helper Methods
void BQ4050::setError(BQ4050_Error error) {
  _lastError = error;
}

String BQ4050::readSBSString(uint8_t command) {
  BQ4050_DEBUG_HEX("Reading SBS string from register", command);
  
  if (!safeBeginTransmission(command)) {
    return "";
  }

  // Read length byte first (SBS block read format)
  if (safeRequestData(1) == 0) {
    return "";
  }

  uint8_t length = _wire->read();
  BQ4050_DEBUG_PRINTF("SBS string length: %d", length);
  
  // Enhanced buffer overflow protection
  if (length == 0) {
    BQ4050_DEBUG_PRINT("Zero length string received");
    return "";  // Empty string, not an error
  }
  
  if (length > MAX_SBS_STRING_LENGTH) {
    BQ4050_DEBUG_PRINTF("String too long: %d > %d", length, MAX_SBS_STRING_LENGTH);
    setError(BQ4050_ERROR_INVALID_PARAMETER);
    return "";
  }

  // Request the actual string data with bounds check
  uint8_t bytesToRead = _pecEnabled ? length + 1 : length; // +1 for PEC if enabled
  if (safeRequestData(bytesToRead) == 0) {
    return "";
  }

  // Read the string data with buffer overflow protection
  String result = "";
  result.reserve(length + 1);  // Pre-allocate to avoid multiple reallocations
  
  for (uint8_t i = 0; i < length; i++) {
    if (_wire->available() == 0) {
      BQ4050_DEBUG_PRINTF("Unexpected end of data at byte %d of %d", i, length);
      setError(BQ4050_ERROR_I2C_TIMEOUT);
      break;
    }
    
    char c = _wire->read();
    // Filter out invalid characters and add bounds check
    if (c >= 0x20 && c <= 0x7E) {  // Printable ASCII only (space to tilde)
      result += c;
    }
  }

  // Handle PEC if enabled
  if (_pecEnabled) {
    _wire->read(); // Read and discard PEC byte
    // TODO: Validate PEC for block read - more complex than single register PEC
    BQ4050_DEBUG_PRINT("PEC validation skipped for block read");
  }

  setError(BQ4050_ERROR_NONE);
  return result;
}


// Convenience Methods
CellStatus BQ4050::getAllCellStatus() {
  CellStatus status;

  status.voltage1 = getCellVoltage1();
  status.voltage2 = getCellVoltage2();
  status.voltage3 = getCellVoltage3();
  status.voltage4 = getCellVoltage4();

  // Check balancing status from battery status register
  uint16_t batteryStatus = getBatteryStatus();
  status.balancing1 = (batteryStatus & 0x0100) != 0; // CB1
  status.balancing2 = (batteryStatus & 0x0200) != 0; // CB2
  status.balancing3 = (batteryStatus & 0x0400) != 0; // CB3
  status.balancing4 = (batteryStatus & 0x0800) != 0; // CB4

  return status;
}

TemperatureStatus BQ4050::getAllTemperatures() {
  TemperatureStatus temps;

  temps.internal = getTemperature();

  // Read external thermistor temperatures (manufacturer access commands)
  uint16_t ts1Raw = manufacturerAccess16(0x0070); // TS1 Temperature
  uint16_t ts2Raw = manufacturerAccess16(0x0071); // TS2 Temperature
  uint16_t ts3Raw = manufacturerAccess16(0x0072); // TS3 Temperature
  uint16_t ts4Raw = manufacturerAccess16(0x0073); // TS4 Temperature
  uint16_t cellTempRaw = manufacturerAccess16(0x0074); // Cell Temperature
  uint16_t fetTempRaw = manufacturerAccess16(0x0075); // FET Temperature

  temps.ts1 = convertTemperature(ts1Raw);
  temps.ts2 = convertTemperature(ts2Raw);
  temps.ts3 = convertTemperature(ts3Raw);
  temps.ts4 = convertTemperature(ts4Raw);
  temps.cellTemp = convertTemperature(cellTempRaw);
  temps.fetTemp = convertTemperature(fetTempRaw);

  return temps;
}

BatteryInfo BQ4050::getCompleteBatteryStatus() {
  BatteryInfo info;

  info.voltage = getVoltage();
  info.current = getCurrent();
  info.temperature = getTemperature();
  info.soc = getRelativeStateOfCharge();
  info.cycleCount = getCycleCount();
  info.remainingCapacity = getRemainingCapacity();
  info.fullCapacity = getFullChargeCapacity();
  info.batteryStatus = getBatteryStatus();

  info.charging = (info.batteryStatus & 0x0002) != 0;
  info.discharging = (info.batteryStatus & 0x0001) != 0;

  // Safety status parsing
  uint16_t safetyStatus = getSafetyStatus();
  info.overVoltage = (safetyStatus & 0x0001) != 0;
  info.underVoltage = (safetyStatus & 0x0002) != 0;
  info.overTemperature = (safetyStatus & 0x0004) != 0;
  info.underTemperature = (safetyStatus & 0x0008) != 0;
  info.overCurrent = (safetyStatus & 0x0010) != 0;
  info.shortCircuit = (safetyStatus & 0x0020) != 0;

  return info;
}

SafetyStatus BQ4050::getParsedSafetyStatus() {
  SafetyStatus safety;

  safety.safetyAlert = getSafetyAlert();
  safety.safetyStatusRaw = readRegister16(BQ4050_CMD_SAFETY_STATUS);

  // Parse safety status bits
  safety.overVoltage = (safety.safetyStatusRaw & 0x0001) != 0;
  safety.underVoltage = (safety.safetyStatusRaw & 0x0002) != 0;
  safety.overTemperature = (safety.safetyStatusRaw & 0x0004) != 0;
  safety.underTemperature = (safety.safetyStatusRaw & 0x0008) != 0;
  safety.overCurrent = (safety.safetyStatusRaw & 0x0010) != 0;
  safety.shortCircuit = (safety.safetyStatusRaw & 0x0020) != 0;
  safety.cellImbalance = (safety.safetyStatusRaw & 0x0040) != 0;
  safety.chargeFETFailure = (safety.safetyStatusRaw & 0x0080) != 0;
  safety.dischargeFETFailure = (safety.safetyStatusRaw & 0x0100) != 0;

  return safety;
}

// CEDV Methods Implementation
CEDVStatus BQ4050::getCEDVStatus() {
  CEDVStatus status;

  // Read CEDV status from gauging status register
  uint16_t gaugingStatus = getGaugingStatus();

  status.edv0Reached = (gaugingStatus & 0x0001) != 0;
  status.edv1Reached = (gaugingStatus & 0x0002) != 0;
  status.edv2Reached = (gaugingStatus & 0x0004) != 0;
  status.fccUpdated = (gaugingStatus & 0x0008) != 0;
  status.smoothingActive = (gaugingStatus & 0x0010) != 0;
  status.vdq = (gaugingStatus & 0x0020) != 0;
  status.qualifiedDischarge = (gaugingStatus & 0x0040) != 0;

  return status;
}

float BQ4050::getEDV0Threshold() {
  uint16_t edv0Raw = manufacturerAccess16(0x0080); // EDV0 threshold
  return convertVoltage(edv0Raw);
}

float BQ4050::getEDV1Threshold() {
  uint16_t edv1Raw = manufacturerAccess16(0x0081); // EDV1 threshold
  return convertVoltage(edv1Raw);
}

float BQ4050::getEDV2Threshold() {
  uint16_t edv2Raw = manufacturerAccess16(0x0082); // EDV2 threshold
  return convertVoltage(edv2Raw);
}

bool BQ4050::isEDVCompensationEnabled() {
  uint16_t cedvConfig = manufacturerAccess16(0x0083); // CEDV Configuration
  return (cedvConfig & 0x0001) != 0; // Compensation enable bit
}

CEDVConfig BQ4050::getCEDVConfig() {
  CEDVConfig config;

  // Read CEDV configuration parameters from data flash
  config.emf = readDataFlash(0x4120) | (readDataFlash(0x4121) << 8);
  config.c0 = readDataFlash(0x4122) | (readDataFlash(0x4123) << 8);
  config.r0 = readDataFlash(0x4124) | (readDataFlash(0x4125) << 8);
  config.t0 = readDataFlash(0x4126) | (readDataFlash(0x4127) << 8);
  config.r1 = readDataFlash(0x4128) | (readDataFlash(0x4129) << 8);
  config.tc = readDataFlash(0x412A);
  config.c1 = readDataFlash(0x412B);
  config.ageFactor = readDataFlash(0x412C);
  config.batteryLowPercent = readDataFlash(0x412D) | (readDataFlash(0x412E) << 8);

  return config;
}

bool BQ4050::setCEDVConfig(const CEDVConfig& config) {
  bool success = true;

  // Write CEDV configuration parameters to data flash
  success &= writeDataFlash(0x4120, config.emf & 0xFF);
  success &= writeDataFlash(0x4121, (config.emf >> 8) & 0xFF);
  success &= writeDataFlash(0x4122, config.c0 & 0xFF);
  success &= writeDataFlash(0x4123, (config.c0 >> 8) & 0xFF);
  success &= writeDataFlash(0x4124, config.r0 & 0xFF);
  success &= writeDataFlash(0x4125, (config.r0 >> 8) & 0xFF);
  success &= writeDataFlash(0x4126, config.t0 & 0xFF);
  success &= writeDataFlash(0x4127, (config.t0 >> 8) & 0xFF);
  success &= writeDataFlash(0x4128, config.r1 & 0xFF);
  success &= writeDataFlash(0x4129, (config.r1 >> 8) & 0xFF);
  success &= writeDataFlash(0x412A, config.tc);
  success &= writeDataFlash(0x412B, config.c1);
  success &= writeDataFlash(0x412C, config.ageFactor);
  success &= writeDataFlash(0x412D, config.batteryLowPercent & 0xFF);
  success &= writeDataFlash(0x412E, (config.batteryLowPercent >> 8) & 0xFF);

  return success;
}

bool BQ4050::enableEDVCompensation() {
  return manufacturerAccessWrite(0x0083, 0x0001); // Enable CEDV
}

bool BQ4050::disableEDVCompensation() {
  return manufacturerAccessWrite(0x0083, 0x0000); // Disable CEDV
}

bool BQ4050::setFixedEDV0(float voltage) {
  uint16_t voltageRaw = (uint16_t)(voltage * 1000); // Convert to mV
  return manufacturerAccessWrite(0x0084, voltageRaw);
}

bool BQ4050::setFixedEDV1(float voltage) {
  uint16_t voltageRaw = (uint16_t)(voltage * 1000); // Convert to mV
  return manufacturerAccessWrite(0x0085, voltageRaw);
}

bool BQ4050::setFixedEDV2(float voltage) {
  uint16_t voltageRaw = (uint16_t)(voltage * 1000); // Convert to mV
  return manufacturerAccessWrite(0x0086, voltageRaw);
}

bool BQ4050::isUsingFixedEDV() {
  return !isEDVCompensationEnabled();
}

CEDVProfile BQ4050::getCEDVProfile() {
  CEDVProfile profile;

  // Read CEDV profile from data flash (voltage at various DOD levels)
  profile.voltage0DOD = readDataFlash(0x4140) | (readDataFlash(0x4141) << 8);
  profile.voltage10DOD = readDataFlash(0x4142) | (readDataFlash(0x4143) << 8);
  profile.voltage20DOD = readDataFlash(0x4144) | (readDataFlash(0x4145) << 8);
  profile.voltage30DOD = readDataFlash(0x4146) | (readDataFlash(0x4147) << 8);
  profile.voltage40DOD = readDataFlash(0x4148) | (readDataFlash(0x4149) << 8);
  profile.voltage50DOD = readDataFlash(0x414A) | (readDataFlash(0x414B) << 8);
  profile.voltage60DOD = readDataFlash(0x414C) | (readDataFlash(0x414D) << 8);
  profile.voltage70DOD = readDataFlash(0x414E) | (readDataFlash(0x414F) << 8);
  profile.voltage80DOD = readDataFlash(0x4150) | (readDataFlash(0x4151) << 8);
  profile.voltage90DOD = readDataFlash(0x4152) | (readDataFlash(0x4153) << 8);
  profile.voltage100DOD = readDataFlash(0x4154) | (readDataFlash(0x4155) << 8);

  return profile;
}

bool BQ4050::setCEDVProfile(const CEDVProfile& profile) {
  bool success = true;

  // Write CEDV profile to data flash
  success &= writeDataFlash(0x4140, profile.voltage0DOD & 0xFF);
  success &= writeDataFlash(0x4141, (profile.voltage0DOD >> 8) & 0xFF);
  success &= writeDataFlash(0x4142, profile.voltage10DOD & 0xFF);
  success &= writeDataFlash(0x4143, (profile.voltage10DOD >> 8) & 0xFF);
  success &= writeDataFlash(0x4144, profile.voltage20DOD & 0xFF);
  success &= writeDataFlash(0x4145, (profile.voltage20DOD >> 8) & 0xFF);
  success &= writeDataFlash(0x4146, profile.voltage30DOD & 0xFF);
  success &= writeDataFlash(0x4147, (profile.voltage30DOD >> 8) & 0xFF);
  success &= writeDataFlash(0x4148, profile.voltage40DOD & 0xFF);
  success &= writeDataFlash(0x4149, (profile.voltage40DOD >> 8) & 0xFF);
  success &= writeDataFlash(0x414A, profile.voltage50DOD & 0xFF);
  success &= writeDataFlash(0x414B, (profile.voltage50DOD >> 8) & 0xFF);
  success &= writeDataFlash(0x414C, profile.voltage60DOD & 0xFF);
  success &= writeDataFlash(0x414D, (profile.voltage60DOD >> 8) & 0xFF);
  success &= writeDataFlash(0x414E, profile.voltage70DOD & 0xFF);
  success &= writeDataFlash(0x414F, (profile.voltage70DOD >> 8) & 0xFF);
  success &= writeDataFlash(0x4150, profile.voltage80DOD & 0xFF);
  success &= writeDataFlash(0x4151, (profile.voltage80DOD >> 8) & 0xFF);
  success &= writeDataFlash(0x4152, profile.voltage90DOD & 0xFF);
  success &= writeDataFlash(0x4153, (profile.voltage90DOD >> 8) & 0xFF);
  success &= writeDataFlash(0x4154, profile.voltage100DOD & 0xFF);
  success &= writeDataFlash(0x4155, (profile.voltage100DOD >> 8) & 0xFF);

  return success;
}

CEDVSmoothingConfig BQ4050::getSmoothingConfig() {
  CEDVSmoothingConfig config;

  // Read smoothing configuration from data flash
  config.smoothingStartVoltage = readDataFlash(0x4160) | (readDataFlash(0x4161) << 8);
  config.smoothingDeltaVoltage = readDataFlash(0x4162) | (readDataFlash(0x4163) << 8);
  config.maxSmoothingCurrent = readDataFlash(0x4164) | (readDataFlash(0x4165) << 8);
  config.eocSmoothCurrent = readDataFlash(0x4166);
  config.eocSmoothCurrentTime = readDataFlash(0x4167);

  uint8_t smoothingFlags = readDataFlash(0x4168);
  config.smoothToEDV0 = (smoothingFlags & 0x01) != 0;
  config.smoothToEDV1 = (smoothingFlags & 0x02) != 0;
  config.extendedSmoothing = (smoothingFlags & 0x04) != 0;

  return config;
}

bool BQ4050::setSmoothingConfig(const CEDVSmoothingConfig& config) {
  bool success = true;

  success &= writeDataFlash(0x4160, config.smoothingStartVoltage & 0xFF);
  success &= writeDataFlash(0x4161, (config.smoothingStartVoltage >> 8) & 0xFF);
  success &= writeDataFlash(0x4162, config.smoothingDeltaVoltage & 0xFF);
  success &= writeDataFlash(0x4163, (config.smoothingDeltaVoltage >> 8) & 0xFF);
  success &= writeDataFlash(0x4164, config.maxSmoothingCurrent & 0xFF);
  success &= writeDataFlash(0x4165, (config.maxSmoothingCurrent >> 8) & 0xFF);
  success &= writeDataFlash(0x4166, config.eocSmoothCurrent);
  success &= writeDataFlash(0x4167, config.eocSmoothCurrentTime);

  uint8_t smoothingFlags = 0;
  if (config.smoothToEDV0) smoothingFlags |= 0x01;
  if (config.smoothToEDV1) smoothingFlags |= 0x02;
  if (config.extendedSmoothing) smoothingFlags |= 0x04;
  success &= writeDataFlash(0x4168, smoothingFlags);

  return success;
}

bool BQ4050::isLearningDischarge() {
  uint16_t gaugingStatus = getGaugingStatus();
  return (gaugingStatus & 0x0040) != 0; // Qualified discharge bit
}

uint16_t BQ4050::getQualifiedDischargeCount() {
  return manufacturerAccess16(0x0087); // Qualified discharge count
}

bool BQ4050::resetLearning() {
  return manufacturerAccessWrite(0x0088, 0x0000); // Reset learning data
}

CEDVInfo BQ4050::getCompleteCEDVInfo() {
  CEDVInfo info;

  info.status = getCEDVStatus();
  info.currentEDV0 = getEDV0Threshold();
  info.currentEDV1 = getEDV1Threshold();
  info.currentEDV2 = getEDV2Threshold();
  info.compensationEnabled = isEDVCompensationEnabled();
  info.smoothingActive = info.status.smoothingActive;

  // Calculate remaining capacity to each EDV threshold
  uint16_t currentCapacity = getRemainingCapacity();
  float currentVoltage = getVoltage();

  // Estimate remaining to each threshold (simplified calculation)
  if (currentVoltage > info.currentEDV2) {
    info.remainingToEDV2 = (currentCapacity * (currentVoltage - info.currentEDV2)) /
                          (currentVoltage - info.currentEDV0);
  } else {
    info.remainingToEDV2 = 0;
  }

  if (currentVoltage > info.currentEDV1) {
    info.remainingToEDV1 = (currentCapacity * (currentVoltage - info.currentEDV1)) /
                          (currentVoltage - info.currentEDV0);
  } else {
    info.remainingToEDV1 = 0;
  }

  info.remainingToEDV0 = currentCapacity;

  return info;
}

// Settings Flash Configuration Methods
CellCount BQ4050::getCellCount() {
  uint8_t daConfig = readDataFlash(0x4000); // DA Configuration register
  return (CellCount)((daConfig >> 0) & 0x03); // CC1:CC0 bits
}

bool BQ4050::setCellCount(CellCount count) {
  uint8_t daConfig = readDataFlash(0x4000);
  daConfig &= ~0x03; // Clear CC1:CC0 bits
  daConfig |= ((uint8_t)count & 0x03); // Set new cell count
  return writeDataFlash(0x4000, daConfig);
}

DAConfiguration BQ4050::getDAConfiguration() {
  DAConfiguration config;
  uint8_t daReg = readDataFlash(0x4000);

  config.cellCount = (CellCount)(daReg & 0x03);
  config.nonRemovable = (daReg & 0x04) != 0;
  config.inSystemSleep = (daReg & 0x08) != 0;
  config.sleepMode = (daReg & 0x10) != 0;
  config.emergencyShutdown = (daReg & 0x20) != 0;
  config.cellTempAverage = (daReg & 0x40) != 0;
  config.fetTempAverage = (daReg & 0x80) != 0;

  return config;
}

bool BQ4050::setDAConfiguration(const DAConfiguration& config) {
  uint8_t daReg = 0;

  daReg |= ((uint8_t)config.cellCount & 0x03);
  if (config.nonRemovable) daReg |= 0x04;
  if (config.inSystemSleep) daReg |= 0x08;
  if (config.sleepMode) daReg |= 0x10;
  if (config.emergencyShutdown) daReg |= 0x20;
  if (config.cellTempAverage) daReg |= 0x40;
  if (config.fetTempAverage) daReg |= 0x80;

  return writeDataFlash(0x4000, daReg);
}

FETOptions BQ4050::getFETOptions() {
  FETOptions options;
  uint8_t fetReg = readDataFlash(0x4001); // FET Options register

  options.prechargeComm = (fetReg & 0x01) != 0;
  options.chargeSuspendFET = (fetReg & 0x02) != 0;
  options.chargeInhibitFET = (fetReg & 0x04) != 0;
  options.overTempFETDisable = (fetReg & 0x08) != 0;

  return options;
}

bool BQ4050::setFETOptions(const FETOptions& options) {
  uint8_t fetReg = 0;

  if (options.prechargeComm) fetReg |= 0x01;
  if (options.chargeSuspendFET) fetReg |= 0x02;
  if (options.chargeInhibitFET) fetReg |= 0x04;
  if (options.overTempFETDisable) fetReg |= 0x08;

  return writeDataFlash(0x4001, fetReg);
}

PowerConfig BQ4050::getPowerConfig() {
  PowerConfig config;
  uint8_t powerReg = readDataFlash(0x4002); // Power Configuration register

  config.autoShipEnable = (powerReg & 0x01) != 0;

  return config;
}

bool BQ4050::setPowerConfig(const PowerConfig& config) {
  uint8_t powerReg = 0;

  if (config.autoShipEnable) powerReg |= 0x01;

  return writeDataFlash(0x4002, powerReg);
}

IOConfig BQ4050::getIOConfig() {
  IOConfig config;
  uint8_t ioReg = readDataFlash(0x4003); // I/O Configuration register

  config.btpEnable = (ioReg & 0x01) != 0;
  config.btpPolarity = (ioReg & 0x02) != 0;

  return config;
}

bool BQ4050::setIOConfig(const IOConfig& config) {
  uint8_t ioReg = 0;

  if (config.btpEnable) ioReg |= 0x01;
  if (config.btpPolarity) ioReg |= 0x02;

  return writeDataFlash(0x4003, ioReg);
}

TemperatureConfig BQ4050::getTemperatureConfig() {
  TemperatureConfig config;
  uint8_t tempReg1 = readDataFlash(0x4004); // Temperature Configuration register 1
  uint8_t tempReg2 = readDataFlash(0x4005); // Temperature Configuration register 2

  config.ts1Enable = (tempReg1 & 0x01) != 0;
  config.ts2Enable = (tempReg1 & 0x02) != 0;
  config.ts3Enable = (tempReg1 & 0x04) != 0;
  config.ts4Enable = (tempReg1 & 0x08) != 0;
  config.internalTSEnable = (tempReg1 & 0x10) != 0;

  config.ts1CellMode = (tempReg2 & 0x01) != 0;
  config.ts2CellMode = (tempReg2 & 0x02) != 0;
  config.ts3CellMode = (tempReg2 & 0x04) != 0;
  config.ts4CellMode = (tempReg2 & 0x08) != 0;
  config.internalCellMode = (tempReg2 & 0x10) != 0;

  return config;
}

bool BQ4050::setTemperatureConfig(const TemperatureConfig& config) {
  uint8_t tempReg1 = 0;
  uint8_t tempReg2 = 0;

  if (config.ts1Enable) tempReg1 |= 0x01;
  if (config.ts2Enable) tempReg1 |= 0x02;
  if (config.ts3Enable) tempReg1 |= 0x04;
  if (config.ts4Enable) tempReg1 |= 0x08;
  if (config.internalTSEnable) tempReg1 |= 0x10;

  if (config.ts1CellMode) tempReg2 |= 0x01;
  if (config.ts2CellMode) tempReg2 |= 0x02;
  if (config.ts3CellMode) tempReg2 |= 0x04;
  if (config.ts4CellMode) tempReg2 |= 0x08;
  if (config.internalCellMode) tempReg2 |= 0x10;

  bool success = writeDataFlash(0x4004, tempReg1);
  success &= writeDataFlash(0x4005, tempReg2);

  return success;
}

LEDConfig BQ4050::getLEDConfig() {
  LEDConfig config;

  config.displayMask = readDataFlash(0x4006) | (readDataFlash(0x4007) << 8);

  uint8_t ledCtrl = readDataFlash(0x4008);
  config.ledEnable = (ledCtrl & 0x01) != 0;
  config.blinkRate = (ledCtrl >> 1) & 0x07;
  config.flashRate = (ledCtrl >> 4) & 0x0F;

  return config;
}

bool BQ4050::setLEDConfig(const LEDConfig& config) {
  bool success = true;

  success &= writeDataFlash(0x4006, config.displayMask & 0xFF);
  success &= writeDataFlash(0x4007, (config.displayMask >> 8) & 0xFF);

  uint8_t ledCtrl = 0;
  if (config.ledEnable) ledCtrl |= 0x01;
  ledCtrl |= ((config.blinkRate & 0x07) << 1);
  ledCtrl |= ((config.flashRate & 0x0F) << 4);

  success &= writeDataFlash(0x4008, ledCtrl);

  return success;
}

BalancingConfig BQ4050::getBalancingConfig() {
  BalancingConfig config;

  uint8_t balanceCtrl = readDataFlash(0x4009);
  config.cellBalancingEnable = (balanceCtrl & 0x01) != 0;

  config.balanceVoltage = readDataFlash(0x400A) | (readDataFlash(0x400B) << 8);
  config.balanceTime = readDataFlash(0x400C) | (readDataFlash(0x400D) << 8);

  return config;
}

bool BQ4050::setBalancingConfig(const BalancingConfig& config) {
  bool success = true;

  uint8_t balanceCtrl = 0;
  if (config.cellBalancingEnable) balanceCtrl |= 0x01;

  success &= writeDataFlash(0x4009, balanceCtrl);
  success &= writeDataFlash(0x400A, config.balanceVoltage & 0xFF);
  success &= writeDataFlash(0x400B, (config.balanceVoltage >> 8) & 0xFF);
  success &= writeDataFlash(0x400C, config.balanceTime & 0xFF);
  success &= writeDataFlash(0x400D, (config.balanceTime >> 8) & 0xFF);

  return success;
}

SBSGaugingConfig BQ4050::getSBSGaugingConfig() {
  SBSGaugingConfig config;
  uint8_t gaugingReg = readDataFlash(0x400E);

  config.rsocHold = (gaugingReg & 0x01) != 0;
  config.capacitySync = (gaugingReg & 0x02) != 0;
  config.smoothSOC = (gaugingReg & 0x04) != 0;

  return config;
}

bool BQ4050::setSBSGaugingConfig(const SBSGaugingConfig& config) {
  uint8_t gaugingReg = 0;

  if (config.rsocHold) gaugingReg |= 0x01;
  if (config.capacitySync) gaugingReg |= 0x02;
  if (config.smoothSOC) gaugingReg |= 0x04;

  return writeDataFlash(0x400E, gaugingReg);
}

SBSConfig BQ4050::getSBSConfig() {
  SBSConfig config;
  uint8_t sbsReg = readDataFlash(0x400F);

  config.specificationMode = (sbsReg & 0x01) != 0;
  config.packetErrorCheck = (sbsReg & 0x02) != 0;
  config.smbusTimeout = (sbsReg >> 2) & 0x3F;

  return config;
}

bool BQ4050::setSBSConfig(const SBSConfig& config) {
  uint8_t sbsReg = 0;

  if (config.specificationMode) sbsReg |= 0x01;
  if (config.packetErrorCheck) sbsReg |= 0x02;
  sbsReg |= ((config.smbusTimeout & 0x3F) << 2);

  return writeDataFlash(0x400F, sbsReg);
}

SOCFlagConfig BQ4050::getSOCFlagConfig() {
  SOCFlagConfig config;
  uint8_t socReg = readDataFlash(0x4010);

  config.tcSetOnCharge = (socReg & 0x01) != 0;
  config.fcSetOnCharge = (socReg & 0x02) != 0;
  config.tcClearOnRSOC = (socReg & 0x04) != 0;
  config.tdClearOnRSOC = (socReg & 0x08) != 0;
  config.rsocThresholds = (socReg >> 4) & 0x0F;

  return config;
}

bool BQ4050::setSOCFlagConfig(const SOCFlagConfig& config) {
  uint8_t socReg = 0;

  if (config.tcSetOnCharge) socReg |= 0x01;
  if (config.fcSetOnCharge) socReg |= 0x02;
  if (config.tcClearOnRSOC) socReg |= 0x04;
  if (config.tdClearOnRSOC) socReg |= 0x08;
  socReg |= ((config.rsocThresholds & 0x0F) << 4);

  return writeDataFlash(0x4010, socReg);
}

ProtectionConfig BQ4050::getProtectionConfig() {
  ProtectionConfig config;
  uint8_t protReg = readDataFlash(0x4011);

  config.protectionEnable = (protReg & 0x01) != 0;
  config.protectionDelay = (protReg >> 1) & 0x7F;

  return config;
}

bool BQ4050::setProtectionConfig(const ProtectionConfig& config) {
  uint8_t protReg = 0;

  if (config.protectionEnable) protReg |= 0x01;
  protReg |= ((config.protectionDelay & 0x7F) << 1);

  return writeDataFlash(0x4011, protReg);
}

// Quick Setup Methods
bool BQ4050::configureFor1S(bool balancing) {
  DAConfiguration daConfig = getDAConfiguration();
  daConfig.cellCount = ONE_CELL;

  BalancingConfig balanceConfig = getBalancingConfig();
  balanceConfig.cellBalancingEnable = balancing;

  bool success = setDAConfiguration(daConfig);
  success &= setBalancingConfig(balanceConfig);

  return success;
}

bool BQ4050::configureFor2S(bool balancing) {
  DAConfiguration daConfig = getDAConfiguration();
  daConfig.cellCount = TWO_CELL;

  BalancingConfig balanceConfig = getBalancingConfig();
  balanceConfig.cellBalancingEnable = balancing;

  bool success = setDAConfiguration(daConfig);
  success &= setBalancingConfig(balanceConfig);

  return success;
}

bool BQ4050::configureFor3S(bool balancing) {
  DAConfiguration daConfig = getDAConfiguration();
  daConfig.cellCount = THREE_CELL;

  BalancingConfig balanceConfig = getBalancingConfig();
  balanceConfig.cellBalancingEnable = balancing;

  bool success = setDAConfiguration(daConfig);
  success &= setBalancingConfig(balanceConfig);

  return success;
}

bool BQ4050::configureFor4S(bool balancing) {
  DAConfiguration daConfig = getDAConfiguration();
  daConfig.cellCount = FOUR_CELL;

  BalancingConfig balanceConfig = getBalancingConfig();
  balanceConfig.cellBalancingEnable = balancing;

  bool success = setDAConfiguration(daConfig);
  success &= setBalancingConfig(balanceConfig);

  return success;
}

bool BQ4050::configureForRemovableBattery() {
  DAConfiguration daConfig = getDAConfiguration();
  daConfig.nonRemovable = false;
  daConfig.inSystemSleep = false;

  return setDAConfiguration(daConfig);
}

bool BQ4050::configureForEmbeddedBattery() {
  DAConfiguration daConfig = getDAConfiguration();
  daConfig.nonRemovable = true;
  daConfig.inSystemSleep = true;

  return setDAConfiguration(daConfig);
}

bool BQ4050::configureForPortableDevice() {
  PowerConfig powerConfig = getPowerConfig();
  powerConfig.autoShipEnable = true;

  DAConfiguration daConfig = getDAConfiguration();
  daConfig.sleepMode = true;

  bool success = setPowerConfig(powerConfig);
  success &= setDAConfiguration(daConfig);

  return success;
}

bool BQ4050::configureForPowerBank() {
  LEDConfig ledConfig = getLEDConfig();
  ledConfig.ledEnable = true;

  IOConfig ioConfig = getIOConfig();
  ioConfig.btpEnable = true;

  bool success = setLEDConfig(ledConfig);
  success &= setIOConfig(ioConfig);

  return success;
}

// Configuration Management
bool BQ4050::validateConfiguration() {
  // Basic validation checks
  CellCount cellCount = getCellCount();
  if (cellCount > FOUR_CELL) return false;

  // Check if balancing is enabled for multi-cell configurations
  if (cellCount > ONE_CELL) {
    BalancingConfig balanceConfig = getBalancingConfig();
    if (!balanceConfig.cellBalancingEnable) {
      BQ4050_DEBUG_PRINT("Warning: Cell balancing disabled for multi-cell configuration");
    }
  }

  return true;
}

bool BQ4050::isConfigurationValid() {
  return validateConfiguration();
}

String BQ4050::getConfigurationErrors() {
  String errors = "";

  CellCount cellCount = getCellCount();
  if (cellCount > FOUR_CELL) {
    errors += "Invalid cell count; ";
  }

  if (cellCount > ONE_CELL) {
    BalancingConfig balanceConfig = getBalancingConfig();
    if (!balanceConfig.cellBalancingEnable) {
      errors += "Cell balancing disabled for multi-cell; ";
    }
  }

  return errors;
}

FullConfiguration BQ4050::backupConfiguration() {
  FullConfiguration config;

  config.daConfig = getDAConfiguration();
  config.fetOptions = getFETOptions();
  config.powerConfig = getPowerConfig();
  config.ioConfig = getIOConfig();
  config.tempConfig = getTemperatureConfig();
  config.ledConfig = getLEDConfig();
  config.balanceConfig = getBalancingConfig();
  config.sbsGauging = getSBSGaugingConfig();
  config.sbsConfig = getSBSConfig();
  config.socFlags = getSOCFlagConfig();
  config.protection = getProtectionConfig();

  return config;
}

bool BQ4050::restoreConfiguration(const FullConfiguration& config) {
  bool success = true;

  success &= setDAConfiguration(config.daConfig);
  success &= setFETOptions(config.fetOptions);
  success &= setPowerConfig(config.powerConfig);
  success &= setIOConfig(config.ioConfig);
  success &= setTemperatureConfig(config.tempConfig);
  success &= setLEDConfig(config.ledConfig);
  success &= setBalancingConfig(config.balanceConfig);
  success &= setSBSGaugingConfig(config.sbsGauging);
  success &= setSBSConfig(config.sbsConfig);
  success &= setSOCFlagConfig(config.socFlags);
  success &= setProtectionConfig(config.protection);

  return success;
}

bool BQ4050::resetToFactoryDefaults() {
  // Reset all configuration registers to factory defaults
  // This is a simplified implementation - actual factory defaults would vary
  DAConfiguration daConfig = {false, false, false, false, false, false, THREE_CELL};
  FETOptions fetOptions = {false, false, false, false};
  PowerConfig powerConfig = {false};
  IOConfig ioConfig = {false, false};
  TemperatureConfig tempConfig = {true, true, false, false, true, false, false, false, false, false};
  LEDConfig ledConfig = {0x0000, false, 0, 0};
  BalancingConfig balanceConfig = {true, 4200, 300};
  SBSGaugingConfig sbsGauging = {false, true, true};
  SBSConfig sbsConfig = {true, 35, true};
  SOCFlagConfig socFlags = {true, true, false, false, 0x03};
  ProtectionConfig protection = {true, 5};

  bool success = true;
  success &= setDAConfiguration(daConfig);
  success &= setFETOptions(fetOptions);
  success &= setPowerConfig(powerConfig);
  success &= setIOConfig(ioConfig);
  success &= setTemperatureConfig(tempConfig);
  success &= setLEDConfig(ledConfig);
  success &= setBalancingConfig(balanceConfig);
  success &= setSBSGaugingConfig(sbsGauging);
  success &= setSBSConfig(sbsConfig);
  success &= setSOCFlagConfig(socFlags);
  success &= setProtectionConfig(protection);

  return success;
}

// Direct Register Access
bool BQ4050::writeConfigRegister(uint16_t address, uint8_t value) {
  return writeDataFlash(address, value);
}

uint8_t BQ4050::readConfigRegister(uint16_t address) {
  return readDataFlash(address);
}

// Utility Methods

uint8_t BQ4050::calculatePEC(const uint8_t* data, uint8_t length) {
  // PEC calculation using CRC-8 with polynomial x^8 + x^2 + x^1 + 1
  // SMBus uses CRC-8 for Packet Error Check
  // Lookup table implementation for better performance (based on NMIoT implementation)
  static const uint8_t crc8_lookup[256] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
  };

  uint8_t crc = 0x00;
  for (uint8_t i = 0; i < length; i++) {
    crc = crc8_lookup[crc ^ data[i]];
  }

  return crc;
}

bool BQ4050::validatePEC(const uint8_t* data, uint8_t length, uint8_t expectedPEC) {
  uint8_t calculatedPEC = calculatePEC(data, length);
  if (calculatedPEC != expectedPEC) {
    setError(BQ4050_ERROR_PEC_MISMATCH);
    return false;
  }
  return true;
}
