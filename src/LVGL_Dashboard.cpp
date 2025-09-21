#include "LVGL_Dashboard.h"
#include "VESC_Driver.h"
#include <stdio.h>
#include <string.h>

// Dashboard UI elements
static lv_obj_t * dashboard_screen = NULL;
static lv_obj_t * voltage_label = NULL;
static lv_obj_t * current_label = NULL;
static lv_obj_t * power_label = NULL;
static lv_obj_t * speed_label = NULL;
static lv_obj_t * rpm_label = NULL;
static lv_obj_t * esc_temp_label = NULL;
static lv_obj_t * motor_temp_label = NULL;
static lv_obj_t * trip_label = NULL;
static lv_obj_t * total_label = NULL;
static lv_obj_t * range_label = NULL;
static lv_obj_t * battery_bar = NULL;
static lv_obj_t * duty_label = NULL;
static lv_obj_t * fw_label = NULL;
static lv_obj_t * status_label = NULL;
static lv_obj_t * connection_dot = NULL;

// New dashboard UI elements for large block layout
static lv_obj_t * bat_percent_label = NULL;
static lv_obj_t * bat_level = NULL;

// Dashboard data with initial values
vesc_dashboard_data_t dashboard_data = {
    0.0f,           // voltage
    0.0f,           // current_a
    0.0f,           // power_w
    0.0f,           // speed_kmh
    0.0f,           // rpm
    0.0f,           // esc_temp
    0.0f,           // motor_temp
    0.0f,           // trip_km
    0.0f,           // total_km
    0.0f,           // range_km
    0.0f,           // battery_percent
    0.0f,           // duty_cycle
    0.0f,           // amp_hours
    0.0f,           // watt_hours
    false,          // connected
    "00:00:00",     // time_str
    "v1.0",         // fw_version
    "Disconnected"  // status_str
};

