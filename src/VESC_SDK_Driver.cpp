/*****************************************************************************
 * | File         :   VESC_SDK_Driver.cpp
 * | Author       :   VESC Display Team
 * | Function     :   VESC CAN driver using professional VESC CAN SDK
 * | Info         :   Integration with waas-rent/vesc_can_sdk library
 * ----------------
 * | This version :   V2.0
 * | Date         :   2024-12-25
 * | Info         :   Professional VESC CAN SDK integration
 *
 ******************************************************************************/

#include "VESC_SDK_Driver.h"
#include <string.h>

// Global variables
static bool twai_driver_installed = false;
static bool twai_driver_started = false;
static vesc_sdk_values_t vesc_values;
static vesc_sdk_config_t vesc_config;
static unsigned long last_values_request = 0;
static unsigned long last_ping_time = 0;

// CAN send function for VESC SDK
static bool can_send_function(uint32_t can_id, uint8_t *data, uint8_t len) {
    if (!twai_driver_started) {
        return false;
    }
    
    twai_message_t message;
    message.identifier = can_id;
    message.extd = 0;  // Standard frame
    message.rtr = 0;   // Data frame
    message.data_length_code = len;
    
    // Copy data
    for (int i = 0; i < len && i < 8; i++) {
        message.data[i] = data[i];
    }
    
    esp_err_t result = twai_transmit(&message, pdMS_TO_TICKS(100));
    return (result == ESP_OK);
}

// Response callback for VESC SDK
static void response_callback(uint8_t controller_id, uint8_t command, uint8_t *data, uint8_t len) {
    vesc_values.connected = true;
    vesc_values.last_update = millis();
    
    Serial.printf("VESC SDK: Received response from ID %d, command %d, len %d\n", 
                  controller_id, command, len);
    
    switch (command) {
        case COMM_GET_VALUES:
            if (vesc_parse_get_values(data, len, &vesc_values.vesc_values)) {
                Serial.println("VESC SDK: Parsed GET_VALUES successfully");
                
                // Calculate derived values
                float wheel_circumference = vesc_config.wheel_diameter * 3.14159f;
                float motor_rpm = vesc_values.vesc_values.rpm / vesc_config.gear_ratio;
                float wheel_rpm = motor_rpm / (vesc_config.motor_poles / 2.0f);
                vesc_values.speed_kmh = (wheel_rpm * wheel_circumference * 60.0f) / 1000.0f;
                
                // Calculate power
                vesc_values.power_watts = vesc_values.vesc_values.v_in * vesc_values.vesc_values.current_in;
                
                // Calculate battery percentage
                float cell_voltage = vesc_values.vesc_values.v_in / vesc_config.battery_cells;
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
            break;
            
        case CAN_PACKET_STATUS:
            if (vesc_parse_status_msg_1(data, len, &vesc_values.status_msg_1)) {
                Serial.println("VESC SDK: Parsed STATUS_MSG_1 successfully");
            }
            break;
            
        case CAN_PACKET_STATUS_2:
            if (vesc_parse_status_msg_2(data, len, &vesc_values.status_msg_2)) {
                Serial.println("VESC SDK: Parsed STATUS_MSG_2 successfully");
            }
            break;
            
        case CAN_PACKET_STATUS_3:
            if (vesc_parse_status_msg_3(data, len, &vesc_values.status_msg_3)) {
                Serial.println("VESC SDK: Parsed STATUS_MSG_3 successfully");
            }
            break;
            
        case CAN_PACKET_STATUS_4:
            if (vesc_parse_status_msg_4(data, len, &vesc_values.status_msg_4)) {
                Serial.println("VESC SDK: Parsed STATUS_MSG_4 successfully");
            }
            break;
            
        case CAN_PACKET_STATUS_5:
            if (vesc_parse_status_msg_5(data, len, &vesc_values.status_msg_5)) {
                Serial.println("VESC SDK: Parsed STATUS_MSG_5 successfully");
            }
            break;
            
        case CAN_PACKET_STATUS_6:
            if (vesc_parse_status_msg_6(data, len, &vesc_values.status_msg_6)) {
                Serial.println("VESC SDK: Parsed STATUS_MSG_6 successfully");
            }
            break;
            
        case CAN_PACKET_PONG:
            Serial.println("VESC SDK: Received PONG");
            break;
            
        default:
            Serial.printf("VESC SDK: Unknown command: %d\n", command);
            break;
    }
}

bool VESC_SDK_Init(uint8_t vesc_id) {
    Serial.println("Initializing VESC SDK Driver...");
    
    // Initialize values structure
    memset(&vesc_values, 0, sizeof(vesc_values));
    vesc_values.connected = false;
    vesc_values.last_update = 0;
    
    // Initialize configuration with defaults (minimal setup)
    vesc_config.wheel_diameter = 1.0f;    // Default value (not used for main functionality)
    vesc_config.motor_poles = 1;          // Default value (not used for main functionality)
    vesc_config.gear_ratio = 1.0f;        // Default value (not used for main functionality)
    vesc_config.battery_cells = 1;        // Default value (not used for main functionality)
    vesc_config.battery_capacity = 1.0f;  // Default value (not used for main functionality)
    vesc_config.vesc_id = vesc_id;
    
    // TWAI configuration settings
    static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS(); // 250 kbps
    static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL(); // Accept all messages
    static const twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(VESC_CAN_TX_GPIO_NUM, VESC_CAN_RX_GPIO_NUM, TWAI_MODE_NORMAL);
    
    // Install TWAI driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        twai_driver_installed = true;
        Serial.println("VESC SDK: TWAI driver installed successfully");
    } else {
        Serial.println("VESC SDK: Failed to install TWAI driver");
        return false;
    }

    // Start TWAI driver
    if (twai_start() == ESP_OK) {
        twai_driver_started = true;
        Serial.println("VESC SDK: TWAI driver started successfully");
    } else {
        Serial.println("VESC SDK: Failed to start TWAI driver");
        return false;
    }

    // Configure alerts for CAN events
    uint32_t alerts_to_enable = TWAI_ALERT_TX_SUCCESS | TWAI_ALERT_TX_FAILED |
                                TWAI_ALERT_RX_DATA | TWAI_ALERT_RX_QUEUE_FULL |
                                TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_ERROR;
    if (twai_reconfigure_alerts(alerts_to_enable, NULL) == ESP_OK) {
        Serial.println("VESC SDK: TWAI alerts configured successfully");
    } else {
        Serial.println("VESC SDK: Failed to configure TWAI alerts");
        return false;
    }

    // Initialize VESC CAN SDK
    if (vesc_can_init(can_send_function, vesc_id, vesc_id)) {
        Serial.println("VESC SDK: CAN SDK initialized successfully");
    } else {
        Serial.println("VESC SDK: Failed to initialize CAN SDK");
        return false;
    }
    
    // Set response callback
    vesc_set_response_callback(response_callback);

    Serial.printf("VESC SDK initialized for VESC ID: %d\n", vesc_id);
    Serial.printf("CAN TX Pin: %d, CAN RX Pin: %d\n", VESC_CAN_TX_GPIO_NUM, VESC_CAN_RX_GPIO_NUM);
    Serial.println("CAN Speed: 250 kbps");
    
    return true;
}

