#include "VESC_SDK_Driver.h"

// Global variables
static bool twai_initialized = false;
static bool vesc_sdk_initialized = false;
static vesc_sdk_data_t vesc_data;
static uint8_t current_vesc_id = VESC_CAN_ID;

// TWAI message queue
static QueueHandle_t twai_rx_queue = NULL;

// Custom debug output function for VESC SDK
void vesc_debug_output(const char *message) {
    Serial.print("🔧 VESC SDK: ");
    Serial.print(message);
}

// CAN send function for VESC SDK
bool can_send_function(uint32_t id, uint8_t* data, uint8_t len) {
    if (!twai_initialized) {
        Serial.println("❌ TWAI not initialized for CAN send");
        return false;
    }
    
    twai_message_t message;
    message.identifier = id;
    message.extd = 0;  // Standard frame
    message.rtr = 0;   // Data frame
    message.data_length_code = len;
    
    // Copy data
    for (int i = 0; i < len && i < 8; i++) {
        message.data[i] = data[i];
    }
    
    // Send message
    esp_err_t result = twai_transmit(&message, pdMS_TO_TICKS(100));
    if (result == ESP_OK) {
#ifdef DEBUG_CAN
        Serial.printf("📤 CAN TX: ID=0x%03X, Len=%d, Data=", id, len);
        for (int i = 0; i < len; i++) {
            Serial.printf("%02X ", data[i]);
        }
        Serial.println();
#endif
        return true;
    } else {
        Serial.printf("❌ CAN TX failed: ID=0x%03X, Error=%d\n", id, result);
        return false;
    }
}

// Response callback for VESC SDK (based on simple_example.c)
void response_callback(uint8_t controller_id, uint8_t command, uint8_t *data, uint8_t len) {
#ifdef DEBUG_CAN
    Serial.printf("📥 VESC#%d response: command=0x%02X, len=%d\n", controller_id, command, len);
#endif
    
    vesc_data.connected = true;
    vesc_data.last_update = millis();
    
    // Store debug info
    vesc_data.last_can_id = controller_id;
    vesc_data.last_can_length = len;
    for (int i = 0; i < len && i < 8; i++) {
        vesc_data.last_can_data[i] = data[i];
    }
    
    // Parse VESC responses - PONG and GET_VALUES
    switch (command) {
        case COMM_GET_VALUES: {
            vesc_values_t values;
            if (vesc_parse_get_values(data, len, &values)) {
                // Update our data structure with parsed values
                vesc_data.rpm = values.rpm;
                vesc_data.current = values.current_motor;
                vesc_data.duty_cycle = values.duty_cycle;
                vesc_data.voltage = values.v_in;
                vesc_data.temp_fet = values.temp_fet;
                vesc_data.temp_motor = values.temp_motor;
                vesc_data.amp_hours = values.amp_hours;
                vesc_data.amp_hours_charged = values.amp_hours_charged;
                vesc_data.watt_hours = values.watt_hours;
                vesc_data.watt_hours_charged = values.watt_hours_charged;
                vesc_data.fault_code = values.fault_code;
                
                // Calculate derived values
                vesc_data.battery_level = (vesc_data.voltage - 3.0f * 1) / (4.2f * 1 - 3.0f * 1) * 100.0f;
                vesc_data.speed_kmh = (vesc_data.rpm * 3.14159f * 1.0f * 60.0f) / (1.0f * 1000.0f);
                
                // Print data every 10th update (every 1 second)
                static uint8_t update_counter = 0;
                if (++update_counter >= 10) {
                    Serial.printf("    📊 Data: RPM=%.0f, Current=%.2fA, Duty=%.1f%%, Temp=%.1f°C, Voltage=%.1fV\n",
                                  values.rpm, values.current_motor, values.duty_cycle * 100.0f, values.temp_fet, values.v_in);
                    update_counter = 0;
                }
            } else {
                Serial.printf("    ❌ GET_VALUES parse failed (len=%d)\n", len);
            }
        } break;
        
        case CAN_PACKET_PONG: {
            vesc_pong_response_t pong;
            if (vesc_parse_pong_response(data, len, &pong)) {
                //Serial.printf("    🎯 PONG from VESC#%d - CONNECTION OK!\n", pong.controller_id);
            } else {
                Serial.printf("    🎯 PONG received but parse failed (len=%d)\n", len);
            }
        } break;
        
        default:
#ifdef DEBUG_CAN
            Serial.printf("    📋 Response: command=0x%02X, len=%d\n", command, len);
            if (len > 0) {
                Serial.print("    Data: ");
                for (int i = 0; i < len && i < 8; i++) {
                    Serial.printf("%02X ", data[i]);
                }
                Serial.println();
            }
#endif
            break;
    }
}

