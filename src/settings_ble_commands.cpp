/*
	Copyright 2025 Super VESC Display
	
	Settings management via BLE commands
	
	Usage examples (send as text via BLE):
	  SET_TARGET_ID:10      - Set target VESC ID to 10
	  SET_CAN_SPEED:500     - Set CAN speed to 500 kbps (125/250/500/1000)
	  SET_BRIGHTNESS:80     - Set screen brightness to 80%
	  SET_CONTROLLER_ID:2   - Set this device's CAN ID to 2
	  GET_SETTINGS          - Get current settings
	  RESET_SETTINGS        - Reset all settings to defaults
	  
	Note: CAN speed and Controller ID changes require device restart to take effect!
*/

#include "settings.h"
#include "debug_log.h"
#include <Arduino.h>
#include <string.h>

// Process BLE settings commands
// Returns true if command was handled, false otherwise
bool process_settings_command(const char* cmd, char* response, size_t response_max_len) {
    if (!cmd || !response) {
        return false;
    }
    
    // SET_TARGET_ID:10
    if (strncmp(cmd, "SET_TARGET_ID:", 14) == 0) {
        int id = atoi(cmd + 14);
        if (id > 0 && id <= 254) {
            settings_set_target_vesc_id((uint8_t)id);
            snprintf(response, response_max_len, "Target VESC ID set to %d", id);
        } else {
            snprintf(response, response_max_len, "Invalid ID (must be 1-254)");
        }
        return true;
    }
    
    // SET_CAN_SPEED:500
    if (strncmp(cmd, "SET_CAN_SPEED:", 14) == 0) {
        int speed = atoi(cmd + 14);
        can_speed_t can_speed;
        
        switch (speed) {
            case 125:
                can_speed = CAN_SPEED_125_KBPS;
                break;
            case 250:
                can_speed = CAN_SPEED_250_KBPS;
                break;
            case 500:
                can_speed = CAN_SPEED_500_KBPS;
                break;
            case 1000:
                can_speed = CAN_SPEED_1000_KBPS;
                break;
            default:
                snprintf(response, response_max_len, "Invalid speed (use 125/250/500/1000)");
                return true;
        }
        
        settings_set_can_speed(can_speed);
        snprintf(response, response_max_len, "CAN speed set to %d kbps (restart required)", speed);
        return true;
    }
    
    // SET_BRIGHTNESS:80
    if (strncmp(cmd, "SET_BRIGHTNESS:", 15) == 0) {
        int brightness = atoi(cmd + 15);
        if (brightness >= 0 && brightness <= 100) {
            settings_set_screen_brightness((uint8_t)brightness);
            snprintf(response, response_max_len, "Brightness set to %d%%", brightness);
        } else {
            snprintf(response, response_max_len, "Invalid brightness (must be 0-100)");
        }
        return true;
    }
    
    // SET_CONTROLLER_ID:2
    if (strncmp(cmd, "SET_CONTROLLER_ID:", 18) == 0) {
        int id = atoi(cmd + 18);
        if (id > 0 && id <= 254) {
            settings_set_controller_id((uint8_t)id);
            snprintf(response, response_max_len, "Controller ID set to %d (restart required)", id);
        } else {
            snprintf(response, response_max_len, "Invalid ID (must be 1-254)");
        }
        return true;
    }
    
    // GET_SETTINGS
    if (strcmp(cmd, "GET_SETTINGS") == 0) {
        snprintf(response, response_max_len, 
                 "Settings:\n"
                 "Target VESC ID: %d\n"
                 "CAN Speed: %d kbps\n"
                 "Brightness: %d%%\n"
                 "Controller ID: %d",
                 settings_get_target_vesc_id(),
                 (int)settings_get_can_speed(),
                 settings_get_screen_brightness(),
                 settings_get_controller_id());
        return true;
    }
    
    // RESET_SETTINGS
    if (strcmp(cmd, "RESET_SETTINGS") == 0) {
        settings_reset_to_defaults();
        snprintf(response, response_max_len, "Settings reset to defaults (restart recommended)");
        return true;
    }
    
    return false;
}

