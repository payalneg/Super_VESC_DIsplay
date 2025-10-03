/*****************************************************************************
 * | File         :   VESC_TWAI_Interface.cpp
 * | Author       :   VESC Display Team
 * | Function     :   Interface functions for LVGL integration
 * | Info         :   Provides easy access to VESC data for display
 * ----------------
 * | This version :   V1.0
 * | Date         :   2024-12-25
 * | Info         :   LVGL integration helper functions
 *
 ******************************************************************************/

#include "VESC_TWAI_Interface.h"
#include <stdio.h>

bool VESC_IsConnected() {
    return VESC_TWAI_IsConnected();
}

float VESC_GetSpeed() {
    if (!VESC_TWAI_IsConnected()) return 0.0f;
    vesc_twai_values_t values = VESC_TWAI_GetValues();
    return values.speed_kmh;
}

float VESC_GetBatteryVoltage() {
    if (!VESC_TWAI_IsConnected()) return 0.0f;
    vesc_twai_values_t values = VESC_TWAI_GetValues();
    return values.v_in;
}

float VESC_GetBatteryPercentage() {
    if (!VESC_TWAI_IsConnected()) return 0.0f;
    vesc_twai_values_t values = VESC_TWAI_GetValues();
    return values.battery_percentage;
}

float VESC_GetMotorCurrent() {
    if (!VESC_TWAI_IsConnected()) return 0.0f;
    vesc_twai_values_t values = VESC_TWAI_GetValues();
    return values.current;
}

float VESC_GetInputCurrent() {
    if (!VESC_TWAI_IsConnected()) return 0.0f;
    vesc_twai_values_t values = VESC_TWAI_GetValues();
    return values.current_in;
}

float VESC_GetPower() {
    if (!VESC_TWAI_IsConnected()) return 0.0f;
    vesc_twai_values_t values = VESC_TWAI_GetValues();
    return values.power_watts;
}

float VESC_GetRPM() {
    if (!VESC_TWAI_IsConnected()) return 0.0f;
    vesc_twai_values_t values = VESC_TWAI_GetValues();
    return values.rpm;
}

float VESC_GetDutyCycle() {
    if (!VESC_TWAI_IsConnected()) return 0.0f;
    vesc_twai_values_t values = VESC_TWAI_GetValues();
    return values.duty_cycle;
}

float VESC_GetTempFET() {
    if (!VESC_TWAI_IsConnected()) return 0.0f;
    vesc_twai_values_t values = VESC_TWAI_GetValues();
    return values.temp_fet;
}

float VESC_GetTempMotor() {
    if (!VESC_TWAI_IsConnected()) return 0.0f;
    vesc_twai_values_t values = VESC_TWAI_GetValues();
    return values.temp_motor;
}

float VESC_GetDistance() {
    if (!VESC_TWAI_IsConnected()) return 0.0f;
    vesc_twai_values_t values = VESC_TWAI_GetValues();
    return values.distance_km;
}

float VESC_GetAmpHours() {
    if (!VESC_TWAI_IsConnected()) return 0.0f;
    vesc_twai_values_t values = VESC_TWAI_GetValues();
    return values.amp_hours;
}

float VESC_GetWattHours() {
    if (!VESC_TWAI_IsConnected()) return 0.0f;
    vesc_twai_values_t values = VESC_TWAI_GetValues();
    return values.watt_hours;
}

const char* VESC_GetStatusString() {
    return VESC_TWAI_IsConnected() ? "Connected" : "Disconnected";
}

void VESC_GetSpeedString(char* buffer, size_t size) {
    if (!buffer || size == 0) return;
    
    if (VESC_TWAI_IsConnected()) {
        float speed = VESC_GetSpeed();
        snprintf(buffer, size, "%.1f km/h", speed);
    } else {
        snprintf(buffer, size, "-- km/h");
    }
}

void VESC_GetBatteryString(char* buffer, size_t size) {
    if (!buffer || size == 0) return;
    
    if (VESC_TWAI_IsConnected()) {
        float voltage = VESC_GetBatteryVoltage();
        float percentage = VESC_GetBatteryPercentage();
        snprintf(buffer, size, "%.1fV (%.0f%%)", voltage, percentage);
    } else {
        snprintf(buffer, size, "--V (--)");
    }
}

void VESC_GetPowerString(char* buffer, size_t size) {
    if (!buffer || size == 0) return;
    
    if (VESC_TWAI_IsConnected()) {
        float power = VESC_GetPower();
        if (power >= 1000.0f) {
            snprintf(buffer, size, "%.1fkW", power / 1000.0f);
        } else {
            snprintf(buffer, size, "%.0fW", power);
        }
    } else {
        snprintf(buffer, size, "--W");
    }
}

void VESC_GetTemperatureString(char* buffer, size_t size) {
    if (!buffer || size == 0) return;
    
    if (VESC_TWAI_IsConnected()) {
        float temp_fet = VESC_GetTempFET();
        float temp_motor = VESC_GetTempMotor();
        snprintf(buffer, size, "FET: %.0f°C Motor: %.0f°C", temp_fet, temp_motor);
    } else {
        snprintf(buffer, size, "FET: --°C Motor: --°C");
    }
}

void VESC_GetDistanceString(char* buffer, size_t size) {
    if (!buffer || size == 0) return;
    
    if (VESC_TWAI_IsConnected()) {
        float distance = VESC_GetDistance();
        if (distance >= 1.0f) {
            snprintf(buffer, size, "%.1f km", distance);
        } else {
            snprintf(buffer, size, "%.0f m", distance * 1000.0f);
        }
    } else {
        snprintf(buffer, size, "-- km");
    }
}

void VESC_SetDuty(float duty_cycle) {
    if (VESC_TWAI_IsConnected()) {
        // Clamp duty cycle to safe range
        if (duty_cycle > 1.0f) duty_cycle = 1.0f;
        if (duty_cycle < -1.0f) duty_cycle = -1.0f;
        
        VESC_TWAI_SetDuty(duty_cycle);
    }
}

void VESC_SetCurrent(float current) {
    if (VESC_TWAI_IsConnected()) {
        VESC_TWAI_SetCurrent(current);
    }
}

void VESC_SetBrake(float brake_current) {
    if (VESC_TWAI_IsConnected()) {
        // Ensure brake current is positive
        if (brake_current < 0.0f) brake_current = 0.0f;
        
        VESC_TWAI_SetBrakeCurrent(brake_current);
    }
}

void VESC_EmergencyStop() {
    if (VESC_TWAI_IsConnected()) {
        // Send zero duty cycle to stop immediately
        VESC_TWAI_SetDuty(0.0f);
        
        // Also send zero current command
        VESC_TWAI_SetCurrent(0.0f);
        
        Serial.println("EMERGENCY STOP: All motor outputs set to zero!");
    }
}
