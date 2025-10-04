#ifndef __VESC_SDK_DRIVER_H
#define __VESC_SDK_DRIVER_H

#pragma once

// Uncomment to enable detailed CAN debugging
//#define DEBUG_CAN

#include <Arduino.h>
#include "driver/twai.h"

// VESC CAN SDK
#include "vesc_can_sdk.h"

// Hardware Configuration
#define VESC_CAN_TX_GPIO_NUM GPIO_NUM_6
#define VESC_CAN_RX_GPIO_NUM GPIO_NUM_0
#define VESC_CAN_TAG "VESC_SDK"
#define VESC_CAN_ID 50 

// CAN Configuration
#define VESC_CAN_SPEED  TWAI_TIMING_CONFIG_250KBITS()
#define VESC_CAN_UPDATE_RATE_MS 100
#define VESC_CAN_TIMEOUT_MS 5000


// VESC Data Structure
typedef struct {
    // Connection status
    bool connected;
    unsigned long last_update;
    
    // STATUS_1: Basic motor values (RPM, current, duty)
    float rpm;
    float current;
    float duty_cycle;
    
    // STATUS_2: Energy consumption
    float amp_hours;
    float amp_hours_charged;
    
    // STATUS_3: Energy consumption in watts
    float watt_hours;
    float watt_hours_charged;
    
    // STATUS_4: Temperatures and input
    float temp_fet;
    float temp_motor;
    float current_in;
    float pid_pos_now;
    
    // STATUS_5: Voltage and tachometer
    float voltage;
    int32_t tacho_value;
    
    // STATUS_6: ADC and PPM values
    float adc_1;
    float adc_2;
    float adc_3;
    float ppm;
    
    // Other values from COMM_GET_VALUES
    uint32_t fault_code;
    
    // Battery info (calculated)
    float battery_level;
    float speed_kmh;
    
    // Debug info
    uint32_t last_can_id;
    uint8_t last_can_data[8];
    uint8_t last_can_length;
} vesc_sdk_data_t;

// Function declarations
bool VESC_SDK_Init(uint8_t vesc_id = VESC_CAN_ID);
void VESC_SDK_Loop();
bool VESC_SDK_IsConnected();
vesc_sdk_data_t VESC_SDK_GetData();

// VESC Control Functions
void VESC_SDK_SetDuty(float duty);
void VESC_SDK_SetCurrent(float current);
void VESC_SDK_SetBrakeCurrent(float current);
void VESC_SDK_SetRPM(float rpm);
void VESC_SDK_RequestStatus();
void VESC_SDK_Ping();

// Debug Functions
void VESC_SDK_PrintDebug();

#endif // __VESC_SDK_DRIVER_H
