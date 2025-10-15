/*
	Copyright 2022 Benjamin Vedder	benjamin@vedder.se
	Copyright 2025 Adapted for Super VESC Display

	This file is part of the VESC firmware.

	The VESC firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The VESC firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    */

#ifndef DATATYPES_H_
#define DATATYPES_H_

#include <stdint.h>
#include <stdbool.h>
#include "ble_config.h"

// CAN commands
typedef enum {
	CAN_PACKET_SET_DUTY						= 0,
	CAN_PACKET_SET_CURRENT					= 1,
	CAN_PACKET_SET_CURRENT_BRAKE			= 2,
	CAN_PACKET_SET_RPM						= 3,
	CAN_PACKET_SET_POS						= 4,
	CAN_PACKET_FILL_RX_BUFFER				= 5,
	CAN_PACKET_FILL_RX_BUFFER_LONG			= 6,
	CAN_PACKET_PROCESS_RX_BUFFER			= 7,
	CAN_PACKET_PROCESS_SHORT_BUFFER			= 8,
	CAN_PACKET_STATUS						= 9,
	CAN_PACKET_SET_CURRENT_REL				= 10,
	CAN_PACKET_SET_CURRENT_BRAKE_REL		= 11,
	CAN_PACKET_SET_CURRENT_HANDBRAKE		= 12,
	CAN_PACKET_SET_CURRENT_HANDBRAKE_REL	= 13,
	CAN_PACKET_STATUS_2						= 14,
	CAN_PACKET_STATUS_3						= 15,
	CAN_PACKET_STATUS_4						= 16,
	CAN_PACKET_PING							= 17,
	CAN_PACKET_PONG							= 18,
	CAN_PACKET_DETECT_APPLY_ALL_FOC			= 19,
	CAN_PACKET_DETECT_APPLY_ALL_FOC_RES		= 20,
	CAN_PACKET_CONF_CURRENT_LIMITS			= 21,
	CAN_PACKET_CONF_STORE_CURRENT_LIMITS	= 22,
	CAN_PACKET_CONF_CURRENT_LIMITS_IN		= 23,
	CAN_PACKET_CONF_STORE_CURRENT_LIMITS_IN	= 24,
	CAN_PACKET_CONF_FOC_ERPMS				= 25,
	CAN_PACKET_CONF_STORE_FOC_ERPMS			= 26,
	CAN_PACKET_STATUS_5						= 27,
	CAN_PACKET_POLL_TS5700N8501_STATUS		= 28,
	CAN_PACKET_CONF_BATTERY_CUT				= 29,
	CAN_PACKET_CONF_STORE_BATTERY_CUT		= 30,
	CAN_PACKET_SHUTDOWN						= 31,
	CAN_PACKET_IO_BOARD_ADC_1_TO_4			= 32,
	CAN_PACKET_IO_BOARD_ADC_5_TO_8			= 33,
	CAN_PACKET_IO_BOARD_ADC_9_TO_12			= 34,
	CAN_PACKET_IO_BOARD_DIGITAL_IN			= 35,
	CAN_PACKET_IO_BOARD_SET_OUTPUT_DIGITAL	= 36,
	CAN_PACKET_IO_BOARD_SET_OUTPUT_PWM		= 37,
	CAN_PACKET_BMS_V_TOT					= 38,
	CAN_PACKET_BMS_I						= 39,
	CAN_PACKET_BMS_AH_WH					= 40,
	CAN_PACKET_BMS_V_CELL					= 41,
	CAN_PACKET_BMS_BAL						= 42,
	CAN_PACKET_BMS_TEMPS					= 43,
	CAN_PACKET_BMS_HUM						= 44,
	CAN_PACKET_BMS_SOC_SOH_TEMP_STAT		= 45,
	CAN_PACKET_PSW_STAT						= 46,
	CAN_PACKET_PSW_SWITCH					= 47,
	CAN_PACKET_BMS_HW_DATA_1				= 48,
	CAN_PACKET_BMS_HW_DATA_2				= 49,
	CAN_PACKET_BMS_HW_DATA_3				= 50,
	CAN_PACKET_BMS_HW_DATA_4				= 51,
	CAN_PACKET_BMS_HW_DATA_5				= 52,
	CAN_PACKET_BMS_AH_WH_CHG_TOTAL			= 53,
	CAN_PACKET_BMS_AH_WH_DIS_TOTAL			= 54,
	CAN_PACKET_UPDATE_PID_POS_OFFSET		= 55,
	CAN_PACKET_POLL_ROTOR_POS				= 56,
	CAN_PACKET_NOTIFY_BOOT					= 57,
	CAN_PACKET_STATUS_6						= 58,
	CAN_PACKET_GNSS_TIME					= 59,
	CAN_PACKET_GNSS_LAT						= 60,
	CAN_PACKET_GNSS_LON						= 61,
	CAN_PACKET_GNSS_ALT_SPEED_HDOP			= 62,
	CAN_PACKET_UPDATE_BAUD					= 63,
	CAN_PACKET_BMS_STATUS_1					= 64,
	CAN_PACKET_BMS_STATUS_2					= 65,
	CAN_PACKET_BMS_STATUS_3					= 66,
	CAN_PACKET_BMS_STATUS_4					= 67,
	CAN_PACKET_BMS_STATUS_5					= 68,
	CAN_PACKET_MAKE_ENUM_32_BITS = 0xFFFFFFFF,
} CAN_PACKET_ID;

