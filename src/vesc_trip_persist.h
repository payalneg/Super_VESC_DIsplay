/*
	Copyright 2025 Super VESC Display
	
	Trip, Amp-Hours and Uptime Persistence Module
	Saves trip distance, amp-hours and uptime to NVS every 10 seconds
	and restores values on startup to maintain continuity across power cycles
*/

#ifndef VESC_TRIP_PERSIST_H_
#define VESC_TRIP_PERSIST_H_

#include <stdint.h>
#include <stdbool.h>

// Initialize trip persistence module (loads saved state from NVS)
void trip_persist_init(void);

// Update trip, amp-hours and uptime values (call this from main loop)
// This will periodically save to NVS (every 10 seconds)
// Parameters:
//   - vesc_trip_meters: Current trip distance from VESC (meters)
//   - vesc_amp_hours: Current amp-hours from VESC
//   - vesc_uptime_ms: Current uptime from VESC (milliseconds)
void trip_persist_update(float vesc_trip_meters, float vesc_amp_hours, uint32_t vesc_uptime_ms);

// Get persistent trip distance (km) - includes saved offset
float trip_persist_get_trip_km(void);

// Get persistent amp-hours - includes saved offset
float trip_persist_get_amp_hours(void);

// Get persistent uptime (milliseconds) - includes saved offset
uint32_t trip_persist_get_uptime_ms(void);

// Reset trip, amp-hours and uptime to zero (call when battery is swapped)
// This will clear saved state and start fresh
void trip_persist_reset(void);

// Check if trip persistence has been initialized
bool trip_persist_is_initialized(void);

#endif /* VESC_TRIP_PERSIST_H_ */

