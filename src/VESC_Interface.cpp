#include "VESC_Interface.h"

void VESC_Interface_Init() {
#if USE_CAN_CONVERTER
    Serial.println("Using CAN Converter interface");
    VESC_CAN_Init();
#else
    Serial.println("Using UART interface");
    VESC_Init();
#endif
}

void VESC_Interface_Loop() {
#if USE_CAN_CONVERTER
    VESC_CAN_Loop();
#else
    VESC_Loop();
#endif
}

bool VESC_Interface_IsConnected() {
#if USE_CAN_CONVERTER
    return vesc_can.is_connected();
#else
    return vesc.is_connected();
#endif
}

float VESC_Interface_GetSpeedKmh() {
#if USE_CAN_CONVERTER
    return vesc_can.get_speed_kmh();
#else
    return vesc.get_speed_kmh();
#endif
}

float VESC_Interface_GetDistanceKm() {
#if USE_CAN_CONVERTER
    return vesc_can.get_distance_km();
#else
    return vesc.get_distance_km();
#endif
}

float VESC_Interface_GetBatteryPercentage() {
#if USE_CAN_CONVERTER
    return vesc_can.get_battery_percentage();
#else
    return vesc.get_battery_percentage();
#endif
}

float VESC_Interface_GetPowerWatts() {
#if USE_CAN_CONVERTER
    return vesc_can.get_power_watts();
#else
    return vesc.get_power_watts();
#endif
}

int VESC_Interface_GetBatteryBars() {
#if USE_CAN_CONVERTER
    return vesc_can.get_battery_bars();
#else
    return vesc.get_battery_bars();
#endif
}

void VESC_Interface_SetCurrent(float current) {
#if USE_CAN_CONVERTER
    vesc_can.set_current(current);
#else
    vesc.set_current(current);
#endif
}

void VESC_Interface_SetBrakeCurrent(float current) {
#if USE_CAN_CONVERTER
    vesc_can.set_brake_current(current);
#else
    vesc.set_brake_current(current);
#endif
}

void VESC_Interface_SetRpm(float rpm) {
#if USE_CAN_CONVERTER
    vesc_can.set_rpm(rpm);
#else
    vesc.set_rpm(rpm);
#endif
}

void VESC_Interface_SetDutyCycle(float duty) {
#if USE_CAN_CONVERTER
    vesc_can.set_duty_cycle(duty);
#else
    vesc.set_duty_cycle(duty);
#endif
}

#if USE_CAN_CONVERTER
vesc_can_values_t VESC_Interface_GetValues() {
    return vesc_can.get_values();
}
#else
vesc_values_t VESC_Interface_GetValues() {
    return vesc.get_values();
}
#endif
