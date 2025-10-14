/*
	Copyright 2025 Super VESC Display
	
	Global settings and configuration with NVS storage
*/

#include "settings.h"
#include "Display_ST7701.h"
#include "comm_can.h"
#include "debug_log.h"
#include <Preferences.h>

// NVS namespace for settings
#define SETTINGS_NAMESPACE "vesc_settings"

// NVS keys
#define KEY_TARGET_VESC_ID    "target_id"
#define KEY_CAN_SPEED         "can_speed"
#define KEY_BRIGHTNESS        "brightness"
#define KEY_CONTROLLER_ID     "controller_id"

// Default settings
#define DEFAULT_TARGET_VESC_ID      10
#define DEFAULT_CAN_SPEED           CAN_SPEED_1000_KBPS
#define DEFAULT_BRIGHTNESS          80
#define DEFAULT_CONTROLLER_ID       2

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
    
    // Load from NVS
    settings_load();
    
    settings_initialized = true;
    LOG_INFO(SYSTEM, "Settings initialized: Target ID=%d, CAN Speed=%d kbps, Brightness=%d%%, Controller ID=%d", 
             g_settings.target_vesc_id, (int)g_settings.can_speed, g_settings.screen_brightness, g_settings.controller_id);
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
    
    // Validate CAN speed
    if (g_settings.can_speed != CAN_SPEED_125_KBPS && 
        g_settings.can_speed != CAN_SPEED_250_KBPS && 
        g_settings.can_speed != CAN_SPEED_500_KBPS && 
        g_settings.can_speed != CAN_SPEED_1000_KBPS) {
        LOG_WARN(SYSTEM, "Invalid CAN speed %d, using default %d", (int)g_settings.can_speed, DEFAULT_CAN_SPEED);
        g_settings.can_speed = DEFAULT_CAN_SPEED;
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
    LOG_INFO(SYSTEM, "CAN speed set to %d kbps (will take effect after restart)", (int)speed);
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
    LOG_INFO(SYSTEM, "Controller ID set to %d (will take effect after restart)", id);
}

// Apply brightness to hardware
void settings_apply_brightness(void) {
    Set_Backlight(g_settings.screen_brightness);
}

// Apply CAN speed - requires restart
bool settings_apply_can_speed(void) {
    // Note: This requires restarting CAN bus
    // Return true to indicate restart is needed
    LOG_INFO(SYSTEM, "CAN speed change requires system restart");
    return true;
}

