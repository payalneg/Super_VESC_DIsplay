/*****************************************************************************
 * | File         :   VESC_TWAI_Driver.cpp
 * | Author       :   VESC Display Team
 * | Function     :   VESC CAN driver using ESP32 TWAI interface
 * | Info         :   Direct CAN communication with VESC controllers
 * ----------------
 * | This version :   V1.0
 * | Date         :   2024-12-25
 * | Info         :   TWAI-based VESC CAN implementation
 *
 ******************************************************************************/

#include "VESC_TWAI_Driver.h"
#include <string.h>

// Global variables
static bool twai_driver_installed = false;
static bool twai_driver_started = false;
static vesc_twai_values_t vesc_values;
static vesc_twai_config_t vesc_config;
static unsigned long last_status_request = 0;
static unsigned long last_ping_time = 0;

// Helper function to convert bytes to float (big-endian)
float bytes_to_float(uint8_t* bytes, int32_t scale) {
    int32_t value = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
    return (float)value / (float)scale;
}

// Helper function to convert float to bytes (big-endian)
void float_to_bytes(float value, int32_t scale, uint8_t* bytes) {
    int32_t int_value = (int32_t)(value * scale);
    bytes[0] = (int_value >> 24) & 0xFF;
    bytes[1] = (int_value >> 16) & 0xFF;
    bytes[2] = (int_value >> 8) & 0xFF;
    bytes[3] = int_value & 0xFF;
}

// Helper function to convert bytes to int32 (big-endian)
int32_t bytes_to_int32(uint8_t* bytes) {
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

// Helper function to convert bytes to int16 (big-endian)
int16_t bytes_to_int16(uint8_t* bytes) {
    return (bytes[0] << 8) | bytes[1];
}

bool VESC_TWAI_Init(uint8_t vesc_id) {
    Serial.println("Initializing VESC TWAI Driver...");
    
    // Initialize values structure
    memset(&vesc_values, 0, sizeof(vesc_values));
    vesc_values.connected = false;
    vesc_values.last_update = 0;
    
    // Initialize configuration with defaults
    vesc_config.wheel_diameter = 0.107f;  // 107mm wheel diameter
    vesc_config.motor_poles = 14;         // 14 pole motor
    vesc_config.gear_ratio = 1.0f;        // Direct drive
    vesc_config.battery_cells = 10;       // 10S battery
    vesc_config.battery_capacity = 5.0f;  // 5Ah capacity
    vesc_config.vesc_id = vesc_id;
    
    // TWAI configuration settings
    static const twai_timing_config_t t_config = VESC_CAN_SPEED; // 500 kbps for VESC
    static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL(); // Accept all messages
    static const twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(VESC_CAN_TX_GPIO_NUM, VESC_CAN_RX_GPIO_NUM, TWAI_MODE_NORMAL);
    
    // Install TWAI driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        twai_driver_installed = true;
        Serial.println("VESC TWAI driver installed successfully");
    } else {
        Serial.println("Failed to install VESC TWAI driver");
        return false;
    }

    // Start TWAI driver
    if (twai_start() == ESP_OK) {
        twai_driver_started = true;
        Serial.println("VESC TWAI driver started successfully");
    } else {
        Serial.println("Failed to start VESC TWAI driver");
        return false;
    }

    // Configure alerts for CAN events
    uint32_t alerts_to_enable = TWAI_ALERT_TX_SUCCESS | TWAI_ALERT_TX_FAILED |
                                TWAI_ALERT_RX_DATA | TWAI_ALERT_RX_QUEUE_FULL |
                                TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_ERROR;
    if (twai_reconfigure_alerts(alerts_to_enable, NULL) == ESP_OK) {
        Serial.println("VESC TWAI alerts configured successfully");
    } else {
        Serial.println("Failed to configure VESC TWAI alerts");
        return false;
    }

    Serial.printf("VESC TWAI initialized for VESC ID: %d\n", vesc_id);
    Serial.printf("CAN TX Pin: %d, CAN RX Pin: %d\n", VESC_CAN_TX_GPIO_NUM, VESC_CAN_RX_GPIO_NUM);
    
    return true;
}

