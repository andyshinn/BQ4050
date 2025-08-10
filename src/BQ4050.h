#ifndef BQ4050_H
#define BQ4050_H

#include <Arduino.h>
#include <Wire.h>

// Optional utilities inclusion
// Define BQ4050_INCLUDE_UTILS to include utility functions for human-readable output
// If not defined, only core BQ4050 functionality is included (saves ~8KB)
// Can be defined in platformio.ini with: build_flags = -DBQ4050_INCLUDE_UTILS

/*
 * BQ4050 Command Structure:
 * 
 * 1. Regular SBS Commands (0x01-0x5F): Standard Smart Battery commands, work in sealed mode
 * 2. Manufacturer Access (0x00 + subcommand): BQ4050-specific commands, may require unsealed mode
 * 
 * Note: Some registers (0x50-0x57) are available both ways:
 * - As direct SBS commands (preferred for sealed devices)
 * - As manufacturer access commands (for advanced features)
 */

// Enhanced debug macros - define BQ4050_DEBUG to enable debug output
#ifdef BQ4050_DEBUG
  #define BQ4050_DEBUG_PRINT(x) Serial.println(F("[BQ4050] ") + String(x))
  #define BQ4050_DEBUG_PRINTF(fmt, ...) Serial.printf("[BQ4050] " fmt "\n", ##__VA_ARGS__)
  #define BQ4050_DEBUG_HEX(label, value) Serial.printf("[BQ4050] %s: 0x%04X\n", label, value)
  #define BQ4050_DEBUG_BEGIN() Serial.println(F("[BQ4050] === Debug Output Enabled ==="))
#else
  #define BQ4050_DEBUG_PRINT(x)
  #define BQ4050_DEBUG_PRINTF(fmt, ...)
  #define BQ4050_DEBUG_HEX(label, value)
  #define BQ4050_DEBUG_BEGIN()
#endif

// Regular SBS Command Register Addresses (Standard Smart Battery System commands)
#define BQ4050_CMD_REMAINING_CAPACITY_ALARM     0x01
#define BQ4050_CMD_REMAINING_TIME_ALARM         0x02
#define BQ4050_CMD_BATTERY_MODE                 0x03
#define BQ4050_CMD_TEMPERATURE                  0x08
#define BQ4050_CMD_VOLTAGE                      0x09
#define BQ4050_CMD_CURRENT                      0x0A
#define BQ4050_CMD_AVERAGE_CURRENT              0x0B
#define BQ4050_CMD_RELATIVE_STATE_OF_CHARGE     0x0D
#define BQ4050_CMD_ABSOLUTE_STATE_OF_CHARGE     0x0E
#define BQ4050_CMD_REMAINING_CAPACITY           0x0F
#define BQ4050_CMD_FULL_CHARGE_CAPACITY         0x10
#define BQ4050_CMD_BATTERY_STATUS               0x16
#define BQ4050_CMD_CYCLE_COUNT                  0x17
#define BQ4050_CMD_DESIGN_CAPACITY              0x18
#define BQ4050_CMD_DESIGN_VOLTAGE               0x19
#define BQ4050_CMD_MANUFACTURER_DATE            0x1B
#define BQ4050_CMD_SERIAL_NUMBER                0x1C
#define BQ4050_CMD_MANUFACTURER_NAME            0x20
#define BQ4050_CMD_DEVICE_NAME                  0x21
#define BQ4050_CMD_DEVICE_CHEMISTRY             0x22
#define BQ4050_CMD_MANUFACTURER_DATA            0x23
#define BQ4050_CMD_CELL_VOLTAGE_4               0x3C
#define BQ4050_CMD_CELL_VOLTAGE_3               0x3D
#define BQ4050_CMD_CELL_VOLTAGE_2               0x3E
#define BQ4050_CMD_CELL_VOLTAGE_1               0x3F
#define BQ4050_CMD_SAFETY_ALERT                 0x50
#define BQ4050_CMD_SAFETY_STATUS                0x51
#define BQ4050_CMD_PF_ALERT                     0x52
#define BQ4050_CMD_PF_STATUS                    0x53
#define BQ4050_CMD_OPERATION_STATUS             0x54
#define BQ4050_CMD_CHARGING_STATUS              0x55
#define BQ4050_CMD_GAUGING_STATUS               0x56
#define BQ4050_CMD_MANUFACTURING_STATUS         0x57
#define BQ4050_CMD_AFE_REGISTER                 0x58
#define BQ4050_CMD_LIFETIME_DATA_BLOCK_1        0x60
#define BQ4050_CMD_LIFETIME_DATA_BLOCK_2        0x61
#define BQ4050_CMD_LIFETIME_DATA_BLOCK_3        0x62
#define BQ4050_CMD_LIFETIME_DATA_BLOCK_4        0x63
#define BQ4050_CMD_LIFETIME_DATA_BLOCK_5        0x64
#define BQ4050_CMD_MANUFACTURER_INFO            0x70
#define BQ4050_CMD_DA_STATUS_1                  0x71
#define BQ4050_CMD_DA_STATUS_2                  0x72