typedef struct {
	int id;
	uint32_t rx_time;
	float rpm;
	float current;
	float duty;
} can_status_msg;

typedef struct {
	int id;
	uint32_t rx_time;
	float amp_hours;
	float amp_hours_charged;
} can_status_msg_2;

typedef struct {
	int id;
	uint32_t rx_time;
	float watt_hours;
	float watt_hours_charged;
} can_status_msg_3;

typedef struct {
	int id;
	uint32_t rx_time;
	float temp_fet;
	float temp_motor;
	float current_in;
	float pid_pos_now;
} can_status_msg_4;

typedef struct {
	int id;
	uint32_t rx_time;
	float v_in;
	int32_t tacho_value;
} can_status_msg_5;

typedef struct {
	int id;
	uint32_t rx_time;
	float adc_1;
	float adc_2;
	float adc_3;
	float ppm;
} can_status_msg_6;

typedef struct {
	int id;
	uint32_t rx_time;
	float adc_voltages[4];
} io_board_adc_values;

typedef struct {
	int id;
	uint32_t rx_time;
	uint64_t inputs;
} io_board_digial_inputs;

typedef struct {
	int id;
	uint32_t rx_time;
	float v_in;
	float v_out;
	float temp;
	bool is_out_on;
	bool is_pch_on;
	bool is_dsc_on;
} psw_status;

// HW types
typedef enum {
	HW_TYPE_VESC = 0,
	HW_TYPE_VESC_BMS,
	HW_TYPE_CUSTOM_MODULE
} HW_TYPE;

// CAN baudrate
typedef enum {
	CAN_BAUD_125K = 0,
	CAN_BAUD_250K,
	CAN_BAUD_500K,
	CAN_BAUD_1M,
	CAN_BAUD_10K,
	CAN_BAUD_20K,
	CAN_BAUD_50K,
	CAN_BAUD_75K,
	CAN_BAUD_100K,
	CAN_BAUD_INVALID,
} CAN_BAUD;

