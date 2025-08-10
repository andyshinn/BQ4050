/*
  BQ4050 Configuration Example
  
  This example demonstrates how to configure the BQ4050 for different 
  battery pack configurations using the convenient setup methods.
  
  WARNING: This example modifies the BQ4050 configuration. Make sure 
  you understand the implications before running this code.
  
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
  Serial.println("BQ4050 Configuration Example");
  Serial.println("=============================");
  
  if (!battery.begin()) {
    Serial.println("Failed to initialize BQ4050!");
    while (1) delay(1000);
  }
  
  Serial.println("BQ4050 initialized successfully\n");
  
  // Display current configuration
  displayCurrentConfiguration();
  
  // Menu for configuration options
  showConfigurationMenu();
}

void loop() {
  if (Serial.available()) {
    char choice = Serial.read();
    Serial.read(); // consume newline
    
    switch (choice) {
      case '1':
        configureFor1S();
        break;
      case '2':
        configureFor2S();
        break;
      case '3':
        configureFor3S();
        break;
      case '4':
        configureFor4S();
        break;
      case 'r':
        configureForRemovable();
        break;
      case 'e':
        configureForEmbedded();
        break;
      case 'p':
        configureForPortable();
        break;
      case 'b':
        configureForPowerBank();
        break;
      case 's':
        displayCurrentConfiguration();
        break;
      case 'v':
        validateCurrentConfiguration();
        break;
      case 'f':
        resetToFactoryDefaults();
        break;
      case 'm':
        showConfigurationMenu();
        break;
      default:
        Serial.println("Invalid choice. Press 'm' for menu.");
        break;
    }
  }
}

void showConfigurationMenu() {
  Serial.println("Configuration Options:");
  Serial.println("======================");
  Serial.println("1 - Configure for 1S battery (1 cell)");
  Serial.println("2 - Configure for 2S battery (2 cells)");
  Serial.println("3 - Configure for 3S battery (3 cells)");
  Serial.println("4 - Configure for 4S battery (4 cells)");
  Serial.println("r - Configure for removable battery");
  Serial.println("e - Configure for embedded battery");
  Serial.println("p - Configure for portable device");
  Serial.println("b - Configure for power bank");
  Serial.println("s - Show current configuration");
  Serial.println("v - Validate configuration");
  Serial.println("f - Reset to factory defaults");
  Serial.println("m - Show this menu");
  Serial.println("\nEnter your choice:");
}

void displayCurrentConfiguration() {
  Serial.println("Current Configuration:");
  Serial.println("=====================");
  
  // Device Architecture Configuration
  DAConfiguration daConfig = battery.getDAConfiguration();
  Serial.print("Cell Count: ");
  Serial.println((int)daConfig.cellCount + 1);
  Serial.print("Non-removable: ");
  Serial.println(daConfig.nonRemovable ? "Yes" : "No");
  Serial.print("Sleep Mode: ");
  Serial.println(daConfig.sleepMode ? "Enabled" : "Disabled");
  Serial.print("Emergency Shutdown: ");
  Serial.println(daConfig.emergencyShutdown ? "Enabled" : "Disabled");
  
  // Cell Balancing Configuration
  BalancingConfig balanceConfig = battery.getBalancingConfig();
  Serial.print("Cell Balancing: ");
  Serial.println(balanceConfig.cellBalancingEnable ? "Enabled" : "Disabled");
  Serial.print("Balance Voltage: ");
  Serial.print(balanceConfig.balanceVoltage);
  Serial.println(" mV");
  
  // FET Options
  FETOptions fetOptions = battery.getFETOptions();
  Serial.print("Over-temp FET Disable: ");
  Serial.println(fetOptions.overTempFETDisable ? "Enabled" : "Disabled");
  
  // Power Configuration
  PowerConfig powerConfig = battery.getPowerConfig();
  Serial.print("Auto Ship Mode: ");
  Serial.println(powerConfig.autoShipEnable ? "Enabled" : "Disabled");
  
  // LED Configuration
  LEDConfig ledConfig = battery.getLEDConfig();
  Serial.print("LED Display: ");
  Serial.println(ledConfig.ledEnable ? "Enabled" : "Disabled");
  
  Serial.println();
}

void configureFor1S() {
  Serial.println("Configuring for 1S battery...");
  if (battery.configureFor1S(false)) { // No balancing for single cell
    Serial.println("✓ Successfully configured for 1S battery");
  } else {
    Serial.println("✗ Failed to configure for 1S battery");
  }
  displayCurrentConfiguration();
}

void configureFor2S() {
  Serial.println("Configuring for 2S battery...");
  if (battery.configureFor2S(true)) { // Enable balancing
    Serial.println("✓ Successfully configured for 2S battery");
  } else {
    Serial.println("✗ Failed to configure for 2S battery");
  }
  displayCurrentConfiguration();
}

void configureFor3S() {
  Serial.println("Configuring for 3S battery...");
  if (battery.configureFor3S(true)) { // Enable balancing
    Serial.println("✓ Successfully configured for 3S battery");
  } else {
    Serial.println("✗ Failed to configure for 3S battery");
  }
  displayCurrentConfiguration();
}

void configureFor4S() {
  Serial.println("Configuring for 4S battery...");
  if (battery.configureFor4S(true)) { // Enable balancing
    Serial.println("✓ Successfully configured for 4S battery");
  } else {
    Serial.println("✗ Failed to configure for 4S battery");
  }
  displayCurrentConfiguration();
}

void configureForRemovable() {
  Serial.println("Configuring for removable battery...");
  if (battery.configureForRemovableBattery()) {
    Serial.println("✓ Successfully configured for removable battery");
  } else {
    Serial.println("✗ Failed to configure for removable battery");
  }
  displayCurrentConfiguration();
}

void configureForEmbedded() {
  Serial.println("Configuring for embedded battery...");
  if (battery.configureForEmbeddedBattery()) {
    Serial.println("✓ Successfully configured for embedded battery");
  } else {
    Serial.println("✗ Failed to configure for embedded battery");
  }
  displayCurrentConfiguration();
}

void configureForPortable() {
  Serial.println("Configuring for portable device...");
  if (battery.configureForPortableDevice()) {
    Serial.println("✓ Successfully configured for portable device");
  } else {
    Serial.println("✗ Failed to configure for portable device");
  }
  displayCurrentConfiguration();
}

void configureForPowerBank() {
  Serial.println("Configuring for power bank...");
  if (battery.configureForPowerBank()) {
    Serial.println("✓ Successfully configured for power bank");
  } else {
    Serial.println("✗ Failed to configure for power bank");
  }
  displayCurrentConfiguration();
}

void validateCurrentConfiguration() {
  Serial.println("Validating current configuration...");
  
  if (battery.validateConfiguration()) {
    Serial.println("✓ Configuration is valid");
  } else {
    Serial.println("✗ Configuration validation failed");
    String errors = battery.getConfigurationErrors();
    if (errors.length() > 0) {
      Serial.print("Errors: ");
      Serial.println(errors);
    }
  }
  Serial.println();
}

void resetToFactoryDefaults() {
  Serial.println("WARNING: This will reset all configuration to factory defaults!");
  Serial.println("Are you sure? Type 'YES' to confirm:");
  
  String input = "";
  while (input.length() == 0) {
    if (Serial.available()) {
      input = Serial.readString();
      input.trim();
    }
    delay(100);
  }
  
  if (input == "YES") {
    Serial.println("Resetting to factory defaults...");
    if (battery.resetToFactoryDefaults()) {
      Serial.println("✓ Successfully reset to factory defaults");
    } else {
      Serial.println("✗ Failed to reset to factory defaults");
    }
    displayCurrentConfiguration();
  } else {
    Serial.println("Reset cancelled.");
  }
}