#include "VESC_CAN_Driver.h"
#include <string.h>

// Global VESC CAN instance
VESC_CAN_Driver vesc_can;

VESC_CAN_Driver::VESC_CAN_Driver() {
    uart = nullptr;
    last_update = 0;
    connected = false;
    rx_buffer_pos = 0;
    packet_received = false;
    
    // Initialize values to zero
    memset(&values, 0, sizeof(values));
    
    // Default configuration from config file
    config.wheel_diameter = DEFAULT_WHEEL_DIAMETER;
    config.motor_poles = DEFAULT_MOTOR_POLES;
    config.gear_ratio = DEFAULT_GEAR_RATIO;
    config.battery_cells = DEFAULT_BATTERY_CELLS;
    config.battery_capacity = DEFAULT_BATTERY_CAPACITY;
}

void VESC_CAN_Driver::begin(HardwareSerial* serial_port) {
    uart = serial_port;
    // Initialize Serial0 with specific pins for CAN converter
    uart->begin(VESC_CAN_BAUD_RATE, SERIAL_8N1, VESC_CAN_RX_PIN, VESC_CAN_TX_PIN);
    uart->setTimeout(50);
    
    // Clear buffers
    uart->flush();
    while (uart->available()) {
        uart->read();
    }
    
    Serial.println("VESC CAN Converter (Serial0) initialized");
}

void VESC_CAN_Driver::set_config(float wheel_diameter, float motor_poles, float gear_ratio, 
                            float battery_cells, float battery_capacity) {
    config.wheel_diameter = wheel_diameter;
    config.motor_poles = motor_poles;
    config.gear_ratio = gear_ratio;
    config.battery_cells = battery_cells;
    config.battery_capacity = battery_capacity;
}

void VESC_CAN_Driver::request_values() {
    if (!uart) return;
    
    // Send PING command to request status updates
    uint8_t command = CAN_PACKET_PING;
    send_packet(&command, 1);
}

void VESC_CAN_Driver::update() {
    if (!uart) return;
    
    // Read incoming data
    while (uart->available()) {
        uint8_t byte = uart->read();
        
        if (rx_buffer_pos == 0 && byte != VESC_CAN_PACKET_START_BYTE) {
            continue; // Wait for start byte
        }
        
        rx_buffer[rx_buffer_pos++] = byte;
        
        if (rx_buffer_pos >= 3) {
            // Check if we have a complete packet
            uint16_t payload_length = (rx_buffer[1] << 8) | rx_buffer[2];
            uint16_t total_length = payload_length + 5; // start + length(2) + payload + crc(2) + end
            
            if (rx_buffer_pos >= total_length) {
                if (rx_buffer[total_length - 1] == VESC_CAN_PACKET_END_BYTE) {
                    // Valid packet received
                    uint8_t* payload = &rx_buffer[3];
                    process_packet(payload, payload_length);
                    if (!connected) {
                        Serial.println("VESC CAN connection established");
                    }
                    connected = true;
                    last_update = millis();
                }
                rx_buffer_pos = 0; // Reset buffer
            }
            
            if (rx_buffer_pos >= sizeof(rx_buffer)) {
                rx_buffer_pos = 0; // Prevent buffer overflow
            }
        }
    }
    
    // Check connection timeout
    if (millis() - last_update > CONNECTION_TIMEOUT_MS) {
        if (connected) {
            Serial.println("VESC CAN connection lost");
        }
        connected = false;
        
    }
    
    // Request values periodically
    static unsigned long last_request = 0;
    if (millis() - last_request > VESC_UPDATE_RATE_MS) {
        request_values();
        last_request = millis();
    }
}

bool VESC_CAN_Driver::is_connected() {
    return connected && (millis() - last_update < CONNECTION_TIMEOUT_MS);
}

vesc_can_values_t VESC_CAN_Driver::get_values() {
    return values;
}

