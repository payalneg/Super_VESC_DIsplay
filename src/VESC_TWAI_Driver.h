/*****************************************************************************
 * | File         :   VESC_TWAI_Driver.h
 * | Author       :   VESC Display Team
 * | Function     :   VESC CAN driver using ESP32 TWAI interface
 * | Info         :   Direct CAN communication with VESC controllers
 * ----------------
 * | This version :   V1.0
 * | Date         :   2024-12-25
 * | Info         :   TWAI-based VESC CAN implementation
 *
 ******************************************************************************/

#ifndef __VESC_TWAI_DRIVER_H
#define __VESC_TWAI_DRIVER_H

#pragma once

// Include necessary driver headers
#include <Arduino.h>
#include "driver/twai.h"   // TWAI (CAN) driver

// Define the GPIO pins used for CAN communication (from your board examples)
#define VESC_CAN_TX_GPIO_NUM GPIO_NUM_6 // Transmit GPIO number for CAN
#define VESC_CAN_RX_GPIO_NUM GPIO_NUM_0 // Receive GPIO number for CAN
#define VESC_CAN_TAG "VESC_TWAI" // Log tag for the CAN driver

// VESC CAN Configuration
#define VESC_CAN_SPEED  TWAI_TIMING_CONFIG_250KBITS()      // Standard VESC CAN speed
#define VESC_CAN_UPDATE_RATE_MS 100 // Request data every 100ms
#define VESC_CAN_TIMEOUT_MS 1000    // Connection timeout

// VESC CAN Message IDs (based on VESC CAN protocol)
#define VESC_CAN_PACKET_SET_DUTY 0
#define VESC_CAN_PACKET_SET_CURRENT 1
#define VESC_CAN_PACKET_SET_CURRENT_BRAKE 2
#define VESC_CAN_PACKET_SET_RPM 3
#define VESC_CAN_PACKET_SET_POS 4
#define VESC_CAN_PACKET_FILL_RX_BUFFER 5
#define VESC_CAN_PACKET_FILL_RX_BUFFER_LONG 6
#define VESC_CAN_PACKET_PROCESS_RX_BUFFER 7
#define VESC_CAN_PACKET_PROCESS_SHORT_BUFFER 8
#define VESC_CAN_PACKET_STATUS 9
#define VESC_CAN_PACKET_SET_CURRENT_REL 10
#define VESC_CAN_PACKET_SET_CURRENT_BRAKE_REL 11
#define VESC_CAN_PACKET_SET_CURRENT_HANDBRAKE 12
#define VESC_CAN_PACKET_SET_CURRENT_HANDBRAKE_REL 13
#define VESC_CAN_PACKET_STATUS_2 14
#define VESC_CAN_PACKET_STATUS_3 15
#define VESC_CAN_PACKET_STATUS_4 16
#define VESC_CAN_PACKET_PING 17
#define VESC_CAN_PACKET_PONG 18
#define VESC_CAN_PACKET_DETECT_APPLY_ALL_FOC 19
#define VESC_CAN_PACKET_DETECT_APPLY_ALL_FOC_RES 20
#define VESC_CAN_PACKET_CONF_CURRENT_LIMITS 21
#define VESC_CAN_PACKET_CONF_STORE_CURRENT_LIMITS 22
#define VESC_CAN_PACKET_CONF_CURRENT_LIMITS_IN 23
#define VESC_CAN_PACKET_CONF_STORE_CURRENT_LIMITS_IN 24
#define VESC_CAN_PACKET_CONF_FOC_ERPMS 25
#define VESC_CAN_PACKET_CONF_STORE_FOC_ERPMS 26
#define VESC_CAN_PACKET_STATUS_5 27
#define VESC_CAN_PACKET_POLL_TS5700N8501_STATUS 28
#define VESC_CAN_PACKET_CONF_BATTERY_CUT 29
#define VESC_CAN_PACKET_CONF_STORE_BATTERY_CUT 30
#define VESC_CAN_PACKET_SHUTDOWN 31

// VESC CAN ID calculation (VESC_ID << 8 | PACKET_TYPE)
#define VESC_CAN_ID(vesc_id, packet_type) ((vesc_id << 8) | packet_type)

// VESC Data Structure
typedef struct {
    // Connection status
    bool connected;
    unsigned long last_update;
    
    // Status Message 1 - CAN_PACKET_STATUS (9)
    float rpm;
    float current;
    float duty_cycle;
    
    // Status Message 2 - CAN_PACKET_STATUS_2 (14)
    float amp_hours;
    float amp_hours_charged;
    
    // Status Message 3 - CAN_PACKET_STATUS_3 (15)
    float watt_hours;
    float watt_hours_charged;
    
    // Status Message 4 - CAN_PACKET_STATUS_4 (16)
    float temp_fet;
    float temp_motor;
    float current_in;
    float pid_pos_now;
    
    // Status Message 5 - CAN_PACKET_STATUS_5 (27)
    float v_in;
    int32_t tacho_value;
    
    // Calculated values
    float speed_kmh;
    float distance_km;
    float battery_percentage;
    float power_watts;
    
} vesc_twai_values_t;

// VESC Configuration
typedef struct {
    float wheel_diameter;    // Wheel diameter in meters
    float motor_poles;       // Number of motor poles
    float gear_ratio;        // Gear ratio
    float battery_cells;     // Number of battery cells in series
    float battery_capacity;  // Battery capacity in Ah
    uint8_t vesc_id;        // VESC CAN ID (0-255)
} vesc_twai_config_t;

// Function declarations:

/**
 * @brief Initializes the VESC TWAI (CAN) interface
 * @param vesc_id VESC CAN ID (0-255, default 0)
 * @return true on success, false on failure
 */
bool VESC_TWAI_Init(uint8_t vesc_id = 0);

/**
 * @brief Updates VESC communication (call in loop)
 */
void VESC_TWAI_Loop();

/**
 * @brief Checks if VESC is connected
 * @return true if connected, false otherwise
 */
bool VESC_TWAI_IsConnected();

/**
 * @brief Gets current VESC values
 * @return Structure with all VESC telemetry data
 */
vesc_twai_values_t VESC_TWAI_GetValues();

/**
 * @brief Sets VESC configuration parameters
 */
void VESC_TWAI_SetConfig(float wheel_diameter, float motor_poles, float gear_ratio, 
                         float battery_cells, float battery_capacity);

/**
 * @brief Sends duty cycle command to VESC
 * @param duty_cycle Duty cycle (-1.0 to 1.0)
 */
void VESC_TWAI_SetDuty(float duty_cycle);

/**
 * @brief Sends current command to VESC
 * @param current Current in Amperes
 */
void VESC_TWAI_SetCurrent(float current);

/**
 * @brief Sends brake current command to VESC
 * @param brake_current Brake current in Amperes
 */
void VESC_TWAI_SetBrakeCurrent(float brake_current);

/**
 * @brief Sends RPM command to VESC
 * @param rpm RPM value
 */
void VESC_TWAI_SetRPM(float rpm);

/**
 * @brief Requests status updates from VESC
 */
void VESC_TWAI_RequestStatus();

/**
 * @brief Sends ping to VESC
 */
void VESC_TWAI_Ping();

/**
 * @brief Prints debug information
 */
void VESC_TWAI_PrintDebug();

#endif  // __VESC_TWAI_DRIVER_H