// Manufacturer Access Command Codes (BQ4050-specific commands via 0x00 register)
//
// SEALED MODE AVAILABILITY:
// - Commands marked with [SEALED] are available in sealed mode
// - Unmarked commands require unsealed or full access mode
// - Most read commands are available in sealed mode
// - Most write/control commands require unsealed mode
// - Security and authentication commands require specific access levels
//
// USAGE: Write command to register 0x00, then read result from 0x00 (16-bit) or 0x23 (block data)
#define BQ4050_MAC_DEVICE_TYPE                  0x0001  // [SEALED] DeviceType - Read device identification
#define BQ4050_MAC_FIRMWARE_VERSION             0x0002  // [SEALED] FirmwareVersion - Read firmware info
#define BQ4050_MAC_HARDWARE_VERSION             0x0003  // [SEALED] HardwareVersion - Read hardware revision
#define BQ4050_MAC_IF_CHECKSUM                  0x0004  // [SEALED] IFChecksum - Instruction flash signature
#define BQ4050_MAC_STATIC_DF_SIGNATURE          0x0005  // [SEALED] StaticDFSignature - Static data flash checksum
#define BQ4050_MAC_ALL_DF_SIGNATURE             0x0009  // [SEALED] AllDFSignature - All data flash checksum
#define BQ4050_MAC_SHUTDOWN_MODE                0x0010  // [SEALED] ShutdownMode - Write command to shutdown
#define BQ4050_MAC_SLEEP_MODE                   0x0011  // SleepMode - Write command to enter sleep (unsealed only)
#define BQ4050_MAC_FUSE_TOGGLE                  0x001D  // FuseToggle - Write command (unsealed only)
#define BQ4050_MAC_PRECHARGE_FET_CONTROL        0x001E  // PrechargeFET - Write command (unsealed only)
#define BQ4050_MAC_CHARGE_FET_CONTROL           0x001F  // ChargeFET - Write command (unsealed only)
#define BQ4050_MAC_DISCHARGE_FET_CONTROL        0x0020  // DischargeFET - Write command (unsealed only)
#define BQ4050_MAC_FET_CONTROL                  0x0022  // FETControl - Write command (unsealed only)
#define BQ4050_MAC_LIFETIME_DATA_COLLECTION     0x0023  // LifetimeDataCollection - Write command (unsealed only)
#define BQ4050_MAC_PERMANENT_FAILURE            0x0024  // PermanentFailure - Write command (unsealed only)
#define BQ4050_MAC_BLACK_BOX_RECORDER           0x0025  // BlackBoxRecorder - Write command (unsealed only)
#define BQ4050_MAC_FUSE                         0x0026  // Fuse - Write command (unsealed only)
#define BQ4050_MAC_LIFETIME_DATA_RESET          0x0028  // LifetimeDataReset - Write command (unsealed only)
#define BQ4050_MAC_PERMANENT_FAILURE_DATA_RESET 0x0029  // PermanentFailureDataReset - Write command (unsealed only)
#define BQ4050_MAC_BLACK_BOX_RECORDER_RESET     0x002A  // BlackBoxRecorderReset - Write command (unsealed only)
#define BQ4050_MAC_CALIBRATION_MODE             0x002D  // CalibrationMode - Write command (unsealed only)
#define BQ4050_MAC_LIFETIME_DATA_FLUSH          0x002E  // LifetimeDataFlush - Write command (unsealed only)
#define BQ4050_MAC_LIFETIME_DATA_SPEEDUP_MODE   0x002F  // LifetimeDataSpeedUpMode - Write command (unsealed only)
#define BQ4050_MAC_SEAL_DEVICE                  0x0030  // SealDevice - Write command (unsealed only)
#define BQ4050_MAC_SECURITY_KEYS                0x0035  // SecurityKeys - Read/Write (unsealed only)
#define BQ4050_MAC_AUTHENTICATION_KEY           0x0037  // AuthenticationKey - Read/Write (full access only)
#define BQ4050_MAC_RESET_DEVICE                 0x0041  // DeviceReset - Write command (unsealed only)
// Status commands (available in sealed mode)
#define BQ4050_MAC_SAFETY_ALERT                 0x0050  // [SEALED] SafetyAlert - Read safety alerts
#define BQ4050_MAC_SAFETY_STATUS                0x0051  // [SEALED] SafetyStatus - Read safety status
#define BQ4050_MAC_PF_ALERT                     0x0052  // [SEALED] PFAlert - Read permanent failure alerts
#define BQ4050_MAC_PF_STATUS                    0x0053  // [SEALED] PFStatus - Read permanent failure status
#define BQ4050_MAC_OPERATION_STATUS             0x0054  // [SEALED] OperationStatus - Read operation status
#define BQ4050_MAC_CHARGING_STATUS              0x0055  // [SEALED] ChargingStatus - Read charging status
#define BQ4050_MAC_GAUGING_STATUS               0x0056  // [SEALED] GaugingStatus - Read gauging status
#define BQ4050_MAC_MANUFACTURING_STATUS         0x0057  // [SEALED] ManufacturingStatus - Read manufacturing status
#define BQ4050_MAC_AFE_REGISTER                 0x0058  // [SEALED] AFERegister - Read AFE register
#define BQ4050_MAC_LIFETIME_DATA_BLOCK_1        0x0060  // [SEALED] LifetimeDataBlock1 - Read lifetime data block 1
#define BQ4050_MAC_LIFETIME_DATA_BLOCK_2        0x0061  // [SEALED] LifetimeDataBlock2 - Read lifetime data block 2
#define BQ4050_MAC_LIFETIME_DATA_BLOCK_3        0x0062  // [SEALED] LifetimeDataBlock3 - Read lifetime data block 3
#define BQ4050_MAC_MANUFACTURER_INFO            0x0070  // [SEALED] ManufacturerInfo - Read manufacturer information
#define BQ4050_MAC_DA_STATUS_1                  0x0071  // [SEALED] DAStatus1 - Read DA status 1
#define BQ4050_MAC_DA_STATUS_2                  0x0072  // [SEALED] DAStatus2 - Read DA status 2
#define BQ4050_MAC_MANUFACTURER_INFO_2          0x007A  // [SEALED] ManufacturerInfo2 - Read additional manufacturer info
// Special commands
#define BQ4050_MAC_ROM_MODE                     0x0F00  // ROMMode - Write command (unsealed only)
#define BQ4050_MAC_EXIT_CALIBRATION_OUTPUT      0xF080  // ExitCalibrationOutput - Read/Write (unsealed only)
#define BQ4050_MAC_OUTPUT_CC_ADC_CALIBRATION    0xF081  // OutputCCandADCforCalibration - Read/Write (unsealed only)