// Communication commands
typedef enum {
	COMM_FW_VERSION							= 0,
	COMM_JUMP_TO_BOOTLOADER					= 1,
	COMM_ERASE_NEW_APP						= 2,
	COMM_WRITE_NEW_APP_DATA					= 3,
	COMM_GET_VALUES							= 4,
	COMM_SET_DUTY							= 5,
	COMM_SET_CURRENT						= 6,
	COMM_SET_CURRENT_BRAKE					= 7,
	COMM_SET_RPM							= 8,
	COMM_SET_POS							= 9,
	COMM_SET_HANDBRAKE						= 10,
	COMM_SET_DETECT							= 11,
	COMM_SET_SERVO_POS						= 12,
	COMM_SET_MCCONF							= 13,
	COMM_GET_MCCONF							= 14,
	COMM_GET_MCCONF_DEFAULT					= 15,
	COMM_SET_APPCONF						= 16,
	COMM_GET_APPCONF						= 17,
	COMM_GET_APPCONF_DEFAULT				= 18,
	COMM_SAMPLE_PRINT						= 19,
	COMM_TERMINAL_CMD						= 20,
	COMM_PRINT								= 21,
	COMM_ROTOR_POSITION						= 22,
	COMM_EXPERIMENT_SAMPLE					= 23,
	COMM_DETECT_MOTOR_PARAM					= 24,
	COMM_DETECT_MOTOR_R_L					= 25,
	COMM_DETECT_MOTOR_FLUX_LINKAGE			= 26,
	COMM_DETECT_ENCODER						= 27,
	COMM_DETECT_HALL_FOC					= 28,
	COMM_REBOOT								= 29,
	COMM_ALIVE								= 30,
	COMM_GET_DECODED_PPM					= 31,
	COMM_GET_DECODED_ADC					= 32,
	COMM_GET_DECODED_CHUK					= 33,
	COMM_FORWARD_CAN						= 34,
	COMM_SET_CHUCK_DATA						= 35,
	COMM_CUSTOM_APP_DATA					= 36,
	
	COMM_GET_VALUES_SETUP					= 47,
	COMM_GET_VALUES_SELECTIVE				= 50,
	COMM_GET_VALUES_SETUP_SELECTIVE			= 51,
	COMM_ERASE_BOOTLOADER					= 73,
	COMM_WRITE_NEW_APP_DATA_LZO				= 81,
	COMM_SET_CURRENT_REL					= 84,
	
	// Custom configuration for hardware
	COMM_GET_CUSTOM_CONFIG_XML				= 92,
	COMM_GET_CUSTOM_CONFIG					= 93,
	COMM_GET_CUSTOM_CONFIG_DEFAULT			= 94,
	COMM_SET_CUSTOM_CONFIG					= 95,
	
	// BMS commands
	COMM_BMS_GET_VALUES						= 96,
	
	COMM_SET_ODOMETER						= 110,
	
	// Power switch commands
	COMM_PSW_GET_STATUS						= 111,
	COMM_PSW_SWITCH							= 112,
	
	// IO Board
	COMM_IO_BOARD_GET_ALL					= 122,
	COMM_IO_BOARD_SET_PWM					= 123,
	COMM_IO_BOARD_SET_DIGITAL				= 124,
	
	COMM_GET_STATS							= 128,
} COMM_PACKET_ID;

// Packet settings
#define PACKET_MAX_PL_LEN		512

// Firmware version
#define FW_VERSION_MAJOR		6
#define FW_VERSION_MINOR		05
#define FW_TEST_VERSION_NUMBER	0

// Hardware name
#define HW_NAME					"Super VESC Display"

// CAN configuration
// Device CAN ID (our display's ID on the CAN bus)
#define CONF_CONTROLLER_ID		2

#define CONF_CAN_BAUD_RATE		CAN_BAUD_250K


// Main configuration structure (simplified for display)
typedef struct {
    int16_t controller_id;
    uint8_t can_baud_rate;
    int16_t can_status_rate_hz;
} main_config_t;

// Config signature constant (must match VESC Tool expectations)
#define MAIN_CONFIG_T_SIGNATURE 1954583966

// VESC RT Data Structure (SETUP_VALUES)
typedef struct {
	float temp_mos;
	float temp_motor;
	float current_motor;
	float current_in;
	float duty_now;
	float rpm;
	float speed;                // m/s
	float v_in;
	float battery_level;        // 0-1
	float amp_hours;
	float amp_hours_charged;
	float watt_hours;
	float watt_hours_charged;
	float tachometer;           // meters (signed)
	float tachometer_abs;       // meters (trip)
	float position;
	uint8_t fault_code;
	uint8_t vesc_id;
	uint8_t num_vescs;
	float battery_wh;           // battery capacity in Wh
	uint32_t odometer;          // meters
	uint32_t uptime_ms;         // milliseconds
	uint32_t rx_time;           // Reception timestamp
} vesc_setup_values_t;

#endif /* DATATYPES_H_ */