bool VESC_SDK_Init(uint8_t vesc_id) {
    Serial.println("=== VESC SDK Init ===");
    current_vesc_id = vesc_id;
    
    // Initialize data structure
    memset(&vesc_data, 0, sizeof(vesc_data));
    vesc_data.connected = false;
    vesc_data.last_update = 0;

    // TWAI configuration
    twai_timing_config_t t_config = VESC_CAN_SPEED; // 250 kbps for VESC
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(VESC_CAN_TX_GPIO_NUM, VESC_CAN_RX_GPIO_NUM, TWAI_MODE_NORMAL);
    
    // Install TWAI driver
    if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
        Serial.println("❌ Failed to install TWAI driver");
        return false;
    }
    Serial.println("✅ TWAI driver installed");
    
    // Start TWAI driver
    if (twai_start() != ESP_OK) {
        Serial.println("❌ Failed to start TWAI driver");
        return false;
    }
    Serial.println("✅ TWAI driver started");
    
    // Configure alerts
    uint32_t alerts = TWAI_ALERT_TX_SUCCESS | TWAI_ALERT_TX_FAILED | 
                      TWAI_ALERT_RX_DATA | TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_ERROR;
    if (twai_reconfigure_alerts(alerts, NULL) != ESP_OK) {
        Serial.println("❌ Failed to configure TWAI alerts");
        return false;
    }
    Serial.println("✅ TWAI alerts configured");
    
    twai_initialized = true;
    
    if (false) {
        // Setup VESC SDK debug output with full configuration
        vesc_debug_config_t debug_config = {
            .level = VESC_DEBUG_DETAILED,
            .categories = VESC_DEBUG_CAN | VESC_DEBUG_COMMANDS | VESC_DEBUG_RESPONSES | VESC_DEBUG_ERRORS | VESC_DEBUG_PERFORMANCE,
            .output_func = vesc_debug_output,
            .enable_timestamps = true,
            .enable_statistics = true
        };
        
        if (vesc_debug_configure(&debug_config)) {
            Serial.println("✅ VESC SDK debug configured with full monitoring");
            Serial.println("📊 Debug categories: CAN + Commands + Responses + Errors + Performance");
            Serial.println("⏰ Timestamps enabled, Statistics collection enabled");
        } else {
            Serial.println("❌ Failed to configure VESC SDK debug");
        }
    }
    
    // Initialize VESC CAN SDK (receiver_id=0 for any VESC, sender_id=vesc_id)
    if (!vesc_can_init(can_send_function, 0, vesc_id)) {
        Serial.println("❌ Failed to initialize VESC CAN SDK");
        return false;
    }
    Serial.printf("✅ VESC CAN SDK initialized (receiver_id=0, sender_id=%d)\n", vesc_id);
    
    // Set response callback
    vesc_set_response_callback(response_callback);
    Serial.println("✅ VESC response callback set");
    
    vesc_sdk_initialized = true;
    Serial.printf("✅ VESC SDK initialized (ID: %d, TX: GPIO%d, RX: GPIO%d, 250kbps)\n", 
                  vesc_id, VESC_CAN_TX_GPIO_NUM, VESC_CAN_RX_GPIO_NUM);
    
    return true;
}

