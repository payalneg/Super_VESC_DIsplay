#pragma once

#include <lvgl.h>
#include "VESC_Driver.h"

// Dashboard function declarations
void dashboard_create(lv_obj_t * parent);
void dashboard_update_values(void);
void dashboard_cleanup(void);

// Dashboard data structure for VESC display
typedef struct {
    float voltage;
    float current_a;
    float power_w;
    float speed_kmh;
    float rpm;
    float esc_temp;
    float motor_temp;
    float trip_km;
    float total_km;
    float range_km;
    float battery_percent;
    float duty_cycle;
    float amp_hours;
    float watt_hours;
    bool connected;
    char time_str[16];
    char fw_version[8];
    char status_str[32];
} vesc_dashboard_data_t;

// Global dashboard data
extern vesc_dashboard_data_t dashboard_data;


