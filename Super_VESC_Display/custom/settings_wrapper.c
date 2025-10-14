/*
	Copyright 2025 Super VESC Display
	
	Settings wrapper for LVGL GUI (works in simulator and on device)
*/

#include "settings_wrapper.h"
#include "lv_conf.h"

// Determine if we're running in simulator using LVGL's flag
#ifndef LV_REALDEVICE
    #define SIMULATOR_MODE 1
#else
    #define SIMULATOR_MODE 0
    // On device - include real settings
    #include "dev_settings.h"
#endif

// Simulator-mode storage
#if SIMULATOR_MODE
static struct {
    uint8_t target_vesc_id;
    uint8_t can_speed_index;
    uint8_t brightness;
    uint8_t controller_id;
} sim_settings = {
    .target_vesc_id = 10,
    .can_speed_index = 3,  // 1000 kbps
    .brightness = 80,
    .controller_id = 255
};
#endif

void settings_wrapper_init(void) {
#if !SIMULATOR_MODE
    settings_init();
#endif
}

uint8_t settings_wrapper_get_target_vesc_id(void) {
#if SIMULATOR_MODE
    return sim_settings.target_vesc_id;
#else
    return settings_get_target_vesc_id();
#endif
}

uint8_t settings_wrapper_get_can_speed_index(void) {
#if SIMULATOR_MODE
    return sim_settings.can_speed_index;
#else
    // Convert kbps to index
    can_speed_t speed = settings_get_can_speed();
    switch ((int)speed) {
        case 125: return 0;
        case 250: return 1;
        case 500: return 2;
        case 1000: return 3;
        default: return 1; // Default to 250
    }
#endif
}

uint8_t settings_wrapper_get_brightness(void) {
#if SIMULATOR_MODE
    return sim_settings.brightness;
#else
    return settings_get_screen_brightness();
#endif
}

uint8_t settings_wrapper_get_controller_id(void) {
#if SIMULATOR_MODE
    return sim_settings.controller_id;
#else
    return settings_get_controller_id();
#endif
}

void settings_wrapper_set_target_vesc_id(uint8_t id) {
#if SIMULATOR_MODE
    sim_settings.target_vesc_id = id;
#else
    settings_set_target_vesc_id(id);
#endif
}

void settings_wrapper_set_can_speed_index(uint8_t index) {
#if SIMULATOR_MODE
    sim_settings.can_speed_index = index;
#else
    // Convert index to speed
    can_speed_t speed;
    switch (index) {
        case 0: speed = CAN_SPEED_125_KBPS; break;
        case 1: speed = CAN_SPEED_250_KBPS; break;
        case 2: speed = CAN_SPEED_500_KBPS; break;
        case 3: speed = CAN_SPEED_1000_KBPS; break;
        default: speed = CAN_SPEED_250_KBPS; break;
    }
    settings_set_can_speed(speed);
#endif
}

void settings_wrapper_set_brightness(uint8_t brightness) {
#if SIMULATOR_MODE
    sim_settings.brightness = brightness;
#else
    settings_set_screen_brightness(brightness);
#endif
}

void settings_wrapper_set_controller_id(uint8_t id) {
#if SIMULATOR_MODE
    sim_settings.controller_id = id;
#else
    settings_set_controller_id(id);
#endif
}

int settings_wrapper_can_speed_to_kbps(uint8_t index) {
    switch (index) {
        case 0: return 125;
        case 1: return 250;
        case 2: return 500;
        case 3: return 1000;
        default: return 250;
    }
}

