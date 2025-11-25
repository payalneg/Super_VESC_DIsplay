/*
	Copyright 2025 Super VESC Display
	
	Trip, Amp-Hours and Uptime Persistence Module
	Saves trip distance, amp-hours and uptime to NVS every 10 seconds
	and restores values on startup to maintain continuity across power cycles.
	
	Features:
	- Saves trip, amp-hours and uptime every 10 seconds to NVS
	- Detects VESC power cycles (when VESC values reset to zero or decrease)
	- Maintains continuity by adding offset to VESC values
	- Provides reset function for battery swap scenarios
*/

#include "vesc_trip_persist.h"
#include "debug_log.h"
#include <Arduino.h>
#include <Preferences.h>

// NVS namespace for trip persistence
#define TRIP_PERSIST_NAMESPACE "trip_persist"
#define KEY_TRIP_TOTAL         "trip_total"     // Total accumulated trip (meters)
#define KEY_AH_TOTAL           "ah_total"       // Total accumulated amp-hours
#define KEY_UPTIME_TOTAL       "uptime_total"   // Total accumulated uptime (milliseconds)
#define KEY_LAST_VESC_TRIP     "last_vesc_trip" // Last known VESC trip value
#define KEY_LAST_VESC_AH       "last_vesc_ah"   // Last known VESC amp-hours value
#define KEY_LAST_VESC_UPTIME   "last_vesc_uptime" // Last known VESC uptime value

// State variables
static bool initialized = false;
static float trip_offset_meters = 0.0f;  // Offset to add to VESC trip value
static float ah_offset = 0.0f;           // Offset to add to VESC amp-hours value
static uint32_t uptime_offset_ms = 0;    // Offset to add to VESC uptime value
static float current_vesc_trip = 0.0f;   // Current VESC trip value
static float current_vesc_ah = 0.0f;     // Current VESC amp-hours value
static uint32_t current_vesc_uptime = 0; // Current VESC uptime value
static uint32_t last_save_time = 0;      // For periodic NVS saves (10 seconds)
static Preferences preferences;

// Save current state to NVS
static void trip_persist_save_state() {
    if (!preferences.begin(TRIP_PERSIST_NAMESPACE, false)) {
        LOG_ERROR(SYSTEM, "Failed to open trip persist NVS for writing");
        return;
    }
    
    // Save total values (current VESC + offset)
    float trip_total = current_vesc_trip + trip_offset_meters;
    float ah_total = current_vesc_ah + ah_offset;
    uint32_t uptime_total = current_vesc_uptime + uptime_offset_ms;
    
    preferences.putFloat(KEY_TRIP_TOTAL, trip_total);
    preferences.putFloat(KEY_AH_TOTAL, ah_total);
    preferences.putUInt(KEY_UPTIME_TOTAL, uptime_total);
    preferences.putFloat(KEY_LAST_VESC_TRIP, current_vesc_trip);
    preferences.putFloat(KEY_LAST_VESC_AH, current_vesc_ah);
    preferences.putUInt(KEY_LAST_VESC_UPTIME, current_vesc_uptime);
    preferences.end();
    
    LOG_DEBUG(SYSTEM, "Trip state saved: Trip=%.2f m, Ah=%.2f, Uptime=%u ms", 
              trip_total, ah_total, uptime_total);
}

// Load state from NVS
static bool trip_persist_load_state() {
    if (!preferences.begin(TRIP_PERSIST_NAMESPACE, true)) {
        LOG_WARN(SYSTEM, "Failed to open trip persist NVS for reading");
        return false;
    }
    
    float saved_trip_total = preferences.getFloat(KEY_TRIP_TOTAL, -1.0f);
    float saved_ah_total = preferences.getFloat(KEY_AH_TOTAL, -1.0f);
    uint32_t saved_uptime_total = preferences.getUInt(KEY_UPTIME_TOTAL, 0);
    float saved_last_vesc_trip = preferences.getFloat(KEY_LAST_VESC_TRIP, -1.0f);
    float saved_last_vesc_ah = preferences.getFloat(KEY_LAST_VESC_AH, -1.0f);
    uint32_t saved_last_vesc_uptime = preferences.getUInt(KEY_LAST_VESC_UPTIME, 0);
    preferences.end();
    
    // Check if valid data exists
    if (saved_trip_total < 0.0f || saved_ah_total < 0.0f) {
        LOG_INFO(SYSTEM, "No previous trip state found");
        return false;
    }
    
    LOG_INFO(SYSTEM, "Trip state loaded: Trip=%.2f m, Ah=%.2f, Uptime=%u ms", 
             saved_trip_total, saved_ah_total, saved_uptime_total);
    
    // Store loaded values for offset calculation
    // Offsets will be calculated when first VESC values are received
    trip_offset_meters = saved_trip_total;   // Temporary storage
    ah_offset = saved_ah_total;              // Temporary storage
    uptime_offset_ms = saved_uptime_total;   // Temporary storage
    
    return true;
}

void trip_persist_init(void) {
    // Try to load previous state
    if (trip_persist_load_state()) {
        initialized = true;
        LOG_INFO(SYSTEM, "Trip persistence module initialized with saved state");
    } else {
        trip_offset_meters = 0.0f;
        ah_offset = 0.0f;
        uptime_offset_ms = 0;
        initialized = true;
        LOG_INFO(SYSTEM, "Trip persistence module initialized (no saved state)");
    }
    
    current_vesc_trip = 0.0f;
    current_vesc_ah = 0.0f;
    current_vesc_uptime = 0;
    last_save_time = 0;
}

