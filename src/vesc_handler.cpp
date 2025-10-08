/*
	Copyright 2025 Super VESC Display

	VESC Command Handler Module
	Processes incoming VESC commands from CAN bus and sends responses
*/

#include "vesc_handler.h"
#include "comm_can.h"
#include "buffer.h"
#include "datatypes.h"
#include <Arduino.h>
#include <string.h>
#include <esp_mac.h>

// Statistics
static uint32_t command_count = 0;

void vesc_handler_init(void) {
	command_count = 0;
	Serial.println("✅ VESC Handler initialized");
}

uint32_t vesc_handler_get_command_count(void) {
	return command_count;
}

void vesc_handler_process_command(unsigned char *data, unsigned int len) {
	if (len < 1) return;
	
	uint8_t cmd = data[0];
	command_count++;
	
	// Log incoming command
	Serial.printf("[VESC CMD #%04d] Len=%d, CMD=0x%02X ", command_count, len, cmd);
	
	switch (cmd) {
		case COMM_FW_VERSION: {
			Serial.println("(FW_VERSION) - Sending response");
			
			uint8_t send_buffer[80];
			int32_t ind = 0;
			
			send_buffer[ind++] = COMM_FW_VERSION;
			send_buffer[ind++] = FW_VERSION_MAJOR;
			send_buffer[ind++] = FW_VERSION_MINOR;
			
			// Hardware name (null-terminated string)
			strcpy((char*)(send_buffer + ind), HW_NAME);
			ind += strlen(HW_NAME) + 1;
			
			// MAC address (6 bytes) - get ESP32 MAC
			uint8_t mac[6];
			esp_read_mac(mac, ESP_MAC_WIFI_STA);
			memcpy(send_buffer + ind, mac, 6);
			ind += 6;
			
			// UUID (6 bytes) - zeros for now
			memset(send_buffer + ind, 0, 6);
			ind += 6;
			
			send_buffer[ind++] = 0; // Pairing done
			send_buffer[ind++] = FW_TEST_VERSION_NUMBER;
			send_buffer[ind++] = HW_TYPE_CUSTOM_MODULE; // HW Type
			send_buffer[ind++] = 1; // One custom config
			send_buffer[ind++] = 0; // No phase filters
			send_buffer[ind++] = 0; // No HW QML
			send_buffer[ind++] = 0; // QML flags
			send_buffer[ind++] = 0; // No NRF flags
			
			// Firmware name (null-terminated string) - use HW_NAME
			strcpy((char*)(send_buffer + ind), HW_NAME);
			ind += strlen(HW_NAME) + 1;
			
			// HW CRC (4 bytes)
			uint32_t hw_crc = 0x12345678; // Dummy CRC
			buffer_append_uint32(send_buffer, hw_crc, &ind);
			
			comm_can_send_buffer(255, send_buffer, ind, 1); // Send to broadcast
			Serial.printf("✅ FW_VERSION response sent: %s v%d.%02d\n", HW_NAME, FW_VERSION_MAJOR, FW_VERSION_MINOR);
		} break;
		
		case COMM_GET_VALUES: {
			Serial.println("(GET_VALUES) - Sending dummy response");
			
			uint8_t send_buffer[70];
			int32_t ind = 0;
			
			send_buffer[ind++] = COMM_GET_VALUES;
			
			// Send dummy values for now
			buffer_append_float16(send_buffer, 0.0, 1e3, &ind);     // temp_mos
			buffer_append_float16(send_buffer, 0.0, 1e3, &ind);     // temp_motor  
			buffer_append_float32(send_buffer, 0.0, 1e2, &ind);     // avg_motor_current
			buffer_append_float32(send_buffer, 0.0, 1e2, &ind);     // avg_input_current
			buffer_append_float32(send_buffer, 0.0, 1e2, &ind);     // avg_id
			buffer_append_float32(send_buffer, 0.0, 1e2, &ind);     // avg_iq
			buffer_append_float16(send_buffer, 0.0, 1e1, &ind);     // duty_now
			buffer_append_float32(send_buffer, 0.0, 1e0, &ind);     // rpm
			buffer_append_float16(send_buffer, 24.0, 1e1, &ind);    // v_in (24V)
			buffer_append_float32(send_buffer, 0.0, 1e4, &ind);     // amp_hours
			buffer_append_float32(send_buffer, 0.0, 1e4, &ind);     // amp_hours_charged
			buffer_append_float32(send_buffer, 0.0, 1e4, &ind);     // watt_hours
			buffer_append_float32(send_buffer, 0.0, 1e4, &ind);     // watt_hours_charged
			buffer_append_int32(send_buffer, 0, &ind);              // tachometer
			buffer_append_int32(send_buffer, 0, &ind);              // tachometer_abs
			send_buffer[ind++] = 0;                                 // mc_fault_code
			buffer_append_int32(send_buffer, 0, &ind);              // pid_pos
			send_buffer[ind++] = 0;                                 // app_controller_id
			buffer_append_float16(send_buffer, 0.0, 1e3, &ind);     // temp_mos_1
			buffer_append_float16(send_buffer, 0.0, 1e3, &ind);     // temp_mos_2
			buffer_append_float16(send_buffer, 0.0, 1e3, &ind);     // temp_mos_3
			buffer_append_float32(send_buffer, 0.0, 1e5, &ind);     // avg_vd
			buffer_append_float32(send_buffer, 0.0, 1e5, &ind);     // avg_vq
			
			comm_can_send_buffer(255, send_buffer, ind, 1);
			Serial.println("✅ GET_VALUES response sent");
		} break;
		
		case COMM_SET_DUTY:
			Serial.println("(SET_DUTY)");
			break;
			
		case COMM_SET_CURRENT:
			Serial.println("(SET_CURRENT)");
			break;
			
		case COMM_SET_RPM:
			Serial.println("(SET_RPM)");
			break;
			
		case COMM_SET_CURRENT_BRAKE:
			Serial.println("(SET_CURRENT_BRAKE)");
			break;
			
		case COMM_SET_POS:
			Serial.println("(SET_POS)");
			break;
			
		case COMM_GET_VALUES_SELECTIVE: {
			Serial.println("(GET_VALUES_SELECTIVE)");
			
			// Parse which values are requested
			uint32_t mask = 0;
			if (len >= 5) {
				int32_t ind = 1;
				mask = buffer_get_uint32(data, &ind);
				Serial.printf("   Requested values mask: 0x%08X\n", mask);
			}
			
			// For now, send empty response
			uint8_t send_buffer[4];
			int32_t ind = 0;
			send_buffer[ind++] = COMM_GET_VALUES_SELECTIVE;
			send_buffer[ind++] = 0; // No values yet
			
			comm_can_send_buffer(255, send_buffer, ind, 1);
		} break;
		
		default:
			Serial.printf("(Unknown command 0x%02X)\n", cmd);
			break;
	}
	
	// Log raw HEX for debugging (first 16 bytes)
	if (len <= 16) {
		Serial.print("    Raw: ");
		for (unsigned int i = 0; i < len; i++) {
			Serial.printf("%02X ", data[i]);
		}
		Serial.println();
	}
}
