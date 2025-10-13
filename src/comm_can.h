/*
	Copyright 2022 Benjamin Vedder	benjamin@vedder.se
	Copyright 2025 Adapted for Super VESC Display

	This file is part of the VESC firmware.

	The VESC firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The VESC firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    */

#ifndef COMM_CAN_H_
#define COMM_CAN_H_

#include "datatypes.h"
#include <stdint.h>
#include <stdbool.h>

#define CAN_STATUS_MSGS_TO_STORE	10

// Configuration structure
typedef struct {
	uint8_t controller_id;
	int can_baud_rate_kbps;
} can_config_t;

// Functions
void comm_can_start(int pin_tx, int pin_rx, uint8_t controller_id, int can_speed_kbps);
void comm_can_stop(void);
void comm_can_transmit_eid(uint32_t id, const uint8_t *data, uint8_t len);
void comm_can_send_buffer(uint8_t controller_id, uint8_t *data, unsigned int len, uint8_t send);
bool comm_can_ping(uint8_t controller_id, HW_TYPE *hw_type);

// Control functions
void comm_can_set_duty(uint8_t controller_id, float duty);
void comm_can_set_current(uint8_t controller_id, float current);
void comm_can_set_current_brake(uint8_t controller_id, float current);
void comm_can_set_rpm(uint8_t controller_id, float rpm);

// Status message getters
can_status_msg *comm_can_get_status_msg_id(int id);
can_status_msg_2 *comm_can_get_status_msg_2_id(int id);
can_status_msg_3 *comm_can_get_status_msg_3_id(int id);
can_status_msg_4 *comm_can_get_status_msg_4_id(int id);
can_status_msg_5 *comm_can_get_status_msg_5_id(int id);
can_status_msg_6 *comm_can_get_status_msg_6_id(int id);

// Packet processing callback
typedef void (*can_packet_handler_t)(unsigned char *data, unsigned int len);
void comm_can_set_packet_handler(can_packet_handler_t handler);

#endif /* COMM_CAN_H_ */