void trip_persist_update(float vesc_trip_meters, float vesc_amp_hours, uint32_t vesc_uptime_ms) {
    if (!initialized) {
        LOG_WARN(SYSTEM, "Trip persist not initialized");
        return;
    }
    
    static bool first_update = true;
    
    // On first update after loading saved state, calculate offsets
    if (first_update) {
        // If we have saved state (stored temporarily in offset variables)
        if (trip_offset_meters > 0.0f || ah_offset > 0.0f || uptime_offset_ms > 0) {
            float saved_trip_total = trip_offset_meters;
            float saved_ah_total = ah_offset;
            uint32_t saved_uptime_total = uptime_offset_ms;
            
            // Calculate offsets: saved_total - current_vesc_value
            // This maintains continuity when VESC resets after power cycle
            trip_offset_meters = saved_trip_total - vesc_trip_meters;
            ah_offset = saved_ah_total - vesc_amp_hours;
            uptime_offset_ms = saved_uptime_total - vesc_uptime_ms;
            
            // Prevent negative offsets (shouldn't happen, but safety check)
            if (trip_offset_meters < 0.0f) trip_offset_meters = 0.0f;
            if (ah_offset < 0.0f) ah_offset = 0.0f;
            if (uptime_offset_ms > 0xFFFFFFFF) uptime_offset_ms = 0; // overflow check
            
            LOG_INFO(SYSTEM, "Trip offsets calculated: Trip=%.2f m, Ah=%.2f, Uptime=%u ms", 
                     trip_offset_meters, ah_offset, uptime_offset_ms);
        } else {
            // No saved state - start from zero
            trip_offset_meters = 0.0f;
            ah_offset = 0.0f;
            uptime_offset_ms = 0;
            LOG_INFO(SYSTEM, "Starting fresh trip tracking");
        }
        first_update = false;
    }
    
    // Check if VESC values decreased (power cycle or reset)
    // This can happen if VESC was powered off and back on
    if (vesc_trip_meters < current_vesc_trip - 1.0f) {  // Allow 1m tolerance for noise
        LOG_INFO(SYSTEM, "VESC trip decreased (%.2f -> %.2f) - adjusting offset", 
                 current_vesc_trip, vesc_trip_meters);
        // Add previous VESC value to offset before it resets
        trip_offset_meters += current_vesc_trip;
    }
    
    if (vesc_amp_hours < current_vesc_ah - 0.01f) {  // Allow 0.01 Ah tolerance
        LOG_INFO(SYSTEM, "VESC amp-hours decreased (%.2f -> %.2f) - adjusting offset", 
                 current_vesc_ah, vesc_amp_hours);
        ah_offset += current_vesc_ah;
    }
    
    if (vesc_uptime_ms < current_vesc_uptime - 1000) {  // Allow 1 second tolerance
        LOG_INFO(SYSTEM, "VESC uptime decreased (%u -> %u) - adjusting offset", 
                 current_vesc_uptime, vesc_uptime_ms);
        uptime_offset_ms += current_vesc_uptime;
    }
    
    // Update current values
    current_vesc_trip = vesc_trip_meters;
    current_vesc_ah = vesc_amp_hours;
    current_vesc_uptime = vesc_uptime_ms;
    
    // Periodically save state to NVS (every 10 seconds to reduce wear)
    uint32_t now = millis();
    if (now - last_save_time >= 10000) {
        trip_persist_save_state();
        last_save_time = now;
    }
    
    // Log values periodically
    static uint32_t last_log_time = 0;
    if (now - last_log_time >= 10000) {
        float total_trip_km = (current_vesc_trip + trip_offset_meters) / 1000.0f;
        float total_ah = current_vesc_ah + ah_offset;
        uint32_t total_uptime = current_vesc_uptime + uptime_offset_ms;
        uint32_t uptime_sec = total_uptime / 1000;
        LOG_INFO(SYSTEM, "Trip persist: Trip=%.2f km, Ah=%.2f, Uptime=%02u:%02u:%02u", 
                 total_trip_km, total_ah, uptime_sec/3600, (uptime_sec%3600)/60, uptime_sec%60);
        last_log_time = now;
    }
}

float trip_persist_get_trip_km(void) {
    if (!initialized) return 0.0f;
    
    // Return total trip: VESC value + offset
    float total_meters = current_vesc_trip + trip_offset_meters;
    return total_meters / 1000.0f; // Convert to km
}

float trip_persist_get_amp_hours(void) {
    if (!initialized) return 0.0f;
    
    // Return total amp-hours: VESC value + offset
    return current_vesc_ah + ah_offset;
}

uint32_t trip_persist_get_uptime_ms(void) {
    if (!initialized) return 0;
    
    // Return total uptime: VESC value + offset
    return current_vesc_uptime + uptime_offset_ms;
}

void trip_persist_reset(void) {
    LOG_INFO(SYSTEM, "Resetting trip, amp-hours and uptime to zero");
    
    // Clear offsets
    trip_offset_meters = 0.0f;
    ah_offset = 0.0f;
    uptime_offset_ms = 0;
    current_vesc_trip = 0.0f;
    current_vesc_ah = 0.0f;
    current_vesc_uptime = 0;
    
    // Clear saved state in NVS
    if (!preferences.begin(TRIP_PERSIST_NAMESPACE, false)) {
        LOG_ERROR(SYSTEM, "Failed to open trip persist NVS for clearing");
        return;
    }
    
    preferences.clear();  // Clear all keys in this namespace
    preferences.end();
    
    LOG_INFO(SYSTEM, "Trip, amp-hours and uptime reset complete");
}

bool trip_persist_is_initialized(void) {
    return initialized;
}