float VESC_CAN_Driver::get_speed_kmh() {
    // Calculate speed from RPM
    // Speed = (RPM * wheel_circumference * 60) / (motor_poles * gear_ratio * 1000)
    float wheel_circumference = config.wheel_diameter * PI;
    float speed_ms = (abs(values.rpm) * wheel_circumference) / (config.motor_poles * config.gear_ratio * 60.0f);
    return speed_ms * 3.6f; // Convert m/s to km/h
}

float VESC_CAN_Driver::get_distance_km() {
    // Calculate distance from tachometer
    float wheel_circumference = config.wheel_diameter * PI;
    float distance_m = (abs(values.tachometer) * wheel_circumference) / config.motor_poles;
    return distance_m / 1000.0f; // Convert to km
}

float VESC_CAN_Driver::get_battery_percentage() {
    // Calculate battery percentage based on voltage
    float min_voltage = config.battery_cells * BATTERY_MIN_VOLTAGE_PER_CELL;
    float max_voltage = config.battery_cells * BATTERY_MAX_VOLTAGE_PER_CELL;
    float percentage = ((values.v_in - min_voltage) / (max_voltage - min_voltage)) * 100.0f;
    return constrain(percentage, 0.0f, 100.0f);
}

float VESC_CAN_Driver::get_power_watts() {
    return values.current_in * values.v_in;
}

int VESC_CAN_Driver::get_battery_bars() {
    float percentage = get_battery_percentage();
    return map(percentage, 0, 100, 0, 10);
}

void VESC_CAN_Driver::send_packet(uint8_t* payload, int length) {
    if (!uart) return;
    
    uint8_t packet[VESC_CAN_MAX_PAYLOAD_LENGTH + 10];
    int packet_pos = 0;
    
    // Start byte
    packet[packet_pos++] = VESC_CAN_PACKET_START_BYTE;
    
    // Length (2 bytes, big endian)
    packet[packet_pos++] = (length >> 8) & 0xFF;
    packet[packet_pos++] = length & 0xFF;
    
    // Payload
    for (int i = 0; i < length; i++) {
        packet[packet_pos++] = payload[i];
    }
    
    // CRC (2 bytes, big endian)
    unsigned short crc = crc16(payload, length);
    packet[packet_pos++] = (crc >> 8) & 0xFF;
    packet[packet_pos++] = crc & 0xFF;
    
    // End byte
    packet[packet_pos++] = VESC_CAN_PACKET_END_BYTE;
    
    // Send packet
    //Serial.println("Sending packet");
    //for (int i = 0; i < packet_pos; i++) {
    //    Serial.print(packet[i], HEX);
    //    Serial.print(" ");
    //}
    //Serial.println();
    uart->write(packet, packet_pos);
}

bool VESC_CAN_Driver::process_packet(uint8_t* payload, int length) {
    if (length < 1) return false;
    
    uint8_t command = payload[0];
    
    switch (command) {
        case CAN_PACKET_STATUS:  // Status Message 1 (9)
            if (length >= 9) { // 1 cmd + 8 data bytes
                parse_status_1(payload + 1, length - 1);
                return true;
            }
            break;
            
        case CAN_PACKET_STATUS_2:  // Status Message 2 (14)
            if (length >= 9) {
                parse_status_2(payload + 1, length - 1);
                return true;
            }
            break;
            
        case CAN_PACKET_STATUS_3:  // Status Message 3 (15)
            if (length >= 9) {
                parse_status_3(payload + 1, length - 1);
                return true;
            }
            break;
            
        case CAN_PACKET_STATUS_4:  // Status Message 4 (16)
            if (length >= 9) {
                parse_status_4(payload + 1, length - 1);
                return true;
            }
            break;
            
        case CAN_PACKET_STATUS_5:  // Status Message 5 (27)
            if (length >= 9) {
                parse_status_5(payload + 1, length - 1);
                return true;
            }
            break;
            
        case CAN_PACKET_PONG:  // Pong response (18)
            return true;
            
        default:
            break;
    }
    
    return false;
}

