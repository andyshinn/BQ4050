#include "BQ4050Utils.h"



BQ4050_ChemistryType BQ4050Utils::parseChemistryString(const String& chemistryStr) {
  String chemistry = chemistryStr;
  chemistry.toUpperCase();
  
  if (chemistry.indexOf("LIION") >= 0 || chemistry.indexOf("LI-ION") >= 0) {
    return CHEMISTRY_LIION;
  } else if (chemistry.indexOf("LIFEPO4") >= 0 || chemistry.indexOf("LFP") >= 0) {
    return CHEMISTRY_LIFEPO4;
  } else if (chemistry.indexOf("LICOO2") >= 0 || chemistry.indexOf("LCO") >= 0) {
    return CHEMISTRY_LICOO2;
  } else if (chemistry.indexOf("LIMN2O4") >= 0 || chemistry.indexOf("LMO") >= 0) {
    return CHEMISTRY_LIMN2O4;
  } else if (chemistry.indexOf("NMC") >= 0) {
    return CHEMISTRY_NMC;
  } else if (chemistry.indexOf("LTO") >= 0) {
    return CHEMISTRY_LTO;
  }
  
  return CHEMISTRY_UNKNOWN;
}

String BQ4050Utils::getChemistryName(BQ4050_ChemistryType chemistry) {
  switch (chemistry) {
    case CHEMISTRY_LIION: return "Li-Ion";
    case CHEMISTRY_LIFEPO4: return "LiFePO4";
    case CHEMISTRY_LICOO2: return "LiCoO2";
    case CHEMISTRY_LIMN2O4: return "LiMn2O4";
    case CHEMISTRY_NMC: return "NMC";
    case CHEMISTRY_LTO: return "LTO";
    default: return "Unknown";
  }
}

String BQ4050Utils::parseBatteryStatus(uint16_t batteryStatus) {
  String result = "";
  
  appendStatusFlag(result, batteryStatus & BATTERY_STATUS_OCA, "Over Charged Alarm");
  appendStatusFlag(result, batteryStatus & BATTERY_STATUS_TCA, "Terminate Charge Alarm");
  appendStatusFlag(result, batteryStatus & BATTERY_STATUS_OTA, "Over Temperature Alarm");
  appendStatusFlag(result, batteryStatus & BATTERY_STATUS_TDA, "Terminate Discharge Alarm");
  appendStatusFlag(result, batteryStatus & BATTERY_STATUS_RCA, "Remaining Capacity Alarm");
  appendStatusFlag(result, batteryStatus & BATTERY_STATUS_RTA, "Remaining Time Alarm");
  
  if (result.length() == 0) {
    result = "Normal";
  }
  
  return result;
}

String BQ4050Utils::parseSafetyStatus(uint16_t safetyStatus) {
  String result = "";
  
  appendStatusFlag(result, safetyStatus & SAFETY_CUV, "Cell Undervoltage");
  appendStatusFlag(result, safetyStatus & SAFETY_COV, "Cell Overvoltage");
  appendStatusFlag(result, safetyStatus & (SAFETY_OCC1 | SAFETY_OCC2), "Overcurrent Charge");
  appendStatusFlag(result, safetyStatus & (SAFETY_OCD1 | SAFETY_OCD2), "Overcurrent Discharge");
  appendStatusFlag(result, safetyStatus & SAFETY_AOLDL, "Overload During Discharge Latch");
  appendStatusFlag(result, safetyStatus & SAFETY_ASCDL, "Short Circuit During Discharge Latch");
  appendStatusFlag(result, safetyStatus & SAFETY_ASCCL, "Short Circuit During Charge Latch");
  appendStatusFlag(result, safetyStatus & SAFETY_PTO, "Precharge Timeout");
  appendStatusFlag(result, safetyStatus & SAFETY_CTO, "Charge Timeout");
  appendStatusFlag(result, safetyStatus & SAFETY_OTD, "Over Temperature Discharge");
  appendStatusFlag(result, safetyStatus & SAFETY_OTC, "Over Temperature Charge");
  appendStatusFlag(result, safetyStatus & SAFETY_OTF, "Over Temperature FET");
  appendStatusFlag(result, safetyStatus & SAFETY_UTC, "Under Temperature Charge");
  appendStatusFlag(result, safetyStatus & SAFETY_UTD, "Under Temperature Discharge");
  
  if (result.length() == 0) {
    result = "Normal";
  }
  
  return result;
}

String BQ4050Utils::parseSafetyAlert(uint16_t safetyAlert) {
  // Safety Alert uses the same bit definitions as Safety Status
  return parseSafetyStatus(safetyAlert);
}