// Data Flash Address Range
#define BQ4050_DATA_FLASH_START                 0x4000
#define BQ4050_DATA_FLASH_END                   0x5FFF

// Error codes
enum BQ4050_Error {
  BQ4050_ERROR_NONE = 0,
  BQ4050_ERROR_I2C_TIMEOUT,
  BQ4050_ERROR_I2C_NACK,
  BQ4050_ERROR_INVALID_PARAMETER,
  BQ4050_ERROR_CRC_MISMATCH,
  BQ4050_ERROR_PEC_MISMATCH,
  BQ4050_ERROR_DEVICE_NOT_FOUND
};

// Security modes
enum BQ4050_SecurityMode {
  BQ4050_SECURITY_SEALED = 0,
  BQ4050_SECURITY_UNSEALED = 1,
  BQ4050_SECURITY_FULL_ACCESS = 2,
  BQ4050_SECURITY_UNKNOWN = 3
};

// Cell count enumeration
enum CellCount {
  ONE_CELL = 0,
  TWO_CELL = 1,
  THREE_CELL = 2,
  FOUR_CELL = 3
};

// Structure definitions for convenience methods
struct CellStatus {
  float voltage1, voltage2, voltage3, voltage4;
  bool balancing1, balancing2, balancing3, balancing4;
};

