/*
	Copyright 2025 Super VESC Display
	
	Smart Battery Calculation Module
	Calculates battery percentage based on capacity and amp-hours usage
	with persistent state storage and automatic battery swap/charging detection.
	
	Features:
	- Reads controller percentage once at initialization
	- Continuously monitors controller percent for increases (battery swap detection)
	- Automatically resets remaining capacity when new battery is detected (>1% increase)
	- Persistent state storage in NVS
	- Precise tracking based on actual amp-hours consumption
*/

#include "vesc_battery_calc.h"
#include "debug_log.h"
#include <Arduino.h>
#include <Preferences.h>
#include "vesc_trip_persist.h"

// NVS namespace for battery calculation
#define BATTERY_CALC_NAMESPACE "battery_calc"
#define KEY_REMAINING_AH       "remaining_ah"
#define KEY_LAST_PERCENT       "last_percent"
#define KEY_LAST_CAPACITY      "last_capacity"

// Threshold for detecting charging (percent difference)
#define CHARGING_DETECT_THRESHOLD 5.0f  // If percent increased by >10%, assume charging happened

// Battery calculation state
static bool initialized = false;
static float remaining_battery_ah = 0.0f;    // Current remaining capacity in Ah (persistent)
static float last_saved_percent = 0.0f;      // Last saved battery percent for charging detection
static float last_saved_capacity = 0.0f;     // Last saved battery capacity
static float last_amp_hours = 0.0f;          // Last amp-hours reading from controller
static float last_controller_percent = 0.0f; // Last controller percent for continuous monitoring
static bool first_calculation = true;
static bool capacity_changed_flag = false;   // Flag to reset on capacity change
static uint32_t last_save_time = 0;          // For periodic NVS saves
static Preferences preferences;

// Save current state to NVS
static void battery_calc_save_state() {
    if (!preferences.begin(BATTERY_CALC_NAMESPACE, false)) {
        LOG_ERROR(SYSTEM, "Failed to open battery calc NVS for writing");
        return;
    }
    
    preferences.putFloat(KEY_REMAINING_AH, remaining_battery_ah);
    preferences.putFloat(KEY_LAST_CAPACITY, last_saved_capacity);
    preferences.end();
    
    LOG_DEBUG(SYSTEM, "Battery state saved: %.2f Ah, %.1f%%", remaining_battery_ah);
}

// Load state from NVS
static bool battery_calc_load_state() {
    if (!preferences.begin(BATTERY_CALC_NAMESPACE, true)) {
        LOG_WARN(SYSTEM, "Failed to open battery calc NVS for reading");
        return false;
    }
    
    remaining_battery_ah= preferences.getFloat(KEY_REMAINING_AH, -1.0f);
    last_saved_capacity = preferences.getFloat(KEY_LAST_CAPACITY, -1.0f);
    preferences.end();
    
    // Check if valid data exists
    if (remaining_battery_ah < 0.0f) {
        LOG_INFO(SYSTEM, "No previous battery state found");
        return false;
    }
    
    LOG_INFO(SYSTEM, "Battery state loaded: %.2f Ah, capacity: %.1f Ah", 
             remaining_battery_ah, last_saved_capacity);
    return true;
}

// Save current state to NVS
static void battery_calc_save_percent() {
    if (!preferences.begin(BATTERY_CALC_NAMESPACE, false)) {
        LOG_ERROR(SYSTEM, "Failed to open battery calc NVS for writing");
        return;
    }
    
    preferences.putFloat(KEY_LAST_PERCENT, last_saved_percent);
    preferences.end();
    
    LOG_DEBUG(SYSTEM, "Battery state saved: %.1f%%", last_saved_percent);
}

// Load state from NVS
static bool battery_calc_load_percent() {
    if (!preferences.begin(BATTERY_CALC_NAMESPACE, true)) {
        LOG_WARN(SYSTEM, "Failed to open battery calc NVS for reading");
        return false;
    }
    
    last_saved_percent = preferences.getFloat(KEY_LAST_PERCENT, -1.0f);
    preferences.end();
    
    // Check if valid data exists
    if (last_saved_percent < 0.0f) {
        LOG_INFO(SYSTEM, "No previous battery state found");
        return false;
    }
    
    LOG_INFO(SYSTEM, "Battery state loaded: %.1f%%", last_saved_percent);
    return true;
}

void battery_calc_init(void) {
    // Try to load previous state
    float loaded_remaining_ah = 0.0f;
    float loaded_last_percent = 0.0f;
    float loaded_last_capacity = 0.0f;
    
    if (battery_calc_load_state()) {
        initialized = true;
        LOG_INFO(SYSTEM, "Battery calculation module initialized with saved state");
    } else {
        remaining_battery_ah = 0.0f;
        last_saved_percent = 0.0f;
        last_saved_capacity = 0.0f;
        initialized = false;
        LOG_INFO(SYSTEM, "Battery calculation module initialized (no saved state)");
    }
    
    last_amp_hours = 0.0f;
    first_calculation = true;
    capacity_changed_flag = false;
    last_save_time = 0;
}

void battery_calc_reset(float current_battery_percent, float battery_capacity) {
    // Reset to current reading from controller
    //remaining_battery_ah = (current_battery_percent / 100.0f) * battery_capacity;
    remaining_battery_ah = battery_capacity;
    last_saved_percent = current_battery_percent;
    last_saved_capacity = battery_capacity;
    last_controller_percent = current_battery_percent;
    last_amp_hours = 0.0f;
    first_calculation = true;
    capacity_changed_flag = false;
    initialized = true;
    
    // Save to NVS immediately
    battery_calc_save_state();
    
    LOG_INFO(SYSTEM, "Battery calculation reset: %.1f%% = %.2f Ah of %.1f Ah capacity", 
             current_battery_percent, remaining_battery_ah, battery_capacity);
}

