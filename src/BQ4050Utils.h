#ifndef BQ4050UTILS_H
#define BQ4050UTILS_H

#include <Arduino.h>

// Device Type Constants
#define BQ4050_DEVICE_TYPE 0x4050
#define BQ20Z45_DEVICE_TYPE 0x0045
#define BQ40Z50_DEVICE_TYPE 0x0550


// Battery Chemistry Types
enum BQ4050_ChemistryType {
  CHEMISTRY_UNKNOWN = 0,
  CHEMISTRY_LIION,
  CHEMISTRY_LIFEPO4,
  CHEMISTRY_LICOO2,
  CHEMISTRY_LIMN2O4,
  CHEMISTRY_NMC,
  CHEMISTRY_LTO
};

// Battery Status Flags (from Battery Status register 0x16)
enum BQ4050_BatteryStatusFlags {
  BATTERY_STATUS_OCA = 0x0001,          // Over Charged Alarm
  BATTERY_STATUS_TCA = 0x0002,          // Terminate Charge Alarm  
  BATTERY_STATUS_OTA = 0x1000,          // Over Temperature Alarm
  BATTERY_STATUS_TDA = 0x2000,          // Terminate Discharge Alarm
  BATTERY_STATUS_RCA = 0x4000,          // Remaining Capacity Alarm
  BATTERY_STATUS_RTA = 0x8000           // Remaining Time Alarm
};

// Compact bitfield union for battery status
union BatteryStatusBits {
  uint16_t raw;
  struct {
    uint16_t oca : 1;           // Over Charged Alarm
    uint16_t tca : 1;           // Terminate Charge Alarm
    uint16_t reserved1 : 10;    // Reserved bits 2-11
    uint16_t ota : 1;           // Over Temperature Alarm
    uint16_t tda : 1;           // Terminate Discharge Alarm
    uint16_t rca : 1;           // Remaining Capacity Alarm
    uint16_t rta : 1;           // Remaining Time Alarm
  } bits;
};

// Complete Safety Status/Alert Flags (32-bit register)
enum BQ4050_SafetyFlags {
  // Low 16 bits (bits 0-15)
  SAFETY_CUV    = 0x00000001,  // Bit 0: Cell Undervoltage
  SAFETY_COV    = 0x00000002,  // Bit 1: Cell Overvoltage
  SAFETY_OCC1   = 0x00000004,  // Bit 2: Overcurrent During Charge 1
  SAFETY_OCC2   = 0x00000008,  // Bit 3: Overcurrent During Charge 2
  SAFETY_OCD1   = 0x00000010,  // Bit 4: Overcurrent During Discharge 1
  SAFETY_OCD2   = 0x00000020,  // Bit 5: Overcurrent During Discharge 2
  SAFETY_AOLD   = 0x00000040,  // Bit 6: Overload During Discharge (PFStatus only)
  SAFETY_AOLDL  = 0x00000080,  // Bit 7: Overload During Discharge Latch
  SAFETY_ASCC   = 0x00000100,  // Bit 8: Short-Circuit During Charge (PFStatus only)
  SAFETY_ASCCL  = 0x00000200,  // Bit 9: Short-Circuit During Charge Latch
  SAFETY_ASCD   = 0x00000400,  // Bit 10: Short-Circuit During Discharge (PFStatus only)
  SAFETY_ASCDL  = 0x00000800,  // Bit 11: Short-Circuit During Discharge Latch
  SAFETY_OTC    = 0x00001000,  // Bit 12: Overtemperature During Charge
  SAFETY_OTD    = 0x00002000,  // Bit 13: Overtemperature During Discharge
  SAFETY_CUVC   = 0x00004000,  // Bit 14: Cell Undervoltage Compensated
  // Bit 15: Reserved
  