// Parse Status Message 1: RPM, Current, Duty Cycle
void VESC_CAN_Driver::parse_status_1(uint8_t* payload, int length) {
    if (length < 8) return;
    int32_t index = 0;
    
    values.rpm = (float)buffer_get_int32(payload, &index);  // RPM (32-bit)
    values.current_motor = (float)buffer_get_int16(payload, &index) / 10.0f;  // Current * 10 (16-bit)
    values.duty_now = (float)buffer_get_int16(payload, &index) / 1000.0f;  // Duty * 1000 (16-bit)
}

// Parse Status Message 2: Amp Hours
void VESC_CAN_Driver::parse_status_2(uint8_t* payload, int length) {
    if (length < 8) return;
    int32_t index = 0;
    
    values.amp_hours_charged = (float)buffer_get_int32(payload, &index) / 10000.0f;  // Ah charged * 10000
    values.amp_hours = (float)buffer_get_int32(payload, &index) / 10000.0f;  // Ah consumed * 10000
}

// Parse Status Message 3: Watt Hours  
void VESC_CAN_Driver::parse_status_3(uint8_t* payload, int length) {
    if (length < 8) return;
    int32_t index = 0;
    
    values.watt_hours_charged = (float)buffer_get_int32(payload, &index) / 10000.0f;  // Wh charged * 10000
    values.watt_hours = (float)buffer_get_int32(payload, &index) / 10000.0f;  // Wh consumed * 10000
}

// Parse Status Message 4: Temperatures, Input Current, PID Position
void VESC_CAN_Driver::parse_status_4(uint8_t* payload, int length) {
    if (length < 8) return;
    int32_t index = 0;
    
    values.temp_fet = (float)buffer_get_int16(payload, &index) / 10.0f;  // MOSFET temp * 10
    values.temp_motor = (float)buffer_get_int16(payload, &index) / 10.0f;  // Motor temp * 10
    values.current_in = (float)buffer_get_int16(payload, &index) / 10.0f;  // Input current * 10
    values.pid_pos_now = (float)buffer_get_int16(payload, &index) / 50.0f;  // PID position * 50
}

// Parse Status Message 5: Tachometer, Input Voltage
void VESC_CAN_Driver::parse_status_5(uint8_t* payload, int length) {
    if (length < 8) return;
    int32_t index = 0;
    
    // Skip reserved field (16-bit)
    buffer_get_int16(payload, &index);
    values.v_in = (float)buffer_get_int16(payload, &index) / 10.0f;  // Input voltage * 10
    values.tachometer = buffer_get_int32(payload, &index);  // Tachometer (32-bit)
    
    // Calculate absolute tachometer (simple approximation)
    values.tachometer_abs = abs(values.tachometer);
}

int16_t VESC_CAN_Driver::buffer_get_int16(const uint8_t* buffer, int32_t* index) {
    int16_t res = ((uint16_t) buffer[*index]) << 8 | ((uint16_t) buffer[*index + 1]);
    *index += 2;
    return res;
}

int32_t VESC_CAN_Driver::buffer_get_int32(const uint8_t* buffer, int32_t* index) {
    int32_t res = ((uint32_t) buffer[*index]) << 24 |
                  ((uint32_t) buffer[*index + 1]) << 16 |
                  ((uint32_t) buffer[*index + 2]) << 8 |
                  ((uint32_t) buffer[*index + 3]);
    *index += 4;
    return res;
}

float VESC_CAN_Driver::buffer_get_float32(const uint8_t* buffer, int32_t* index, float scale) {
    return (float)buffer_get_int32(buffer, index) / scale;
}

unsigned short VESC_CAN_Driver::crc16(unsigned char *buf, unsigned int len) {
    unsigned short cksum = 0;
    for (unsigned int i = 0; i < len; i++) {
        cksum = cksum ^ ((unsigned short)buf[i] << 8);
        for (unsigned char j = 0; j < 8; j++) {
            if (cksum & 0x8000) {
                cksum = (cksum << 1) ^ 0x1021;
            } else {
                cksum = cksum << 1;
            }
        }
    }
    return cksum;
}

