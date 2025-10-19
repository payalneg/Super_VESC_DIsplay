/*
	Copyright 2025 Super VESC Display
	
	Global settings and configuration with NVS storage
*/

#include "dev_settings.h"
#include "Display_ST7701.h"
#include "comm_can.h"
#include "debug_log.h"
#include "vesc_battery_calc.h"
#include <Preferences.h>

// NVS namespace for settings
#define SETTINGS_NAMESPACE "vesc_settings"

// NVS keys
#define KEY_TARGET_VESC_ID    "target_id"
#define KEY_CAN_SPEED         "can_speed"
#define KEY_BRIGHTNESS        "brightness"
#define KEY_CONTROLLER_ID     "controller_id"
#define KEY_BATTERY_CAPACITY  "bat_capacity"
#define KEY_BATTERY_CALC_MODE "bat_calc_mode"
#define KEY_SHOW_FPS          "show_fps"
#define KEY_WHEEL_DIAMETER    "wheel_diam"
#define KEY_MOTOR_POLES       "motor_poles"

// Default settings
#define DEFAULT_TARGET_VESC_ID      10
#define DEFAULT_CAN_SPEED           CAN_SPEED_1000_KBPS
#define DEFAULT_BRIGHTNESS          80
#define DEFAULT_CONTROLLER_ID       255 //DO NOT CHANGE PLEASE
#define DEFAULT_BATTERY_CAPACITY    15.0f
#define DEFAULT_BATTERY_CALC_MODE   BATTERY_CALC_DIRECT
#define DEFAULT_SHOW_FPS            false
#define DEFAULT_WHEEL_DIAMETER_MM   200  // 200mm = typical skateboard/scooter wheel
#define DEFAULT_MOTOR_POLES         7    // Standard for most VESC motors

// Global settings storage
static device_settings_t g_settings;
static Preferences preferences;
static bool settings_initialized = false;

// Legacy compatibility
const uint8_t target_vesc_id = DEFAULT_TARGET_VESC_ID;

// Initialize settings system
void settings_init(void) {
    if (settings_initialized) {
        return;
    }
    
    LOG_INFO(SYSTEM, "Initializing settings...");
    
    // Set defaults first
    g_settings.target_vesc_id = DEFAULT_TARGET_VESC_ID;
    g_settings.can_speed = DEFAULT_CAN_SPEED;
    g_settings.screen_brightness = DEFAULT_BRIGHTNESS;
    g_settings.controller_id = DEFAULT_CONTROLLER_ID;
    g_settings.battery_capacity = DEFAULT_BATTERY_CAPACITY;
    g_settings.battery_calc_mode = DEFAULT_BATTERY_CALC_MODE;
    g_settings.show_fps = DEFAULT_SHOW_FPS;
    g_settings.wheel_diameter_mm = DEFAULT_WHEEL_DIAMETER_MM;
    g_settings.motor_poles = DEFAULT_MOTOR_POLES;
    
    // Load from NVS
    settings_load();
    
    settings_initialized = true;
    LOG_INFO(SYSTEM, "Settings initialized: Target ID=%d, CAN Speed=%d kbps, Brightness=%d%%, Controller ID=%d, Battery Capacity=%.1f Ah, Calc Mode=%d", 
             g_settings.target_vesc_id, (int)g_settings.can_speed, g_settings.screen_brightness, g_settings.controller_id,
             g_settings.battery_capacity, g_settings.battery_calc_mode);
}