struct TemperatureStatus {
  float internal;
  float ts1, ts2, ts3, ts4;
  float cellTemp;
  float fetTemp;
};

struct BatteryInfo {
  float voltage, current, temperature;
  int soc, cycleCount;
  int remainingCapacity, fullCapacity;
  bool charging, discharging;
  uint16_t batteryStatus;
  bool overVoltage, underVoltage;
  bool overTemperature, underTemperature;
  bool overCurrent, shortCircuit;
};

struct SafetyStatus {
  bool overVoltage, underVoltage;
  bool overTemperature, underTemperature;
  bool overCurrent, shortCircuit;
  bool cellImbalance;
  bool chargeFETFailure, dischargeFETFailure;
  uint16_t safetyAlert;
  uint16_t safetyStatusRaw;
};

// CEDV Structures
struct CEDVStatus {
  bool edv0Reached;
  bool edv1Reached;
  bool edv2Reached;
  bool fccUpdated;
  bool smoothingActive;
  bool vdq;
  bool qualifiedDischarge;
};

struct CEDVConfig {
  uint16_t emf;
  uint16_t c0;
  uint16_t r0;
  uint16_t t0;
  uint16_t r1;
  uint8_t tc;
  uint8_t c1;
  uint8_t ageFactor;
  uint16_t batteryLowPercent;
};

struct CEDVProfile {
  uint16_t voltage0DOD;
  uint16_t voltage10DOD;
  uint16_t voltage20DOD;
  uint16_t voltage30DOD;
  uint16_t voltage40DOD;
  uint16_t voltage50DOD;
  uint16_t voltage60DOD;
  uint16_t voltage70DOD;
  uint16_t voltage80DOD;
  uint16_t voltage90DOD;
  uint16_t voltage100DOD;
};

struct CEDVSmoothingConfig {
  uint16_t smoothingStartVoltage;
  uint16_t smoothingDeltaVoltage;
  uint16_t maxSmoothingCurrent;
  uint8_t eocSmoothCurrent;
  uint8_t eocSmoothCurrentTime;
  bool smoothToEDV0;
  bool smoothToEDV1;
  bool extendedSmoothing;
};

struct CEDVInfo {
  CEDVStatus status;
  float currentEDV0, currentEDV1, currentEDV2;
  bool compensationEnabled;
  bool smoothingActive;
  int remainingToEDV2;
  int remainingToEDV1;
  int remainingToEDV0;
};