String BQ4050Utils::parseSafetyFlags(uint32_t safetyFlags, bool includeCode) {
  String result = "";
  
  // Check each flag in order of severity/importance
  if (safetyFlags & SAFETY_ASCDL) {
    appendStatusFlag(result, true, includeCode ? "ASCDL: Short-Circuit During Discharge Latch" : "Short-Circuit During Discharge Latch");
  }
  if (safetyFlags & SAFETY_ASCCL) {
    appendStatusFlag(result, true, includeCode ? "ASCCL: Short-Circuit During Charge Latch" : "Short-Circuit During Charge Latch");
  }
  if (safetyFlags & SAFETY_COV) {
    appendStatusFlag(result, true, includeCode ? "COV: Cell Overvoltage" : "Cell Overvoltage");
  }
  if (safetyFlags & SAFETY_CUV) {
    appendStatusFlag(result, true, includeCode ? "CUV: Cell Undervoltage" : "Cell Undervoltage");
  }
  if (safetyFlags & SAFETY_CUVC) {
    appendStatusFlag(result, true, includeCode ? "CUVC: Cell Undervoltage Compensated" : "Cell Undervoltage Compensated");
  }
  if (safetyFlags & SAFETY_OCC1) {
    appendStatusFlag(result, true, includeCode ? "OCC1: Overcurrent During Charge 1" : "Overcurrent During Charge 1");
  }
  if (safetyFlags & SAFETY_OCC2) {
    appendStatusFlag(result, true, includeCode ? "OCC2: Overcurrent During Charge 2" : "Overcurrent During Charge 2");
  }
  if (safetyFlags & SAFETY_OCD1) {
    appendStatusFlag(result, true, includeCode ? "OCD1: Overcurrent During Discharge 1" : "Overcurrent During Discharge 1");
  }
  if (safetyFlags & SAFETY_OCD2) {
    appendStatusFlag(result, true, includeCode ? "OCD2: Overcurrent During Discharge 2" : "Overcurrent During Discharge 2");
  }
  if (safetyFlags & SAFETY_AOLDL) {
    appendStatusFlag(result, true, includeCode ? "AOLDL: Overload During Discharge Latch" : "Overload During Discharge Latch");
  }
  if (safetyFlags & SAFETY_OTC) {
    appendStatusFlag(result, true, includeCode ? "OTC: Overtemperature During Charge" : "Overtemperature During Charge");
  }
  if (safetyFlags & SAFETY_OTD) {
    appendStatusFlag(result, true, includeCode ? "OTD: Overtemperature During Discharge" : "Overtemperature During Discharge");
  }
  if (safetyFlags & SAFETY_OTF) {
    appendStatusFlag(result, true, includeCode ? "OTF: Overtemperature FET" : "Overtemperature FET");
  }
  if (safetyFlags & SAFETY_UTC) {
    appendStatusFlag(result, true, includeCode ? "UTC: Undertemperature During Charge" : "Undertemperature During Charge");
  }
  if (safetyFlags & SAFETY_UTD) {
    appendStatusFlag(result, true, includeCode ? "UTD: Undertemperature During Discharge" : "Undertemperature During Discharge");
  }
  if (safetyFlags & SAFETY_PTO) {
    appendStatusFlag(result, true, includeCode ? "PTO: Precharge Timeout" : "Precharge Timeout");
  }
  if (safetyFlags & SAFETY_PTOS) {
    appendStatusFlag(result, true, includeCode ? "PTOS: Precharge Timeout Suspend" : "Precharge Timeout Suspend");
  }
  if (safetyFlags & SAFETY_CTO) {
    appendStatusFlag(result, true, includeCode ? "CTO: Charge Timeout" : "Charge Timeout");
  }
  if (safetyFlags & SAFETY_CTOS) {
    appendStatusFlag(result, true, includeCode ? "CTOS: Charge Timeout Suspend" : "Charge Timeout Suspend");
  }
  if (safetyFlags & SAFETY_OC) {
    appendStatusFlag(result, true, includeCode ? "OC: Overcharge" : "Overcharge");
  }
  if (safetyFlags & SAFETY_CHGC) {
    appendStatusFlag(result, true, includeCode ? "CHGC: Overcharging Current" : "Overcharging Current");
  }
  if (safetyFlags & SAFETY_CHGV) {
    appendStatusFlag(result, true, includeCode ? "CHGV: Overcharging Voltage" : "Overcharging Voltage");
  }
  if (safetyFlags & SAFETY_PCHGC) {
    appendStatusFlag(result, true, includeCode ? "PCHGC: Over-Precharge Current" : "Over-Precharge Current");
  }
  
  if (result.length() == 0) {
    result = "Normal";
  }
  
  return result;
}

