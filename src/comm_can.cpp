/*
	Copyright 2022 - 2024 Benjamin Vedder	benjamin@vedder.se
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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "datatypes.h"
#include "buffer.h"
#include "driver/twai.h"
#include "comm_can.h"
#include "crc.h"
#include <Arduino.h>
#include <string.h>

// Status messages
static can_status_msg stat_msgs[CAN_STATUS_MSGS_TO_STORE];
static can_status_msg_2 stat_msgs_2[CAN_STATUS_MSGS_TO_STORE];
static can_status_msg_3 stat_msgs_3[CAN_STATUS_MSGS_TO_STORE];
static can_status_msg_4 stat_msgs_4[CAN_STATUS_MSGS_TO_STORE];
static can_status_msg_5 stat_msgs_5[CAN_STATUS_MSGS_TO_STORE];
static can_status_msg_6 stat_msgs_6[CAN_STATUS_MSGS_TO_STORE];

#define RX_BUFFER_NUM				3
#define RX_BUFFER_SIZE				512
#define RXBUF_LEN					50

static twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();
static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
static twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_6, GPIO_NUM_0, TWAI_MODE_NORMAL);

static volatile bool init_done = false;
static volatile bool sem_init_done = false;
static volatile bool stop_threads = false;
static volatile bool stop_rx = false;
static volatile bool rx_running = false;

static SemaphoreHandle_t ping_sem;
static SemaphoreHandle_t send_mutex;
static volatile HW_TYPE ping_hw_last = HW_TYPE_VESC;
static uint8_t rx_buffer[RX_BUFFER_NUM][RX_BUFFER_SIZE];
static int rx_buffer_offset[RX_BUFFER_NUM];
static volatile unsigned int rx_buffer_last_id;
static volatile unsigned int rx_buffer_response_type = 1;

static twai_message_t rx_buf[RXBUF_LEN];
static volatile int rx_write = 0;
static volatile int rx_read = 0;

static can_config_t can_config;

// Packet handler callback
static can_packet_handler_t packet_handler = nullptr;

// Private functions
static void send_packet_wrapper(unsigned char *data, unsigned int len);
static void decode_msg(uint32_t eid, uint8_t *data8, int len);
static void process_task(void *arg);
static void rx_task(void *arg);
static void start_rx_thd(void);
static void stop_rx_thd(void);

void send_packet_wrapper(unsigned char *data, unsigned int len) {
	comm_can_send_buffer(rx_buffer_last_id, data, len, rx_buffer_response_type);
}

void decode_msg(uint32_t eid, uint8_t *data8, int len) {
	int32_t ind = 0;
	uint8_t crc_low;
	uint8_t crc_high;
	uint8_t commands_send;

	uint8_t id = eid & 0xFF;
	CAN_PACKET_ID cmd = (CAN_PACKET_ID)(eid >> 8);
	
	// Log all incoming CAN packets with their type
	static uint32_t packet_count = 0;
	if (packet_count++ % 100 == 0 || cmd == CAN_PACKET_PING || cmd == CAN_PACKET_PONG) {
		Serial.printf("🔍 CAN RX: ID=0x%03X (VESC:%d, CMD:0x%02X) Len=%d\n", 
		             eid, id, cmd, len);
	}

	// Handle messages addressed to us
	if (id == 255 || id == can_config.controller_id) {
		switch (cmd) {
		case CAN_PACKET_FILL_RX_BUFFER: {
			int buf_ind = -1;
			int offset = data8[0];
			data8++;
			len--;

			for (int i = 0; i < RX_BUFFER_NUM;i++) {
				if ((rx_buffer_offset[i]) == offset ) {
					buf_ind = i;
					break;
				}
			}

			if (buf_ind < 0) {
				if (offset == 0) {
					buf_ind = 0;
				} else {
					break;
				}
			}

			memcpy(rx_buffer[buf_ind] + offset, data8, len);
			rx_buffer_offset[buf_ind] = offset + len;
		} break;

		case CAN_PACKET_FILL_RX_BUFFER_LONG: {
			int buf_ind = -1;
			int offset = (int)data8[0] << 8;
			offset |= data8[1];
			data8 += 2;
			len -= 2;

			for (int i = 0; i < RX_BUFFER_NUM;i++) {
				if ((rx_buffer_offset[i]) == offset ) {
					buf_ind = i;
					break;
				}
			}

			if (buf_ind < 0) {
				if (offset == 0) {
					buf_ind = 0;
				} else {
					break;
				}
			}

			if ((offset + len) <= RX_BUFFER_SIZE) {
				memcpy(rx_buffer[buf_ind] + offset, data8, len);
				rx_buffer_offset[buf_ind] = offset + len;
			}
		} break;

		case CAN_PACKET_PROCESS_RX_BUFFER: {
			ind = 0;
			unsigned int last_id = data8[ind++];
			commands_send = data8[ind++];

			if (commands_send == 0 || commands_send == 3) {
				rx_buffer_last_id = last_id;
			}

			if (commands_send == 3) {
				rx_buffer_response_type = 0;
			} else {
				rx_buffer_response_type = 1;
			}

			int rxbuf_len = (int)data8[ind++] << 8;
			rxbuf_len |= (int)data8[ind++];

			if (rxbuf_len > RX_BUFFER_SIZE) {
				break;
			}

			int buf_ind = -1;
			for (int i = 0; i < RX_BUFFER_NUM;i++) {
				if ((rx_buffer_offset[i]) == rxbuf_len ) {
					buf_ind = i;
					break;
				}
			}

			// Something is wrong, reset all buffers
			if (buf_ind < 0) {
				for (int i = 0; i < RX_BUFFER_NUM;i++) {
					rx_buffer_offset[i] = 0;
				}
				break;
			}

			rx_buffer_offset[buf_ind] = 0;

			crc_high = data8[ind++];
			crc_low = data8[ind++];

			if (crc16(rx_buffer[buf_ind], rxbuf_len)
					== ((unsigned short) crc_high << 8
							| (unsigned short) crc_low)) {
				
				// Process packet if handler is set
				if (packet_handler) {
					packet_handler(rx_buffer[buf_ind], rxbuf_len);
				}
			}
		} break;

		case CAN_PACKET_PROCESS_SHORT_BUFFER: {
			ind = 0;
			unsigned int last_id = data8[ind++];
			commands_send = data8[ind++];

			if (commands_send == 0 || commands_send == 3) {
				rx_buffer_last_id = last_id;
			}

			if (commands_send == 3) {
				rx_buffer_response_type = 0;
			} else {
				rx_buffer_response_type = 1;
			}

			// Process packet if handler is set
			if (packet_handler) {
				packet_handler(data8 + ind, len - ind);
			}
		} break;

		case CAN_PACKET_PING: {
			Serial.printf("📥 CAN PING received from ID %d\n", data8[0]);
			uint8_t buffer[2];
			buffer[0] = can_config.controller_id;
			buffer[1] = HW_TYPE_CUSTOM_MODULE;
			uint32_t pong_id = data8[0] | ((uint32_t)CAN_PACKET_PONG << 8);
			Serial.printf("📤 Sending PONG: ID=0x%03X, MyID=%d, HW_TYPE=%d\n", 
			             pong_id, buffer[0], buffer[1]);
			comm_can_transmit_eid(pong_id, buffer, 2);
		} break;

		case CAN_PACKET_PONG:
			Serial.printf("📥 CAN PONG received: ID=%d, HW_TYPE=%d\n", 
			             data8[0], len >= 2 ? data8[1] : -1);
			xSemaphoreGive(ping_sem);
			if (len >= 2) {
				ping_hw_last = (HW_TYPE)data8[1];
			} else {
				ping_hw_last = HW_TYPE_VESC_BMS;
			}
			break;

		default:
			break;
		}
	}

	// The packets below are addressed to all devices, mainly containing status information.
	switch (cmd) {
	case CAN_PACKET_STATUS:
		for (int i = 0;i < CAN_STATUS_MSGS_TO_STORE;i++) {
			can_status_msg *stat_tmp = &stat_msgs[i];
			if (stat_tmp->id == id || stat_tmp->id == -1) {
				ind = 0;
				stat_tmp->id = id;
				stat_tmp->rx_time = xTaskGetTickCount();
				stat_tmp->rpm = (float)buffer_get_int32(data8, &ind);
				stat_tmp->current = (float)buffer_get_int16(data8, &ind) / 10.0;
				stat_tmp->duty = (float)buffer_get_int16(data8, &ind) / 1000.0;
				
				// Log first STATUS packet from new VESC
				static int8_t last_status_id = -1;
				if (last_status_id != id) {
					Serial.printf("📊 STATUS packet from VESC #%d: RPM=%.0f, Current=%.2fA\n", 
					             id, stat_tmp->rpm, stat_tmp->current);
					last_status_id = id;
				}
				break;
			}
		}
		break;

	case CAN_PACKET_STATUS_2:
		for (int i = 0;i < CAN_STATUS_MSGS_TO_STORE;i++) {
			can_status_msg_2 *stat_tmp_2 = &stat_msgs_2[i];
			if (stat_tmp_2->id == id || stat_tmp_2->id == -1) {
				ind = 0;
				stat_tmp_2->id = id;
				stat_tmp_2->rx_time = xTaskGetTickCount();
				stat_tmp_2->amp_hours = (float)buffer_get_int32(data8, &ind) / 1e4;
				stat_tmp_2->amp_hours_charged = (float)buffer_get_int32(data8, &ind) / 1e4;
				break;
			}
		}
		break;

	case CAN_PACKET_STATUS_3:
		for (int i = 0;i < CAN_STATUS_MSGS_TO_STORE;i++) {
			can_status_msg_3 *stat_tmp_3 = &stat_msgs_3[i];
			if (stat_tmp_3->id == id || stat_tmp_3->id == -1) {
				ind = 0;
				stat_tmp_3->id = id;
				stat_tmp_3->rx_time = xTaskGetTickCount();
				stat_tmp_3->watt_hours = (float)buffer_get_int32(data8, &ind) / 1e4;
				stat_tmp_3->watt_hours_charged = (float)buffer_get_int32(data8, &ind) / 1e4;
				break;
			}
		}
		break;

	case CAN_PACKET_STATUS_4:
		for (int i = 0;i < CAN_STATUS_MSGS_TO_STORE;i++) {
			can_status_msg_4 *stat_tmp_4 = &stat_msgs_4[i];
			if (stat_tmp_4->id == id || stat_tmp_4->id == -1) {
				ind = 0;
				stat_tmp_4->id = id;
				stat_tmp_4->rx_time = xTaskGetTickCount();
				stat_tmp_4->temp_fet = (float)buffer_get_int16(data8, &ind) / 10.0;
				stat_tmp_4->temp_motor = (float)buffer_get_int16(data8, &ind) / 10.0;
				stat_tmp_4->current_in = (float)buffer_get_int16(data8, &ind) / 10.0;
				stat_tmp_4->pid_pos_now = (float)buffer_get_int16(data8, &ind) / 50.0;
				break;
			}
		}
		break;

	case CAN_PACKET_STATUS_5:
		for (int i = 0;i < CAN_STATUS_MSGS_TO_STORE;i++) {
			can_status_msg_5 *stat_tmp_5 = &stat_msgs_5[i];
			if (stat_tmp_5->id == id || stat_tmp_5->id == -1) {
				ind = 0;
				stat_tmp_5->id = id;
				stat_tmp_5->rx_time = xTaskGetTickCount();
				stat_tmp_5->tacho_value = buffer_get_int32(data8, &ind);
				stat_tmp_5->v_in = (float)buffer_get_int16(data8, &ind) / 1e1;
				break;
			}
		}
		break;

	case CAN_PACKET_STATUS_6:
		for (int i = 0;i < CAN_STATUS_MSGS_TO_STORE;i++) {
			can_status_msg_6 *stat_tmp_6 = &stat_msgs_6[i];
			if (stat_tmp_6->id == id || stat_tmp_6->id == -1) {
				ind = 0;
				stat_tmp_6->id = id;
				stat_tmp_6->rx_time = xTaskGetTickCount();
				stat_tmp_6->adc_1 = buffer_get_float16(data8, 1e3, &ind);
				stat_tmp_6->adc_2 = buffer_get_float16(data8, 1e3, &ind);
				stat_tmp_6->adc_3 = buffer_get_float16(data8, 1e3, &ind);
				stat_tmp_6->ppm = buffer_get_float16(data8, 1e3, &ind);
				break;
			}
		}
		break;

	default:
		break;
	}
}

void rx_task(void *arg) {
	rx_running = true;

	while (!stop_rx) {
		twai_message_t msg;
		if (twai_receive(&msg, pdMS_TO_TICKS(10)) == ESP_OK) {
			int next_write = rx_write + 1;
			if (next_write >= RXBUF_LEN) {
				next_write = 0;
			}

			if (next_write != rx_read) {
				rx_buf[rx_write] = msg;
				rx_write = next_write;
			}
		}
	}

	rx_running = false;
	vTaskDelete(NULL);
}

void start_rx_thd(void) {
	stop_rx = false;
	xTaskCreatePinnedToCore(rx_task, "can_rx", 2048, NULL, 8, NULL, tskNO_AFFINITY);
}

void stop_rx_thd(void) {
	stop_rx = true;
	while (rx_running) {
		vTaskDelay(1);
	}
}

void process_task(void *arg) {
	for (;;) {
		vTaskDelay(10 / portTICK_PERIOD_MS);

		while (rx_read != rx_write) {
			twai_message_t *msg = &rx_buf[rx_read];
			rx_read++;
			if (rx_read >= RXBUF_LEN) {
				rx_read = 0;
			}

			if (msg->extd) {
				decode_msg(msg->identifier, msg->data, msg->data_length_code);
			}
		}
	}

	vTaskDelete(NULL);
}

void comm_can_start(int pin_tx, int pin_rx, uint8_t controller_id) {
	// Initialize status message arrays
	for (int i = 0;i < CAN_STATUS_MSGS_TO_STORE;i++) {
		stat_msgs[i].id = -1;
		stat_msgs_2[i].id = -1;
		stat_msgs_3[i].id = -1;
		stat_msgs_4[i].id = -1;
		stat_msgs_5[i].id = -1;
		stat_msgs_6[i].id = -1;
	}

	// Initialize rx buffers
	for (int i = 0;i < RX_BUFFER_NUM;i++) {
		rx_buffer_offset[i] = 0;
	}

	// Store configuration
	can_config.controller_id = controller_id;
	can_config.can_baud_rate_kbps = 250;

	if (!sem_init_done) {
		ping_sem = xSemaphoreCreateBinary();
		send_mutex = xSemaphoreCreateMutex();

		// Create process task
		xTaskCreatePinnedToCore(process_task, "can_proc", 3072, NULL, 8, NULL, tskNO_AFFINITY);

		sem_init_done = true;
	}

	g_config.tx_queue_len = 20;
	g_config.rx_queue_len = 20;
	g_config.tx_io        = (gpio_num_t)pin_tx;
	g_config.rx_io        = (gpio_num_t)pin_rx;

	twai_driver_install(&g_config, &t_config, &f_config);
	twai_start();

	stop_threads = false;

	start_rx_thd();

	init_done = true;

	Serial.printf("CAN initialized: TX=%d, RX=%d, ID=%d, Speed=250kbps\n", 
				  pin_tx, pin_rx, controller_id);
}

void comm_can_stop(void) {
	if (!init_done) {
		return;
	}

	xSemaphoreTake(send_mutex, portMAX_DELAY);
	init_done = false;
	xSemaphoreGive(send_mutex);

	stop_threads = true;
	stop_rx_thd();

	twai_stop();
	twai_driver_uninstall();
}

void comm_can_transmit_eid(uint32_t id, const uint8_t *data, uint8_t len) {
	if (!init_done) {
		return;
	}

	if (len > 8) {
		len = 8;
	}

	twai_message_t tx_msg = {0};
	tx_msg.extd = 1;
	tx_msg.identifier = id;

	memcpy(tx_msg.data, data, len);
	tx_msg.data_length_code = len;

	xSemaphoreTake(send_mutex, portMAX_DELAY);

	if (!init_done) {
		xSemaphoreGive(send_mutex);
		return;
	}

	twai_transmit(&tx_msg, pdMS_TO_TICKS(5));

	xSemaphoreGive(send_mutex);
}

void comm_can_send_buffer(uint8_t controller_id, uint8_t *data, unsigned int len, uint8_t send) {
	uint8_t send_buffer[8];

	if (len <= 6) {
		uint32_t ind = 0;
		send_buffer[ind++] = can_config.controller_id;
		send_buffer[ind++] = send;
		memcpy(send_buffer + ind, data, len);
		ind += len;
		comm_can_transmit_eid(controller_id |
				((uint32_t)CAN_PACKET_PROCESS_SHORT_BUFFER << 8), send_buffer, ind);
	} else {
		unsigned int end_a = 0;
		for (unsigned int i = 0;i < len;i += 7) {
			if (i > 255) {
				break;
			}

			end_a = i + 7;

			uint8_t send_len = 7;
			send_buffer[0] = i;

			if ((i + 7) <= len) {
				memcpy(send_buffer + 1, data + i, send_len);
			} else {
				send_len = len - i;
				memcpy(send_buffer + 1, data + i, send_len);
			}

			comm_can_transmit_eid(controller_id |
					((uint32_t)CAN_PACKET_FILL_RX_BUFFER << 8), send_buffer, send_len + 1);
		}

		for (unsigned int i = end_a;i < len;i += 6) {
			uint8_t send_len = 6;
			send_buffer[0] = i >> 8;
			send_buffer[1] = i & 0xFF;

			if ((i + 6) <= len) {
				memcpy(send_buffer + 2, data + i, send_len);
			} else {
				send_len = len - i;
				memcpy(send_buffer + 2, data + i, send_len);
			}

			comm_can_transmit_eid(controller_id |
					((uint32_t)CAN_PACKET_FILL_RX_BUFFER_LONG << 8), send_buffer, send_len + 2);
		}

		uint32_t ind = 0;
		send_buffer[ind++] = can_config.controller_id;
		send_buffer[ind++] = send;
		send_buffer[ind++] = len >> 8;
		send_buffer[ind++] = len & 0xFF;
		unsigned short crc = crc16(data, len);
		send_buffer[ind++] = (uint8_t)(crc >> 8);
		send_buffer[ind++] = (uint8_t)(crc & 0xFF);

		comm_can_transmit_eid(controller_id |
				((uint32_t)CAN_PACKET_PROCESS_RX_BUFFER << 8), send_buffer, ind++);
	}
}

bool comm_can_ping(uint8_t controller_id, HW_TYPE *hw_type) {
	if (!init_done) {
		return false;
	}

	uint8_t buffer[1];
	buffer[0] = can_config.controller_id;
	comm_can_transmit_eid(controller_id | ((uint32_t)CAN_PACKET_PING << 8), buffer, 1);

	bool ret = xSemaphoreTake(ping_sem, 10 / portTICK_PERIOD_MS) == pdTRUE;

	if (ret && hw_type) {
		*hw_type = ping_hw_last;
	}

	return ret;
}

void comm_can_set_duty(uint8_t controller_id, float duty) {
	int32_t send_index = 0;
	uint8_t buffer[4];
	buffer_append_int32(buffer, (int32_t)(duty * 100000.0), &send_index);
	comm_can_transmit_eid(controller_id | ((uint32_t)CAN_PACKET_SET_DUTY << 8), buffer, send_index);
}

void comm_can_set_current(uint8_t controller_id, float current) {
	int32_t send_index = 0;
	uint8_t buffer[4];
	buffer_append_int32(buffer, (int32_t)(current * 1000.0), &send_index);
	comm_can_transmit_eid(controller_id | ((uint32_t)CAN_PACKET_SET_CURRENT << 8), buffer, send_index);
}

void comm_can_set_current_brake(uint8_t controller_id, float current) {
	int32_t send_index = 0;
	uint8_t buffer[4];
	buffer_append_int32(buffer, (int32_t)(current * 1000.0), &send_index);
	comm_can_transmit_eid(controller_id | ((uint32_t)CAN_PACKET_SET_CURRENT_BRAKE << 8), buffer, send_index);
}

void comm_can_set_rpm(uint8_t controller_id, float rpm) {
	int32_t send_index = 0;
	uint8_t buffer[4];
	buffer_append_int32(buffer, (int32_t)rpm, &send_index);
	comm_can_transmit_eid(controller_id | ((uint32_t)CAN_PACKET_SET_RPM << 8), buffer, send_index);
}

// Status getters
can_status_msg *comm_can_get_status_msg_id(int id) {
	for (int i = 0;i < CAN_STATUS_MSGS_TO_STORE;i++) {
		if (stat_msgs[i].id == id) {
			return &stat_msgs[i];
		}
	}
	return 0;
}

can_status_msg_2 *comm_can_get_status_msg_2_id(int id) {
	for (int i = 0;i < CAN_STATUS_MSGS_TO_STORE;i++) {
		if (stat_msgs_2[i].id == id) {
			return &stat_msgs_2[i];
		}
	}
	return 0;
}

can_status_msg_3 *comm_can_get_status_msg_3_id(int id) {
	for (int i = 0;i < CAN_STATUS_MSGS_TO_STORE;i++) {
		if (stat_msgs_3[i].id == id) {
			return &stat_msgs_3[i];
		}
	}
	return 0;
}

can_status_msg_4 *comm_can_get_status_msg_4_id(int id) {
	for (int i = 0;i < CAN_STATUS_MSGS_TO_STORE;i++) {
		if (stat_msgs_4[i].id == id) {
			return &stat_msgs_4[i];
		}
	}
	return 0;
}

can_status_msg_5 *comm_can_get_status_msg_5_id(int id) {
	for (int i = 0;i < CAN_STATUS_MSGS_TO_STORE;i++) {
		if (stat_msgs_5[i].id == id) {
			return &stat_msgs_5[i];
		}
	}
	return 0;
}

can_status_msg_6 *comm_can_get_status_msg_6_id(int id) {
	for (int i = 0;i < CAN_STATUS_MSGS_TO_STORE;i++) {
		if (stat_msgs_6[i].id == id) {
			return &stat_msgs_6[i];
		}
	}
	return 0;
}

void comm_can_set_packet_handler(can_packet_handler_t handler) {
	packet_handler = handler;
}