void VESC_TWAI_Loop() {
    if (!twai_driver_installed || !twai_driver_started) {
        return;
    }
    
    // Read CAN alerts
    uint32_t alerts_triggered;
    twai_read_alerts(&alerts_triggered, pdMS_TO_TICKS(10));
    
    // Handle alerts
    if (alerts_triggered & TWAI_ALERT_ERR_PASS) {
        Serial.println("VESC TWAI: Controller in error passive state");
    }
    
    if (alerts_triggered & TWAI_ALERT_BUS_ERROR) {
        Serial.println("VESC TWAI: Bus error occurred");
        twai_status_info_t status;
        twai_get_status_info(&status);
        Serial.printf("Bus error count: %lu\n", status.bus_error_count);
    }
    
    if (alerts_triggered & TWAI_ALERT_TX_FAILED) {
        Serial.println("VESC TWAI: Transmission failed");
    }
    
    // Process received messages
    if (alerts_triggered & TWAI_ALERT_RX_DATA) {
        twai_message_t message;
        while (twai_receive(&message, 0) == ESP_OK) {
            // Parse VESC CAN message
            uint8_t vesc_id = (message.identifier >> 8) & 0xFF;
            uint8_t packet_type = message.identifier & 0xFF;
            
            // Only process messages from our VESC
            if (vesc_id == vesc_config.vesc_id) {
                switch (packet_type) {
                    case VESC_CAN_PACKET_STATUS:
                        if (message.data_length_code >= 8) {
                            vesc_values.rpm = bytes_to_float(&message.data[0], 1);
                            vesc_values.current = bytes_to_float(&message.data[4], 100);
                            vesc_values.connected = true;
                            vesc_values.last_update = millis();
                        }
                        break;
                        
                    case VESC_CAN_PACKET_STATUS_2:
                        if (message.data_length_code >= 8) {
                            vesc_values.amp_hours = bytes_to_float(&message.data[0], 10000);
                            vesc_values.amp_hours_charged = bytes_to_float(&message.data[4], 10000);
                            vesc_values.last_update = millis();
                        }
                        break;
                        
                    case VESC_CAN_PACKET_STATUS_3:
                        if (message.data_length_code >= 8) {
                            vesc_values.watt_hours = bytes_to_float(&message.data[0], 10000);
                            vesc_values.watt_hours_charged = bytes_to_float(&message.data[4], 10000);
                            vesc_values.last_update = millis();
                        }
                        break;
                        
                    case VESC_CAN_PACKET_STATUS_4:
                        if (message.data_length_code >= 8) {
                            vesc_values.temp_fet = bytes_to_int16(&message.data[0]) / 10.0f;
                            vesc_values.temp_motor = bytes_to_int16(&message.data[2]) / 10.0f;
                            vesc_values.current_in = bytes_to_float(&message.data[4], 100);
                            vesc_values.last_update = millis();
                        }
                        break;
                        
                    case VESC_CAN_PACKET_STATUS_5:
                        if (message.data_length_code >= 8) {
                            vesc_values.v_in = bytes_to_float(&message.data[0], 100);
                            vesc_values.tacho_value = bytes_to_int32(&message.data[4]);
                            vesc_values.last_update = millis();
                        }
                        break;
                        
                    case VESC_CAN_PACKET_PONG:
                        Serial.println("VESC TWAI: Received PONG");
                        vesc_values.connected = true;
                        vesc_values.last_update = millis();
                        break;
                        
                    default:
                        // Unknown packet type
                        break;
                }
                
                // Calculate derived values
                if (vesc_values.connected) {
                    // Calculate speed in km/h
                    float wheel_circumference = vesc_config.wheel_diameter * 3.14159f;
                    float motor_rpm = vesc_values.rpm / vesc_config.gear_ratio;
                    float wheel_rpm = motor_rpm / (vesc_config.motor_poles / 2.0f);
                    vesc_values.speed_kmh = (wheel_rpm * wheel_circumference * 60.0f) / 1000.0f;
                    
                    // Calculate power
                    vesc_values.power_watts = vesc_values.v_in * vesc_values.current_in;
                    
                    // Calculate battery percentage (simple voltage-based estimation)
                    float cell_voltage = vesc_values.v_in / vesc_config.battery_cells;
                    if (cell_voltage > 4.2f) {
                        vesc_values.battery_percentage = 100.0f;
                    } else if (cell_voltage < 3.0f) {
                        vesc_values.battery_percentage = 0.0f;
                    } else {
                        vesc_values.battery_percentage = ((cell_voltage - 3.0f) / 1.2f) * 100.0f;
                    }
                    
                    // Calculate distance (basic integration)
                    static unsigned long last_distance_update = 0;
                    unsigned long now = millis();
                    if (last_distance_update > 0) {
                        float time_hours = (now - last_distance_update) / 3600000.0f;
                        vesc_values.distance_km += vesc_values.speed_kmh * time_hours;
                    }
                    last_distance_update = now;
                }
            }
        }
    }
    
    // Check connection timeout
    if (vesc_values.connected && (millis() - vesc_values.last_update > VESC_CAN_TIMEOUT_MS)) {
        vesc_values.connected = false;
        Serial.println("VESC TWAI: Connection timeout");
    }
    
    // Request status updates periodically
    if (millis() - last_status_request > VESC_CAN_UPDATE_RATE_MS) {
        VESC_TWAI_RequestStatus();
        last_status_request = millis();
    }
    
    // Send ping periodically
    if (millis() - last_ping_time > 5000) { // Ping every 5 seconds
        VESC_TWAI_Ping();
        last_ping_time = millis();
    }
}