String BQ4050Utils::parsePFFlags(uint32_t pfFlags, bool includeCode) {
  String result = "";
  
  // Check each flag in order of severity
  if (pfFlags & PF_SUV) {
    appendStatusFlag(result, true, includeCode ? "SUV: Safety Cell Undervoltage Failure" : "Safety Cell Undervoltage Failure");
  }
  if (pfFlags & PF_SOV) {
    appendStatusFlag(result, true, includeCode ? "SOV: Safety Cell Overvoltage Failure" : "Safety Cell Overvoltage Failure");
  }
  if (pfFlags & PF_SOCC) {
    appendStatusFlag(result, true, includeCode ? "SOCC: Safety Overcurrent in Charge" : "Safety Overcurrent in Charge");
  }
  if (pfFlags & PF_SOCD) {
    appendStatusFlag(result, true, includeCode ? "SOCD: Safety Overcurrent in Discharge" : "Safety Overcurrent in Discharge");
  }
  if (pfFlags & PF_SOT) {
    appendStatusFlag(result, true, includeCode ? "SOT: Safety Overtemperature Cell Failure" : "Safety Overtemperature Cell Failure");
  }
  if (pfFlags & PF_SOTF) {
    appendStatusFlag(result, true, includeCode ? "SOTF: Safety Overtemperature FET Failure" : "Safety Overtemperature FET Failure");
  }
  if (pfFlags & PF_VIMR) {
    appendStatusFlag(result, true, includeCode ? "VIMR: Voltage Imbalance While Pack At Rest Failure" : "Voltage Imbalance While Pack At Rest Failure");
  }
  if (pfFlags & PF_VIMA) {
    appendStatusFlag(result, true, includeCode ? "VIMA: Voltage Imbalance While Pack Is Active Failure" : "Voltage Imbalance While Pack Is Active Failure");
  }
  if (pfFlags & PF_CFETF) {
    appendStatusFlag(result, true, includeCode ? "CFETF: Charge FET Failure" : "Charge FET Failure");
  }
  if (pfFlags & PF_DFETF) {
    appendStatusFlag(result, true, includeCode ? "DFETF: Discharge FET Failure" : "Discharge FET Failure");
  }
  if (pfFlags & PF_FUSE) {
    appendStatusFlag(result, true, includeCode ? "FUSE: Chemical Fuse Failure" : "Chemical Fuse Failure");
  }
  if (pfFlags & PF_AFER) {
    appendStatusFlag(result, true, includeCode ? "AFER: AFE Register Failure" : "AFE Register Failure");
  }
  if (pfFlags & PF_AFEC) {
    appendStatusFlag(result, true, includeCode ? "AFEC: AFE Communication Failure" : "AFE Communication Failure");
  }
  if (pfFlags & PF_2LVL) {
    appendStatusFlag(result, true, includeCode ? "2LVL: Second Level Protector Failure" : "Second Level Protector Failure");
  }
  if (pfFlags & PF_PTC) {
    appendStatusFlag(result, true, includeCode ? "PTC: PTC Failure" : "PTC Failure");
  }
  if (pfFlags & PF_IFC) {
    appendStatusFlag(result, true, includeCode ? "IFC: Instruction Flash Checksum Failure" : "Instruction Flash Checksum Failure");
  }
  if (pfFlags & PF_OPNCELL) {
    appendStatusFlag(result, true, includeCode ? "OPNCELL: Open Cell Tab Connection Failure" : "Open Cell Tab Connection Failure");
  }
  if (pfFlags & PF_DFW) {
    appendStatusFlag(result, true, includeCode ? "DFW: Data Flash Wearout Failure" : "Data Flash Wearout Failure");
  }
  if (pfFlags & PF_TS1) {
    appendStatusFlag(result, true, includeCode ? "TS1: Open Thermistor–TS1 Failure" : "Open Thermistor–TS1 Failure");
  }
  if (pfFlags & PF_TS2) {
    appendStatusFlag(result, true, includeCode ? "TS2: Open Thermistor–TS2 Failure" : "Open Thermistor–TS2 Failure");
  }
  if (pfFlags & PF_TS3) {
    appendStatusFlag(result, true, includeCode ? "TS3: Open Thermistor–TS3 Failure" : "Open Thermistor–TS3 Failure");
  }
  if (pfFlags & PF_TS4) {
    appendStatusFlag(result, true, includeCode ? "TS4: Open Thermistor–TS4 Failure" : "Open Thermistor–TS4 Failure");
  }
  
  if (result.length() == 0) {
    result = "Normal";
  }
  
  return result;
}

String BQ4050Utils::parseOperationFlags(uint32_t operationFlags, bool includeCode) {
  String result = "";
  
  if (operationFlags & OP_EMSHUT) {
    appendStatusFlag(result, true, includeCode ? "EMSHUT: Emergency Shutdown" : "Emergency Shutdown");
  }
  if (operationFlags & OP_PF) {
    appendStatusFlag(result, true, includeCode ? "PF: PERMANENT FAILURE mode status" : "PERMANENT FAILURE mode status");
  }
  if (operationFlags & OP_SS) {
    appendStatusFlag(result, true, includeCode ? "SS: SAFETY mode status" : "SAFETY mode status");
  }
  if (operationFlags & OP_XCHG) {
    appendStatusFlag(result, true, includeCode ? "XCHG: Charging disabled" : "Charging disabled");
  }
  if (operationFlags & OP_XDSG) {
    appendStatusFlag(result, true, includeCode ? "XDSG: Discharging disabled" : "Discharging disabled");
  }
  if (operationFlags & OP_SLEEP) {
    appendStatusFlag(result, true, includeCode ? "SLEEP: SLEEP mode conditions met" : "SLEEP mode conditions met");
  }
  if (operationFlags & OP_SDV) {
    appendStatusFlag(result, true, includeCode ? "SDV: Shutdown triggered via low pack voltage" : "Shutdown triggered via low pack voltage");
  }
  if (operationFlags & OP_SDM) {
    appendStatusFlag(result, true, includeCode ? "SDM: Shutdown triggered via command" : "Shutdown triggered via command");
  }
  if (operationFlags & OP_SLEEPM) {
    appendStatusFlag(result, true, includeCode ? "SLEEPM: SLEEP mode triggered via command" : "SLEEP mode triggered via command");
  }
  if (operationFlags & OP_CB) {
    appendStatusFlag(result, true, includeCode ? "CB: Cell balancing active" : "Cell balancing active");
  }
  if (operationFlags & OP_AUTH) {
    appendStatusFlag(result, true, includeCode ? "AUTH: Authentication in progress" : "Authentication in progress");
  }
  if (operationFlags & OP_LED) {
    appendStatusFlag(result, true, includeCode ? "LED: LED Display on" : "LED Display on");
  }
  if (operationFlags & OP_INIT) {
    appendStatusFlag(result, true, includeCode ? "INIT: Initialization after full reset" : "Initialization after full reset");
  }
  if (operationFlags & OP_CAL) {
    appendStatusFlag(result, true, includeCode ? "CAL: Calibration Output active" : "Calibration Output active");
  }
  if (operationFlags & OP_SMOOTH) {
    appendStatusFlag(result, true, includeCode ? "SMOOTH: Smoothing active" : "Smoothing active");
  }
  if (operationFlags & OP_FUSE) {
    appendStatusFlag(result, true, includeCode ? "FUSE: Fuse active" : "Fuse active");
  }
  if (operationFlags & OP_PCHG) {
    appendStatusFlag(result, true, includeCode ? "PCHG: Precharge FET active" : "Precharge FET active");
  }
  if (operationFlags & OP_CHG) {
    appendStatusFlag(result, true, includeCode ? "CHG: CHG FET active" : "CHG FET active");
  }
  if (operationFlags & OP_DSG) {
    appendStatusFlag(result, true, includeCode ? "DSG: DSG FET active" : "DSG FET active");
  }
  if (operationFlags & OP_PRES) {
    appendStatusFlag(result, true, includeCode ? "PRES: System present low" : "System present low");
  }
  
  // Security mode decoding
  uint8_t secMode = (operationFlags >> 8) & 0x03; // Bits 8-9
  if (secMode == 0x01) {
    appendStatusFlag(result, true, includeCode ? "SEC: Full Access" : "Full Access");
  } else if (secMode == 0x02) {
    appendStatusFlag(result, true, includeCode ? "SEC: Unsealed" : "Unsealed");
  } else if (secMode == 0x03) {
    appendStatusFlag(result, true, includeCode ? "SEC: Sealed" : "Sealed");
  }
  
  if (result.length() == 0) {
    result = "Normal";
  }
  
  return result;
}