// Settings Flash Configuration Structures
struct DAConfiguration {
  bool fetTempAverage;
  bool cellTempAverage;
  bool emergencyShutdown;
  bool sleepMode;
  bool inSystemSleep;
  bool nonRemovable;
  CellCount cellCount;
};

struct FETOptions {
  bool overTempFETDisable;
  bool chargeInhibitFET;
  bool chargeSuspendFET;
  bool prechargeComm;
};

struct PowerConfig {
  bool autoShipEnable;
};

struct IOConfig {
  bool btpEnable;
  bool btpPolarity;
};

struct TemperatureConfig {
  bool ts1Enable, ts2Enable, ts3Enable, ts4Enable;
  bool internalTSEnable;
  bool ts1CellMode, ts2CellMode, ts3CellMode, ts4CellMode;
  bool internalCellMode;
};

struct LEDConfig {
  uint16_t displayMask;
  bool ledEnable;
  uint8_t blinkRate;
  uint8_t flashRate;
};

struct BalancingConfig {
  bool cellBalancingEnable;
  uint16_t balanceVoltage;
  uint16_t balanceTime;
};

struct SBSGaugingConfig {
  bool rsocHold;
  bool capacitySync;
  bool smoothSOC;
};

struct SBSConfig {
  bool specificationMode;
  uint8_t smbusTimeout;
  bool packetErrorCheck;
};

struct SOCFlagConfig {
  bool tcSetOnCharge;
  bool fcSetOnCharge;
  bool tcClearOnRSOC;
  bool tdClearOnRSOC;
  uint8_t rsocThresholds;
};

struct ProtectionConfig {
  bool protectionEnable;
  uint8_t protectionDelay;
};

struct FullConfiguration {
  DAConfiguration daConfig;
  FETOptions fetOptions;
  PowerConfig powerConfig;
  IOConfig ioConfig;
  TemperatureConfig tempConfig;
  LEDConfig ledConfig;
  BalancingConfig balanceConfig;
  SBSGaugingConfig sbsGauging;
  SBSConfig sbsConfig;
  SOCFlagConfig socFlags;
  ProtectionConfig protection;
};

class BQ4050 {
public:
  explicit BQ4050(uint8_t address = 0x0B, TwoWire& wire = Wire);
  
  // Flexible initialization methods
  bool begin();                                          // Use default I2C
  bool begin(TwoWire& wire);                            // Specify I2C interface
  bool begin(int sda, int scl);                         // Specify I2C pins (ESP32 style)
  bool begin(int sda, int scl, uint32_t frequency);     // Specify pins and frequency

  // Basic SBS Commands
  uint16_t getRemainingCapacityAlarm();
  uint16_t getRemainingTimeAlarm();
  uint16_t getBatteryMode();
  bool setBatteryMode(uint16_t mode);
  float getTemperature();
  float getVoltage();
  float getCurrent();
  float getAverageCurrent();
  uint8_t getRelativeStateOfCharge();
  uint8_t getAbsoluteStateOfCharge();
  uint16_t getRemainingCapacity();
  uint16_t getFullChargeCapacity();
  uint16_t getBatteryStatus();
  uint16_t getCycleCount();
  uint16_t getDesignCapacity();
  uint16_t getDesignVoltage();
  uint16_t getManufacturerDate();
  uint16_t getSerialNumber();

  // Cell Voltages
  float getCellVoltage1();
  float getCellVoltage2();
  float getCellVoltage3();
  float getCellVoltage4();

  // Status and Alerts
  uint16_t getSafetyAlert();
  uint16_t getSafetyStatus();
  uint16_t getPFAlert();
  uint16_t getPFStatus();
  uint16_t getOperationStatus();
  uint16_t getChargingStatus();
  uint16_t getGaugingStatus();
  uint16_t getManufacturingStatus();
  
  // Extended SBS Commands
  uint16_t getAFERegister();
  uint32_t getLifeTimeDataBlock1();
  uint32_t getLifeTimeDataBlock2();
  uint32_t getLifeTimeDataBlock3();
  uint32_t getLifeTimeDataBlock4();
  uint32_t getLifeTimeDataBlock5();
  String getManufacturerInfo();
  String getDAStatus1();
  String getDAStatus2();
  

