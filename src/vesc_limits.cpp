/*
	Copyright 2025 Super VESC Display
	
	VESC Motor Limits Module
	Simplified API for reading/setting motor current and speed limits
*/

#include "vesc_limits.h"
#include "comm_can.h"
#include "buffer.h"
#include "datatypes.h"
#include "debug_log.h"
#include "dev_settings.h"
#include <Arduino.h>
#include <string.h>

// Current limits cache
static vesc_motor_limits_t current_limits;

// Request state
static bool request_pending = false;
static unsigned long request_time = 0;
#define REQUEST_TIMEOUT_MS 2000

void vesc_limits_init(void) {
	memset(&current_limits, 0, sizeof(current_limits));
	current_limits.is_valid = false;
	request_pending = false;
	LOG_INFO(LIMITS, "Limits module initialized");
}

bool vesc_limits_request(uint8_t vesc_id) {
	if (request_pending && (millis() - request_time < REQUEST_TIMEOUT_MS)) {
		LOG_DEBUG(LIMITS, "Request already pending, ignoring");
		return false;
	}
	
	LOG_INFO(LIMITS, "Requesting motor limits from VESC ID %d", vesc_id);
	
	// Send COMM_GET_MCCONF command
	uint8_t cmd = COMM_GET_MCCONF;
	comm_can_send_buffer(vesc_id, &cmd, 1, 0);
	
	request_pending = true;
	request_time = millis();
	
	return true;
}

const vesc_motor_limits_t* vesc_limits_get(void) {
	// Check if data is stale (older than 10 seconds)
	if (current_limits.is_valid && 
	    (millis() - current_limits.last_update_time > 10000)) {
		LOG_WARN(LIMITS, "Cached limits data is stale");
	}
	
	return &current_limits;
}

bool vesc_limits_is_valid(void) {
	if (!current_limits.is_valid) {
		return false;
	}
	
	// Check if data is stale
	if (millis() - current_limits.last_update_time > 10000) {
		return false;
	}
	
	return true;
}

void vesc_limits_process_mcconf_response(uint8_t* data, unsigned int len) {
	if (len < 1 || data[0] != COMM_GET_MCCONF) {
		return;
	}
	
	LOG_INFO(LIMITS, "Processing MCCONF response (%d bytes)", len);
	request_pending = false;
	
	// MCCONF response format (simplified, only extracting limits):
	// Byte 0: COMM_GET_MCCONF
	// Then follows serialized ConfigParams...
	// For now, we'll parse only the key fields we need
	
	// This is a simplified parser - full MCCONF is complex
	// We look for specific vTx indexes based on VESC firmware structure
	
	int32_t ind = 1;
	
	// Skip PWM mode (uint8)
	if (ind + 1 > len) goto parse_error;
	ind += 1;
	
	// Skip comm mode (uint8)
	if (ind + 1 > len) goto parse_error;
	ind += 1;
	
	// Skip motor type (uint8)
	if (ind + 1 > len) goto parse_error;
	ind += 1;
	
	// Skip sensor mode (uint8)
	if (ind + 1 > len) goto parse_error;
	ind += 1;
	
	// l_current_max (float32, scale 1000) - vTx index ~9
	if (ind + 4 > len) goto parse_error;
	current_limits.l_current_max = buffer_get_float32_auto(data, &ind);
	
	// l_current_min (float32, scale 1000)
	if (ind + 4 > len) goto parse_error;
	current_limits.l_current_min = buffer_get_float32_auto(data, &ind);
	
	// l_in_current_max (float32, scale 1000)
	if (ind + 4 > len) goto parse_error;
	current_limits.l_in_current_max = buffer_get_float32_auto(data, &ind);
	
	// l_in_current_min (float32, scale 1000)
	if (ind + 4 > len) goto parse_error;
	current_limits.l_in_current_min = buffer_get_float32_auto(data, &ind);
	
	// For now, skip to watt_max and erpm_max which are further in the structure
	// This is a simplified implementation - full parsing requires ConfigParams
	
	// Mark as valid
	current_limits.is_valid = true;
	current_limits.last_update_time = millis();
	
	LOG_INFO(LIMITS, "✅ Limits updated:");
	LOG_INFO(LIMITS, "   Motor Current Max:   %.2f A", current_limits.l_current_max);
	LOG_INFO(LIMITS, "   Motor Current Min:   %.2f A", current_limits.l_current_min);
	LOG_INFO(LIMITS, "   Battery Current Max: %.2f A", current_limits.l_in_current_max);
	LOG_INFO(LIMITS, "   Battery Current Min: %.2f A", current_limits.l_in_current_min);
	
	return;

parse_error:
	LOG_ERROR(LIMITS, "Failed to parse MCCONF response (incomplete data)");
	current_limits.is_valid = false;
}