String BQ4050Utils::parseChargingFlags(uint16_t chargingFlags, bool includeCode) {
  String result = "";
  
  if (chargingFlags & CHG_TAPER) {
    appendStatusFlag(result, true, includeCode ? "TAPER: Taper voltage condition satisfied" : "Taper voltage condition satisfied");
  }
  if (chargingFlags & CHG_CCC) {
    appendStatusFlag(result, true, includeCode ? "CCC: Charging Loss Compensation" : "Charging Loss Compensation");
  }
  if (chargingFlags & CHG_CVR) {
    appendStatusFlag(result, true, includeCode ? "CVR: Charging Voltage Rate of Change" : "Charging Voltage Rate of Change");
  }
  if (chargingFlags & CHG_CCR) {
    appendStatusFlag(result, true, includeCode ? "CCR: Charging Current Rate of Change" : "Charging Current Rate of Change");
  }
  if (chargingFlags & CHG_VCT) {
    appendStatusFlag(result, true, includeCode ? "VCT: Charge Termination" : "Charge Termination");
  }
  if (chargingFlags & CHG_MCHG) {
    appendStatusFlag(result, true, includeCode ? "MCHG: Maintenance Charge" : "Maintenance Charge");
  }
  if (chargingFlags & CHG_SU) {
    appendStatusFlag(result, true, includeCode ? "SU: Charge Suspend" : "Charge Suspend");
  }
  if (chargingFlags & CHG_IN) {
    appendStatusFlag(result, true, includeCode ? "IN: Charge Inhibit" : "Charge Inhibit");
  }
  if (chargingFlags & CHG_HV) {
    appendStatusFlag(result, true, includeCode ? "HV: High Voltage Region" : "High Voltage Region");
  }
  if (chargingFlags & CHG_MV) {
    appendStatusFlag(result, true, includeCode ? "MV: Mid Voltage Region" : "Mid Voltage Region");
  }
  if (chargingFlags & CHG_LV) {
    appendStatusFlag(result, true, includeCode ? "LV: Low Voltage Region" : "Low Voltage Region");
  }
  if (chargingFlags & CHG_PV) {
    appendStatusFlag(result, true, includeCode ? "PV: Precharge Voltage Region" : "Precharge Voltage Region");
  }
  
  if (result.length() == 0) {
    result = "Normal";
  }
  
  return result;
}

String BQ4050Utils::getPFFlagCode(uint32_t flag) {
  switch (flag) {
    case PF_SUV: return "SUV";
    case PF_SOV: return "SOV";
    case PF_SOCC: return "SOCC";
    case PF_SOCD: return "SOCD";
    case PF_SOT: return "SOT";
    case PF_SOTF: return "SOTF";
    case PF_VIMR: return "VIMR";
    case PF_VIMA: return "VIMA";
    case PF_CFETF: return "CFETF";
    case PF_DFETF: return "DFETF";
    case PF_FUSE: return "FUSE";
    case PF_AFER: return "AFER";
    case PF_AFEC: return "AFEC";
    case PF_2LVL: return "2LVL";
    case PF_PTC: return "PTC";
    case PF_IFC: return "IFC";
    case PF_OPNCELL: return "OPNCELL";
    case PF_DFW: return "DFW";
    case PF_TS1: return "TS1";
    case PF_TS2: return "TS2";
    case PF_TS3: return "TS3";
    case PF_TS4: return "TS4";
    default: return "UNKNOWN";
  }
}