void VESC_SDK_Loop() {
    if (!twai_initialized || !vesc_sdk_initialized) return;
    
    // Read alerts
    uint32_t alerts;
    twai_read_alerts(&alerts, pdMS_TO_TICKS(10));
    
    // Handle errors
    if (alerts & TWAI_ALERT_ERR_PASS) {
        Serial.println("⚠️  TWAI: Error passive state");
    }
    if (alerts & TWAI_ALERT_BUS_ERROR) {
        Serial.println("⚠️  TWAI: Bus error");
    }
    
    // Process received messages
    twai_message_t message;
    while (twai_receive(&message, pdMS_TO_TICKS(1)) == ESP_OK) {
        // Store debug info
        vesc_data.last_can_id = message.identifier;
        vesc_data.last_can_length = message.data_length_code;
        for (int i = 0; i < message.data_length_code && i < 8; i++) {
            vesc_data.last_can_data[i] = message.data[i];
        }
        
        uint8_t vesc_id = message.identifier & 0xFF;
        uint8_t packet_type = (message.identifier >> 8) & 0xFF;
        
#ifdef DEBUG_CAN
        Serial.printf("📥 CAN RX: ID=0x%03X (VESC#%d, Type=0x%02X), Len=%d, Data=", 
                      message.identifier, vesc_id, packet_type, message.data_length_code);
        for (int i = 0; i < message.data_length_code; i++) {
            Serial.printf("%02X ", message.data[i]);
        }
        Serial.println();
#endif
        
        // Handle direct CAN packets (STATUS messages) before SDK processing
        if (packet_type == CAN_PACKET_STATUS && message.data_length_code == 8) {
            // Parse STATUS packet directly (RPM, current, duty)
            vesc_status_msg_1_t status;
            if (vesc_parse_status_msg_1(message.data, message.data_length_code, &status)) {
                vesc_data.connected = true;
                vesc_data.last_update = millis();
                vesc_data.rpm = status.rpm;
                vesc_data.current = status.current;
                vesc_data.duty_cycle = status.duty;
                
#ifdef DEBUG_CAN
                Serial.printf("    📊 STATUS_1: RPM=%.0f, Current=%.2fA, Duty=%.1f%%\n",
                              status.rpm, status.current, status.duty * 100.0f);
#endif
            }
        }
        else if (packet_type == CAN_PACKET_STATUS_2 && message.data_length_code == 8) {
            // Parse STATUS_2 packet directly (Amp hours)
            vesc_status_msg_2_t status2;
            if (vesc_parse_status_msg_2(message.data, message.data_length_code, &status2)) {
                vesc_data.connected = true;
                vesc_data.last_update = millis();
                vesc_data.amp_hours = status2.amp_hours;
                vesc_data.amp_hours_charged = status2.amp_hours_charged;
                
#ifdef DEBUG_CAN
                Serial.printf("    🔋 STATUS_2: Ah=%.3f, Ah_chg=%.3f\n",
                              status2.amp_hours, status2.amp_hours_charged);
#endif
            }
        }
        else if (packet_type == CAN_PACKET_STATUS_3 && message.data_length_code == 8) {
            // Parse STATUS_3 packet directly (Watt hours)
            vesc_status_msg_3_t status3;
            if (vesc_parse_status_msg_3(message.data, message.data_length_code, &status3)) {
                vesc_data.connected = true;
                vesc_data.last_update = millis();
                vesc_data.watt_hours = status3.watt_hours;
                vesc_data.watt_hours_charged = status3.watt_hours_charged;
                
#ifdef DEBUG_CAN
                Serial.printf("    ⚡ STATUS_3: Wh=%.2f, Wh_chg=%.2f\n",
                              status3.watt_hours, status3.watt_hours_charged);
#endif
            }
        }
        else if (packet_type == CAN_PACKET_STATUS_4 && message.data_length_code == 8) {
            // Parse STATUS_4 packet directly (Temperatures, input current, PID)
            vesc_status_msg_4_t status4;
            if (vesc_parse_status_msg_4(message.data, message.data_length_code, &status4)) {
                vesc_data.connected = true;
                vesc_data.last_update = millis();
                vesc_data.temp_fet = status4.temp_fet;
                vesc_data.temp_motor = status4.temp_motor;
                vesc_data.current_in = status4.current_in;
                vesc_data.pid_pos_now = status4.pid_pos_now;
                
#ifdef DEBUG_CAN
                Serial.printf("    🌡️  STATUS_4: FET=%.1f°C, Motor=%.1f°C, I_in=%.2fA, PID=%.2f\n",
                              status4.temp_fet, status4.temp_motor, status4.current_in, status4.pid_pos_now);
#endif
            }
        }
        else if (packet_type == CAN_PACKET_STATUS_5 && message.data_length_code == 8) {
            // Parse STATUS_5 packet directly (Tachometer, input voltage)
            vesc_status_msg_5_t status5;
            if (vesc_parse_status_msg_5(message.data, message.data_length_code, &status5)) {
                vesc_data.connected = true;
                vesc_data.last_update = millis();
                vesc_data.voltage = status5.v_in;
                vesc_data.tacho_value = status5.tacho_value;
                
#ifdef DEBUG_CAN
                Serial.printf("    ⚡ STATUS_5: V_in=%.1fV, Tacho=%ld\n",
                              status5.v_in, status5.tacho_value);
#endif
            }
        }
        else if (packet_type == CAN_PACKET_STATUS_6 && message.data_length_code == 8) {
            // Parse STATUS_6 packet directly (ADC values, PPM)
            vesc_status_msg_6_t status6;
            if (vesc_parse_status_msg_6(message.data, message.data_length_code, &status6)) {
                vesc_data.connected = true;
                vesc_data.last_update = millis();
                vesc_data.adc_1 = status6.adc_1;
                vesc_data.adc_2 = status6.adc_2;
                vesc_data.adc_3 = status6.adc_3;
                vesc_data.ppm = status6.ppm;
                
#ifdef DEBUG_CAN
                Serial.printf("    📡 STATUS_6: ADC1=%.3f, ADC2=%.3f, ADC3=%.3f, PPM=%.3f\n",
                              status6.adc_1, status6.adc_2, status6.adc_3, status6.ppm);
#endif
            }
        }
        
        // Also process with VESC SDK for other commands
        vesc_process_can_frame(message.identifier, message.data, message.data_length_code);
    }
    
    // Check connection timeout
    if (vesc_data.connected && (millis() - vesc_data.last_update > VESC_CAN_TIMEOUT_MS)) {
        vesc_data.connected = false;
        Serial.println("⚠️  VESC connection timeout");
    }
    
    // Don't send GET_VALUES - wait for automatic STATUS packets from VESC
    // VESC automatically sends STATUS packets when motor is running or configured to do so
    
    /*
    // Send periodic data requests every 1000ms (slower for debugging)
    static unsigned long last_data_request = 0;
    if (millis() - last_data_request > 1000) { // Every 1 second
        Serial.printf("📤 Requesting values from VESC#%d\n", current_vesc_id);
        vesc_get_values(current_vesc_id);
        last_data_request = millis();
    }
    */
    
    // Send PING less frequently for connectivity test
    //static unsigned long last_ping_time = 0;
    //if (millis() - last_ping_time > 5000) { // Every 5 seconds
        //Serial.printf("📤 Pinging VESC#%d\n", current_vesc_id);
    //    vesc_ping(current_vesc_id);
    //    last_ping_time = millis();
    //}
}