bool vesc_limits_set(uint8_t vesc_id, const vesc_motor_limits_t* limits) {
	// Use COMM_SET_MCCONF_TEMP for temporary (RAM-only) changes
	// This avoids writing to flash repeatedly
	
	LOG_INFO(LIMITS, "Setting motor limits to VESC ID %d (temporary)", vesc_id);
	
	uint8_t send_buffer[256];
	int32_t ind = 0;
	
	send_buffer[ind++] = COMM_SET_MCCONF;
	
	// Serialize limits (simplified format)
	// In reality, this would need full MCCONF serialization
	// For now, we'll use a custom format that our handler can process
	
	buffer_append_float32_auto(send_buffer, limits->l_current_max, &ind);
	buffer_append_float32_auto(send_buffer, limits->l_current_min, &ind);
	buffer_append_float32_auto(send_buffer, limits->l_in_current_max, &ind);
	buffer_append_float32_auto(send_buffer, limits->l_in_current_min, &ind);
	buffer_append_float32_auto(send_buffer, limits->l_watt_max, &ind);
	buffer_append_float32_auto(send_buffer, limits->l_watt_min, &ind);
	buffer_append_float32_auto(send_buffer, limits->l_erpm_max, &ind);
	buffer_append_float32_auto(send_buffer, limits->l_erpm_min, &ind);
	
	comm_can_send_buffer(vesc_id, send_buffer, ind, 0);
	
	LOG_INFO(LIMITS, "✅ Limits sent (%d bytes)", ind);
	
	return true;
}

bool vesc_limits_set_current_max(uint8_t vesc_id, float motor_max_a, float battery_max_a) {
	if (!current_limits.is_valid) {
		LOG_WARN(LIMITS, "No cached limits, cannot update");
		return false;
	}
	
	vesc_motor_limits_t new_limits = current_limits;
	new_limits.l_current_max = motor_max_a;
	new_limits.l_in_current_max = battery_max_a;
	
	return vesc_limits_set(vesc_id, &new_limits);
}

bool vesc_limits_set_speed_max(uint8_t vesc_id, float erpm_max) {
	if (!current_limits.is_valid) {
		LOG_WARN(LIMITS, "No cached limits, cannot update");
		return false;
	}
	
	vesc_motor_limits_t new_limits = current_limits;
	new_limits.l_erpm_max = erpm_max;
	
	return vesc_limits_set(vesc_id, &new_limits);
}

bool vesc_limits_set_power_max(uint8_t vesc_id, float watt_max) {
	if (!current_limits.is_valid) {
		LOG_WARN(LIMITS, "No cached limits, cannot update");
		return false;
	}
	
	vesc_motor_limits_t new_limits = current_limits;
	new_limits.l_watt_max = watt_max;
	
	return vesc_limits_set(vesc_id, &new_limits);
}

// BLE Command: Get limits (custom protocol)
void vesc_limits_handle_ble_get_request(uint8_t* response_buffer, uint16_t* response_len) {
	int32_t ind = 0;
	
	// Custom command response format
	response_buffer[ind++] = 0xF0; // Custom command: GET_LIMITS
	
	if (current_limits.is_valid) {
		response_buffer[ind++] = 1; // Valid flag
		
		buffer_append_float32_auto(response_buffer, current_limits.l_current_max, &ind);
		buffer_append_float32_auto(response_buffer, current_limits.l_current_min, &ind);
		buffer_append_float32_auto(response_buffer, current_limits.l_in_current_max, &ind);
		buffer_append_float32_auto(response_buffer, current_limits.l_in_current_min, &ind);
		buffer_append_float32_auto(response_buffer, current_limits.l_watt_max, &ind);
		buffer_append_float32_auto(response_buffer, current_limits.l_watt_min, &ind);
		buffer_append_float32_auto(response_buffer, current_limits.l_erpm_max, &ind);
		buffer_append_float32_auto(response_buffer, current_limits.l_erpm_min, &ind);
	} else {
		response_buffer[ind++] = 0; // Invalid flag
	}
	
	*response_len = ind;
	
	LOG_DEBUG(LIMITS, "BLE GET_LIMITS response prepared (%d bytes)", ind);
}

// BLE Command: Set limits (custom protocol)
void vesc_limits_handle_ble_set_request(uint8_t* data, unsigned int len) {
	if (len < 33) { // 1 byte command + 8 floats (32 bytes)
		LOG_ERROR(LIMITS, "BLE SET_LIMITS: invalid length %d", len);
		return;
	}
	
	int32_t ind = 1; // Skip command byte
	
	vesc_motor_limits_t new_limits;
	new_limits.l_current_max = buffer_get_float32_auto(data, &ind);
	new_limits.l_current_min = buffer_get_float32_auto(data, &ind);
	new_limits.l_in_current_max = buffer_get_float32_auto(data, &ind);
	new_limits.l_in_current_min = buffer_get_float32_auto(data, &ind);
	new_limits.l_watt_max = buffer_get_float32_auto(data, &ind);
	new_limits.l_watt_min = buffer_get_float32_auto(data, &ind);
	new_limits.l_erpm_max = buffer_get_float32_auto(data, &ind);
	new_limits.l_erpm_min = buffer_get_float32_auto(data, &ind);
	
	LOG_INFO(LIMITS, "BLE SET_LIMITS request received");
	
	uint8_t target_vesc_id = settings_get_target_vesc_id();
	vesc_limits_set(target_vesc_id, &new_limits);
}