String BQ4050Utils::getPFFlagDescription(uint32_t flag) {
  switch (flag) {
    case PF_SUV: return "Safety Cell Undervoltage Failure";
    case PF_SOV: return "Safety Cell Overvoltage Failure";
    case PF_SOCC: return "Safety Overcurrent in Charge";
    case PF_SOCD: return "Safety Overcurrent in Discharge";
    case PF_SOT: return "Safety Overtemperature Cell Failure";
    case PF_SOTF: return "Safety Overtemperature FET Failure";
    case PF_VIMR: return "Voltage Imbalance While Pack At Rest Failure";
    case PF_VIMA: return "Voltage Imbalance While Pack Is Active Failure";
    case PF_CFETF: return "Charge FET Failure";
    case PF_DFETF: return "Discharge FET Failure";
    case PF_FUSE: return "Chemical Fuse Failure";
    case PF_AFER: return "AFE Register Failure";
    case PF_AFEC: return "AFE Communication Failure";
    case PF_2LVL: return "Second Level Protector Failure";
    case PF_PTC: return "PTC Failure";
    case PF_IFC: return "Instruction Flash Checksum Failure";
    case PF_OPNCELL: return "Open Cell Tab Connection Failure";
    case PF_DFW: return "Data Flash Wearout Failure";
    case PF_TS1: return "Open Thermistor–TS1 Failure";
    case PF_TS2: return "Open Thermistor–TS2 Failure";
    case PF_TS3: return "Open Thermistor–TS3 Failure";
    case PF_TS4: return "Open Thermistor–TS4 Failure";
    default: return "Unknown PF Flag";
  }
}

String BQ4050Utils::getOperationFlagCode(uint32_t flag) {
  switch (flag) {
    case OP_PRES: return "PRES";
    case OP_DSG: return "DSG";
    case OP_CHG: return "CHG";
    case OP_PCHG: return "PCHG";
    case OP_FUSE: return "FUSE";
    case OP_SMOOTH: return "SMOOTH";
    case OP_BTP_INT: return "BTP_INT";
    case OP_SEC0: return "SEC0";
    case OP_SEC1: return "SEC1";
    case OP_SDV: return "SDV";
    case OP_SS: return "SS";
    case OP_PF: return "PF";
    case OP_XDSG: return "XDSG";
    case OP_XCHG: return "XCHG";
    case OP_SLEEP: return "SLEEP";
    case OP_SDM: return "SDM";
    case OP_LED: return "LED";
    case OP_AUTH: return "AUTH";
    case OP_AUTOCALM: return "AUTOCALM";
    case OP_CAL: return "CAL";
    case OP_CAL_OFFSET: return "CAL_OFFSET";
    case OP_XL: return "XL";
    case OP_SLEEPM: return "SLEEPM";
    case OP_INIT: return "INIT";
    case OP_SMBLCAL: return "SMBLCAL";
    case OP_SLPAD: return "SLPAD";
    case OP_SLPCC: return "SLPCC";
    case OP_CB: return "CB";
    case OP_EMSHUT: return "EMSHUT";
    default: return "UNKNOWN";
  }
}

String BQ4050Utils::getOperationFlagDescription(uint32_t flag) {
  switch (flag) {
    case OP_PRES: return "System present low";
    case OP_DSG: return "DSG FET active";
    case OP_CHG: return "CHG FET active";
    case OP_PCHG: return "Precharge FET active";
    case OP_FUSE: return "Fuse active";
    case OP_SMOOTH: return "Smoothing active";
    case OP_BTP_INT: return "Battery Trip Point Interrupt";
    case OP_SEC0: return "SECURITY mode bit 0";
    case OP_SEC1: return "SECURITY mode bit 1";
    case OP_SDV: return "Shutdown triggered via low pack voltage";
    case OP_SS: return "SAFETY mode status";
    case OP_PF: return "PERMANENT FAILURE mode status";
    case OP_XDSG: return "Discharging disabled";
    case OP_XCHG: return "Charging disabled";
    case OP_SLEEP: return "SLEEP mode conditions met";
    case OP_SDM: return "Shutdown triggered via command";
    case OP_LED: return "LED Display on";
    case OP_AUTH: return "Authentication in progress";
    case OP_AUTOCALM: return "Auto CC Offset Calibration";
    case OP_CAL: return "Calibration Output active";
    case OP_CAL_OFFSET: return "Calibration Output (raw CC offset data)";
    case OP_XL: return "400-kHz SMBus mode";
    case OP_SLEEPM: return "SLEEP mode triggered via command";
    case OP_INIT: return "Initialization after full reset";
    case OP_SMBLCAL: return "Auto CC calibration when the bus is low";
    case OP_SLPAD: return "ADC Measurement in SLEEP mode";
    case OP_SLPCC: return "CC Measurement in SLEEP mode";
    case OP_CB: return "Cell balancing active";
    case OP_EMSHUT: return "Emergency Shutdown";
    default: return "Unknown Operation Flag";
  }
}

String BQ4050Utils::getChargingFlagCode(uint16_t flag) {
  switch (flag) {
    case CHG_PV: return "PV";
    case CHG_LV: return "LV";
    case CHG_MV: return "MV";
    case CHG_HV: return "HV";
    case CHG_IN: return "IN";
    case CHG_SU: return "SU";
    case CHG_MCHG: return "MCHG";
    case CHG_VCT: return "VCT";
    case CHG_CCR: return "CCR";
    case CHG_CVR: return "CVR";
    case CHG_CCC: return "CCC";
    case CHG_TAPER: return "TAPER";
    default: return "UNKNOWN";
  }
}

String BQ4050Utils::getChargingFlagDescription(uint16_t flag) {
  switch (flag) {
    case CHG_PV: return "Precharge Voltage Region";
    case CHG_LV: return "Low Voltage Region";
    case CHG_MV: return "Mid Voltage Region";
    case CHG_HV: return "High Voltage Region";
    case CHG_IN: return "Charge Inhibit";
    case CHG_SU: return "Charge Suspend";
    case CHG_MCHG: return "Maintenance Charge";
    case CHG_VCT: return "Charge Termination";
    case CHG_CCR: return "Charging Current Rate of Change";
    case CHG_CVR: return "Charging Voltage Rate of Change";
    case CHG_CCC: return "Charging Loss Compensation";
    case CHG_TAPER: return "Taper voltage condition satisfied";
    default: return "Unknown Charging Flag";
  }
}

