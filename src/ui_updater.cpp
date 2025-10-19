/*
	Copyright 2025 Super VESC Display

	UI Updater Module
	Updates LVGL UI with real-time VESC data
*/

#include "ui_updater.h"
#include "vesc_rt_data.h"
#include "vesc_battery_calc.h"
#include "dev_settings.h"
#include "debug_log.h"
#include "ble_vesc_driver.h"
#include <Arduino.h>

// Include LVGL and custom UI functions
extern "C" {
	#include "lvgl.h"
	#include "../../Super_VESC_Display/custom/custom.h"
}

// UI updater state
static bool ui_updater_active = false;
static uint32_t last_update_time = 0;
static uint32_t update_interval_ms = 50; // Update every 50ms (20 Hz)

// FPS counter
static uint32_t fps_counter = 0;           // Count frames
static uint32_t fps_last_time = 0;         // Last FPS calculation time
static int current_fps = 0;                 // Current FPS value

void ui_updater_init(void) {
	ui_updater_active = false;
	last_update_time = 0;
	
	// Initialize battery calculation module
	battery_calc_init();
	
	LOG_INFO(UI, "UI updater initialized (manual mode)");
}

void ui_updater_set_zeros(void) {
	// Initialize all UI elements with zero values
	
	update_speed(0.0f);           // Speed: 0 km/h
	update_current(0.0f);          // Current: 0 A
	update_battery_proc(0.0f);     // Battery: 0%
	update_trip(0.0f);             // Trip: 0 km
	update_range(0.0f);            // Range: 0 km
	update_temp_fet(0.0f);         // Temp FET: 0°C
	update_temp_motor(0.0f);       // Temp Motor: 0°C
	update_amp_hours(0.0f);        // Amp hours: 0.0 Ah
	update_battery_temp(0.0f);     // Battery temp: 0°C
	update_battery_voltage(0.0f);       // Battery Voltage: 0 V
	update_odometer(0.0f);       // Odometer: 0 km
	update_fps(0);                 // FPS: 0

	LOG_INFO(UI, "UI initialized with zero values");
}

void ui_updater_start(void) {
	ui_updater_active = true;
	last_update_time = 0; // Force immediate update
	LOG_INFO(UI, "UI updates started");
}

void ui_updater_stop(void) {
	ui_updater_active = false;
	LOG_INFO(UI, "UI updates stopped");
}

void ui_updater_update(void) {
	if (!ui_updater_active) return;
	
	// Check if enough time has passed (50ms interval)
	uint32_t now = millis();
	if (now - last_update_time < update_interval_ms) {
		return; // Too early, skip update
	}
	last_update_time = now;
	
	// Get latest RT data
	const vesc_setup_values_t* rt = vesc_rt_data_get_latest();
	
	// Update UI with RT data
	
	// Speed (km/h)
	float speed_kmh = vesc_rt_data_get_speed_kmh();
	update_speed(speed_kmh);
	
	// Current (A) - positive = discharge, negative = regen
	update_current(rt->current_in);
	
	// Battery level (0-100%) - use smart calculation or direct from controller
	float battery_percent;
	battery_calc_mode_t calc_mode = settings_get_battery_calc_mode();
	
	if (calc_mode == BATTERY_CALC_SMART) {
		// Smart calculation based on capacity and amp-hours
		float battery_capacity = settings_get_battery_capacity();
		battery_percent = battery_calc_get_smart_percentage(rt->battery_level, rt->amp_hours, battery_capacity);
	} else {
		// Direct from controller
		battery_percent = rt->battery_level * 100.0f;
	}
	
	update_battery_proc(battery_percent);
	
	// Trip distance (km)
	float trip_km = vesc_rt_data_get_trip_km();
	update_trip(trip_km);
	
	// Range (km)
	float range_km = vesc_rt_data_get_range_km();
	update_range(range_km);
	
	// Temperature MOSFET (°C)
	update_temp_fet(rt->temp_mos);
	
	// Temperature Motor (°C)
	update_temp_motor(rt->temp_motor);
	
	// Amp hours consumed (Ah)
	update_amp_hours(rt->amp_hours);
	
	// Battery temperature (if available) - for now use MOSFET temp
	update_battery_temp(rt->temp_mos);
	
	// Battery voltage (V)
	update_battery_voltage(rt->v_in);
	
	// Odometer (km)
	update_odometer(rt->odometer/1000.0f);
	
	// Uptime (ms)
	update_uptime(rt->uptime_ms);
	
	// BLE connection status
	bool ble_connected = BLE_IsConnected();
	update_ble_status(ble_connected);
	
	// ESC connection status (check if RT data is fresh)
	if (!BLE_IsSubscribed()) {
		bool esc_connected = vesc_rt_data_is_fresh();

		update_esc_connection_status(esc_connected);
	}
	// Debug log every 1 second (20 updates = 1 second at 50ms interval)
	static uint32_t update_counter = 0;
	update_counter++;
	if (update_counter >= 20) {
		update_counter = 0;
		LOG_DEBUG(UI, "UI: Speed=%.1f km/h | I=%.1fA | Bat=%.0f%% | V=%.1fV | Trip=%.2f km | Range=%.1f km | Odo=%.1f km | Temp:FET=%.0f°C/Mot=%.0f°C | Ah=%.1f", 
		         speed_kmh, rt->current_in, battery_percent, rt->v_in, trip_km, range_km, 
		         rt->odometer/1000.0f, rt->temp_mos, rt->temp_motor, rt->amp_hours);
	}
}

void ui_updater_update_fps(void) {
	// FPS counter - count every frame
	fps_counter++;
	
	// Calculate and update FPS every second
	uint32_t now = millis();
	if (now - fps_last_time >= 1000) {
		current_fps = fps_counter;
		update_fps(current_fps);
		
		LOG_DEBUG(UI, "FPS: %d", current_fps);
		
		fps_counter = 0;
		fps_last_time = now;
	}
}

// Media control UI update functions
void update_current_song_title(const char* title) {
	// Update current song title
	// This might need a new UI element
	LOG_DEBUG(UI, "Song title: %s", title ? title : "Unknown");
}

void update_current_song_artist(const char* artist) {
	// Update current song artist
	// This might need a new UI element
	LOG_DEBUG(UI, "Song artist: %s", artist ? artist : "Unknown");
}

void update_current_song_album(const char* album) {
	// Update current song album
	// This might need a new UI element
	LOG_DEBUG(UI, "Song album: %s", album ? album : "Unknown");
}

void update_playback_progress(uint32_t position_ms, uint32_t duration_ms) {
	// Update playback progress bar
	// This might need a new UI element
	if (duration_ms > 0) {
		float progress = (float)position_ms / (float)duration_ms * 100.0f;
		LOG_DEBUG(UI, "Playback progress: %.1f%%", progress);
	}
}

void update_playback_state(bool is_playing) {
	// Update playback state (play/pause indicator)
	// This might need a new UI element
	LOG_DEBUG(UI, "Playback state: %s", is_playing ? "Playing" : "Paused");
}

void update_volume_level(uint8_t volume) {
	// Update volume level
	// This might need a new UI element
	LOG_DEBUG(UI, "Volume: %d%%", volume);
}