bool VESC_SDK_IsConnected() {
    return vesc_data.connected && (millis() - vesc_data.last_update < VESC_CAN_TIMEOUT_MS);
}

vesc_sdk_data_t VESC_SDK_GetData() {
    return vesc_data;
}

void VESC_SDK_SetDuty(float duty) {
    if (!vesc_sdk_initialized) return;
    
    Serial.printf("📤 Setting duty: %.3f\n", duty);
    vesc_set_duty(current_vesc_id, duty);
}

void VESC_SDK_SetCurrent(float current) {
    if (!vesc_sdk_initialized) return;
    
    Serial.printf("📤 Setting current: %.2fA\n", current);
    vesc_set_current(current_vesc_id, current);
}

void VESC_SDK_SetBrakeCurrent(float current) {
    if (!vesc_sdk_initialized) return;
    
    Serial.printf("📤 Setting brake current: %.2fA\n", current);
    vesc_set_current_brake(current_vesc_id, current);
}

void VESC_SDK_SetRPM(float rpm) {
    if (!vesc_sdk_initialized) return;
    
    Serial.printf("📤 Setting RPM: %.0f\n", rpm);
    vesc_set_rpm(current_vesc_id, rpm);
}

void VESC_SDK_RequestStatus() {
    if (!vesc_sdk_initialized) return;
    
    Serial.printf("📤 Requesting status from VESC %d\n", current_vesc_id);
    vesc_get_values(current_vesc_id);
}