String BQ4050Utils::listAllActivePFFlags(uint32_t pfFlags) {
  String result = "";
  
  // Array of all possible flags for easy iteration
  uint32_t allFlags[] = {
    PF_SUV, PF_SOV, PF_SOCC, PF_SOCD, PF_SOT, PF_SOTF, PF_VIMR, PF_VIMA,
    PF_CFETF, PF_DFETF, PF_FUSE, PF_AFER, PF_AFEC, PF_2LVL, PF_PTC, PF_IFC,
    PF_OPNCELL, PF_DFW, PF_TS1, PF_TS2, PF_TS3, PF_TS4
  };
  
  for (int i = 0; i < 22; i++) {
    if (pfFlags & allFlags[i]) {
      if (result.length() > 0) {
        result += "\\n";
      }
      result += getPFFlagCode(allFlags[i]) + ": " + getPFFlagDescription(allFlags[i]);
    }
  }
  
  if (result.length() == 0) {
    result = "No PF flags active";
  }
  
  return result;
}

String BQ4050Utils::listAllActiveOperationFlags(uint32_t operationFlags) {
  String result = "";
  
  // Array of all possible flags for easy iteration
  uint32_t allFlags[] = {
    OP_PRES, OP_DSG, OP_CHG, OP_PCHG, OP_FUSE, OP_SMOOTH, OP_BTP_INT,
    OP_SDV, OP_SS, OP_PF, OP_XDSG, OP_XCHG, OP_SLEEP, OP_SDM, OP_LED,
    OP_AUTH, OP_AUTOCALM, OP_CAL, OP_CAL_OFFSET, OP_XL, OP_SLEEPM, OP_INIT,
    OP_SMBLCAL, OP_SLPAD, OP_SLPCC, OP_CB, OP_EMSHUT
  };
  
  for (int i = 0; i < 27; i++) {
    if (operationFlags & allFlags[i]) {
      if (result.length() > 0) {
        result += "\\n";
      }
      result += getOperationFlagCode(allFlags[i]) + ": " + getOperationFlagDescription(allFlags[i]);
    }
  }
  
  // Handle security mode separately
  uint8_t secMode = (operationFlags >> 8) & 0x03; // Bits 8-9
  if (secMode != 0) {
    if (result.length() > 0) {
      result += "\\n";
    }
    if (secMode == 0x01) {
      result += "SEC: Full Access";
    } else if (secMode == 0x02) {
      result += "SEC: Unsealed";
    } else if (secMode == 0x03) {
      result += "SEC: Sealed";
    }
  }
  
  if (result.length() == 0) {
    result = "No operation flags active";
  }
  
  return result;
}

String BQ4050Utils::listAllActiveChargingFlags(uint16_t chargingFlags) {
  String result = "";
  
  // Array of all possible flags for easy iteration
  uint16_t allFlags[] = {
    CHG_PV, CHG_LV, CHG_MV, CHG_HV, CHG_IN, CHG_SU, CHG_MCHG, CHG_VCT,
    CHG_CCR, CHG_CVR, CHG_CCC, CHG_TAPER
  };
  
  for (int i = 0; i < 12; i++) {
    if (chargingFlags & allFlags[i]) {
      if (result.length() > 0) {
        result += "\\n";
      }
      result += getChargingFlagCode(allFlags[i]) + ": " + getChargingFlagDescription(allFlags[i]);
    }
  }
  
  if (result.length() == 0) {
    result = "No charging flags active";
  }
  
  return result;
}

String BQ4050Utils::getSafetyFlagCode(uint32_t flag) {
  switch (flag) {
    case SAFETY_CUV: return "CUV";
    case SAFETY_COV: return "COV";
    case SAFETY_OCC1: return "OCC1";
    case SAFETY_OCC2: return "OCC2";
    case SAFETY_OCD1: return "OCD1";
    case SAFETY_OCD2: return "OCD2";
    case SAFETY_AOLDL: return "AOLDL";
    case SAFETY_ASCCL: return "ASCCL";
    case SAFETY_ASCDL: return "ASCDL";
    case SAFETY_OTC: return "OTC";
    case SAFETY_OTD: return "OTD";
    case SAFETY_CUVC: return "CUVC";
    case SAFETY_OTF: return "OTF";
    case SAFETY_PTO: return "PTO";
    case SAFETY_PTOS: return "PTOS";
    case SAFETY_CTO: return "CTO";
    case SAFETY_CTOS: return "CTOS";
    case SAFETY_OC: return "OC";
    case SAFETY_CHGC: return "CHGC";
    case SAFETY_CHGV: return "CHGV";
    case SAFETY_PCHGC: return "PCHGC";
    case SAFETY_UTC: return "UTC";
    case SAFETY_UTD: return "UTD";
    default: return "UNKNOWN";
  }
}

