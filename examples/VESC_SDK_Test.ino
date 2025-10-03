/*
 * VESC SDK Test Example
 * 
 * This example demonstrates how to use the professional VESC CAN SDK
 * to communicate with VESC controllers over CAN bus.
 * 
 * Hardware Requirements:
 * - ESP32-S3 (or compatible)
 * - CAN transceiver (TJA1050, MCP2551, etc.)
 * - VESC controller with CAN enabled
 * 
 * Connections:
 * ESP32-S3  <->  CAN Transceiver  <->  VESC
 * GPIO 6    <->  CTX              
 * GPIO 0    <->  CRX              
 *           <->  CAN_H            <->  CAN_H
 *           <->  CAN_L            <->  CAN_L
 *           <->  GND              <->  GND
 * 
 * VESC Configuration:
 * 1. Connect VESC to computer via USB
 * 2. Open VESC Tool
 * 3. Go to App Settings -> CAN
 * 4. Set CAN ID to 0 (or change VESC_ID below)
 * 5. Enable CAN status messages
 * 6. Set CAN baud rate to 250k (important!)
 */

#include "VESC_SDK_Driver.h"

// Configuration
#define VESC_ID 0  // VESC CAN ID (0-255)

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("=== VESC SDK Test ===");
    Serial.println("Initializing VESC SDK driver...");
    
    // Initialize VESC SDK driver
    if (VESC_SDK_Init(VESC_ID)) {
        Serial.println("VESC SDK driver initialized successfully!");
        
        // Configure VESC parameters (adjust for your setup)
        VESC_SDK_SetConfig(
            0.107f,  // Wheel diameter in meters (107mm skateboard wheel)
            14,      // Motor poles (check your motor specs)
            1.0f,    // Gear ratio (1.0 = direct drive)
            10,      // Battery cells in series (10S battery)
            5.0f     // Battery capacity in Ah
        );
        
        Serial.println("VESC configuration set!");
        Serial.println("Waiting for VESC connection...");
        Serial.println("CAN Speed: 250 kbps");
        Serial.println("CAN TX Pin: GPIO 6, RX Pin: GPIO 0");
        
    } else {
        Serial.println("Failed to initialize VESC SDK driver!");
        Serial.println("Check your hardware connections and try again.");
    }
}

void loop() {
    // Update VESC communication
    VESC_SDK_Loop();
    
    // Print status every 2 seconds
    static unsigned long last_print = 0;
    if (millis() - last_print > 2000) {
        printVESCStatus();
        last_print = millis();
    }
    
    // Test commands every 15 seconds (be careful!)
    static unsigned long last_command = 0;
    if (millis() - last_command > 15000) {
        testCommands();
        last_command = millis();
    }
    
    // Handle serial commands
    handleSerialCommands();
    
    delay(10);
}

void printVESCStatus() {
    Serial.println("\n=== VESC SDK Status ===");
    
    if (VESC_SDK_IsConnected()) {
        Serial.println("✓ VESC Connected via SDK");
        
        vesc_sdk_values_t values = VESC_SDK_GetValues();
        
        if (values.vesc_values.valid) {
            Serial.printf("Speed: %.2f km/h\n", values.speed_kmh);
            Serial.printf("RPM: %.0f\n", values.vesc_values.rpm);
            Serial.printf("Motor Current: %.2f A\n", values.vesc_values.current_motor);
            Serial.printf("Input Current: %.2f A\n", values.vesc_values.current_in);
            Serial.printf("Duty Cycle: %.3f\n", values.vesc_values.duty_now);
            Serial.printf("Battery: %.1f V (%.1f%%)\n", values.vesc_values.v_in, values.battery_percentage);
            Serial.printf("Power: %.1f W\n", values.power_watts);
            Serial.printf("Temperature FET: %.1f°C\n", values.vesc_values.temp_mos);
            Serial.printf("Temperature Motor: %.1f°C\n", values.vesc_values.temp_motor);
            Serial.printf("Distance: %.3f km\n", values.distance_km);
            Serial.printf("Fault Code: %d\n", values.vesc_values.fault_code);
            
            if (values.vesc_values.fault_code != 0) {
                Serial.printf("⚠️  FAULT DETECTED: Code %d\n", values.vesc_values.fault_code);
            }
        } else {
            Serial.println("Waiting for VESC values...");
        }
        
        // Print status messages if available
        if (values.status_msg_1.valid) {
            Serial.printf("Status Msg 1: RPM=%.0f, Current=%.2fA, Duty=%.3f\n",
                         values.status_msg_1.rpm, values.status_msg_1.current, values.status_msg_1.duty);
        }
        
        if (values.status_msg_4.valid) {
            Serial.printf("Status Msg 4: FET=%.1f°C, Motor=%.1f°C, Input=%.2fA\n",
                         values.status_msg_4.temp_fet, values.status_msg_4.temp_motor, values.status_msg_4.current_in);
        }
        
    } else {
        Serial.println("✗ VESC Not Connected");
        Serial.println("Check CAN bus connections and VESC configuration");
        Serial.println("Make sure VESC CAN speed is set to 250 kbps!");
    }
}

void testCommands() {
    if (!VESC_SDK_IsConnected()) {
        return;
    }
    
    Serial.println("\n=== Testing SDK Commands ===");
    
    // WARNING: These commands will move the motor!
    // Make sure the motor is not connected to wheels or remove this section
    
    /*
    // Test duty cycle (very small value for safety)
    Serial.println("Testing duty cycle command...");
    VESC_SDK_SetDuty(0.01f);  // 1% duty cycle
    delay(1000);
    VESC_SDK_SetDuty(0.0f);   // Stop
    
    delay(2000);
    
    // Test current command (very small value for safety)
    Serial.println("Testing current command...");
    VESC_SDK_SetCurrent(0.5f);  // 0.5A current
    delay(1000);
    VESC_SDK_SetCurrent(0.0f);  // Stop
    
    delay(2000);
    
    // Test RPM command (very low RPM for safety)
    Serial.println("Testing RPM command...");
    VESC_SDK_SetRPM(100.0f);  // 100 RPM
    delay(1000);
    VESC_SDK_SetRPM(0.0f);    // Stop
    */
    
    // Safe commands that don't move the motor
    Serial.println("Sending ping...");
    VESC_SDK_Ping();
    
    delay(500);
    
    Serial.println("Requesting values...");
    VESC_SDK_RequestValues();
}

void handleSerialCommands() {
    if (Serial.available()) {
        char command = Serial.read();
        switch (command) {
            case 'd':
            case 'D':
                Serial.println("\n=== Debug Info ===");
                VESC_SDK_PrintDebug();
                break;
                
            case 's':
            case 'S':
                printVESCStatus();
                break;
                
            case 'p':
            case 'P':
                VESC_SDK_Ping();
                Serial.println("Ping sent!");
                break;
                
            case 'v':
            case 'V':
                VESC_SDK_RequestValues();
                Serial.println("Values requested!");
                break;
                
            case 'e':
            case 'E':
                VESC_SDK_EmergencyStop();
                Serial.println("Emergency stop executed!");
                break;
                
            case 'h':
            case 'H':
            case '?':
                Serial.println("\n=== Available Commands ===");
                Serial.println("d - Debug info");
                Serial.println("s - Status");
                Serial.println("p - Ping");
                Serial.println("v - Request values");
                Serial.println("e - Emergency stop");
                Serial.println("h - Help");
                break;
                
            default:
                Serial.println("Unknown command. Send 'h' for help.");
                break;
        }
    }
}
