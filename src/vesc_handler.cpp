/*
	Copyright 2025 Super VESC Display

	VESC Command Handler Module
	Processes incoming VESC commands from CAN bus and sends responses
*/

#include "vesc_handler.h"
#include "comm_can.h"
#include "buffer.h"
#include "datatypes.h"
#include "confparser.h"
#include "debug_log.h"
#include <Arduino.h>
#include <string.h>
#include <esp_mac.h>
#include <Preferences.h>

// Statistics
static uint32_t command_count = 0;

// Current configuration
static main_config_t current_config;
static Preferences preferences;

// Response callback (for BLE or other interfaces)
static vesc_response_callback_t response_callback = nullptr;

void vesc_handler_init(void) {
	command_count = 0;
	
	// Initialize preferences
	preferences.begin("vesc_config", false);
	
	// Try to load config from NVS
	size_t config_size = preferences.getBytesLength("main_config");
	if (config_size == sizeof(main_config_t)) {
		preferences.getBytes("main_config", &current_config, sizeof(main_config_t));
		LOG_INFO(VESC, "Config loaded from NVS: ID=%d, Baud=%d, Rate=%d Hz",
		         current_config.controller_id, 
		         current_config.can_baud_rate,
		         current_config.can_status_rate_hz);
	} else {
		// Set defaults if no config found
		confparser_set_defaults_main_config_t(&current_config);
		LOG_INFO(VESC, "Config set to defaults (no saved config)");
	}
	
	LOG_INFO(VESC, "Handler initialized");
}

// Helper function to save config to NVS
static void save_config_to_nvs(void) {
	preferences.putBytes("main_config", &current_config, sizeof(main_config_t));
	LOG_INFO(VESC, "💾 Config saved to NVS");
}

uint32_t vesc_handler_get_command_count(void) {
	return command_count;
}

const main_config_t* vesc_handler_get_config(void) {
	return &current_config;
}

void vesc_handler_set_response_callback(vesc_response_callback_t callback) {
	response_callback = callback;
	LOG_INFO(VESC, "Response callback %s", callback ? "enabled" : "disabled");
}