// Load settings from NVS
void settings_load(void) {
    if (!preferences.begin(SETTINGS_NAMESPACE, true)) {  // read-only
        LOG_WARN(SYSTEM, "Failed to open NVS for reading, using defaults");
        return;
    }
    
    // Load each setting with defaults as fallback
    g_settings.target_vesc_id = preferences.getUChar(KEY_TARGET_VESC_ID, DEFAULT_TARGET_VESC_ID);
    g_settings.can_speed = (can_speed_t)preferences.getUInt(KEY_CAN_SPEED, DEFAULT_CAN_SPEED);
    g_settings.screen_brightness = preferences.getUChar(KEY_BRIGHTNESS, DEFAULT_BRIGHTNESS);
    g_settings.controller_id = preferences.getUChar(KEY_CONTROLLER_ID, DEFAULT_CONTROLLER_ID);
    g_settings.battery_capacity = preferences.getFloat(KEY_BATTERY_CAPACITY, DEFAULT_BATTERY_CAPACITY);
    g_settings.battery_calc_mode = (battery_calc_mode_t)preferences.getUChar(KEY_BATTERY_CALC_MODE, DEFAULT_BATTERY_CALC_MODE);
    g_settings.show_fps = preferences.getBool(KEY_SHOW_FPS, DEFAULT_SHOW_FPS);
    g_settings.wheel_diameter_mm = preferences.getUShort(KEY_WHEEL_DIAMETER, DEFAULT_WHEEL_DIAMETER_MM);
    g_settings.motor_poles = preferences.getUChar(KEY_MOTOR_POLES, DEFAULT_MOTOR_POLES);
    
    preferences.end();
    
    // Validate settings
    if (g_settings.target_vesc_id == 0 || g_settings.target_vesc_id > 254) {
        LOG_WARN(SYSTEM, "Invalid target VESC ID %d, using default %d", g_settings.target_vesc_id, DEFAULT_TARGET_VESC_ID);
        g_settings.target_vesc_id = DEFAULT_TARGET_VESC_ID;
    }
    
    if (g_settings.screen_brightness > 100) {
        LOG_WARN(SYSTEM, "Invalid brightness %d, using default %d", g_settings.screen_brightness, DEFAULT_BRIGHTNESS);
        g_settings.screen_brightness = DEFAULT_BRIGHTNESS;
    }
    
    if (g_settings.controller_id == 0 || g_settings.controller_id > 254) {
        LOG_WARN(SYSTEM, "Invalid controller ID %d, using default %d", g_settings.controller_id, DEFAULT_CONTROLLER_ID);
        g_settings.controller_id = DEFAULT_CONTROLLER_ID;
    }
    
    // Validate battery capacity
    if (g_settings.battery_capacity < 1.0f || g_settings.battery_capacity > 200.0f) {
        LOG_WARN(SYSTEM, "Invalid battery capacity %.1f, using default %.1f", g_settings.battery_capacity, DEFAULT_BATTERY_CAPACITY);
        g_settings.battery_capacity = DEFAULT_BATTERY_CAPACITY;
    }
    
    // Validate battery calculation mode
    if (g_settings.battery_calc_mode != BATTERY_CALC_DIRECT && g_settings.battery_calc_mode != BATTERY_CALC_SMART) {
        LOG_WARN(SYSTEM, "Invalid battery calc mode %d, using default %d", g_settings.battery_calc_mode, DEFAULT_BATTERY_CALC_MODE);
        g_settings.battery_calc_mode = DEFAULT_BATTERY_CALC_MODE;
    }
    
    // Validate CAN speed
    if (g_settings.can_speed != CAN_SPEED_125_KBPS && 
        g_settings.can_speed != CAN_SPEED_250_KBPS && 
        g_settings.can_speed != CAN_SPEED_500_KBPS && 
        g_settings.can_speed != CAN_SPEED_1000_KBPS) {
        LOG_WARN(SYSTEM, "Invalid CAN speed %d, using default %d", (int)g_settings.can_speed, DEFAULT_CAN_SPEED);
        g_settings.can_speed = DEFAULT_CAN_SPEED;
    }
    
    // Validate wheel diameter
    if (g_settings.wheel_diameter_mm < 50 || g_settings.wheel_diameter_mm > 2000) {
        LOG_WARN(SYSTEM, "Invalid wheel diameter %d mm, using default %d", g_settings.wheel_diameter_mm, DEFAULT_WHEEL_DIAMETER_MM);
        g_settings.wheel_diameter_mm = DEFAULT_WHEEL_DIAMETER_MM;
    }

    // Validate motor poles
    if (g_settings.motor_poles < 2 || g_settings.motor_poles > 20) {
        LOG_WARN(SYSTEM, "Invalid motor poles %d, using default %d", g_settings.motor_poles, DEFAULT_MOTOR_POLES);
        g_settings.motor_poles = DEFAULT_MOTOR_POLES;
    }

    LOG_INFO(SYSTEM, "Settings loaded from NVS");
}