bool VESC_TWAI_IsConnected() {
    return vesc_values.connected && (millis() - vesc_values.last_update < VESC_CAN_TIMEOUT_MS);
}

vesc_twai_values_t VESC_TWAI_GetValues() {
    return vesc_values;
}

void VESC_TWAI_SetConfig(float wheel_diameter, float motor_poles, float gear_ratio, 
                         float battery_cells, float battery_capacity) {
    vesc_config.wheel_diameter = wheel_diameter;
    vesc_config.motor_poles = motor_poles;
    vesc_config.gear_ratio = gear_ratio;
    vesc_config.battery_cells = battery_cells;
    vesc_config.battery_capacity = battery_capacity;
    
    Serial.printf("VESC TWAI Config updated: Wheel=%.3fm, Poles=%.0f, Ratio=%.2f, Cells=%.0f, Capacity=%.1fAh\n",
                  wheel_diameter, motor_poles, gear_ratio, battery_cells, battery_capacity);
}

void VESC_TWAI_SetDuty(float duty_cycle) {
    if (!twai_driver_started) return;
    
    twai_message_t message;
    message.identifier = VESC_CAN_ID(vesc_config.vesc_id, VESC_CAN_PACKET_SET_DUTY);
    message.extd = 0;
    message.data_length_code = 4;
    
    float_to_bytes(duty_cycle, 100000, message.data);
    
    if (twai_transmit(&message, pdMS_TO_TICKS(100)) == ESP_OK) {
        Serial.printf("VESC TWAI: Set duty cycle to %.3f\n", duty_cycle);
    } else {
        Serial.println("VESC TWAI: Failed to send duty cycle command");
    }
}

void VESC_TWAI_SetCurrent(float current) {
    if (!twai_driver_started) return;
    
    twai_message_t message;
    message.identifier = VESC_CAN_ID(vesc_config.vesc_id, VESC_CAN_PACKET_SET_CURRENT);
    message.extd = 0;
    message.data_length_code = 4;
    
    float_to_bytes(current, 1000, message.data);
    
    if (twai_transmit(&message, pdMS_TO_TICKS(100)) == ESP_OK) {
        Serial.printf("VESC TWAI: Set current to %.2fA\n", current);
    } else {
        Serial.println("VESC TWAI: Failed to send current command");
    }
}

void VESC_TWAI_SetBrakeCurrent(float brake_current) {
    if (!twai_driver_started) return;
    
    twai_message_t message;
    message.identifier = VESC_CAN_ID(vesc_config.vesc_id, VESC_CAN_PACKET_SET_CURRENT_BRAKE);
    message.extd = 0;
    message.data_length_code = 4;
    
    float_to_bytes(brake_current, 1000, message.data);
    
    if (twai_transmit(&message, pdMS_TO_TICKS(100)) == ESP_OK) {
        Serial.printf("VESC TWAI: Set brake current to %.2fA\n", brake_current);
    } else {
        Serial.println("VESC TWAI: Failed to send brake current command");
    }
}

void VESC_TWAI_SetRPM(float rpm) {
    if (!twai_driver_started) return;
    
    twai_message_t message;
    message.identifier = VESC_CAN_ID(vesc_config.vesc_id, VESC_CAN_PACKET_SET_RPM);
    message.extd = 0;
    message.data_length_code = 4;
    
    float_to_bytes(rpm, 1, message.data);
    
    if (twai_transmit(&message, pdMS_TO_TICKS(100)) == ESP_OK) {
        Serial.printf("VESC TWAI: Set RPM to %.0f\n", rpm);
    } else {
        Serial.println("VESC TWAI: Failed to send RPM command");
    }
}

