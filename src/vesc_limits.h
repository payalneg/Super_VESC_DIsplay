/*
	Copyright 2025 Super VESC Display
	
	VESC Motor Limits Module
	Simplified API for reading/setting motor current and speed limits
*/

#ifndef VESC_LIMITS_H_
#define VESC_LIMITS_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Motor configuration limits structure
typedef struct {
	float l_current_max;        // Motor Current Max (A)
	float l_current_min;        // Motor Current Max Brake (A, negative)
	float l_in_current_max;     // Battery Current Max (A)
	float l_in_current_min;     // Battery Current Max Regen (A, negative)
	float l_watt_max;           // Watt Max (W)
	float l_watt_min;           // Watt Max Brake (W, negative)
	float l_erpm_max;           // ERPM Max (electrical RPM)
	float l_erpm_min;           // ERPM Min (electrical RPM, negative)
	uint32_t last_update_time;  // Last update timestamp (ms)
	bool is_valid;              // Data validity flag
} vesc_motor_limits_t;

// Initialize the limits module
void vesc_limits_init(void);

// Request current limits from VESC (async)
// Response will be available via vesc_limits_get()
bool vesc_limits_request(uint8_t vesc_id);

// Get current cached limits
const vesc_motor_limits_t* vesc_limits_get(void);

// Check if limits data is valid and fresh
bool vesc_limits_is_valid(void);

// Set new limits (sends to VESC immediately)
bool vesc_limits_set(uint8_t vesc_id, const vesc_motor_limits_t* limits);

// Set individual limit values (convenience functions)
bool vesc_limits_set_current_max(uint8_t vesc_id, float motor_max_a, float battery_max_a);
bool vesc_limits_set_speed_max(uint8_t vesc_id, float erpm_max);
bool vesc_limits_set_power_max(uint8_t vesc_id, float watt_max);

// Process received MCCONF response (called internally by packet handler)
void vesc_limits_process_mcconf_response(uint8_t* data, unsigned int len);

// BLE Command handlers
void vesc_limits_handle_ble_get_request(uint8_t* response_buffer, uint16_t* response_len);
void vesc_limits_handle_ble_set_request(uint8_t* data, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* VESC_LIMITS_H_ */


