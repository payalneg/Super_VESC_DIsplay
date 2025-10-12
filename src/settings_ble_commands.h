/*
	Copyright 2025 Super VESC Display
	
	Settings management via BLE commands
*/

#ifndef SETTINGS_BLE_COMMANDS_H_
#define SETTINGS_BLE_COMMANDS_H_

#include <stddef.h>
#include <stdbool.h>

// Process BLE settings commands
// Returns true if command was handled, false otherwise
bool process_settings_command(const char* cmd, char* response, size_t response_max_len);

#endif /* SETTINGS_BLE_COMMANDS_H_ */

