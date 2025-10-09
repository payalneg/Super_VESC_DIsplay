/*
	Copyright 2025 Super VESC Display

	VESC Packet Parser Module
	Handles packet framing protocol with start/end bytes and CRC validation
*/

#ifndef PACKET_PARSER_H_
#define PACKET_PARSER_H_

#include <stdint.h>
#include <stdbool.h>

// Packet framing bytes
#define PACKET_START_BYTE_SHORT   0x02
#define PACKET_START_BYTE_LONG    0x03
#define PACKET_END_BYTE		      0x03

// Parser states
typedef enum {
	PARSER_STATE_IDLE = 0,
	PARSER_STATE_LENGTH,
	PARSER_STATE_LENGTH_HIGH,
	PARSER_STATE_LENGTH_LOW,
	PARSER_STATE_PAYLOAD,
	PARSER_STATE_CRC_HIGH,
	PARSER_STATE_CRC_LOW,
	PARSER_STATE_END_BYTE
} parser_state_t;

// Packet parser structure
typedef struct {
	parser_state_t state;
	bool is_long_packet;
	uint16_t payload_length;
	uint16_t bytes_received;
	uint16_t crc_received;
	uint8_t buffer[512];  // Max payload size
} packet_parser_t;

// Callback for processed packets
typedef void (*packet_processed_callback_t)(uint8_t* data, uint16_t len);

// Initialize parser
void packet_parser_init(packet_parser_t* parser);

// Process incoming byte, returns true if complete packet was processed
bool packet_parser_process_byte(packet_parser_t* parser, uint8_t byte, 
                                 packet_processed_callback_t callback);

// Reset parser to idle state
void packet_parser_reset(packet_parser_t* parser);

// Build a framed packet from payload (returns total packet length)
uint16_t packet_build_frame(uint8_t* payload, uint16_t payload_len, 
                            uint8_t* out_buffer, uint16_t out_buffer_size);

#endif /* PACKET_PARSER_H_ */