float battery_calc_get_smart_percentage(float controller_battery_level, 
                                         float controller_amp_hours,
                                         float battery_capacity) {
    // Validate inputs
    if (battery_capacity <= 0.0f) {
        LOG_WARN(SYSTEM, "Invalid battery capacity: %.1f", battery_capacity);
        return controller_battery_level * 100.0f; // Fallback to direct
    }
    
    float current_controller_percent = controller_battery_level * 100.0f;
    
    // Check if capacity changed - reset if so
    if (capacity_changed_flag || (initialized && last_saved_capacity > 0.0f && 
        abs(last_saved_capacity - battery_capacity) > 0.1f)) {
        LOG_INFO(SYSTEM, "Battery capacity changed from %.1f to %.1f Ah - resetting", 
                 last_saved_capacity, battery_capacity);
        battery_calc_reset(current_controller_percent, battery_capacity);
        capacity_changed_flag = false;
        return current_controller_percent;
    }
    
    // First-time initialization
    if (!initialized || first_calculation) {
        if (initialized) {
            battery_calc_load_percent();
            // We have saved state - check if battery was charged
            float percent_diff = current_controller_percent - last_saved_percent;
            last_saved_percent = current_controller_percent;
            battery_calc_save_percent();
            if (percent_diff > CHARGING_DETECT_THRESHOLD) {
                // Battery was charged! Reset to controller reading
                LOG_INFO(SYSTEM, "Charging detected on startup! Saved: %.1f%%, Current: %.1f%% (diff: +%.1f%%) - resetting", 
                         last_saved_percent, current_controller_percent, percent_diff);
                battery_calc_reset(current_controller_percent, battery_capacity);
                battery_calc_reset_trip_and_ah();
                return current_controller_percent;
            } else {
                // Continue from saved state
                LOG_INFO(SYSTEM, "Continuing from saved state: %.2f Ah (saved: %.1f%%, current: %.1f%%)", 
                         remaining_battery_ah, last_saved_percent, current_controller_percent);
                last_controller_percent = current_controller_percent;
                first_calculation = false;
            }
        
        } else {
            // No saved state - initialize from controller reading
            battery_calc_reset(current_controller_percent, battery_capacity);
            return current_controller_percent;
        }
    }
    
    /*
    // Continuous monitoring: check if controller percent increased (battery swap detection)
    float controller_percent_diff = current_controller_percent - last_controller_percent;
    if (controller_percent_diff > CHARGING_DETECT_THRESHOLD) {
        // Controller percent increased - battery was swapped or charged
        LOG_INFO(SYSTEM, "Battery swap/charging detected! Previous controller: %.1f%%, Current: %.1f%% (diff: +%.1f%%) - resetting to full capacity", 
                 last_controller_percent, current_controller_percent, controller_percent_diff);
        battery_calc_reset(current_controller_percent, battery_capacity);
        return current_controller_percent;
    }
    */
   
    // Update last controller percent for next comparison
    last_controller_percent = current_controller_percent;
    
    // Calculate amp-hours consumed since last reading
    float amp_hours_consumed = controller_amp_hours - last_amp_hours;
    last_amp_hours = controller_amp_hours;
    
    // Update remaining capacity
    remaining_battery_ah -= amp_hours_consumed;
    
    // Prevent negative values
    if (remaining_battery_ah < 0.0f) {
        remaining_battery_ah = 0.0f;
    }
    
    // Prevent going over capacity (in case of regenerative braking)
    if (remaining_battery_ah > battery_capacity) {
        remaining_battery_ah = battery_capacity;
    }
    
    // Calculate percentage
    float battery_percent = (remaining_battery_ah / battery_capacity) * 100.0f;
    
    // Clamp to 0-100%
    if (battery_percent < 0.0f) battery_percent = 0.0f;
    if (battery_percent > 100.0f) battery_percent = 100.0f;
    
    // Periodically save state to NVS (every 30 seconds to reduce wear)
    uint32_t now = millis();
    if (now - last_save_time >= 30000) {
        battery_calc_save_state();
        last_save_time = now;
    }
    
    // Log calculation periodically
    static uint32_t last_log_time = 0;
    if (now - last_log_time >= 5000) {
        LOG_INFO(SYSTEM, "Smart battery: Remaining=%.2f Ah, Consumed=%.2f Ah, Percent=%.1f%% (Controller: %.1f%%)", 
                 remaining_battery_ah, controller_amp_hours, battery_percent, current_controller_percent);
        last_log_time = now;
    }
    
    return battery_percent;
}

bool battery_calc_is_initialized(void) {
    return initialized;
}

void battery_calc_capacity_changed(void) {
    capacity_changed_flag = true;
    LOG_INFO(SYSTEM, "Battery capacity change flagged - will reset on next calculation");
}

void battery_calc_reset_trip_and_ah(void) {
    LOG_INFO(SYSTEM, "Resetting trip, amp-hours and uptime due to battery swap");
    
    // Reset trip persistence (trip distance, amp-hours and uptime)
    trip_persist_reset();
    
    LOG_INFO(SYSTEM, "Trip, amp-hours and uptime reset complete");
}

float battery_calc_get_remaining_ah(void) {
    return remaining_battery_ah;
}
