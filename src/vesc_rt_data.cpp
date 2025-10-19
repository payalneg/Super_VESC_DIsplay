/*
	Copyright 2025 Super VESC Display

	VESC Real-Time Data Module
	Handles periodic RT data requests and parsing
*/

#include "vesc_rt_data.h"
#include "comm_can.h"
#include "buffer.h"
#include "debug_log.h"
#include <Arduino.h>
#include <string.h>
#include "dev_settings.h"
#include "ble_vesc_driver.h"

// Range calculation mode:
// 0 = VESC Tool original (uses full battery_wh capacity - shows range for full battery)
// 1 = Realistic mode (uses remaining energy: battery_wh * battery_level - shows actual range)
#define USE_REALISTIC_RANGE_CALCULATION 1

// Latest RT data
static vesc_setup_values_t rt_data;
static bool data_received = false;

// Timer for RT data requests
static bool rt_data_active = false;
static uint32_t last_request_time = 0;
static uint32_t request_interval_ms = 100; // Request all data every 50ms (20 Hz)

// target_vesc_id is defined in settings.cpp and declared in settings.h

// SETUP_VALUES field bit positions (for selective request mask)
#define MASK_TEMP_MOS           (1 << 0)   // Temperature MOSFET
#define MASK_TEMP_MOTOR         (1 << 1)   // Temperature Motor
#define MASK_CURRENT_MOTOR      (1 << 2)   // Motor current
#define MASK_CURRENT_IN         (1 << 3)   // Battery current (input)
#define MASK_DUTY_NOW           (1 << 4)   // Duty cycle
#define MASK_RPM                (1 << 5)   // Motor RPM
#define MASK_SPEED              (1 << 6)   // Speed (m/s)
#define MASK_V_IN               (1 << 7)   // Battery voltage
#define MASK_BATTERY_LEVEL      (1 << 8)   // Battery level (0-1)
#define MASK_AMP_HOURS          (1 << 9)   // Amp hours consumed
#define MASK_AMP_HOURS_CHARGED  (1 << 10)  // Amp hours charged
#define MASK_WATT_HOURS         (1 << 11)  // Watt hours consumed
#define MASK_WATT_HOURS_CHARGED (1 << 12)  // Watt hours charged
#define MASK_TACHOMETER         (1 << 13)  // Tachometer (signed)
#define MASK_TACHOMETER_ABS     (1 << 14)  // Tachometer absolute (trip distance)
#define MASK_POSITION           (1 << 15)  // Position
#define MASK_FAULT_CODE         (1 << 16)  // Fault code
#define MASK_VESC_ID            (1 << 17)  // VESC ID
#define MASK_NUM_VESCS          (1 << 18)  // Number of VESCs
#define MASK_BATTERY_WH         (1 << 19)  // Battery capacity (Wh)
#define MASK_ODOMETER           (1 << 20)  // Odometer (meters)
#define MASK_UPTIME_MS          (1 << 21)  // Uptime (milliseconds)

void vesc_rt_data_init(void) {
	memset(&rt_data, 0, sizeof(rt_data));
	data_received = false;
	rt_data_active = false;
	LOG_INFO(VESC, "RT Data module initialized");
}

void vesc_rt_data_start(void) {
	rt_data_active = true;
	last_request_time = 0; // Force immediate request
	LOG_INFO(VESC, "RT Data requests started (interval: %d ms)", request_interval_ms);
}

void vesc_rt_data_stop(void) {
	rt_data_active = false;
	LOG_INFO(VESC, "RT Data requests stopped");
}

void vesc_rt_data_request(void) {
	// Request RT data (50ms interval)
	uint32_t mask = 
		MASK_AMP_HOURS |
		MASK_TEMP_MOS |         // Temperature MOSFET
		MASK_TEMP_MOTOR |       // Temperature Moto
		MASK_CURRENT_IN |       // Battery current (A)
		MASK_BATTERY_LEVEL |    // Battery level (0-1)
		MASK_SPEED |            // Speed (m/s → km/h)
		MASK_V_IN |             // Battery voltage (V)
		MASK_WATT_HOURS |     // Wh consumed (for range calculation)
		MASK_WATT_HOURS_CHARGED | // Wh charged (for range calculation)
		MASK_TACHOMETER_ABS |   // Trip distance (meters)
		MASK_BATTERY_WH |     // Battery capacity (Wh, for range)
		MASK_ODOMETER |         // Total distance (meters)
		MASK_UPTIME_MS;         // Uptime (ms)
	
	uint8_t send_buffer[10];
	int32_t ind = 0;
	
	send_buffer[ind++] = COMM_GET_VALUES_SETUP_SELECTIVE;
	buffer_append_uint32(send_buffer, mask, &ind);
	
	// Send to VESC via CAN
	// send_type = 0: wait for response
	comm_can_send_buffer(settings_get_target_vesc_id(), send_buffer, ind, 0);
}