void dashboard_create(lv_obj_t * parent)
{
    // Set black background
    lv_obj_set_style_bg_color(parent, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, 0);
    
    // Disable scrollbars
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);

    dashboard_screen = parent;

    // Create optimized layout for 480x480 square display
    // Grid layout: 2x3 (2 columns, 3 rows) with better spacing and larger components
    // Layout optimized for square format with improved visual hierarchy
    
    // Define grid parameters for 480x480
    int container_width = 235;   // Increased width for better use of space
    int container_height_large = 150; // Larger height for main displays
    int container_height_medium = 110; // Medium height for secondary displays
    int margin = 5;              // Margin between containers
    int start_x = margin;
    int start_y = margin;

    // TOP ROW: SPEED (left) | CURRENT (right) - EXTRA LARGE
    lv_obj_t * speed_container = lv_obj_create(parent);
    lv_obj_remove_style_all(speed_container);
    lv_obj_set_size(speed_container, container_width, container_height_large);
    lv_obj_set_pos(speed_container, start_x, start_y);
    lv_obj_set_style_bg_color(speed_container, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(speed_container, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(speed_container, 2, 0);
    lv_obj_set_style_border_color(speed_container, lv_color_hex(0x333333), 0);
    lv_obj_set_style_radius(speed_container, 8, 0);
    lv_obj_clear_flag(speed_container, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * speed_title = lv_label_create(speed_container);
    lv_label_set_text(speed_title, "SPEED");
    lv_obj_set_style_text_color(speed_title, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(speed_title, &lv_font_montserrat_18, 0);
    lv_obj_align(speed_title, LV_ALIGN_TOP_MID, 0, 8);

    speed_label = lv_label_create(speed_container);
    lv_obj_set_style_text_color(speed_label, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_text_font(speed_label, &lv_font_montserrat_48, 0);
    lv_obj_align(speed_label, LV_ALIGN_CENTER, 0, 10);

    lv_obj_t * kmh_label = lv_label_create(speed_container);
    lv_label_set_text(kmh_label, "km/h");
    lv_obj_set_style_text_color(kmh_label, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_style_text_font(kmh_label, &lv_font_montserrat_16, 0);
    lv_obj_align(kmh_label, LV_ALIGN_BOTTOM_MID, 0, -8);

    // TOP RIGHT: CURRENT - EXTRA LARGE
    lv_obj_t * current_container = lv_obj_create(parent);
    lv_obj_remove_style_all(current_container);
    lv_obj_set_size(current_container, container_width, container_height_large);
    lv_obj_set_pos(current_container, start_x + container_width + margin, start_y);
    lv_obj_set_style_bg_color(current_container, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(current_container, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(current_container, 2, 0);
    lv_obj_set_style_border_color(current_container, lv_color_hex(0x333333), 0);
    lv_obj_set_style_radius(current_container, 8, 0);
    lv_obj_clear_flag(current_container, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * current_title = lv_label_create(current_container);
    lv_label_set_text(current_title, "CURRENT");
    lv_obj_set_style_text_color(current_title, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(current_title, &lv_font_montserrat_18, 0);
    lv_obj_align(current_title, LV_ALIGN_TOP_MID, 0, 8);

    current_label = lv_label_create(current_container);
    lv_obj_set_style_text_color(current_label, lv_color_hex(0x00AAFF), 0);
    lv_obj_set_style_text_font(current_label, &lv_font_montserrat_48, 0);
    lv_obj_align(current_label, LV_ALIGN_CENTER, 0, 10);

    lv_obj_t * current_unit = lv_label_create(current_container);
    lv_label_set_text(current_unit, "A");
    lv_obj_set_style_text_color(current_unit, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_style_text_font(current_unit, &lv_font_montserrat_16, 0);
    lv_obj_align(current_unit, LV_ALIGN_BOTTOM_MID, 0, -8);

    // MIDDLE ROW: BATTERY % (left) | VOLTAGE (right) - MEDIUM
    int middle_y = start_y + container_height_large + margin;
    
    lv_obj_t * percent_container = lv_obj_create(parent);
    lv_obj_remove_style_all(percent_container);
    lv_obj_set_size(percent_container, container_width, container_height_medium);
    lv_obj_set_pos(percent_container, start_x, middle_y);
    lv_obj_set_style_bg_color(percent_container, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(percent_container, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(percent_container, 2, 0);
    lv_obj_set_style_border_color(percent_container, lv_color_hex(0x333333), 0);
    lv_obj_set_style_radius(percent_container, 8, 0);
    lv_obj_clear_flag(percent_container, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * percent_title = lv_label_create(percent_container);
    lv_label_set_text(percent_title, "BATTERY %");
    lv_obj_set_style_text_color(percent_title, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(percent_title, &lv_font_montserrat_16, 0);
    lv_obj_align(percent_title, LV_ALIGN_TOP_MID, 0, 8);

    bat_percent_label = lv_label_create(percent_container);
    lv_obj_set_style_text_color(bat_percent_label, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_text_font(bat_percent_label, &lv_font_montserrat_48, 0);
    lv_obj_align(bat_percent_label, LV_ALIGN_CENTER, 0, 8);

    // Keep battery_bar and bat_level as NULL for compatibility
    battery_bar = NULL;
    bat_level = NULL;

    // MIDDLE RIGHT: VOLTAGE - MEDIUM
    lv_obj_t * battery_container = lv_obj_create(parent);
    lv_obj_remove_style_all(battery_container);
    lv_obj_set_size(battery_container, container_width, container_height_medium);
    lv_obj_set_pos(battery_container, start_x + container_width + margin, middle_y);
    lv_obj_set_style_bg_color(battery_container, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(battery_container, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(battery_container, 2, 0);
    lv_obj_set_style_border_color(battery_container, lv_color_hex(0x333333), 0);
    lv_obj_set_style_radius(battery_container, 8, 0);
    lv_obj_clear_flag(battery_container, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * battery_title = lv_label_create(battery_container);
    lv_label_set_text(battery_title, "VOLTAGE");
    lv_obj_set_style_text_color(battery_title, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(battery_title, &lv_font_montserrat_16, 0);
    lv_obj_align(battery_title, LV_ALIGN_TOP_MID, 0, 8);

    voltage_label = lv_label_create(battery_container);
    lv_obj_set_style_text_color(voltage_label, lv_color_hex(0xFFAA00), 0);
    lv_obj_set_style_text_font(voltage_label, &lv_font_montserrat_36, 0);
    lv_obj_align(voltage_label, LV_ALIGN_CENTER, 0, 5);

    lv_obj_t * voltage_unit = lv_label_create(battery_container);
    lv_label_set_text(voltage_unit, "V");
    lv_obj_set_style_text_color(voltage_unit, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_style_text_font(voltage_unit, &lv_font_montserrat_16, 0);
    lv_obj_align(voltage_unit, LV_ALIGN_BOTTOM_MID, 0, -8);

    // BOTTOM ROW: ODOMETER (left) | TEMPERATURE (right) - MEDIUM
    int bottom_y = middle_y + container_height_medium + margin;
    
    lv_obj_t * odo_container = lv_obj_create(parent);
    lv_obj_remove_style_all(odo_container);
    lv_obj_set_size(odo_container, container_width, container_height_medium);
    lv_obj_set_pos(odo_container, start_x, bottom_y);
    lv_obj_set_style_bg_color(odo_container, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(odo_container, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(odo_container, 2, 0);
    lv_obj_set_style_border_color(odo_container, lv_color_hex(0x333333), 0);
    lv_obj_set_style_radius(odo_container, 8, 0);
    lv_obj_clear_flag(odo_container, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * odo_title = lv_label_create(odo_container);
    lv_label_set_text(odo_title, "ODOMETER");
    lv_obj_set_style_text_color(odo_title, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(odo_title, &lv_font_montserrat_16, 0);
    lv_obj_align(odo_title, LV_ALIGN_TOP_MID, 0, 8);

    total_label = lv_label_create(odo_container);
    lv_obj_set_style_text_color(total_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(total_label, &lv_font_montserrat_32, 0);
    lv_obj_align(total_label, LV_ALIGN_CENTER, 0, 5);

    lv_obj_t * km_label = lv_label_create(odo_container);
    lv_label_set_text(km_label, "km");
    lv_obj_set_style_text_color(km_label, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_style_text_font(km_label, &lv_font_montserrat_16, 0);
    lv_obj_align(km_label, LV_ALIGN_BOTTOM_MID, 0, -8);

    // BOTTOM RIGHT: TEMPERATURE - MEDIUM
    lv_obj_t * temp_container = lv_obj_create(parent);
    lv_obj_remove_style_all(temp_container);
    lv_obj_set_size(temp_container, container_width, container_height_medium);
    lv_obj_set_pos(temp_container, start_x + container_width + margin, bottom_y);
    lv_obj_set_style_bg_color(temp_container, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(temp_container, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(temp_container, 2, 0);
    lv_obj_set_style_border_color(temp_container, lv_color_hex(0x333333), 0);
    lv_obj_set_style_radius(temp_container, 8, 0);
    lv_obj_clear_flag(temp_container, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * temp_title = lv_label_create(temp_container);
    lv_label_set_text(temp_title, "TEMPERATURE");
    lv_obj_set_style_text_color(temp_title, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(temp_title, &lv_font_montserrat_14, 0);
    lv_obj_align(temp_title, LV_ALIGN_TOP_MID, 0, 8);

    // Motor temperature
    motor_temp_label = lv_label_create(temp_container);
    lv_obj_set_style_text_color(motor_temp_label, lv_color_hex(0xFF6600), 0);
    lv_obj_set_style_text_font(motor_temp_label, &lv_font_montserrat_24, 0);
    lv_obj_set_pos(motor_temp_label, 20, 35);

    lv_obj_t * motor_temp_unit = lv_label_create(temp_container);
    lv_label_set_text(motor_temp_unit, "°C");
    lv_obj_set_style_text_color(motor_temp_unit, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_style_text_font(motor_temp_unit, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(motor_temp_unit, 70, 38);

    lv_obj_t * motor_label = lv_label_create(temp_container);
    lv_label_set_text(motor_label, "MOTOR");
    lv_obj_set_style_text_color(motor_label, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_style_text_font(motor_label, &lv_font_montserrat_12, 0);
    lv_obj_set_pos(motor_label, 20, 60);

    // Controller temperature
    esc_temp_label = lv_label_create(temp_container);
    lv_obj_set_style_text_color(esc_temp_label, lv_color_hex(0xFF6600), 0);
    lv_obj_set_style_text_font(esc_temp_label, &lv_font_montserrat_24, 0);
    lv_obj_set_pos(esc_temp_label, 130, 35);

    lv_obj_t * ctrl_temp_unit = lv_label_create(temp_container);
    lv_label_set_text(ctrl_temp_unit, "°C");
    lv_obj_set_style_text_color(ctrl_temp_unit, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_style_text_font(ctrl_temp_unit, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(ctrl_temp_unit, 180, 38);

    lv_obj_t * ctrl_label = lv_label_create(temp_container);
    lv_label_set_text(ctrl_label, "CTRL");
    lv_obj_set_style_text_color(ctrl_label, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_style_text_font(ctrl_label, &lv_font_montserrat_12, 0);
    lv_obj_set_pos(ctrl_label, 130, 60);

    // Initialize other needed variables
    power_label = NULL;
    rpm_label = NULL;
    trip_label = NULL;
    range_label = NULL;
    duty_label = NULL;
    fw_label = NULL;
    status_label = NULL;
    connection_dot = NULL;

    // Update all values
    dashboard_update_values();
}

void dashboard_update_values(void)
{
    if (dashboard_screen == NULL) return;

    static uint32_t last_update = 0;
    uint32_t now = lv_tick_get();
    
    // Update only every 100ms to avoid excessive updates
    if (now - last_update < 100) return;
    last_update = now;

    char buf[32];
    
    // Get VESC values
    vesc_values_t vesc_values = vesc.get_values();
    
    // Update dashboard data from VESC
    dashboard_data.connected = vesc.is_connected();
    dashboard_data.voltage = vesc_values.v_in;
    dashboard_data.current_a = vesc_values.current_motor;
    dashboard_data.power_w = vesc.get_power_watts();
    dashboard_data.speed_kmh = vesc.get_speed_kmh();
    dashboard_data.rpm = vesc_values.rpm;
    dashboard_data.esc_temp = vesc_values.temp_fet;
    dashboard_data.motor_temp = vesc_values.temp_motor;
    dashboard_data.duty_cycle = vesc_values.duty_now * 100.0f;
    dashboard_data.battery_percent = vesc.get_battery_percentage();
    dashboard_data.total_km = vesc.get_distance_km();
    dashboard_data.amp_hours = vesc_values.amp_hours;
    dashboard_data.watt_hours = vesc_values.watt_hours;
    
    // Update speed (main display)
    if (speed_label) {
        snprintf(buf, sizeof(buf), "%.0f", dashboard_data.speed_kmh);
        lv_label_set_text(speed_label, buf);
    }

    // Update current
    if (current_label) {
        snprintf(buf, sizeof(buf), "%.1f", dashboard_data.current_a);
        lv_label_set_text(current_label, buf);
    }

    // Update voltage in battery section
    if (voltage_label) {
        snprintf(buf, sizeof(buf), "%.1f", dashboard_data.voltage);
        lv_label_set_text(voltage_label, buf);
    }

    // Update total distance (odometer)
    if (total_label) {
        snprintf(buf, sizeof(buf), "%.0f", dashboard_data.total_km);
        lv_label_set_text(total_label, buf);
    }

    // Update temperatures
    if (esc_temp_label) {
        snprintf(buf, sizeof(buf), "%.0f", dashboard_data.esc_temp);
        lv_label_set_text(esc_temp_label, buf);
    }
    
    if (motor_temp_label) {
        snprintf(buf, sizeof(buf), "%.0f", dashboard_data.motor_temp);
        lv_label_set_text(motor_temp_label, buf);
    }

    // Update battery percentage (without % symbol)
    if (bat_percent_label) {
        snprintf(buf, sizeof(buf), "%.0f", dashboard_data.battery_percent);
        lv_label_set_text(bat_percent_label, buf);
    }

    // Update battery percentage text color based on level
    if (bat_percent_label) {
        lv_color_t battery_color;
        
        if (dashboard_data.battery_percent > 50) {
            battery_color = lv_color_hex(0x00FF00); // Green
        } else if (dashboard_data.battery_percent > 20) {
            battery_color = lv_color_hex(0xFFFF00); // Yellow
        } else {
            battery_color = lv_color_hex(0xFF0000); // Red
        }
        
        lv_obj_set_style_text_color(bat_percent_label, battery_color, 0);
    }
    
}

void dashboard_cleanup(void)
{
    dashboard_screen = NULL;
    voltage_label = NULL;
    current_label = NULL;
    power_label = NULL;
    speed_label = NULL;
    rpm_label = NULL;
    esc_temp_label = NULL;
    motor_temp_label = NULL;
    trip_label = NULL;
    total_label = NULL;
    battery_bar = NULL;
    duty_label = NULL;
    fw_label = NULL;
    status_label = NULL;
    connection_dot = NULL;
    bat_percent_label = NULL;
    bat_level = NULL;
}
