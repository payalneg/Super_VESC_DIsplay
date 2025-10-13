#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <Arduino.h>

// CAN Speed options (in kbps)
typedef enum {
    CAN_SPEED_125_KBPS = 125,
    CAN_SPEED_250_KBPS = 250,
    CAN_SPEED_500_KBPS = 500,
    CAN_SPEED_1000_KBPS = 1000
} can_speed_t;

// Device settings structure
typedef struct {
    uint8_t target_vesc_id;      // Target VESC ID for RT data queries
    can_speed_t can_speed;       // CAN bus speed in kbps
    uint8_t screen_brightness;   // Screen brightness 0-100
    uint8_t controller_id;       // This device's CAN ID
} device_settings_t;

// Settings API
void settings_init(void);
void settings_load(void);
void settings_save(void);
void settings_reset_to_defaults(void);

// Getters
uint8_t settings_get_target_vesc_id(void);
can_speed_t settings_get_can_speed(void);
uint8_t settings_get_screen_brightness(void);
uint8_t settings_get_controller_id(void);

// Setters (will save to NVS)
void settings_set_target_vesc_id(uint8_t id);
void settings_set_can_speed(can_speed_t speed);
void settings_set_screen_brightness(uint8_t brightness);
void settings_set_controller_id(uint8_t id);

// Apply settings to hardware
void settings_apply_brightness(void);
bool settings_apply_can_speed(void);  // Returns true if CAN restart needed

// Legacy compatibility
extern const uint8_t target_vesc_id;

#endif