void vesc_rt_data_set_rx_time(void)
{
	rt_data.rx_time = millis();
	data_received = true;
}

void vesc_rt_data_process_response(unsigned char *data, unsigned int len) {
	if (len < 1) return;
	
	uint8_t cmd = data[0];
	
	// Check if this is COMM_GET_VALUES_SETUP or COMM_GET_VALUES_SETUP_SELECTIVE response
	if (cmd != COMM_GET_VALUES_SETUP && cmd != COMM_GET_VALUES_SETUP_SELECTIVE) {
		return;
	}
	
	LOG_DEBUG(VESC, "📥 RT data response received (%d bytes)", len);
	
	int32_t ind = 1; // Skip command byte
	
	uint32_t mask = 0xFFFFFFFF;
	if (cmd == COMM_GET_VALUES_SETUP_SELECTIVE) {
		if (len < 5) {
			LOG_ERROR(VESC, "RT data response too short for selective mode");
			return;
		}
		mask = buffer_get_uint32(data, &ind);
		LOG_DEBUG(VESC, "   Response mask: 0x%08X", mask);
	}
	
	// Parse fields according to mask (using named constants)
	
	if (mask & MASK_TEMP_MOS) {
		if (ind + 2 <= len) rt_data.temp_mos = buffer_get_float16(data, 1e1, &ind);
	}
	if (mask & MASK_TEMP_MOTOR) {
		if (ind + 2 <= len) rt_data.temp_motor = buffer_get_float16(data, 1e1, &ind);
	}
	if (mask & MASK_CURRENT_MOTOR) {
		if (ind + 4 <= len) rt_data.current_motor = buffer_get_float32(data, 1e2, &ind);
	}
	if (mask & MASK_CURRENT_IN) {
		if (ind + 4 <= len) rt_data.current_in = buffer_get_float32(data, 1e2, &ind);
	}
	if (mask & MASK_DUTY_NOW) {
		if (ind + 2 <= len) rt_data.duty_now = buffer_get_float16(data, 1e3, &ind);
	}
	if (mask & MASK_RPM) {
		if (ind + 4 <= len) rt_data.rpm = buffer_get_float32(data, 1e0, &ind);
	}
	if (mask & MASK_SPEED) {
		if (ind + 4 <= len) rt_data.speed = buffer_get_float32(data, 1e3, &ind);
	}
	if (mask & MASK_V_IN) {
		if (ind + 2 <= len) rt_data.v_in = buffer_get_float16(data, 1e1, &ind);
	}
	if (mask & MASK_BATTERY_LEVEL) {
		if (ind + 2 <= len) rt_data.battery_level = buffer_get_float16(data, 1e3, &ind);
	}
	if (mask & MASK_AMP_HOURS) {
		if (ind + 4 <= len) rt_data.amp_hours = buffer_get_float32(data, 1e4, &ind);
	}
	if (mask & MASK_AMP_HOURS_CHARGED) {
		if (ind + 4 <= len) rt_data.amp_hours_charged = buffer_get_float32(data, 1e4, &ind);
	}
	if (mask & MASK_WATT_HOURS) {
		if (ind + 4 <= len) rt_data.watt_hours = buffer_get_float32(data, 1e4, &ind);
	}
	if (mask & MASK_WATT_HOURS_CHARGED) {
		if (ind + 4 <= len) rt_data.watt_hours_charged = buffer_get_float32(data, 1e4, &ind);
	}
	if (mask & MASK_TACHOMETER) {
		if (ind + 4 <= len) rt_data.tachometer = buffer_get_float32(data, 1e3, &ind);
	}
	if (mask & MASK_TACHOMETER_ABS) {
		if (ind + 4 <= len) rt_data.tachometer_abs = buffer_get_float32(data, 1e3, &ind);
	}
	if (mask & MASK_POSITION) {
		if (ind + 4 <= len) rt_data.position = buffer_get_float32(data, 1e6, &ind);
	}
	if (mask & MASK_FAULT_CODE) {
		if (ind + 1 <= len) rt_data.fault_code = data[ind++];
	}
	if (mask & MASK_VESC_ID) {
		if (ind + 1 <= len) rt_data.vesc_id = data[ind++];
	}
	if (mask & MASK_NUM_VESCS) {
		if (ind + 1 <= len) rt_data.num_vescs = data[ind++];
	}
	if (mask & MASK_BATTERY_WH) {
		if (ind + 4 <= len) rt_data.battery_wh = buffer_get_float32(data, 1e3, &ind);
	}
	if (mask & MASK_ODOMETER) {
		if (ind + 4 <= len) rt_data.odometer = buffer_get_uint32(data, &ind);
	}
	if (mask & MASK_UPTIME_MS) {
		if (ind + 4 <= len) rt_data.uptime_ms = buffer_get_uint32(data, &ind);
	}
	
	// Update timestamp
	vesc_rt_data_set_rx_time();
	
	
	// Log parsed data
	static uint32_t log_counter = 0;
	log_counter++;
	
	// Log combined data every 1 second (assuming ~20 fast + ~1 slow = ~21 updates/sec, so log every 20)
	if (log_counter >= 20) {
		log_counter = 0;
		float speed_kmh = rt_data.speed * 3.6f;
		float trip_km = rt_data.tachometer_abs / 1000.0f;
		float odometer_km = rt_data.odometer / 1000.0f;
		float range_km = vesc_rt_data_get_range_km();
		float power_w = rt_data.current_in * rt_data.v_in;
		
		LOG_INFO(VESC, "RT: Speed=%.1f km/h | I=%.1fA | V=%.1fV | P=%.0fW | Trip=%.2f km | Range=%.1f km | Odo=%.1f km", 
		         speed_kmh, rt_data.current_in, rt_data.v_in, power_w, trip_km, range_km, odometer_km);
	}
}