void VESC_TWAI_RequestStatus() {
    if (!twai_driver_started) return;
    
    // Request different status messages in sequence
    static uint8_t status_request_counter = 0;
    
    twai_message_t message;
    message.extd = 0;
    message.data_length_code = 0; // No data for status requests
    
    switch (status_request_counter % 5) {
        case 0:
            message.identifier = VESC_CAN_ID(vesc_config.vesc_id, VESC_CAN_PACKET_STATUS);
            break;
        case 1:
            message.identifier = VESC_CAN_ID(vesc_config.vesc_id, VESC_CAN_PACKET_STATUS_2);
            break;
        case 2:
            message.identifier = VESC_CAN_ID(vesc_config.vesc_id, VESC_CAN_PACKET_STATUS_3);
            break;
        case 3:
            message.identifier = VESC_CAN_ID(vesc_config.vesc_id, VESC_CAN_PACKET_STATUS_4);
            break;
        case 4:
            message.identifier = VESC_CAN_ID(vesc_config.vesc_id, VESC_CAN_PACKET_STATUS_5);
            break;
    }
    
    esp_err_t err = twai_transmit(&message, pdMS_TO_TICKS(10));
    if (err == ESP_OK) {
        // Status request sent successfully
    } else {
        Serial.println("VESC TWAI: Failed to send status request, err: " + String(err));
    }
    
    status_request_counter++;
}

void VESC_TWAI_Ping() {
    if (!twai_driver_started) return;
    
    twai_message_t message;
    message.identifier = VESC_CAN_ID(vesc_config.vesc_id, VESC_CAN_PACKET_PING);
    message.extd = 0;
    message.data_length_code = 0;
    
    if (twai_transmit(&message, pdMS_TO_TICKS(100)) == ESP_OK) {
        Serial.println("VESC TWAI: Ping sent");
    } else {
        Serial.println("VESC TWAI: Failed to send ping");
    }
}

void VESC_TWAI_PrintDebug() {
    Serial.println("=== VESC TWAI Debug Info ===");
    Serial.printf("Driver installed: %s\n", twai_driver_installed ? "Yes" : "No");
    Serial.printf("Driver started: %s\n", twai_driver_started ? "Yes" : "No");
    Serial.printf("Connected: %s\n", vesc_values.connected ? "Yes" : "No");
    Serial.printf("Last update: %lu ms ago\n", millis() - vesc_values.last_update);
    Serial.printf("VESC ID: %d\n", vesc_config.vesc_id);
    Serial.printf("CAN TX Pin: %d, RX Pin: %d\n", VESC_CAN_TX_GPIO_NUM, VESC_CAN_RX_GPIO_NUM);
    
    if (vesc_values.connected) {
        Serial.println("--- VESC Values ---");
        Serial.printf("RPM: %.0f\n", vesc_values.rpm);
        Serial.printf("Current: %.2f A\n", vesc_values.current);
        Serial.printf("Duty Cycle: %.3f\n", vesc_values.duty_cycle);
        Serial.printf("Speed: %.2f km/h\n", vesc_values.speed_kmh);
        Serial.printf("Battery: %.1f V (%.1f%%)\n", vesc_values.v_in, vesc_values.battery_percentage);
        Serial.printf("Power: %.1f W\n", vesc_values.power_watts);
        Serial.printf("Temperature FET: %.1f°C\n", vesc_values.temp_fet);
        Serial.printf("Temperature Motor: %.1f°C\n", vesc_values.temp_motor);
        Serial.printf("Distance: %.3f km\n", vesc_values.distance_km);
    }
    
    // Print TWAI status
    if (twai_driver_started) {
        twai_status_info_t status;
        if (twai_get_status_info(&status) == ESP_OK) {
            Serial.println("--- TWAI Status ---");
            Serial.printf("State: %d\n", status.state);
            Serial.printf("Messages to TX: %lu\n", status.msgs_to_tx);
            Serial.printf("Messages to RX: %lu\n", status.msgs_to_rx);
            Serial.printf("TX error count: %lu\n", status.tx_error_counter);
            Serial.printf("RX error count: %lu\n", status.rx_error_counter);
            Serial.printf("TX failed count: %lu\n", status.tx_failed_count);
            Serial.printf("RX missed count: %lu\n", status.rx_missed_count);
            Serial.printf("Bus error count: %lu\n", status.bus_error_count);
        }
    }
    
    Serial.println("========================");
}