  // Manufacturer Access Commands (basic 16-bit reads)
  uint16_t getDeviceType();           // [SEALED] 0x0001
  uint16_t getFirmwareVersion();      // [SEALED] 0x0002  
  uint16_t getHardwareVersion();      // [SEALED] 0x0003
  uint16_t getIFChecksum();           // [SEALED] 0x0004
  uint16_t getStaticDFSignature();    // [SEALED] 0x0005
  uint16_t getAllDFSignature();       // [SEALED] 0x0009
  
  // Enhanced manufacturer access commands (return full data blocks)
  String getDeviceTypeBlock();        // [SEALED] Full device type block
  String getFirmwareVersionBlock();   // [SEALED] Full firmware version block
  String getHardwareVersionBlock();   // [SEALED] Full hardware version block
  String getManufacturerName();
  String getDeviceName();
  String getDeviceChemistry();
  uint32_t getLifetimeDataBlock1();
  uint32_t getLifetimeDataBlock2();
  uint32_t getLifetimeDataBlock3();

  // FET Control
  bool enableChargeFET();
  bool disableChargeFET();
  bool enableDischargeFET();
  bool disableDischargeFET();
  bool enablePrechargeFET();
  bool disablePrechargeFET();
  bool setFETControl(uint8_t control);

  // Device Control
  bool enterCalibrationMode();
  bool sealDevice();
  bool resetDevice();
  bool enterSleepMode();
  bool enterShutdownMode();
  
  // Security Mode Detection
  BQ4050_SecurityMode getSecurityMode();
  String getSecurityModeString();
  bool isSealed();
  bool isUnsealed();
  bool hasFullAccess();

  // Data Flash Access
  uint8_t readDataFlash(uint16_t address);
  bool writeDataFlash(uint16_t address, uint8_t data);

  // Convenience Methods
  CellStatus getAllCellStatus();
  TemperatureStatus getAllTemperatures();
  BatteryInfo getCompleteBatteryStatus();
  SafetyStatus getParsedSafetyStatus();

  // Simple Status Methods
  bool isCharging();
  bool isDischarging();
  bool isBatteryHealthy();
  uint16_t getEstimatedRuntime();
  uint16_t getEstimatedChargeTime();
  uint8_t getStateOfHealth();

  // CEDV Methods
  CEDVStatus getCEDVStatus();
  float getEDV0Threshold();
  float getEDV1Threshold();
  float getEDV2Threshold();
  bool isEDVCompensationEnabled();
  CEDVConfig getCEDVConfig();
  bool setCEDVConfig(const CEDVConfig& config);
  bool enableEDVCompensation();
  bool disableEDVCompensation();
  bool setFixedEDV0(float voltage);
  bool setFixedEDV1(float voltage);
  bool setFixedEDV2(float voltage);
  bool isUsingFixedEDV();
  CEDVProfile getCEDVProfile();
  bool setCEDVProfile(const CEDVProfile& profile);
  CEDVSmoothingConfig getSmoothingConfig();
  bool setSmoothingConfig(const CEDVSmoothingConfig& config);
  bool isLearningDischarge();
  uint16_t getQualifiedDischargeCount();
  bool resetLearning();
  CEDVInfo getCompleteCEDVInfo();

