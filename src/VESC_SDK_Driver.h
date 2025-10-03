/*****************************************************************************
 * | File         :   VESC_SDK_Driver.h
 * | Author       :   VESC Display Team
 * | Function     :   VESC CAN driver using professional VESC CAN SDK
 * | Info         :   Integration with waas-rent/vesc_can_sdk library
 * ----------------
 * | This version :   V2.0
 * | Date         :   2024-12-25
 * | Info         :   Professional VESC CAN SDK integration
 *
 ******************************************************************************/

#ifndef __VESC_SDK_DRIVER_H
#define __VESC_SDK_DRIVER_H

#pragma once

// Include necessary driver headers
#include <Arduino.h>
#include "driver/twai.h"   // TWAI (CAN) driver

// Include VESC CAN SDK
extern "C" {
    #include "vesc_can_sdk.h"
}

// Define the GPIO pins used for CAN communication
#define VESC_CAN_TX_GPIO_NUM GPIO_NUM_6  // Transmit GPIO number for CAN
#define VESC_CAN_RX_GPIO_NUM GPIO_NUM_0  // Receive GPIO number for CAN
#define VESC_CAN_TAG "VESC_SDK"          // Log tag for the CAN driver

// VESC CAN Configuration
#define VESC_CAN_SPEED_250KBPS           // 250 kbps CAN speed
#define VESC_CAN_UPDATE_RATE_MS 100      // Request data every 100ms
#define VESC_CAN_TIMEOUT_MS 2000         // Connection timeout

// VESC Configuration
typedef struct {
    float wheel_diameter;    // Wheel diameter in meters
    float motor_poles;       // Number of motor poles
    float gear_ratio;        // Gear ratio
    float battery_cells;     // Number of battery cells in series
    float battery_capacity;  // Battery capacity in Ah
    uint8_t vesc_id;        // VESC CAN ID (0-255)
} vesc_sdk_config_t;

// VESC Values (enhanced with SDK data)
typedef struct {
    // Connection status
    bool connected;
    unsigned long last_update;
    
    // Basic values from VESC SDK
    vesc_values_t vesc_values;           // Full VESC values from SDK
    vesc_status_msg_1_t status_msg_1;    // Status message 1
    vesc_status_msg_2_t status_msg_2;    // Status message 2
    vesc_status_msg_3_t status_msg_3;    // Status message 3
    vesc_status_msg_4_t status_msg_4;    // Status message 4
    vesc_status_msg_5_t status_msg_5;    // Status message 5
    vesc_status_msg_6_t status_msg_6;    // Status message 6
    
    // Calculated values
    float speed_kmh;
    float distance_km;
    float battery_percentage;
    float power_watts;
    
} vesc_sdk_values_t;

// Function declarations:

/**
 * @brief Initializes the VESC SDK CAN interface
 * @param vesc_id VESC CAN ID (0-255, default 0)
 * @return true on success, false on failure
 */
bool VESC_SDK_Init(uint8_t vesc_id = 0);

/**
 * @brief Updates VESC communication (call in loop)
 */
void VESC_SDK_Loop();

/**
 * @brief Checks if VESC is connected
 * @return true if connected, false otherwise
 */
bool VESC_SDK_IsConnected();

/**
 * @brief Gets current VESC values
 * @return Structure with all VESC telemetry data
 */
vesc_sdk_values_t VESC_SDK_GetValues();

/**
 * @brief Sets VESC configuration parameters
 */
void VESC_SDK_SetConfig(float wheel_diameter, float motor_poles, float gear_ratio, 
                        float battery_cells, float battery_capacity);

/**
 * @brief Sends duty cycle command to VESC
 * @param duty_cycle Duty cycle (-1.0 to 1.0)
 */
void VESC_SDK_SetDuty(float duty_cycle);

/**
 * @brief Sends current command to VESC
 * @param current Current in Amperes
 */
void VESC_SDK_SetCurrent(float current);

/**
 * @brief Sends brake current command to VESC
 * @param brake_current Brake current in Amperes
 */
void VESC_SDK_SetBrakeCurrent(float brake_current);

/**
 * @brief Sends RPM command to VESC
 * @param rpm RPM value
 */
void VESC_SDK_SetRPM(float rpm);

/**
 * @brief Requests all VESC values
 */
void VESC_SDK_RequestValues();

/**
 * @brief Sends ping to VESC
 */
void VESC_SDK_Ping();

/**
 * @brief Prints debug information
 */
void VESC_SDK_PrintDebug();

/**
 * @brief Get speed in km/h (simple interface)
 */
float VESC_SDK_GetSpeed();

/**
 * @brief Get battery voltage
 */
float VESC_SDK_GetBatteryVoltage();

/**
 * @brief Get battery percentage
 */
float VESC_SDK_GetBatteryPercentage();

/**
 * @brief Get motor current
 */
float VESC_SDK_GetMotorCurrent();

/**
 * @brief Get power consumption
 */
float VESC_SDK_GetPower();

/**
 * @brief Get FET temperature
 */
float VESC_SDK_GetTempFET();

/**
 * @brief Get motor temperature
 */
float VESC_SDK_GetTempMotor();

/**
 * @brief Emergency stop
 */
void VESC_SDK_EmergencyStop();

#endif  // __VESC_SDK_DRIVER_H