// Save settings to NVS
void settings_save(void) {
    if (!preferences.begin(SETTINGS_NAMESPACE, false)) {  // read-write
        LOG_ERROR(SYSTEM, "Failed to open NVS for writing");
        return;
    }
    
    preferences.putUChar(KEY_TARGET_VESC_ID, g_settings.target_vesc_id);
    preferences.putUInt(KEY_CAN_SPEED, (uint32_t)g_settings.can_speed);
    preferences.putUChar(KEY_BRIGHTNESS, g_settings.screen_brightness);
    preferences.putUChar(KEY_CONTROLLER_ID, g_settings.controller_id);
    preferences.putFloat(KEY_BATTERY_CAPACITY, g_settings.battery_capacity);
    preferences.putUChar(KEY_BATTERY_CALC_MODE, (uint8_t)g_settings.battery_calc_mode);
    preferences.putBool(KEY_SHOW_FPS, g_settings.show_fps);
    preferences.putUShort(KEY_WHEEL_DIAMETER, g_settings.wheel_diameter_mm);
    preferences.putUChar(KEY_MOTOR_POLES, g_settings.motor_poles);
    
    preferences.end();
    
    LOG_INFO(SYSTEM, "Settings saved to NVS");
}

// Reset settings to defaults
void settings_reset_to_defaults(void) {
    LOG_INFO(SYSTEM, "Resetting settings to defaults...");
    
    g_settings.target_vesc_id = DEFAULT_TARGET_VESC_ID;
    g_settings.can_speed = DEFAULT_CAN_SPEED;
    g_settings.screen_brightness = DEFAULT_BRIGHTNESS;
    g_settings.controller_id = DEFAULT_CONTROLLER_ID;
    g_settings.battery_capacity = DEFAULT_BATTERY_CAPACITY;
    g_settings.battery_calc_mode = DEFAULT_BATTERY_CALC_MODE;
    g_settings.show_fps = DEFAULT_SHOW_FPS;
    g_settings.wheel_diameter_mm = DEFAULT_WHEEL_DIAMETER_MM;
    g_settings.motor_poles = DEFAULT_MOTOR_POLES;

    settings_save();
    LOG_INFO(SYSTEM, "Settings reset to defaults");
}

// Getters
uint8_t settings_get_target_vesc_id(void) {
    return g_settings.target_vesc_id;
}

can_speed_t settings_get_can_speed(void) {
    return g_settings.can_speed;
}

uint8_t settings_get_screen_brightness(void) {
    return g_settings.screen_brightness;
}

uint8_t settings_get_controller_id(void) {
    return g_settings.controller_id;
}

float settings_get_battery_capacity(void) {
    return g_settings.battery_capacity;
}

battery_calc_mode_t settings_get_battery_calc_mode(void) {
    return g_settings.battery_calc_mode;
}

bool settings_get_show_fps(void) {
    return g_settings.show_fps;
}

uint16_t settings_get_wheel_diameter_mm(void) {
    return g_settings.wheel_diameter_mm;
}

uint8_t settings_get_motor_poles(void) {
    return g_settings.motor_poles;
}

// Setters
void settings_set_target_vesc_id(uint8_t id) {
    if (id == 0 || id > 254) {
        LOG_WARN(SYSTEM, "Invalid target VESC ID %d", id);
        return;
    }
    
    g_settings.target_vesc_id = id;
    settings_save();
    LOG_INFO(SYSTEM, "Target VESC ID set to %d", id);
}

