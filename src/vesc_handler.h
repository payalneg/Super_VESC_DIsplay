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

// Initialize VESC handler module
void vesc_handler_init(void);

// Main VESC command handler - processes incoming VESC commands and sends responses
void vesc_handler_process_command(unsigned char *data, unsigned int len);

// Get statistics
uint32_t vesc_handler_get_command_count(void);

#endif /* VESC_HANDLER_H_ */