void VESC_SDK_Ping() {
    if (!vesc_sdk_initialized) return;
    
    Serial.printf("📤 Pinging VESC %d\n", current_vesc_id);
    vesc_ping(current_vesc_id);
}

void VESC_SDK_PrintDebug() {
    Serial.println("\n=== VESC SDK Debug ===");
    Serial.printf("TWAI Initialized: %s\n", twai_initialized ? "Yes" : "No");
    Serial.printf("SDK Initialized: %s\n", vesc_sdk_initialized ? "Yes" : "No");
    Serial.printf("VESC ID: %d\n", current_vesc_id);
    Serial.printf("Connected: %s\n", vesc_data.connected ? "Yes" : "No");
    Serial.printf("Last Update: %lu ms ago\n", millis() - vesc_data.last_update);
    
    Serial.println("--- VESC Values ---");
    Serial.printf("RPM: %.0f | Current: %.2fA | Duty: %.1f%%\n", vesc_data.rpm, vesc_data.current, vesc_data.duty_cycle * 100.0f);
    Serial.printf("Voltage: %.1fV | Input Current: %.2fA\n", vesc_data.voltage, vesc_data.current_in);
    Serial.printf("FET Temp: %.1f°C | Motor Temp: %.1f°C\n", vesc_data.temp_fet, vesc_data.temp_motor);
    Serial.printf("Amp Hours: %.3fAh (%.3fAh charged)\n", vesc_data.amp_hours, vesc_data.amp_hours_charged);
    Serial.printf("Watt Hours: %.2fWh (%.2fWh charged)\n", vesc_data.watt_hours, vesc_data.watt_hours_charged);
    Serial.printf("Tachometer: %ld | PID Position: %.2f\n", vesc_data.tacho_value, vesc_data.pid_pos_now);
    Serial.printf("ADC: %.3f, %.3f, %.3f | PPM: %.3f\n", vesc_data.adc_1, vesc_data.adc_2, vesc_data.adc_3, vesc_data.ppm);
    Serial.printf("Battery: %.1f%% | Speed: %.1f km/h\n", vesc_data.battery_level, vesc_data.speed_kmh);
    Serial.printf("Fault Code: 0x%08X\n", vesc_data.fault_code);
    
    Serial.println("--- Last CAN Message ---");
    Serial.printf("ID: 0x%03X, Length: %d, Data: ", vesc_data.last_can_id, vesc_data.last_can_length);
    for (int i = 0; i < vesc_data.last_can_length; i++) {
        Serial.printf("%02X ", vesc_data.last_can_data[i]);
    }
    Serial.println();
    
    // TWAI status
    if (twai_initialized) {
        twai_status_info_t status;
        if (twai_get_status_info(&status) == ESP_OK) {
            Serial.println("--- TWAI Status ---");
            Serial.printf("State: %d\n", status.state);
            Serial.printf("TX Queue: %lu\n", status.msgs_to_tx);
            Serial.printf("RX Queue: %lu\n", status.msgs_to_rx);
            Serial.printf("TX Errors: %lu\n", status.tx_error_counter);
            Serial.printf("RX Errors: %lu\n", status.rx_error_counter);
            Serial.printf("Bus Errors: %lu\n", status.bus_error_count);
        }
    }
    
    // VESC SDK Statistics
    Serial.println("--- VESC SDK Statistics ---");
    vesc_debug_stats_t stats;
    if (vesc_debug_get_stats(&stats)) {
        Serial.printf("CAN TX: %lu, CAN RX: %lu\n", stats.can_tx_count, stats.can_rx_count);
        Serial.printf("Commands: %lu, Responses: %lu\n", stats.command_count, stats.response_count);
        Serial.printf("Errors: %lu, CRC Errors: %lu\n", stats.error_count, stats.crc_error_count);
        Serial.printf("Buffer Overflows: %lu\n", stats.buffer_overflow_count);
        Serial.printf("TX Bytes: %llu, RX Bytes: %llu\n", stats.total_tx_bytes, stats.total_rx_bytes);
    } else {
        Serial.println("Statistics not available");
    }
    
    Serial.println("========================\n");
}