  // High 16 bits (bits 16-31)
  SAFETY_OTF    = 0x00010000,  // Bit 16: Overtemperature FET
  // Bit 17: Reserved
  SAFETY_PTO    = 0x00040000,  // Bit 18: Precharge Timeout
  SAFETY_PTOS   = 0x00080000,  // Bit 19: Precharge Timeout Suspend
  SAFETY_CTO    = 0x00100000,  // Bit 20: Charge Timeout
  SAFETY_CTOS   = 0x00200000,  // Bit 21: Charge Timeout Suspend
  SAFETY_OC     = 0x00400000,  // Bit 22: Overcharge
  SAFETY_CHGC   = 0x00800000,  // Bit 23: Overcharging Current
  SAFETY_CHGV   = 0x01000000,  // Bit 24: Overcharging Voltage
  SAFETY_PCHGC  = 0x02000000,  // Bit 25: Over-Precharge Current
  SAFETY_UTC    = 0x04000000,  // Bit 26: Undertemperature During Charge
  SAFETY_UTD    = 0x08000000   // Bit 27: Undertemperature During Discharge
  // Bits 28-31: Reserved
};

// Compact bitfield union for safety status (32-bit)
union SafetyStatusBits {
  uint32_t raw;
  struct {
    // Low 16 bits
    uint32_t cuv : 1;     // Cell Undervoltage
    uint32_t cov : 1;     // Cell Overvoltage  
    uint32_t occ1 : 1;    // Overcurrent Charge 1
    uint32_t occ2 : 1;    // Overcurrent Charge 2
    uint32_t ocd1 : 1;    // Overcurrent Discharge 1
    uint32_t ocd2 : 1;    // Overcurrent Discharge 2
    uint32_t aold : 1;    // Overload Discharge
    uint32_t aoldl : 1;   // Overload Discharge Latch
    uint32_t ascc : 1;    // Short-Circuit Charge
    uint32_t asccl : 1;   // Short-Circuit Charge Latch
    uint32_t ascd : 1;    // Short-Circuit Discharge
    uint32_t ascdl : 1;   // Short-Circuit Discharge Latch
    uint32_t otc : 1;     // Overtemperature Charge
    uint32_t otd : 1;     // Overtemperature Discharge
    uint32_t cuvc : 1;    // Cell Undervoltage Compensated
    uint32_t reserved1 : 1; // Bit 15
    // High 16 bits  
    uint32_t otf : 1;     // Overtemperature FET
    uint32_t reserved2 : 1; // Bit 17
    uint32_t pto : 1;     // Precharge Timeout
    uint32_t ptos : 1;    // Precharge Timeout Suspend
    uint32_t cto : 1;     // Charge Timeout
    uint32_t ctos : 1;    // Charge Timeout Suspend
    uint32_t oc : 1;      // Overcharge
    uint32_t chgc : 1;    // Overcharging Current
    uint32_t chgv : 1;    // Overcharging Voltage
    uint32_t pchgc : 1;   // Over-Precharge Current
    uint32_t utc : 1;     // Undertemperature Charge
    uint32_t utd : 1;     // Undertemperature Discharge
    uint32_t reserved3 : 4; // Bits 28-31
  } bits;
};

// PF (Permanent Failure) Alert/Status Flags (32-bit register)
enum BQ4050_PFFlags {
  PF_SUV      = 0x00000001,  // Bit 0: Safety Cell Undervoltage Failure
  PF_SOV      = 0x00000002,  // Bit 1: Safety Cell Overvoltage Failure
  PF_SOCC     = 0x00000004,  // Bit 2: Safety Overcurrent in Charge
  PF_SOCD     = 0x00000008,  // Bit 3: Safety Overcurrent in Discharge
  PF_SOT      = 0x00000010,  // Bit 4: Safety Overtemperature Cell Failure
  // Bit 5: Reserved
  PF_SOTF     = 0x00000040,  // Bit 6: Safety Overtemperature FET Failure
  // Bits 7-10: Reserved
  PF_VIMR     = 0x00000800,  // Bit 11: Voltage Imbalance While Pack At Rest Failure
  PF_VIMA     = 0x00001000,  // Bit 12: Voltage Imbalance While Pack Is Active Failure
  // Bits 13-15: Reserved
  PF_CFETF    = 0x00010000,  // Bit 16: Charge FET Failure
  PF_DFETF    = 0x00020000,  // Bit 17: Discharge FET Failure
  // Bit 18: Reserved
  PF_FUSE     = 0x00080000,  // Bit 19: Chemical Fuse Failure
  PF_AFER     = 0x00100000,  // Bit 20: AFE Register Failure
  PF_AFEC     = 0x00200000,  // Bit 21: AFE Communication Failure
  PF_2LVL     = 0x00400000,  // Bit 22: Second Level Protector Failure
  PF_PTC      = 0x00800000,  // Bit 23: PTC Failure (PFStatus only)
  PF_IFC      = 0x01000000,  // Bit 24: Instruction Flash Checksum Failure (PFStatus only)
  PF_OPNCELL  = 0x02000000,  // Bit 25: Open Cell Tab Connection Failure
  PF_DFW      = 0x04000000,  // Bit 26: Data Flash Wearout Failure (PFStatus only)
  // Bit 27: Reserved
  PF_TS1      = 0x10000000,  // Bit 28: Open Thermistor–TS1 Failure
  PF_TS2      = 0x20000000,  // Bit 29: Open Thermistor–TS2 Failure
  PF_TS3      = 0x40000000,  // Bit 30: Open Thermistor–TS3 Failure
  PF_TS4      = 0x80000000   // Bit 31: Open Thermistor–TS4 Failure
};

