#pragma once

#include "VESC_Config.h"

// Include both drivers
#include "VESC_Driver.h"
#include "VESC_CAN_Driver.h"

// Unified interface functions that work with either driver
void VESC_Interface_Init();
void VESC_Interface_Loop();
bool VESC_Interface_IsConnected();

// Unified data access
float VESC_Interface_GetSpeedKmh();
float VESC_Interface_GetDistanceKm();
float VESC_Interface_GetBatteryPercentage();
float VESC_Interface_GetPowerWatts();
int VESC_Interface_GetBatteryBars();

// Unified control functions
void VESC_Interface_SetCurrent(float current);
void VESC_Interface_SetBrakeCurrent(float current);
void VESC_Interface_SetRpm(float rpm);
void VESC_Interface_SetDutyCycle(float duty);

// Get raw values (returns appropriate struct based on active driver)
#if USE_CAN_CONVERTER
    vesc_can_values_t VESC_Interface_GetValues();
#else
    vesc_values_t VESC_Interface_GetValues();
#endif