void vesc_handler_process_command(unsigned char *data, unsigned int len) {
	if (len < 1) return;
	
	uint8_t cmd = data[0];
	command_count++;
	
	// Log incoming command
	LOG_DEBUG(VESC, "[CMD #%04d] Len=%d, CMD=0x%02X", command_count, len, cmd);
	
	switch (cmd) {
		case COMM_FW_VERSION: {
			LOG_DEBUG(VESC, "(FW_VERSION) - Sending response");
			
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
			
			// Send via CAN if no callback set, otherwise send via callback (BLE)
			if (response_callback) {
				response_callback(send_buffer, ind);
			} else {
				comm_can_send_buffer(255, send_buffer, ind, 1); // Send to broadcast
			}
			LOG_DEBUG(VESC, "✅ FW_VERSION response sent: %s v%d.%02d", HW_NAME, FW_VERSION_MAJOR, FW_VERSION_MINOR);
		} break;
		
		case COMM_GET_VALUES: {
			LOG_DEBUG(VESC, "(GET_VALUES) - Sending dummy response");
			
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
			
			// Send via CAN if no callback set, otherwise send via callback (BLE)
			if (response_callback) {
				response_callback(send_buffer, ind);
			} else {
				// Send via CAN if no callback set, otherwise send via callback (BLE)
			if (response_callback) {
				response_callback(send_buffer, ind);
			} else {
				comm_can_send_buffer(255, send_buffer, ind, 1);
			}
			}
			LOG_DEBUG(VESC, "✅ GET_VALUES response sent");
		} break;
		
		case COMM_SET_DUTY:
			LOG_DEBUG(VESC, "(SET_DUTY)");
			break;
			
		case COMM_SET_CURRENT:
			LOG_DEBUG(VESC, "(SET_CURRENT)");
			break;
			
		case COMM_SET_RPM:
			LOG_DEBUG(VESC, "(SET_RPM)");
			break;
			
		case COMM_SET_CURRENT_BRAKE:
			LOG_DEBUG(VESC, "(SET_CURRENT_BRAKE)");
			break;
			
		case COMM_SET_POS:
			LOG_DEBUG(VESC, "(SET_POS)");
			break;
			
		case COMM_GET_VALUES_SELECTIVE: {
			LOG_DEBUG(VESC, "(GET_VALUES_SELECTIVE)");
			
			// Parse which values are requested
			uint32_t mask = 0;
			if (len >= 5) {
				int32_t ind = 1;
				mask = buffer_get_uint32(data, &ind);
				LOG_DEBUG(VESC, "   Requested values mask: 0x%08X", mask);
			}
			
			// For now, send empty response
			uint8_t send_buffer[4];
			int32_t ind = 0;
			send_buffer[ind++] = COMM_GET_VALUES_SELECTIVE;
			send_buffer[ind++] = 0; // No values yet
			
			// Send via CAN if no callback set, otherwise send via callback (BLE)
			if (response_callback) {
				response_callback(send_buffer, ind);
			} else {
				comm_can_send_buffer(255, send_buffer, ind, 1);
			}
		} break;
		
		case COMM_GET_CUSTOM_CONFIG_XML:
			LOG_DEBUG(VESC, "(GET_CUSTOM_CONFIG_XML) - Not implemented");
			// Send empty response
			{
				uint8_t send_buffer[2];
				send_buffer[0] = COMM_GET_CUSTOM_CONFIG_XML;
				send_buffer[1] = 0; // No custom config XML
				comm_can_send_buffer(255, send_buffer, 2, 1);
			}
			break;
			
    case COMM_GET_CUSTOM_CONFIG:
        LOG_DEBUG(VESC, "(GET_CUSTOM_CONFIG) - Sending config");
        {
            uint8_t send_buffer[80];
            int32_t ind = 0;

            // Parse config index from request
            int conf_ind = 0;
            if (len >= 2) {
                conf_ind = data[1];
            }

            // Only support config index 0
            if (conf_ind != 0) {
                LOG_DEBUG(VESC, "   ↳ Config index != 0, ignoring");
                break;
            }

            // Build response
            send_buffer[ind++] = COMM_GET_CUSTOM_CONFIG;
            send_buffer[ind++] = conf_ind;

            // Serialize current config
            int32_t serialized_len = confparser_serialize_main_config_t(send_buffer + ind, &current_config);
            ind += serialized_len;

            comm_can_send_buffer(255, send_buffer, ind, 1);
            LOG_DEBUG(VESC, "✅ Custom config sent: %d bytes total", ind);
			}
			break;
			
		case COMM_GET_CUSTOM_CONFIG_DEFAULT:
			LOG_DEBUG(VESC, "(GET_CUSTOM_CONFIG_DEFAULT) - Sending default config");
			{
				uint8_t send_buffer[80];
				int32_t ind = 0;
				
				// Parse config index from request
				int conf_ind = 0;
				if (len >= 2) {
					conf_ind = data[1];
				}
				
				// Only support config index 0
				if (conf_ind != 0) {
					LOG_DEBUG(VESC, "   ↳ Config index != 0, ignoring");
					break;
				}
				
				// Build response
				send_buffer[ind++] = COMM_GET_CUSTOM_CONFIG_DEFAULT;
				send_buffer[ind++] = conf_ind;
				
				// Create default config and serialize it
				main_config_t default_config;
				confparser_set_defaults_main_config_t(&default_config);
				
				int32_t serialized_len = confparser_serialize_main_config_t(send_buffer + ind, &default_config);
				ind += serialized_len;
				
				// Send via CAN if no callback set, otherwise send via callback (BLE)
			if (response_callback) {
				response_callback(send_buffer, ind);
			} else {
				comm_can_send_buffer(255, send_buffer, ind, 1);
			}
				LOG_DEBUG(VESC, "✅ Default custom config sent: %d bytes total", ind);
			}
			break;
			
		case COMM_SET_CUSTOM_CONFIG:
			LOG_DEBUG(VESC, "(SET_CUSTOM_CONFIG) - Setting new config");
			{
				// Parse config index from request
				int conf_ind = 0;
				if (len >= 2) {
					conf_ind = data[1];
				}
				
				// Only support config index 0
				if (conf_ind != 0) {
					LOG_DEBUG(VESC, "   ↳ Config index != 0, ignoring");
					break;
				}
				
				// Create temporary config to deserialize into
				main_config_t new_config;
				
				// Deserialize config from data (skip first byte which is conf_ind)
				if (confparser_deserialize_main_config_t(data + 2, &new_config)) {
					// Check if CAN baud rate changed
					bool baud_changed = current_config.can_baud_rate != new_config.can_baud_rate;
					
					// Update current config
					current_config = new_config;
					
					// Save to NVS
					save_config_to_nvs();
					
					// Send acknowledgement
					uint8_t send_buffer[50];
					int32_t ind = 0;
					send_buffer[ind++] = COMM_SET_CUSTOM_CONFIG;
					// Send via CAN if no callback set, otherwise send via callback (BLE)
			if (response_callback) {
				response_callback(send_buffer, ind);
			} else {
				comm_can_send_buffer(255, send_buffer, ind, 1);
			}
					
					LOG_INFO(VESC, "✅ Config updated successfully!");
					LOG_INFO(VESC, "   ID=%d, Baud=%d, Rate=%d Hz",
					         current_config.controller_id,
					         current_config.can_baud_rate,
					         current_config.can_status_rate_hz);
					
					if (baud_changed) {
						LOG_WARN(VESC, "CAN baud rate changed! Restart required for changes to take effect.");
					}
				} else {
					LOG_ERROR(VESC, "Failed to deserialize config!");
				}
			}
			break;
			
		case COMM_BMS_GET_VALUES:
			LOG_DEBUG(VESC, "(BMS_GET_VALUES) - Not a BMS");
			break;
			
		case COMM_PSW_GET_STATUS:
			LOG_DEBUG(VESC, "(PSW_GET_STATUS) - Not a power switch");
			break;
			
		case COMM_PSW_SWITCH:
			LOG_DEBUG(VESC, "(PSW_SWITCH) - Not a power switch");
			break;
			
		case COMM_IO_BOARD_GET_ALL:
			LOG_DEBUG(VESC, "(IO_BOARD_GET_ALL) - Not an IO board");
			break;
			
		case COMM_IO_BOARD_SET_PWM:
			LOG_DEBUG(VESC, "(IO_BOARD_SET_PWM) - Not an IO board");
			break;
			
		case COMM_IO_BOARD_SET_DIGITAL:
			LOG_DEBUG(VESC, "(IO_BOARD_SET_DIGITAL) - Not an IO board");
			break;
		
		default:
			LOG_WARN(VESC, "(Unknown command 0x%02X)", cmd);
			break;
	}
	
	// Log raw HEX for debugging (first 16 bytes)
	LOG_HEX(VESC, data, len, "   Raw: ");
}
