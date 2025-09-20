#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>
#include "VESC_Config.h"

// VESC Communication Constants
#define VESC_UART_PORT Serial1
#define VESC_PACKET_START_BYTE 0x02
#define VESC_PACKET_END_BYTE 0x03
#define VESC_MAX_PAYLOAD_LENGTH 512

// VESC Commands
#define COMM_GET_VALUES 4
#define COMM_SET_CURRENT 6
#define COMM_SET_CURRENT_BRAKE 7
#define COMM_SET_RPM 8
#define COMM_SET_POS 9
#define COMM_SET_CURRENT_REL 10
#define COMM_SET_CURRENT_BRAKE_REL 11
#define COMM_SET_CURRENT_HANDBRAKE 12
#define COMM_SET_CURRENT_HANDBRAKE_REL 13

// VESC Data Structure
typedef struct {
    float temp_fet;           // FET Temperature (°C)
    float temp_motor;         // Motor Temperature (°C)
    float current_motor;      // Motor Current (A)
    float current_in;         // Input Current (A)
    float id;                 // Current d-axis (A)
    float iq;                 // Current q-axis (A)
    float duty_now;           // Duty Cycle (0-1)
    float rpm;                // RPM
    float v_in;               // Input Voltage (V)
    float amp_hours;          // Amp Hours consumed (Ah)
    float amp_hours_charged;  // Amp Hours charged (Ah)
    float watt_hours;         // Watt Hours consumed (Wh)
    float watt_hours_charged; // Watt Hours charged (Wh)
    int32_t tachometer;       // Tachometer (total rotations)
    int32_t tachometer_abs;   // Absolute tachometer
    uint8_t fault_code;       // Fault code
    float pid_pos_now;        // PID position
    uint8_t vesc_id;          // VESC ID
    float temp_mos1;          // MOSFET 1 temperature
    float temp_mos2;          // MOSFET 2 temperature
    float temp_mos3;          // MOSFET 3 temperature
    float vd;                 // Voltage d-axis
    float vq;                 // Voltage q-axis
} vesc_values_t;

// VESC Configuration
typedef struct {
    float wheel_diameter;     // Wheel diameter in meters
    float motor_poles;        // Number of motor poles
    float gear_ratio;         // Gear ratio
    float battery_cells;      // Number of battery cells
    float battery_capacity;   // Battery capacity in Ah
} vesc_config_t;

// VESC Driver Class
class VESC_Driver {
private:
    HardwareSerial* uart;
    vesc_values_t values;
    vesc_config_t config;
    unsigned long last_update;
    bool connected;
    
    // Packet handling
    uint8_t rx_buffer[VESC_MAX_PAYLOAD_LENGTH + 10];
    uint16_t rx_buffer_pos;
    bool packet_received;
    
    // CRC calculation
    unsigned short crc16(unsigned char *buf, unsigned int len);
    
    // Packet functions
    void send_packet(uint8_t* payload, int length);
    bool process_packet(uint8_t* payload, int length);
    void parse_values(uint8_t* payload, int length);
    
    // Data conversion helpers
    int16_t buffer_get_int16(const uint8_t* buffer, int32_t* index);
    int32_t buffer_get_int32(const uint8_t* buffer, int32_t* index);
    float buffer_get_float32(const uint8_t* buffer, int32_t* index, float scale);
    
public:
    VESC_Driver();
    
    // Initialization
    void begin(HardwareSerial* serial_port = &VESC_UART_PORT);
    void set_config(float wheel_diameter, float motor_poles, float gear_ratio, 
                   float battery_cells, float battery_capacity);
    
    // Communication
    void request_values();
    void update();
    bool is_connected();
    
    // Data access
    vesc_values_t get_values();
    
    // Calculated values
    float get_speed_kmh();
    float get_distance_km();
    float get_battery_percentage();
    float get_power_watts();
    int get_battery_bars();
    
    // Control functions
    void set_current(float current);
    void set_brake_current(float current);
    void set_rpm(float rpm);
    void set_duty_cycle(float duty);
};

// Global VESC instance
extern VESC_Driver vesc;

// VESC initialization function
void VESC_Init();
void VESC_Loop();