void VESC_CAN_Driver::set_current(float current) {
    if (!uart) return;
    
    uint8_t payload[5];
    int32_t index = 0;
    
    payload[index++] = CAN_PACKET_SET_CURRENT;  // Use CAN command (1)
    int32_t current_send = (int32_t)(current * 1000.0f);  // Current in milliamps
    payload[index++] = (current_send >> 24) & 0xFF;
    payload[index++] = (current_send >> 16) & 0xFF;
    payload[index++] = (current_send >> 8) & 0xFF;
    payload[index++] = current_send & 0xFF;
    
    send_packet(payload, index);
}

void VESC_CAN_Driver::set_brake_current(float current) {
    if (!uart) return;
    
    uint8_t payload[5];
    int32_t index = 0;
    
    payload[index++] = CAN_PACKET_SET_CURRENT_BRAKE;  // Use CAN command (2)
    int32_t current_send = (int32_t)(current * 1000.0f);  // Current in milliamps
    payload[index++] = (current_send >> 24) & 0xFF;
    payload[index++] = (current_send >> 16) & 0xFF;
    payload[index++] = (current_send >> 8) & 0xFF;
    payload[index++] = current_send & 0xFF;
    
    send_packet(payload, index);
}

void VESC_CAN_Driver::set_rpm(float rpm) {
    if (!uart) return;
    
    uint8_t payload[5];
    int32_t index = 0;
    
    payload[index++] = CAN_PACKET_SET_RPM;  // Use CAN command (3)
    int32_t rpm_send = (int32_t)rpm;
    payload[index++] = (rpm_send >> 24) & 0xFF;
    payload[index++] = (rpm_send >> 16) & 0xFF;
    payload[index++] = (rpm_send >> 8) & 0xFF;
    payload[index++] = rpm_send & 0xFF;
    
    send_packet(payload, index);
}

void VESC_CAN_Driver::set_duty_cycle(float duty) {
    if (!uart) return;
    
    uint8_t payload[5];
    int32_t index = 0;
    
    payload[index++] = CAN_PACKET_SET_DUTY;  // Use CAN command (0)
    int32_t duty_send = (int32_t)(duty * 100000.0f);  // Duty cycle * 100000
    payload[index++] = (duty_send >> 24) & 0xFF;
    payload[index++] = (duty_send >> 16) & 0xFF;
    payload[index++] = (duty_send >> 8) & 0xFF;
    payload[index++] = duty_send & 0xFF;
    
    send_packet(payload, index);
}

// Global functions
void VESC_CAN_Init() {
    Serial.println("Initializing VESC CAN Converter...");
    vesc_can.begin(&Serial0);
    
    // Configure using values from config file
    vesc_can.set_config(
        DEFAULT_WHEEL_DIAMETER,
        DEFAULT_MOTOR_POLES,
        DEFAULT_GEAR_RATIO,
        DEFAULT_BATTERY_CELLS,
        DEFAULT_BATTERY_CAPACITY
    );
    
    Serial.println("VESC CAN Converter initialized with configuration:");
    Serial.printf("  Interface: Serial0 (CAN Converter)\n");
    Serial.printf("  TX Pin: %d, RX Pin: %d\n", VESC_CAN_TX_PIN, VESC_CAN_RX_PIN);
    Serial.printf("  Baud Rate: %d\n", VESC_CAN_BAUD_RATE);
    Serial.printf("  Wheel diameter: %.3f m\n", DEFAULT_WHEEL_DIAMETER);
    Serial.printf("  Motor poles: %.0f\n", DEFAULT_MOTOR_POLES);
    Serial.printf("  Gear ratio: %.1f\n", DEFAULT_GEAR_RATIO);
    Serial.printf("  Battery: %.0fS %.1fAh\n", DEFAULT_BATTERY_CELLS, DEFAULT_BATTERY_CAPACITY);
}

void VESC_CAN_Loop() {
    vesc_can.update();

}