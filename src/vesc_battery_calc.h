/*
	Copyright 2025 Super VESC Display
	
	Smart Battery Calculation Module
	Calculates battery percentage based on capacity and amp-hours usage
	with persistent state storage and automatic charging detection
*/

#ifndef VESC_BATTERY_CALC_H_
#define VESC_BATTERY_CALC_H_

#include <stdint.h>
#include <stdbool.h>

// Initialize battery calculation module (loads saved state from NVS)
void battery_calc_init(void);

// Reset battery calculation to full capacity based on current controller reading
// Call when:
// - Battery capacity changed
// - Manual reset requested
// - Large difference detected between saved and current battery level
void battery_calc_reset(float current_battery_percent, float battery_capacity);

// Calculate battery percentage with persistent state
// Parameters:
//   - controller_battery_level: Battery level from controller (0.0-1.0)
//   - controller_amp_hours: Amp-hours consumed from controller
//   - battery_capacity: Total battery capacity in Ah
// Returns: Calculated battery percentage (0.0-100.0)
// Note: 
//   - Reads controller percentage once at initialization
//   - Continuously monitors controller percent for battery swap detection
//   - Automatically resets remaining capacity when controller percent increases >1%
//   - Saves state to NVS periodically
float battery_calc_get_smart_percentage(float controller_battery_level, 
                                         float controller_amp_hours,
                                         float battery_capacity);

// Check if smart calculation has been initialized
bool battery_calc_is_initialized(void);

// Notify that battery capacity has changed (triggers reset on next calculation)
void battery_calc_capacity_changed(void);

// Reset trip, amp-hours and uptime when battery is swapped
// This function resets trip/amp-hours/uptime persistence
// Call when user confirms battery swap
void battery_calc_reset_trip_and_ah(void);

// Get remaining battery capacity in Ah
// Returns: Remaining capacity in Ah (used for range calculation)
float battery_calc_get_remaining_ah(void);

#endif /* VESC_BATTERY_CALC_H_ */