// Operation Status Flags (32-bit register)
enum BQ4050_OperationFlags {
  OP_PRES     = 0x00000001,  // Bit 0: System present low
  OP_DSG      = 0x00000002,  // Bit 1: DSG FET status
  OP_CHG      = 0x00000004,  // Bit 2: CHG FET status
  OP_PCHG     = 0x00000008,  // Bit 3: Precharge FET status
  // Bit 4: Reserved
  OP_FUSE     = 0x00000020,  // Bit 5: Fuse status
  OP_SMOOTH   = 0x00000040,  // Bit 6: Smoothing active status
  OP_BTP_INT  = 0x00000080,  // Bit 7: Battery Trip Point Interrupt
  OP_SEC0     = 0x00000100,  // Bit 8: SECURITY mode bit 0
  OP_SEC1     = 0x00000200,  // Bit 9: SECURITY mode bit 1
  OP_SDV      = 0x00000400,  // Bit 10: Shutdown triggered via low pack voltage
  OP_SS       = 0x00000800,  // Bit 11: SAFETY mode status
  OP_PF       = 0x00001000,  // Bit 12: PERMANENT FAILURE mode status
  OP_XDSG     = 0x00002000,  // Bit 13: Discharging disabled
  OP_XCHG     = 0x00004000,  // Bit 14: Charging disabled
  OP_SLEEP    = 0x00008000,  // Bit 15: SLEEP mode conditions met
  OP_SDM      = 0x00010000,  // Bit 16: Shutdown triggered via command
  OP_LED      = 0x00020000,  // Bit 17: LED Display
  OP_AUTH     = 0x00040000,  // Bit 18: Authentication in progress
  OP_AUTOCALM = 0x00080000,  // Bit 19: Auto CC Offset Calibration
  OP_CAL      = 0x00100000,  // Bit 20: Calibration Output
  OP_CAL_OFFSET = 0x00200000, // Bit 21: Calibration Output (raw CC offset data)
  OP_XL       = 0x00400000,  // Bit 22: 400-kHz SMBus mode
  OP_SLEEPM   = 0x00800000,  // Bit 23: SLEEP mode triggered via command
  OP_INIT     = 0x01000000,  // Bit 24: Initialization after full reset
  OP_SMBLCAL  = 0x02000000,  // Bit 25: Auto CC calibration when the bus is low
  OP_SLPAD    = 0x04000000,  // Bit 26: ADC Measurement in SLEEP mode
  OP_SLPCC    = 0x08000000,  // Bit 27: CC Measurement in SLEEP mode
  OP_CB       = 0x10000000,  // Bit 28: Cell balancing status
  OP_EMSHUT   = 0x20000000   // Bit 29: Emergency Shutdown
  // Bits 30-31: Reserved
};

