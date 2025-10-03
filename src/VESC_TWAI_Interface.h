/*****************************************************************************
 * | File         :   VESC_TWAI_Interface.h
 * | Author       :   VESC Display Team
 * | Function     :   Interface functions for LVGL integration
 * | Info         :   Provides easy access to VESC data for display
 * ----------------
 * | This version :   V1.0
 * | Date         :   2024-12-25
 * | Info         :   LVGL integration helper functions
 *
 ******************************************************************************/

#ifndef __VESC_TWAI_INTERFACE_H
#define __VESC_TWAI_INTERFACE_H

#pragma once

#include <Arduino.h>
#include "VESC_TWAI_Driver.h"

// Simple interface functions for LVGL integration
// These functions provide easy access to VESC data without complex structures

/**
 * @brief Check if VESC is connected
 * @return true if connected, false otherwise
 */
bool VESC_IsConnected();

/**
 * @brief Get current speed in km/h
 * @return Speed in km/h
 */
float VESC_GetSpeed();

/**
 * @brief Get battery voltage
 * @return Battery voltage in volts
 */
float VESC_GetBatteryVoltage();

/**
 * @brief Get battery percentage (0-100)
 * @return Battery percentage
 */
float VESC_GetBatteryPercentage();

/**
 * @brief Get motor current in amperes
 * @return Motor current in A
 */
float VESC_GetMotorCurrent();

/**
 * @brief Get input current in amperes
 * @return Input current in A
 */
float VESC_GetInputCurrent();

/**
 * @brief Get power consumption in watts
 * @return Power in watts
 */
float VESC_GetPower();

/**
 * @brief Get motor RPM
 * @return RPM value
 */
float VESC_GetRPM();

/**
 * @brief Get duty cycle (0-1)
 * @return Duty cycle
 */
float VESC_GetDutyCycle();

/**
 * @brief Get FET temperature in Celsius
 * @return Temperature in °C
 */
float VESC_GetTempFET();

/**
 * @brief Get motor temperature in Celsius
 * @return Temperature in °C
 */
float VESC_GetTempMotor();

/**
 * @brief Get total distance traveled in km
 * @return Distance in km
 */
float VESC_GetDistance();

/**
 * @brief Get consumed amp hours
 * @return Amp hours consumed
 */
float VESC_GetAmpHours();

/**
 * @brief Get consumed watt hours
 * @return Watt hours consumed
 */
float VESC_GetWattHours();

/**
 * @brief Get connection status as string
 * @return "Connected" or "Disconnected"
 */
const char* VESC_GetStatusString();

/**
 * @brief Get formatted speed string
 * @param buffer Buffer to store the string
 * @param size Buffer size
 * @return Formatted string like "25.3 km/h"
 */
void VESC_GetSpeedString(char* buffer, size_t size);

/**
 * @brief Get formatted battery string
 * @param buffer Buffer to store the string
 * @param size Buffer size
 * @return Formatted string like "42.0V (85%)"
 */
void VESC_GetBatteryString(char* buffer, size_t size);

/**
 * @brief Get formatted power string
 * @param buffer Buffer to store the string
 * @param size Buffer size
 * @return Formatted string like "1250W"
 */
void VESC_GetPowerString(char* buffer, size_t size);

/**
 * @brief Get formatted temperature string
 * @param buffer Buffer to store the string
 * @param size Buffer size
 * @return Formatted string like "FET: 45°C Motor: 38°C"
 */
void VESC_GetTemperatureString(char* buffer, size_t size);

/**
 * @brief Get formatted distance string
 * @param buffer Buffer to store the string
 * @param size Buffer size
 * @return Formatted string like "12.5 km"
 */
void VESC_GetDistanceString(char* buffer, size_t size);

/**
 * @brief Send motor control command (duty cycle)
 * @param duty_cycle Duty cycle from -1.0 to 1.0
 * @warning Use with caution! This will move the motor!
 */
void VESC_SetDuty(float duty_cycle);

/**
 * @brief Send motor control command (current)
 * @param current Current in amperes
 * @warning Use with caution! This will move the motor!
 */
void VESC_SetCurrent(float current);

/**
 * @brief Send brake command
 * @param brake_current Brake current in amperes
 */
void VESC_SetBrake(float brake_current);

/**
 * @brief Emergency stop - set all outputs to zero
 */
void VESC_EmergencyStop();

#endif  // __VESC_TWAI_INTERFACE_H
