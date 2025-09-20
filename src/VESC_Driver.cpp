#include "VESC_Driver.h"
#include <string.h>

// Global VESC instance
VESC_Driver vesc;

VESC_Driver::VESC_Driver() {
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

void VESC_Driver::begin(HardwareSerial* serial_port) {
    uart = serial_port;
    //uart->begin(VESC_BAUD_RATE, SERIAL_8N1, VESC_UART_RX_PIN, VESC_UART_TX_PIN);
    uart->begin(VESC_BAUD_RATE);
    uart->setTimeout(50);
    
    // Clear buffers
    uart->flush();
    while (uart->available()) {
        uart->read();
    }
    
    Serial.println("VESC UART initialized");
}

void VESC_Driver::set_config(float wheel_diameter, float motor_poles, float gear_ratio, 
                            float battery_cells, float battery_capacity) {
    config.wheel_diameter = wheel_diameter;
    config.motor_poles = motor_poles;
    config.gear_ratio = gear_ratio;
    config.battery_cells = battery_cells;
    config.battery_capacity = battery_capacity;
}

void VESC_Driver::request_values() {
    if (!uart) return;
    
    uint8_t command = COMM_GET_VALUES;
    send_packet(&command, 1);
}

void VESC_Driver::update() {
    if (!uart) return;
    
    // Read incoming data
    while (uart->available()) {
        uint8_t byte = uart->read();
        
        if (rx_buffer_pos == 0 && byte != VESC_PACKET_START_BYTE) {
            continue; // Wait for start byte
        }
        
        rx_buffer[rx_buffer_pos++] = byte;
        
        if (rx_buffer_pos >= 3) {
            // Check if we have a complete packet
            uint16_t payload_length = (rx_buffer[1] << 8) | rx_buffer[2];
            uint16_t total_length = payload_length + 5; // start + length(2) + payload + crc(2) + end
            
            if (rx_buffer_pos >= total_length) {
                if (rx_buffer[total_length - 1] == VESC_PACKET_END_BYTE) {
                    // Valid packet received
                    uint8_t* payload = &rx_buffer[3];
                    process_packet(payload, payload_length);
                    if (!connected) {
                        Serial.println("VESC connection established");
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
            Serial.println("VESC connection lost");
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

bool VESC_Driver::is_connected() {
    return connected && (millis() - last_update < CONNECTION_TIMEOUT_MS);
}

vesc_values_t VESC_Driver::get_values() {
    return values;
}

float VESC_Driver::get_speed_kmh() {
    // Calculate speed from RPM
    // Speed = (RPM * wheel_circumference * 60) / (motor_poles * gear_ratio * 1000)
    float wheel_circumference = config.wheel_diameter * PI;
    float speed_ms = (abs(values.rpm) * wheel_circumference) / (config.motor_poles * config.gear_ratio * 60.0f);
    return speed_ms * 3.6f; // Convert m/s to km/h
}

float VESC_Driver::get_distance_km() {
    // Calculate distance from tachometer
    float wheel_circumference = config.wheel_diameter * PI;
    float distance_m = (abs(values.tachometer) * wheel_circumference) / config.motor_poles;
    return distance_m / 1000.0f; // Convert to km
}

float VESC_Driver::get_battery_percentage() {
    // Calculate battery percentage based on voltage
    float min_voltage = config.battery_cells * BATTERY_MIN_VOLTAGE_PER_CELL;
    float max_voltage = config.battery_cells * BATTERY_MAX_VOLTAGE_PER_CELL;
    float percentage = ((values.v_in - min_voltage) / (max_voltage - min_voltage)) * 100.0f;
    return constrain(percentage, 0.0f, 100.0f);
}

float VESC_Driver::get_power_watts() {
    return values.current_in * values.v_in;
}

int VESC_Driver::get_battery_bars() {
    float percentage = get_battery_percentage();
    return map(percentage, 0, 100, 0, 10);
}

void VESC_Driver::send_packet(uint8_t* payload, int length) {
    if (!uart) return;
    
    uint8_t packet[VESC_MAX_PAYLOAD_LENGTH + 10];
    int packet_pos = 0;
    
    // Start byte
    packet[packet_pos++] = VESC_PACKET_START_BYTE;
    
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
    packet[packet_pos++] = VESC_PACKET_END_BYTE;
    
    // Send packet
    uart->write(packet, packet_pos);
}

bool VESC_Driver::process_packet(uint8_t* payload, int length) {
    if (length < 1) return false;
    
    uint8_t command = payload[0];
    
    switch (command) {
        case COMM_GET_VALUES:
            if (length >= 61) { // Minimum expected length for values packet
                parse_values(payload + 1, length - 1);
                return true;
            }
            break;
        default:
            break;
    }
    
    return false;
}

void VESC_Driver::parse_values(uint8_t* payload, int length) {
    int32_t index = 0;
    
    if (length < 60) return; // Not enough data
    
    values.temp_fet = buffer_get_float32(payload, &index, 10.0f);
    values.temp_motor = buffer_get_float32(payload, &index, 10.0f);
    values.current_motor = buffer_get_float32(payload, &index, 100.0f);
    values.current_in = buffer_get_float32(payload, &index, 100.0f);
    values.id = buffer_get_float32(payload, &index, 100.0f);
    values.iq = buffer_get_float32(payload, &index, 100.0f);
    values.duty_now = buffer_get_float32(payload, &index, 1000.0f);
    values.rpm = buffer_get_float32(payload, &index, 1.0f);
    values.v_in = buffer_get_float32(payload, &index, 10.0f);
    values.amp_hours = buffer_get_float32(payload, &index, 10000.0f);
    values.amp_hours_charged = buffer_get_float32(payload, &index, 10000.0f);
    values.watt_hours = buffer_get_float32(payload, &index, 10000.0f);
    values.watt_hours_charged = buffer_get_float32(payload, &index, 10000.0f);
    values.tachometer = buffer_get_int32(payload, &index);
    values.tachometer_abs = buffer_get_int32(payload, &index);
    values.fault_code = payload[index++];
    values.pid_pos_now = buffer_get_float32(payload, &index, 1000000.0f);
    values.vesc_id = payload[index++];
}

int16_t VESC_Driver::buffer_get_int16(const uint8_t* buffer, int32_t* index) {
    int16_t res = ((uint16_t) buffer[*index]) << 8 | ((uint16_t) buffer[*index + 1]);
    *index += 2;
    return res;
}

int32_t VESC_Driver::buffer_get_int32(const uint8_t* buffer, int32_t* index) {
    int32_t res = ((uint32_t) buffer[*index]) << 24 |
                  ((uint32_t) buffer[*index + 1]) << 16 |
                  ((uint32_t) buffer[*index + 2]) << 8 |
                  ((uint32_t) buffer[*index + 3]);
    *index += 4;
    return res;
}

float VESC_Driver::buffer_get_float32(const uint8_t* buffer, int32_t* index, float scale) {
    return (float)buffer_get_int32(buffer, index) / scale;
}

unsigned short VESC_Driver::crc16(unsigned char *buf, unsigned int len) {
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

void VESC_Driver::set_current(float current) {
    if (!uart) return;
    
    uint8_t payload[5];
    int32_t index = 0;
    
    payload[index++] = COMM_SET_CURRENT;
    int32_t current_send = (int32_t)(current * 1000.0f);
    payload[index++] = (current_send >> 24) & 0xFF;
    payload[index++] = (current_send >> 16) & 0xFF;
    payload[index++] = (current_send >> 8) & 0xFF;
    payload[index++] = current_send & 0xFF;
    
    send_packet(payload, index);
}

void VESC_Driver::set_brake_current(float current) {
    if (!uart) return;
    
    uint8_t payload[5];
    int32_t index = 0;
    
    payload[index++] = COMM_SET_CURRENT_BRAKE;
    int32_t current_send = (int32_t)(current * 1000.0f);
    payload[index++] = (current_send >> 24) & 0xFF;
    payload[index++] = (current_send >> 16) & 0xFF;
    payload[index++] = (current_send >> 8) & 0xFF;
    payload[index++] = current_send & 0xFF;
    
    send_packet(payload, index);
}

void VESC_Driver::set_rpm(float rpm) {
    if (!uart) return;
    
    uint8_t payload[5];
    int32_t index = 0;
    
    payload[index++] = COMM_SET_RPM;
    int32_t rpm_send = (int32_t)rpm;
    payload[index++] = (rpm_send >> 24) & 0xFF;
    payload[index++] = (rpm_send >> 16) & 0xFF;
    payload[index++] = (rpm_send >> 8) & 0xFF;
    payload[index++] = rpm_send & 0xFF;
    
    send_packet(payload, index);
}

void VESC_Driver::set_duty_cycle(float duty) {
    if (!uart) return;
    
    uint8_t payload[5];
    int32_t index = 0;
    
    payload[index++] = COMM_SET_CURRENT; // Using current mode for safety
    int32_t duty_send = (int32_t)(duty * 100000.0f);
    payload[index++] = (duty_send >> 24) & 0xFF;
    payload[index++] = (duty_send >> 16) & 0xFF;
    payload[index++] = (duty_send >> 8) & 0xFF;
    payload[index++] = duty_send & 0xFF;
    
    send_packet(payload, index);
}

// Global functions
void VESC_Init() {
    Serial.println("Initializing VESC...");
    vesc.begin(&Serial1);
    
    // Configure using values from config file
    vesc.set_config(
        DEFAULT_WHEEL_DIAMETER,
        DEFAULT_MOTOR_POLES,
        DEFAULT_GEAR_RATIO,
        DEFAULT_BATTERY_CELLS,
        DEFAULT_BATTERY_CAPACITY
    );
    
    Serial.println("VESC initialized with configuration:");
    Serial.printf("  Wheel diameter: %.3f m\n", DEFAULT_WHEEL_DIAMETER);
    Serial.printf("  Motor poles: %.0f\n", DEFAULT_MOTOR_POLES);
    Serial.printf("  Gear ratio: %.1f\n", DEFAULT_GEAR_RATIO);
    Serial.printf("  Battery: %.0fS %.1fAh\n", DEFAULT_BATTERY_CELLS, DEFAULT_BATTERY_CAPACITY);
}

void VESC_Loop() {
    //vesc.request_values();
    vesc.update();
}