// Charging Status Flags (16-bit register)
enum BQ4050_ChargingFlags {
  CHG_PV      = 0x0001,  // Bit 0: Precharge Voltage Region
  CHG_LV      = 0x0002,  // Bit 1: Low Voltage Region
  CHG_MV      = 0x0004,  // Bit 2: Mid Voltage Region
  CHG_HV      = 0x0008,  // Bit 3: High Voltage Region
  CHG_IN      = 0x0010,  // Bit 4: Charge Inhibit
  CHG_SU      = 0x0020,  // Bit 5: Charge Suspend
  CHG_MCHG    = 0x0040,  // Bit 6: Maintenance Charge
  CHG_VCT     = 0x0080,  // Bit 7: Charge Termination
  CHG_CCR     = 0x0100,  // Bit 8: Charging Current Rate of Change
  CHG_CVR     = 0x0200,  // Bit 9: Charging Voltage Rate of Change
  CHG_CCC     = 0x0400,  // Bit 10: Charging Loss Compensation
  // Bits 11-14: Reserved
  CHG_TAPER   = 0x8000   // Bit 15: Taper voltage condition satisfied
};

// Structure to hold safety flag information
struct SafetyFlagInfo {
  uint32_t flag;
  String code;
  String description;
};

class BQ4050Utils {
public:
  // Device identification utilities
  
  // Chemistry utilities
  static BQ4050_ChemistryType parseChemistryString(const String& chemistryStr);
  static String getChemistryName(BQ4050_ChemistryType chemistry);
  
  // Status interpretation utilities
  static String parseBatteryStatus(uint16_t batteryStatus);
  static String parseSafetyStatus(uint16_t safetyStatus);
  static String parseSafetyAlert(uint16_t safetyAlert);
  
  // Enhanced safety flag utilities
  static String parseSafetyFlags(uint32_t safetyFlags, bool includeCode = true);
  static String getSafetyFlagCode(uint32_t flag);
  static String getSafetyFlagDescription(uint32_t flag);
  static SafetyFlagInfo getSafetyFlagInfo(uint32_t flag);
  static String listAllActiveSafetyFlags(uint32_t safetyFlags);
  
  // PF flag utilities
  static String parsePFFlags(uint32_t pfFlags, bool includeCode = true);
  static String getPFFlagCode(uint32_t flag);
  static String getPFFlagDescription(uint32_t flag);
  static String listAllActivePFFlags(uint32_t pfFlags);
  
  // Operation status utilities
  static String parseOperationFlags(uint32_t operationFlags, bool includeCode = true);
  static String getOperationFlagCode(uint32_t flag);
  static String getOperationFlagDescription(uint32_t flag);
  static String listAllActiveOperationFlags(uint32_t operationFlags);
  
  // Charging status utilities
  static String parseChargingFlags(uint16_t chargingFlags, bool includeCode = true);
  static String getChargingFlagCode(uint16_t flag);
  static String getChargingFlagDescription(uint16_t flag);
  static String listAllActiveChargingFlags(uint16_t chargingFlags);
  
  // Voltage and current formatting utilities
  static String formatVoltage(float voltage, unsigned int decimals = 3);
  static String formatCurrent(float current, unsigned int decimals = 3);
  static String formatTemperature(float temperature, unsigned int decimals = 1);
  static String formatCapacity(uint16_t capacity);
  static String formatPercentage(uint8_t percentage);
  static String formatCycleCount(uint16_t cycleCount);
  static String formatManufacturerDate(uint16_t dateCode);
  static String formatSerialNumber(uint16_t serialNumber);
  
  // Validation utilities
  static bool isValidDeviceType(uint16_t deviceType);
  static bool isValidCycleCount(uint16_t cycleCount);
  
  // Helper functions for common operations
  static String getFullDeviceInfo(uint16_t deviceType, uint16_t fwVersion, uint16_t hwVersion);
  static String getBatteryHealthSummary(uint16_t safetyStatus, uint16_t safetyAlert, uint16_t batteryStatus);
  static String getSecurityModeDescription(const String& securityMode);
  
  // Bitfield union utility functions
  static BatteryStatusBits parseBatteryStatusBits(uint16_t batteryStatus);
  static SafetyStatusBits parseSafetyStatusBits(uint32_t safetyStatus);
  static String describeBatteryStatusBits(const BatteryStatusBits& status);
  static String describeSafetyStatusBits(const SafetyStatusBits& status);
  
private:
  // Internal helper functions
  static void appendStatusFlag(String& result, bool condition, const String& flagName);
};

#endif