void VESC_SDK_Loop() {
    if (!twai_driver_installed || !twai_driver_started) {
        return;
    }
    
    // Read CAN alerts
    uint32_t alerts_triggered;
    twai_read_alerts(&alerts_triggered, pdMS_TO_TICKS(10));
    
    // Handle alerts
    if (alerts_triggered & TWAI_ALERT_ERR_PASS) {
        Serial.println("VESC SDK: Controller in error passive state");
    }
    
    if (alerts_triggered & TWAI_ALERT_BUS_ERROR) {
        Serial.println("VESC SDK: Bus error occurred");
        twai_status_info_t status;
        twai_get_status_info(&status);
        Serial.printf("Bus error count: %lu\n", status.bus_error_count);
    }
    
    if (alerts_triggered & TWAI_ALERT_TX_FAILED) {
        Serial.println("VESC SDK: Transmission failed");
    }
    
    // Process received messages
    if (alerts_triggered & TWAI_ALERT_RX_DATA) {
        twai_message_t message;
        while (twai_receive(&message, 0) == ESP_OK) {
            // Process message with VESC SDK
            vesc_process_can_frame(message.identifier, message.data, message.data_length_code);
        }
    }
    
    // Check connection timeout
    if (vesc_values.connected && (millis() - vesc_values.last_update > VESC_CAN_TIMEOUT_MS)) {
        vesc_values.connected = false;
        Serial.println("VESC SDK: Connection timeout");
    }
    
    // Request values periodically
    if (millis() - last_values_request > VESC_CAN_UPDATE_RATE_MS) {
        VESC_SDK_RequestValues();
        last_values_request = millis();
    }
    
    // Send ping periodically
    if (millis() - last_ping_time > 5000) { // Ping every 5 seconds
        VESC_SDK_Ping();
        last_ping_time = millis();
    }
}

bool VESC_SDK_IsConnected() {
    return vesc_values.connected && (millis() - vesc_values.last_update < VESC_CAN_TIMEOUT_MS);
}

vesc_sdk_values_t VESC_SDK_GetValues() {
    return vesc_values;
}

void VESC_SDK_SetConfig(float wheel_diameter, float motor_poles, float gear_ratio, 
                        float battery_cells, float battery_capacity) {
    vesc_config.wheel_diameter = wheel_diameter;
    vesc_config.motor_poles = motor_poles;
    vesc_config.gear_ratio = gear_ratio;
    vesc_config.battery_cells = battery_cells;
    vesc_config.battery_capacity = battery_capacity;
    
    Serial.printf("VESC SDK Config updated: Wheel=%.3fm, Poles=%.0f, Ratio=%.2f, Cells=%.0f, Capacity=%.1fAh\n",
                  wheel_diameter, motor_poles, gear_ratio, battery_cells, battery_capacity);
}