  // Settings Flash Configuration
  CellCount getCellCount();
  bool setCellCount(CellCount count);
  DAConfiguration getDAConfiguration();
  bool setDAConfiguration(const DAConfiguration& config);
  FETOptions getFETOptions();
  bool setFETOptions(const FETOptions& options);
  PowerConfig getPowerConfig();
  bool setPowerConfig(const PowerConfig& config);
  IOConfig getIOConfig();
  bool setIOConfig(const IOConfig& config);
  TemperatureConfig getTemperatureConfig();
  bool setTemperatureConfig(const TemperatureConfig& config);
  LEDConfig getLEDConfig();
  bool setLEDConfig(const LEDConfig& config);
  BalancingConfig getBalancingConfig();
  bool setBalancingConfig(const BalancingConfig& config);
  SBSGaugingConfig getSBSGaugingConfig();
  bool setSBSGaugingConfig(const SBSGaugingConfig& config);
  SBSConfig getSBSConfig();
  bool setSBSConfig(const SBSConfig& config);
  SOCFlagConfig getSOCFlagConfig();
  bool setSOCFlagConfig(const SOCFlagConfig& config);
  ProtectionConfig getProtectionConfig();
  bool setProtectionConfig(const ProtectionConfig& config);

  // Quick Setup Methods
  bool configureFor1S(bool balancing = false);
  bool configureFor2S(bool balancing = true);
  bool configureFor3S(bool balancing = true);
  bool configureFor4S(bool balancing = true);
  bool configureForRemovableBattery();
  bool configureForEmbeddedBattery();
  bool configureForPortableDevice();
  bool configureForPowerBank();

  // Configuration Management
  bool validateConfiguration();
  bool isConfigurationValid();
  String getConfigurationErrors();
  FullConfiguration backupConfiguration();
  bool restoreConfiguration(const FullConfiguration& config);
  bool resetToFactoryDefaults();

  // Direct Register Access
  bool writeConfigRegister(uint16_t address, uint8_t value);
  uint8_t readConfigRegister(uint16_t address);

  // Error Handling
  BQ4050_Error getLastError() const;
  static String getErrorString(BQ4050_Error error);

  // Debug Support
  void setPECEnabled(bool enable);
  bool isPECEnabled() const;

private:
  uint8_t _address;
  TwoWire* _wire;
  BQ4050_Error _lastError;
  bool _pecEnabled;
  
  // Timing constants (microseconds)
  static const uint16_t I2C_RESPONSE_DELAY_US = 250;   // Delay after I2C write before read
  static const uint16_t I2C_TIMEOUT_MS = 100;          // I2C operation timeout
  static const uint8_t MAX_SBS_STRING_LENGTH = 32;     // Maximum SBS string length for buffer protection

  // I2C Communication Methods
  uint8_t readRegister8(uint8_t reg);
  uint16_t readRegister16(uint8_t reg);
  uint32_t readRegister32(uint8_t reg);
  bool writeRegister8(uint8_t reg, uint8_t value);
  bool writeRegister16(uint8_t reg, uint16_t value);
  
  // Enhanced I2C helper methods
  bool safeBeginTransmission(uint8_t reg);
  uint8_t safeRequestData(uint8_t bytesToRead);
  
  // Smart PEC management
  bool shouldUsePECForRegister(uint8_t reg) const;
  uint16_t readRegister16WithSmartPEC(uint8_t reg);
  uint32_t readRegister32WithSmartPEC(uint8_t reg);
  String readSBSStringWithSmartPEC(uint8_t reg);

  // Manufacturer Access Methods
  uint16_t manufacturerAccess16(uint16_t command);
  uint32_t manufacturerAccess32(uint16_t command);
  bool manufacturerAccessWrite(uint16_t command, uint16_t data);

  // Utility Methods
  static float convertTemperature(uint16_t rawTemp);
  static float convertVoltage(uint16_t rawVoltage);
  static float convertCurrent(int16_t rawCurrent);
  static uint8_t calculatePEC(const uint8_t* data, uint8_t length);
  bool validatePEC(const uint8_t* data, uint8_t length, uint8_t expectedPEC);
  void setError(BQ4050_Error error);
  
  // SBS Block Read Methods
  String readSBSString(uint8_t command);
};

// Conditionally include utility functions
#ifdef BQ4050_INCLUDE_UTILS
  #include "BQ4050Utils.h"
#endif

#endif
