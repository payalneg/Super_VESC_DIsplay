#pragma once

// VESC Display Configuration
// Customize these values for your specific e-skateboard/e-scooter setup

// UART Configuration
#define VESC_UART_TX_PIN 43  // ESP32-S3 UART TX pin for VESC communication
#define VESC_UART_RX_PIN 44  // ESP32-S3 UART RX pin for VESC communication
#define VESC_UART_NUM 1      // Use UART1 for VESC communication
#define VESC_BAUD_RATE 115200

// Motor and Wheel Configuration
#define DEFAULT_WHEEL_DIAMETER 0.097f   // Wheel diameter in meters (97mm)
#define DEFAULT_MOTOR_POLES 14.0f       // Number of motor poles (typical: 14)
#define DEFAULT_GEAR_RATIO 1.0f         // Gear ratio (1.0 = direct drive)

// Battery Configuration
#define DEFAULT_BATTERY_CELLS 10.0f     // Number of battery cells in series (10S)
#define DEFAULT_BATTERY_CAPACITY 5.0f   // Battery capacity in Ah
#define BATTERY_MIN_VOLTAGE_PER_CELL 3.2f  // Minimum safe voltage per cell
#define BATTERY_MAX_VOLTAGE_PER_CELL 4.2f  // Maximum voltage per cell

// Display Configuration
#define VESC_UPDATE_RATE_MS 100         // How often to request VESC data (ms)
#define CONNECTION_TIMEOUT_MS 2000      // Consider disconnected after this time
#define DISPLAY_UPDATE_RATE_MS 100      // How often to update display

// Warning Thresholds
#define TEMP_WARNING_ESC 70.0f          // ESC temperature warning (°C)
#define TEMP_WARNING_MOTOR 80.0f        // Motor temperature warning (°C)
#define BATTERY_LOW_WARNING 20.0f       // Low battery warning (%)
#define BATTERY_CRITICAL_WARNING 10.0f  // Critical battery warning (%)

// Speed and Power Limits (for display scaling)
#define MAX_DISPLAY_SPEED 60.0f         // Maximum speed to display (km/h)
#define MAX_DISPLAY_POWER 3000.0f       // Maximum power to display (W)
#define MAX_DISPLAY_CURRENT 50.0f       // Maximum current to display (A)

// Colors (in hex format for LVGL)
#define COLOR_BACKGROUND 0x000000       // Black background
#define COLOR_TEXT_PRIMARY 0xFFFFFF     // White text
#define COLOR_TEXT_SECONDARY 0x87CEEB   // Light blue text
#define COLOR_WARNING 0xFFFF00          // Yellow warning
#define COLOR_DANGER 0xFF0000           // Red danger
#define COLOR_SUCCESS 0x00FF00          // Green success
#define COLOR_INFO 0x87CEEB             // Light blue info

// CAN Converter Configuration (for VESC_CAN_Driver)
#define VESC_CAN_CONVERTER_TX_PIN 43   // ESP32-S3 TX pin for CAN converter
#define VESC_CAN_CONVERTER_RX_PIN 44   // ESP32-S3 RX pin for CAN converter
#define VESC_CAN_CONVERTER_BAUD 250000 // Baud rate for CAN converter (250k for VESC CAN)
#define VESC_CAN_ID 10                 // VESC ID on CAN bus (0-255, default 10 like craigg96)

// Communication Mode Selection
#define USE_CAN_CONVERTER 0            // 0 = Use UART (VESC_Driver), 1 = Use CAN Converter (VESC_CAN_Driver)

// Enable/Disable Features
#define ENABLE_TEMPERATURE_WARNINGS 1  // Show temperature warnings
#define ENABLE_BATTERY_WARNINGS 1      // Show battery warnings
#define ENABLE_SPEED_LIMIT_WARNING 0   // Show speed limit warnings
#define ENABLE_TRIP_RESET_BUTTON 1     // Enable trip reset functionality
#define ENABLE_DEBUG_OUTPUT 1          // Enable serial debug output