void VESC_SDK_SetDuty(float duty_cycle) {
    if (!twai_driver_started) return;
    
    vesc_set_duty(vesc_config.vesc_id, duty_cycle);
    Serial.printf("VESC SDK: Set duty cycle to %.3f\n", duty_cycle);
}

void VESC_SDK_SetCurrent(float current) {
    if (!twai_driver_started) return;
    
    vesc_set_current(vesc_config.vesc_id, current);
    Serial.printf("VESC SDK: Set current to %.2fA\n", current);
}

void VESC_SDK_SetBrakeCurrent(float brake_current) {
    if (!twai_driver_started) return;
    
    vesc_set_current_brake(vesc_config.vesc_id, brake_current);
    Serial.printf("VESC SDK: Set brake current to %.2fA\n", brake_current);
}

void VESC_SDK_SetRPM(float rpm) {
    if (!twai_driver_started) return;
    
    vesc_set_rpm(vesc_config.vesc_id, rpm);
    Serial.printf("VESC SDK: Set RPM to %.0f\n", rpm);
}

void VESC_SDK_RequestValues() {
    if (!twai_driver_started) return;
    
    vesc_get_values(vesc_config.vesc_id);
    // Values request sent successfully
}

void VESC_SDK_Ping() {
    if (!twai_driver_started) return;
    
    vesc_ping(vesc_config.vesc_id);
    Serial.println("VESC SDK: Ping sent");
}

void VESC_SDK_PrintDebug() {
    Serial.println("=== VESC SDK Debug Info ===");
    Serial.printf("Driver installed: %s\n", twai_driver_installed ? "Yes" : "No");
    Serial.printf("Driver started: %s\n", twai_driver_started ? "Yes" : "No");
    Serial.printf("Connected: %s\n", vesc_values.connected ? "Yes" : "No");
    Serial.printf("Last update: %lu ms ago\n", millis() - vesc_values.last_update);
    Serial.printf("VESC ID: %d\n", vesc_config.vesc_id);
    Serial.printf("CAN TX Pin: %d, RX Pin: %d\n", VESC_CAN_TX_GPIO_NUM, VESC_CAN_RX_GPIO_NUM);
    
    if (vesc_values.connected) {
        Serial.println("--- VESC Values (from SDK) ---");
        Serial.printf("RPM: %.0f\n", vesc_values.vesc_values.rpm);
        Serial.printf("Current: %.2f A\n", vesc_values.vesc_values.current_motor);
        Serial.printf("Input Current: %.2f A\n", vesc_values.vesc_values.current_in);
        Serial.printf("Duty Cycle: %.3f\n", vesc_values.vesc_values.duty_cycle);
        Serial.printf("Speed: %.2f km/h\n", vesc_values.speed_kmh);
        Serial.printf("Battery: %.1f V (%.1f%%)\n", vesc_values.vesc_values.v_in, vesc_values.battery_percentage);
        Serial.printf("Power: %.1f W\n", vesc_values.power_watts);
        Serial.printf("Temperature FET: %.1f°C\n", vesc_values.vesc_values.temp_mos1);
        Serial.printf("Temperature Motor: %.1f°C\n", vesc_values.vesc_values.temp_motor);
        Serial.printf("Distance: %.3f km\n", vesc_values.distance_km);
        Serial.printf("Fault Code: %d\n", vesc_values.vesc_values.fault_code);
    }
    
    // Print status messages
    if (vesc_values.status_msg_1.valid) {
        Serial.println("--- Status Message 1 ---");
        Serial.printf("RPM: %.0f, Current: %.2fA, Duty: %.3f\n", 
                      vesc_values.status_msg_1.rpm, 
                      vesc_values.status_msg_1.current, 
                      vesc_values.status_msg_1.duty);
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

// Simple interface functions
float VESC_SDK_GetSpeed() {
    return VESC_SDK_IsConnected() ? vesc_values.speed_kmh : 0.0f;
}

float VESC_SDK_GetBatteryVoltage() {
    return VESC_SDK_IsConnected() ? vesc_values.vesc_values.v_in : 0.0f;
}

float VESC_SDK_GetBatteryPercentage() {
    return VESC_SDK_IsConnected() ? vesc_values.battery_percentage : 0.0f;
}

float VESC_SDK_GetMotorCurrent() {
    return VESC_SDK_IsConnected() ? vesc_values.vesc_values.current_motor : 0.0f;
}

float VESC_SDK_GetPower() {
    return VESC_SDK_IsConnected() ? vesc_values.power_watts : 0.0f;
}

float VESC_SDK_GetTempFET() {
    return VESC_SDK_IsConnected() ? vesc_values.vesc_values.temp_mos1 : 0.0f;
}

float VESC_SDK_GetTempMotor() {
    return VESC_SDK_IsConnected() ? vesc_values.vesc_values.temp_motor : 0.0f;
}

void VESC_SDK_EmergencyStop() {
    if (VESC_SDK_IsConnected()) {
        VESC_SDK_SetDuty(0.0f);
        VESC_SDK_SetCurrent(0.0f);
        Serial.println("VESC SDK: EMERGENCY STOP executed!");
    }
}