String BQ4050Utils::getSafetyFlagDescription(uint32_t flag) {
  switch (flag) {
    case SAFETY_CUV: return "Cell Undervoltage";
    case SAFETY_COV: return "Cell Overvoltage";
    case SAFETY_OCC1: return "Overcurrent During Charge 1";
    case SAFETY_OCC2: return "Overcurrent During Charge 2";
    case SAFETY_OCD1: return "Overcurrent During Discharge 1";
    case SAFETY_OCD2: return "Overcurrent During Discharge 2";
    case SAFETY_AOLDL: return "Overload During Discharge Latch";
    case SAFETY_ASCCL: return "Short-Circuit During Charge Latch";
    case SAFETY_ASCDL: return "Short-Circuit During Discharge Latch";
    case SAFETY_OTC: return "Overtemperature During Charge";
    case SAFETY_OTD: return "Overtemperature During Discharge";
    case SAFETY_CUVC: return "Cell Undervoltage Compensated";
    case SAFETY_OTF: return "Overtemperature FET";
    case SAFETY_PTO: return "Precharge Timeout";
    case SAFETY_PTOS: return "Precharge Timeout Suspend";
    case SAFETY_CTO: return "Charge Timeout";
    case SAFETY_CTOS: return "Charge Timeout Suspend";
    case SAFETY_OC: return "Overcharge";
    case SAFETY_CHGC: return "Overcharging Current";
    case SAFETY_CHGV: return "Overcharging Voltage";
    case SAFETY_PCHGC: return "Over-Precharge Current";
    case SAFETY_UTC: return "Undertemperature During Charge";
    case SAFETY_UTD: return "Undertemperature During Discharge";
    default: return "Unknown Flag";
  }
}

SafetyFlagInfo BQ4050Utils::getSafetyFlagInfo(uint32_t flag) {
  SafetyFlagInfo info;
  info.flag = flag;
  info.code = getSafetyFlagCode(flag);
  info.description = getSafetyFlagDescription(flag);
  return info;
}

String BQ4050Utils::listAllActiveSafetyFlags(uint32_t safetyFlags) {
  String result = "";
  
  // Array of all possible flags for easy iteration
  uint32_t allFlags[] = {
    SAFETY_CUV, SAFETY_COV, SAFETY_OCC1, SAFETY_OCC2, SAFETY_OCD1, SAFETY_OCD2,
    SAFETY_AOLDL, SAFETY_ASCCL, SAFETY_ASCDL, SAFETY_OTC, SAFETY_OTD, SAFETY_CUVC,
    SAFETY_OTF, SAFETY_PTO, SAFETY_PTOS, SAFETY_CTO, SAFETY_CTOS, SAFETY_OC,
    SAFETY_CHGC, SAFETY_CHGV, SAFETY_PCHGC, SAFETY_UTC, SAFETY_UTD
  };
  
  for (int i = 0; i < 23; i++) {
    if (safetyFlags & allFlags[i]) {
      if (result.length() > 0) {
        result += "\n";
      }
      SafetyFlagInfo info = getSafetyFlagInfo(allFlags[i]);
      result += info.code + ": " + info.description;
    }
  }
  
  if (result.length() == 0) {
    result = "No safety flags active";
  }
  
  return result;
}

String BQ4050Utils::formatVoltage(float voltage, unsigned int decimals) {
  return String(voltage, decimals) + " V";
}

String BQ4050Utils::formatCurrent(float current, unsigned int decimals) {
  return String(current, decimals) + " A";
}

String BQ4050Utils::formatTemperature(float temperature, unsigned int decimals) {
  return String(temperature, decimals) + " °C";
}

String BQ4050Utils::formatCapacity(uint16_t capacity) {
  return String(capacity) + " mAh";
}

String BQ4050Utils::formatPercentage(uint8_t percentage) {
  return String(percentage) + " %";
}

String BQ4050Utils::formatCycleCount(uint16_t cycleCount) {
  if (!isValidCycleCount(cycleCount)) {
    return "Invalid (" + String(cycleCount) + ")";
  }
  return String(cycleCount) + " cycles";
}

String BQ4050Utils::formatManufacturerDate(uint16_t dateCode) {
  // Check for invalid/uninitialized values
  if (dateCode == 0xFFFF || dateCode == 0x0000) {
    return "Invalid (0x" + String(dateCode, HEX) + ")";
  }
  
  // Decode SBS date format: Day + Month*32 + (Year–1980)*256
  uint8_t day = dateCode & 0x1F;           // Bits 0-4
  uint8_t month = (dateCode >> 5) & 0x0F;  // Bits 5-8
  uint16_t year = 1980 + (dateCode >> 9);  // Bits 9-15
  
  // Validate ranges
  if (day < 1 || day > 31 || month < 1 || month > 12 || year > 2100) {
    return "Invalid date (0x" + String(dateCode, HEX) + ")";
  }
  
  // Format as YYYY-MM-DD
  String result = String(year) + "-";
  if (month < 10) result += "0";
  result += String(month) + "-";
  if (day < 10) result += "0";
  result += String(day);
  
  return result;
}

String BQ4050Utils::formatSerialNumber(uint16_t serialNumber) {
  // Check for invalid/uninitialized values
  if (serialNumber == 0xFFFF) {
    return "Invalid (0xFFFF)";
  }
  if (serialNumber == 0x0000) {
    return "Not Set (0x0000)";
  }
  
  // Display both decimal and hex formats
  return String(serialNumber) + " (0x" + String(serialNumber, HEX) + ")";
}

bool BQ4050Utils::isValidDeviceType(uint16_t deviceType) {
  return (deviceType == BQ4050_DEVICE_TYPE || 
          deviceType == BQ20Z45_DEVICE_TYPE || 
          deviceType == BQ40Z50_DEVICE_TYPE);
}