const vesc_setup_values_t* vesc_rt_data_get_latest(void) {
	return &rt_data;
}

bool vesc_rt_data_is_fresh(void) {
	if (!data_received) return false;
	
	uint32_t age_ms = millis() - rt_data.rx_time;
	return age_ms < 5000; // Fresh if received within 2 seconds
}

float vesc_rt_data_get_speed_kmh(void) {
	// Calculate speed based on motor RPM, wheel diameter, and motor poles
	// This gives more accurate speed calculation accounting for wheel size

	// Get settings
	uint16_t wheel_diameter_mm = settings_get_wheel_diameter_mm();
	uint8_t motor_poles = settings_get_motor_poles();

	// Avoid division by zero
	if (motor_poles == 0 || wheel_diameter_mm == 0) {
		return rt_data.speed * 3.6f; // Fallback to VESC speed
	}

	// Convert ERPM to RPM
	float rpm = rt_data.rpm;

	// Convert wheel diameter from mm to meters
	float wheel_radius_m = (float)wheel_diameter_mm / 2000.0f; // mm to meters (diameter -> radius)

	// Calculate angular velocity (rad/s)
	float omega_rad_s = rpm * 2.0f * 3.14159f / 60.0f;

	// Calculate linear velocity (m/s)
	float velocity_ms = omega_rad_s * wheel_radius_m;

	// Convert to km/h
	float velocity_kmh = velocity_ms * 3.6f;

	return velocity_kmh;
}

float vesc_rt_data_get_trip_km(void) {
	return rt_data.tachometer_abs / 1000.0f; // meters to km
}

float vesc_rt_data_get_odometer_km(void) {
	return rt_data.odometer / 1000.0f; // meters to km
}

float vesc_rt_data_get_range_km(void) {
	// Calculate range based on current consumption
	float wh_consumed = rt_data.watt_hours - rt_data.watt_hours_charged;
	float distance_km = rt_data.tachometer_abs / 1000.0f;
	
	if (distance_km < 0.01f) {
		return 0.0f; // Not enough data
	}
	
	float wh_per_km = wh_consumed / distance_km;
	
	if (wh_per_km < 0.1f) {
		return 999.9f; // Infinite range
	}
	
#if USE_REALISTIC_RANGE_CALCULATION
	// Realistic mode: Calculate remaining energy based on current battery level
	float remaining_wh = rt_data.battery_wh * rt_data.battery_level;
	float range = remaining_wh / wh_per_km;
#else
	// VESC Tool original: Use full battery capacity (shows range for full battery)
	float range = rt_data.battery_wh / wh_per_km;
#endif
	
	return range;
}

float vesc_rt_data_get_efficiency_whkm(void) {
	float wh_consumed = rt_data.watt_hours - rt_data.watt_hours_charged;
	float distance_km = rt_data.tachometer_abs / 1000.0f;
	
	if (distance_km < 0.01f) {
		return 0.0f; // Not enough data
	}
	
	return wh_consumed / distance_km;
}

// Call this periodically from main loop
void vesc_rt_data_loop(void) {
	if (!rt_data_active) return;
	
	// Don't request if BLE client is subscribed to notifications
	// When subscribed, the client will request RT data via BLE commands
	if (BLE_IsSubscribed()) {
		return;
	}
	
	uint32_t now = millis();
	
	// Request all RT data every 100ms
	if (now - last_request_time >= request_interval_ms) {
		vesc_rt_data_request();
		last_request_time = now;
	}
}

