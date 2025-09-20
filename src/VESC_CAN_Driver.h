#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>
#include "VESC_Config.h"

// VESC CAN Converter Configuration (via Serial0)
#define VESC_CAN_UART_PORT Serial0
#define VESC_CAN_TX_PIN 43      // ESP32-S3 TX pin for CAN converter
#define VESC_CAN_RX_PIN 44      // ESP32-S3 RX pin for CAN converter  
#define VESC_CAN_BAUD_RATE 250000 // Baud rate for CAN converter
#define VESC_CAN_PACKET_START_BYTE 0x02
#define VESC_CAN_PACKET_END_BYTE 0x03
#define VESC_CAN_MAX_PAYLOAD_LENGTH 512

// VESC CAN Commands (from official VESC CAN documentation)
#define CAN_PACKET_SET_DUTY 0
#define CAN_PACKET_SET_CURRENT 1
#define CAN_PACKET_SET_CURRENT_BRAKE 2
#define CAN_PACKET_SET_RPM 3
#define CAN_PACKET_SET_POS 4
#define CAN_PACKET_FILL_RX_BUFFER 5
#define CAN_PACKET_FILL_RX_BUFFER_LONG 6
#define CAN_PACKET_PROCESS_RX_BUFFER 7
#define CAN_PACKET_PROCESS_SHORT_BUFFER 8
#define CAN_PACKET_STATUS 9
#define CAN_PACKET_SET_CURRENT_REL 10
#define CAN_PACKET_SET_CURRENT_BRAKE_REL 11
#define CAN_PACKET_SET_CURRENT_HANDBRAKE 12
#define CAN_PACKET_SET_CURRENT_HANDBRAKE_REL 13
#define CAN_PACKET_STATUS_2 14
#define CAN_PACKET_STATUS_3 15
#define CAN_PACKET_STATUS_4 16
#define CAN_PACKET_PING 17
#define CAN_PACKET_PONG 18
#define CAN_PACKET_STATUS_5 27

// VESC CAN Data Structure (based on official CAN telemetry format)
typedef struct {
    // Status Message 1 - CAN_PACKET_STATUS (9)
    float rpm;                // RPM (32-bit)
    float current_motor;      // Total Current * 10 (16-bit)
    float duty_now;           // Duty Cycle * 1000 (16-bit)
    
    // Status Message 2 - CAN_PACKET_STATUS_2 (14)
    float amp_hours;          // Amp Hours * 10000 (32-bit)
    float amp_hours_charged;  // Amp Hours Charged * 10000 (32-bit)
    
    // Status Message 3 - CAN_PACKET_STATUS_3 (15)
    float watt_hours;         // Watt Hours * 10000 (32-bit)
    float watt_hours_charged; // Watt Hours Charged * 10000 (32-bit)
    
    // Status Message 4 - CAN_PACKET_STATUS_4 (16)
    float temp_fet;           // MOSFET Temperature * 10 (16-bit)
    float temp_motor;         // Motor Temperature * 10 (16-bit)
    float current_in;         // Input Current * 10 (16-bit)
    float pid_pos_now;        // PID Position * 50 (16-bit)
    
    // Status Message 5 - CAN_PACKET_STATUS_5 (27)
    int32_t tachometer;       // Tachometer value (32-bit)
    float v_in;               // Input Voltage * 10 (16-bit)
    
    // Additional fields for compatibility
    int32_t tachometer_abs;   // Absolute tachometer (calculated)
    uint8_t fault_code;       // Fault code
    uint8_t vesc_id;          // VESC ID (from CAN ID bits 0-7)
    
    // Connection status
    unsigned long last_update;
    bool connected;
} vesc_can_values_t;

// VESC Configuration (same as original)
typedef struct {
    float wheel_diameter;     // Wheel diameter in meters
    float motor_poles;        // Number of motor poles
    float gear_ratio;         // Gear ratio
    float battery_cells;      // Number of battery cells
    float battery_capacity;   // Battery capacity in Ah
} vesc_can_config_t;

// VESC CAN Driver Class (based on original UART driver)
class VESC_CAN_Driver {
private:
    HardwareSerial* uart;
    vesc_can_values_t values;
    vesc_can_config_t config;
    unsigned long last_update;
    bool connected;
    
    // Packet handling (same as original)
    uint8_t rx_buffer[VESC_CAN_MAX_PAYLOAD_LENGTH + 10];
    uint16_t rx_buffer_pos;
    bool packet_received;
    
    // CRC calculation (same as original)
    unsigned short crc16(unsigned char *buf, unsigned int len);
    
    // Packet functions (updated for CAN protocol)
    void send_packet(uint8_t* payload, int length);
    bool process_packet(uint8_t* payload, int length);
    
    // CAN Status message parsers (based on official VESC CAN protocol)
    void parse_status_1(uint8_t* payload, int length);  // RPM, Current, Duty
    void parse_status_2(uint8_t* payload, int length);  // Amp Hours
    void parse_status_3(uint8_t* payload, int length);  // Watt Hours
    void parse_status_4(uint8_t* payload, int length);  // Temperatures, Input Current, PID
    void parse_status_5(uint8_t* payload, int length);  // Tachometer, Input Voltage
    
    // Data conversion helpers (same as original)
    int16_t buffer_get_int16(const uint8_t* buffer, int32_t* index);
    int32_t buffer_get_int32(const uint8_t* buffer, int32_t* index);
    float buffer_get_float32(const uint8_t* buffer, int32_t* index, float scale);
    
public:
    VESC_CAN_Driver();
    
    // Initialization
    void begin(HardwareSerial* serial_port = &VESC_CAN_UART_PORT);
    void set_config(float wheel_diameter, float motor_poles, float gear_ratio, 
                   float battery_cells, float battery_capacity);
    
    // Communication
    void request_values();
    void update();
    bool is_connected();
    
    // Data access
    vesc_can_values_t get_values();
    
    // Calculated values (same as original)
    float get_speed_kmh();
    float get_distance_km();
    float get_battery_percentage();
    float get_power_watts();
    int get_battery_bars();
    
    // Control functions (same as original)
    void set_current(float current);
    void set_brake_current(float current);
    void set_rpm(float rpm);
    void set_duty_cycle(float duty);
};

// Global VESC CAN instance
extern VESC_CAN_Driver vesc_can;

// VESC CAN initialization functions
void VESC_CAN_Init();
void VESC_CAN_Loop();
