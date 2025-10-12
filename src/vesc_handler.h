/*
	Copyright 2025 Super VESC Display

	VESC Command Handler Module
	Processes incoming VESC commands from CAN bus and sends responses

	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/

#ifndef VESC_HANDLER_H_
#define VESC_HANDLER_H_

#include <stdint.h>
#include "confparser.h"  // For main_config_t type

// Initialize VESC handler module
void vesc_handler_init(void);

// Main VESC command handler - processes incoming VESC commands and sends responses
void vesc_handler_process_command(unsigned char *data, unsigned int len);

// Get statistics
uint32_t vesc_handler_get_command_count(void);

// Get current configuration
const main_config_t* vesc_handler_get_config(void);

// Callback type for sending responses
typedef void (*vesc_response_callback_t)(uint8_t *data, unsigned int len);

// Set response callback (for BLE or other interfaces)
void vesc_handler_set_response_callback(vesc_response_callback_t callback);


#endif /* VESC_HANDLER_H_ */
