/*
  BQ4050 Cell Monitoring Example
  
  This example demonstrates how to monitor individual cell voltages and 
  balancing status using the convenience methods provided by the library.
  
  Hardware Requirements:
  - Arduino (any platform supported by the library)
  - BQ4050 battery monitoring IC connected to a multi-cell battery pack
  - Pull-up resistors on SDA and SCL lines (typically 4.7kΩ)
  
  Author: Andy Shinn
  Date: 2024
*/

#include <BQ4050.h>

BQ4050 battery;

void setup() {
  Serial.begin(115200);
  Serial.println("BQ4050 Cell Monitoring Example");
  Serial.println("===============================");
  
  if (!battery.begin()) {
    Serial.println("Failed to initialize BQ4050!");
    while (1) delay(1000);
  }
  
  // Get cell count configuration
  CellCount cellCount = battery.getCellCount();
  Serial.print("Configured for ");
  Serial.print((int)cellCount + 1);
  Serial.println(" cells");
  Serial.println();
}

void loop() {
  // Get all cell status at once using convenience method
  CellStatus cells = battery.getAllCellStatus();
  
  Serial.println("Cell Voltages and Balancing Status:");
  Serial.println("====================================");
  
  // Display individual cell voltages
  Serial.print("Cell 1: ");
  Serial.print(cells.voltage1, 3);
  Serial.print(" V");
  if (cells.balancing1) Serial.print(" [BALANCING]");
  Serial.println();
  
  Serial.print("Cell 2: ");
  Serial.print(cells.voltage2, 3);
  Serial.print(" V");
  if (cells.balancing2) Serial.print(" [BALANCING]");
  Serial.println();
  
  Serial.print("Cell 3: ");
  Serial.print(cells.voltage3, 3);
  Serial.print(" V");
  if (cells.balancing3) Serial.print(" [BALANCING]");
  Serial.println();
  
  Serial.print("Cell 4: ");
  Serial.print(cells.voltage4, 3);
  Serial.print(" V");
  if (cells.balancing4) Serial.print(" [BALANCING]");
  Serial.println();
  
  // Calculate cell statistics
  float minVoltage = cells.voltage1;
  float maxVoltage = cells.voltage1;
  float totalVoltage = cells.voltage1;
  int activeCells = 1;
  
  if (cells.voltage2 > 0.1) {
    minVoltage = min(minVoltage, cells.voltage2);
    maxVoltage = max(maxVoltage, cells.voltage2);
    totalVoltage += cells.voltage2;
    activeCells++;
  }
  
  if (cells.voltage3 > 0.1) {
    minVoltage = min(minVoltage, cells.voltage3);
    maxVoltage = max(maxVoltage, cells.voltage3);
    totalVoltage += cells.voltage3;
    activeCells++;
  }
  
  if (cells.voltage4 > 0.1) {
    minVoltage = min(minVoltage, cells.voltage4);
    maxVoltage = max(maxVoltage, cells.voltage4);
    totalVoltage += cells.voltage4;
    activeCells++;
  }
  
  float averageVoltage = totalVoltage / activeCells;
  float voltageImbalance = maxVoltage - minVoltage;
  
  Serial.println("\nCell Statistics:");
  Serial.print("Active Cells: ");
  Serial.println(activeCells);
  Serial.print("Total Voltage: ");
  Serial.print(totalVoltage, 3);
  Serial.println(" V");
  Serial.print("Average Voltage: ");
  Serial.print(averageVoltage, 3);
  Serial.println(" V");
  Serial.print("Min Voltage: ");
  Serial.print(minVoltage, 3);
  Serial.println(" V");
  Serial.print("Max Voltage: ");
  Serial.print(maxVoltage, 3);
  Serial.println(" V");
  Serial.print("Voltage Imbalance: ");
  Serial.print(voltageImbalance * 1000, 1);
  Serial.println(" mV");
  
  // Warning for high imbalance
  if (voltageImbalance > 0.100) { // 100mV threshold
    Serial.println("⚠️  WARNING: High cell imbalance detected!");
  }
  
  // Check if any cells are balancing
  bool anyBalancing = cells.balancing1 || cells.balancing2 || 
                      cells.balancing3 || cells.balancing4;
  if (anyBalancing) {
    Serial.println("🔄 Cell balancing active");
  }
  
  Serial.println("====================================\n");
  
  delay(3000); // Update every 3 seconds
}