bool BQ4050Utils::isValidCycleCount(uint16_t cycleCount) {
  // Check for common error values that indicate communication issues
  if (cycleCount == 0xFFFF) {
    return false;  // Read error - all bits set
  }
  if (cycleCount == 0x0000) {
    // Could be valid (new battery) but suspicious in context
    return true;  // Let's consider 0 as potentially valid
  }
  
  // Reasonable cycle count range for most batteries
  // Most consumer batteries don't exceed 5000-10000 cycles
  // But industrial/specialty batteries might go higher
  if (cycleCount > 50000) {
    return false;  // Probably an error
  }
  
  return true;
}

String BQ4050Utils::getFullDeviceInfo(uint16_t deviceType, uint16_t fwVersion, uint16_t hwVersion) {
  String result = "Device: 0x" + String(deviceType, HEX);
  result += " (FW: 0x" + String(fwVersion, HEX);
  result += ", HW: 0x" + String(hwVersion, HEX) + ")";
  return result;
}

String BQ4050Utils::getBatteryHealthSummary(uint16_t safetyStatus, uint16_t safetyAlert, uint16_t batteryStatus) {
  if (safetyAlert != 0) {
    return "ALERT: " + parseSafetyAlert(safetyAlert);
  }
  
  if (safetyStatus != 0) {
    return "WARNING: " + parseSafetyStatus(safetyStatus);
  }
  
  if (batteryStatus & (BATTERY_STATUS_OCA | BATTERY_STATUS_TCA | BATTERY_STATUS_OTA | BATTERY_STATUS_TDA)) {
    return "CAUTION: " + parseBatteryStatus(batteryStatus);
  }
  
  return "Healthy";
}

String BQ4050Utils::getSecurityModeDescription(const String& securityMode) {
  if (securityMode == "Sealed") {
    return "Sealed - Limited access, manufacturer strings may not be available";
  } else if (securityMode == "Unsealed") {
    return "Unsealed - Extended access, most manufacturer commands available";
  } else if (securityMode == "Full Access") {
    return "Full Access - Complete access, all manufacturer commands available";
  } else {
    return "Unknown - Could not determine security mode";
  }
}

// Bitfield union utility functions
BatteryStatusBits BQ4050Utils::parseBatteryStatusBits(uint16_t batteryStatus) {
  BatteryStatusBits bits;
  bits.raw = batteryStatus;
  return bits;
}

SafetyStatusBits BQ4050Utils::parseSafetyStatusBits(uint32_t safetyStatus) {
  SafetyStatusBits bits;
  bits.raw = safetyStatus;
  return bits;
}

String BQ4050Utils::describeBatteryStatusBits(const BatteryStatusBits& status) {
  String result = "";
  if (status.bits.oca) appendStatusFlag(result, true, "OCA");
  if (status.bits.tca) appendStatusFlag(result, true, "TCA");
  if (status.bits.ota) appendStatusFlag(result, true, "OTA");
  if (status.bits.tda) appendStatusFlag(result, true, "TDA");
  if (status.bits.rca) appendStatusFlag(result, true, "RCA");
  if (status.bits.rta) appendStatusFlag(result, true, "RTA");
  
  if (result.length() == 0) {
    result = "OK";
  }
  return result;
}

String BQ4050Utils::describeSafetyStatusBits(const SafetyStatusBits& status) {
  String result = "";
  if (status.bits.cuv) appendStatusFlag(result, true, "CUV");
  if (status.bits.cov) appendStatusFlag(result, true, "COV");
  if (status.bits.occ1) appendStatusFlag(result, true, "OCC1");
  if (status.bits.occ2) appendStatusFlag(result, true, "OCC2");
  if (status.bits.ocd1) appendStatusFlag(result, true, "OCD1");
  if (status.bits.ocd2) appendStatusFlag(result, true, "OCD2");
  if (status.bits.aold) appendStatusFlag(result, true, "AOLD");
  if (status.bits.aoldl) appendStatusFlag(result, true, "AOLDL");
  if (status.bits.ascc) appendStatusFlag(result, true, "ASCC");
  if (status.bits.asccl) appendStatusFlag(result, true, "ASCCL");
  if (status.bits.ascd) appendStatusFlag(result, true, "ASCD");
  if (status.bits.ascdl) appendStatusFlag(result, true, "ASCDL");
  if (status.bits.otc) appendStatusFlag(result, true, "OTC");
  if (status.bits.otd) appendStatusFlag(result, true, "OTD");
  if (status.bits.cuvc) appendStatusFlag(result, true, "CUVC");
  if (status.bits.otf) appendStatusFlag(result, true, "OTF");
  if (status.bits.pto) appendStatusFlag(result, true, "PTO");
  if (status.bits.ptos) appendStatusFlag(result, true, "PTOS");
  if (status.bits.cto) appendStatusFlag(result, true, "CTO");
  if (status.bits.ctos) appendStatusFlag(result, true, "CTOS");
  if (status.bits.oc) appendStatusFlag(result, true, "OC");
  if (status.bits.chgc) appendStatusFlag(result, true, "CHGC");
  if (status.bits.chgv) appendStatusFlag(result, true, "CHGV");
  if (status.bits.pchgc) appendStatusFlag(result, true, "PCHGC");
  if (status.bits.utc) appendStatusFlag(result, true, "UTC");
  if (status.bits.utd) appendStatusFlag(result, true, "UTD");
  
  if (result.length() == 0) {
    result = "OK";
  }
  return result;
}

void BQ4050Utils::appendStatusFlag(String& result, bool condition, const String& flagName) {
  if (condition) {
    if (result.length() > 0) {
      result += ", ";
    }
    result += flagName;
  }
}