void settings_set_can_speed(can_speed_t speed) {
    if (speed != CAN_SPEED_125_KBPS && 
        speed != CAN_SPEED_250_KBPS && 
        speed != CAN_SPEED_500_KBPS && 
        speed != CAN_SPEED_1000_KBPS) {
        LOG_WARN(SYSTEM, "Invalid CAN speed %d", (int)speed);
        return;
    }
    
    g_settings.can_speed = speed;
    settings_save();
    LOG_INFO(SYSTEM, "CAN speed set to %d kbps, reinitializing CAN...", (int)speed);
    
    // Reinitialize CAN with new speed
    comm_can_reinit(g_settings.controller_id, (int)speed);
}

void settings_set_screen_brightness(uint8_t brightness) {
    if (brightness > 100) {
        LOG_WARN(SYSTEM, "Invalid brightness %d (must be 0-100)", brightness);
        return;
    }
    
    g_settings.screen_brightness = brightness;
    settings_save();
    settings_apply_brightness();
    LOG_INFO(SYSTEM, "Screen brightness set to %d%%", brightness);
}

void settings_set_controller_id(uint8_t id) {
    if (id == 0 || id > 254) {
        LOG_WARN(SYSTEM, "Invalid controller ID %d", id);
        return;
    }
    
    g_settings.controller_id = id;
    settings_save();
    LOG_INFO(SYSTEM, "Controller ID set to %d, reinitializing CAN...", id);
    
    // Reinitialize CAN with new controller ID
    comm_can_reinit(id, (int)g_settings.can_speed);
}

void settings_set_battery_capacity(float capacity) {
    if (capacity < 1.0f || capacity > 200.0f) {
        LOG_WARN(SYSTEM, "Invalid battery capacity %.1f (must be 1.0-200.0)", capacity);
        return;
    }
    
    g_settings.battery_capacity = capacity;
    settings_save();
    
    // Notify battery calculation module that capacity changed (will reset on next calculation)
    battery_calc_capacity_changed();
    
    LOG_INFO(SYSTEM, "Battery capacity set to %.1f Ah (battery calc will reset)", capacity);
}

void settings_set_battery_calc_mode(battery_calc_mode_t mode) {
    if (mode != BATTERY_CALC_DIRECT && mode != BATTERY_CALC_SMART) {
        LOG_WARN(SYSTEM, "Invalid battery calc mode %d", mode);
        return;
    }
    
    g_settings.battery_calc_mode = mode;
    settings_save();
    LOG_INFO(SYSTEM, "Battery calculation mode set to %s", 
             mode == BATTERY_CALC_SMART ? "Smart Calculation" : "Direct from Controller");
}

void settings_set_show_fps(bool show) {
    g_settings.show_fps = show;
    settings_save();
    LOG_INFO(SYSTEM, "FPS display set to %s", show ? "shown" : "hidden");
}

void settings_set_wheel_diameter_mm(uint16_t diameter_mm) {
    if (diameter_mm < 50 || diameter_mm > 2000) {
        LOG_WARN(SYSTEM, "Invalid wheel diameter %d mm (must be 50-2000)", diameter_mm);
        return;
    }

    g_settings.wheel_diameter_mm = diameter_mm;
    settings_save();
    LOG_INFO(SYSTEM, "Wheel diameter set to %d mm", diameter_mm);
}

void settings_set_motor_poles(uint8_t poles) {
    if (poles < 2 || poles > 20) {
        LOG_WARN(SYSTEM, "Invalid motor poles %d (must be 2-20)", poles);
        return;
    }

    g_settings.motor_poles = poles;
    settings_save();
    LOG_INFO(SYSTEM, "Motor poles set to %d", poles);
}

// Apply brightness to hardware
void settings_apply_brightness(void) {
    Set_Backlight(g_settings.screen_brightness);
}

// Apply CAN speed - no longer requires restart, happens automatically
bool settings_apply_can_speed(void) {
    // CAN is now automatically reinitialized when speed changes
    // Return false to indicate no system restart is needed
    return false;
}

