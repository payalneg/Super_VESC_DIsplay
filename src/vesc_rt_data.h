/*
	Copyright 2025 Super VESC Display

	VESC Real-Time Data Module
	Handles periodic RT data requests and parsing
*/

#ifndef VESC_RT_DATA_H_
#define VESC_RT_DATA_H_

#include <stdint.h>
#include "datatypes.h"

// Initialize RT data module
void vesc_rt_data_init(void);

// Start/stop periodic RT data requests
void vesc_rt_data_start(void);
void vesc_rt_data_stop(void);

// Request RT data manually
void vesc_rt_data_request(void);

// Process incoming RT data response
void vesc_rt_data_process_response(unsigned char *data, unsigned int len);

// Get last received RT data
const vesc_setup_values_t* vesc_rt_data_get_latest(void);

// Check if data is fresh (received in last 2 seconds)
bool vesc_rt_data_is_fresh(void);

// Get calculated values
float vesc_rt_data_get_speed_kmh(void);      // Speed in km/h
float vesc_rt_data_get_trip_km(void);        // Current trip in km (with persistence)
float vesc_rt_data_get_odometer_km(void);    // Odometer in km
float vesc_rt_data_get_range_km(void);       // Estimated range in km (based on Ah/km)
float vesc_rt_data_get_efficiency_whkm(void); // Wh/km consumption
float vesc_rt_data_get_efficiency_ahkm(void); // Ah/km consumption
float vesc_rt_data_get_amp_hours(void);      // Amp-hours consumed (with persistence)

// Call this periodically from main loop (handles automatic requests)
void vesc_rt_data_loop(void);

// Set RX time
void vesc_rt_data_set_rx_time(void);

#endif /* VESC_RT_DATA_H_ */

