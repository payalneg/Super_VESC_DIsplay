/*
	Copyright 2025 Super VESC Display

	VESC Packet Parser Module
	Handles packet framing protocol with start/end bytes and CRC validation
*/

#include "packet_parser.h"
#include "crc.h"
#include <Arduino.h>
#include <string.h>

// Initialize parser
void packet_parser_init(packet_parser_t* parser) {
	parser->state = PARSER_STATE_IDLE;
	parser->is_long_packet = false;
	parser->payload_length = 0;
	parser->bytes_received = 0;
	parser->crc_received = 0;
	memset(parser->buffer, 0, sizeof(parser->buffer));
}

// Reset parser to idle state
void packet_parser_reset(packet_parser_t* parser) {
	packet_parser_init(parser);
}

// Process incoming byte, returns true if complete packet was processed
bool packet_parser_process_byte(packet_parser_t* parser, uint8_t byte, 
                                 packet_processed_callback_t callback) {
	
	switch (parser->state) {
		case PARSER_STATE_IDLE:
			if (byte == PACKET_START_BYTE_SHORT) {
				parser->is_long_packet = false;
				parser->state = PARSER_STATE_LENGTH;
				parser->payload_length = 0;
				parser->bytes_received = 0;
				parser->crc_received = 0;
			} else if (byte == PACKET_START_BYTE_LONG) {
				parser->is_long_packet = true;
				parser->state = PARSER_STATE_LENGTH_HIGH;
				parser->payload_length = 0;
				parser->bytes_received = 0;
				parser->crc_received = 0;
			}
			break;
			
		case PARSER_STATE_LENGTH:
			parser->payload_length = byte;
			
			if (parser->payload_length == 0 || parser->payload_length > sizeof(parser->buffer)) {
				Serial.printf("❌ Invalid payload length: %d\n", parser->payload_length);
				packet_parser_reset(parser);
				return false;
			}
			
			parser->state = PARSER_STATE_PAYLOAD;
			parser->bytes_received = 0;
			break;
			
		case PARSER_STATE_LENGTH_HIGH:
			parser->payload_length = byte << 8;
			parser->state = PARSER_STATE_LENGTH_LOW;
			break;
			
		case PARSER_STATE_LENGTH_LOW:
			parser->payload_length |= byte;
			
			if (parser->payload_length == 0 || parser->payload_length > sizeof(parser->buffer)) {
				Serial.printf("❌ Invalid payload length: %d\n", parser->payload_length);
				packet_parser_reset(parser);
				return false;
			}
			
			parser->state = PARSER_STATE_PAYLOAD;
			parser->bytes_received = 0;
			break;
			
		case PARSER_STATE_PAYLOAD:
			parser->buffer[parser->bytes_received++] = byte;
			
			if (parser->bytes_received >= parser->payload_length) {
				parser->state = PARSER_STATE_CRC_HIGH;
			}
			break;
			
		case PARSER_STATE_CRC_HIGH:
			parser->crc_received = byte << 8;
			parser->state = PARSER_STATE_CRC_LOW;
			break;
			
		case PARSER_STATE_CRC_LOW:
			parser->crc_received |= byte;
			parser->state = PARSER_STATE_END_BYTE;
			break;
			
		case PARSER_STATE_END_BYTE: {
			bool valid_end = (parser->is_long_packet && byte == PACKET_END_BYTE_LONG) ||
			                (!parser->is_long_packet && byte == PACKET_END_BYTE_SHORT);
			
			if (!valid_end) {
				Serial.printf("❌ Invalid end byte: 0x%02X (expected 0x%02X)\n", 
				             byte, parser->is_long_packet ? PACKET_END_BYTE_LONG : PACKET_END_BYTE_SHORT);
				packet_parser_reset(parser);
				return false;
			}
			
			// Verify CRC
			uint16_t crc_calculated = crc16(parser->buffer, parser->payload_length);
			
			if (crc_calculated != parser->crc_received) {
				Serial.printf("❌ CRC mismatch: calculated=0x%04X, received=0x%04X\n", 
				             crc_calculated, parser->crc_received);
				packet_parser_reset(parser);
				return false;
			}
			
			// Packet is valid! Call callback
			Serial.printf("✅ Valid packet received: %d bytes (CRC: 0x%04X)\n", 
			             parser->payload_length, crc_calculated);
			
			if (callback) {
				callback(parser->buffer, parser->payload_length);
			}
			
			packet_parser_reset(parser);
			return true;
		}
		break;
	}
	
	return false;
}

// Build a framed packet from payload (returns total packet length)
uint16_t packet_build_frame(uint8_t* payload, uint16_t payload_len, 
                            uint8_t* out_buffer, uint16_t out_buffer_size) {
	
	if (payload_len == 0 || payload_len > 512) {
		Serial.printf("❌ Invalid payload length for framing: %d\n", payload_len);
		return 0;
	}
	
	bool use_long_packet = (payload_len > 255);
	uint16_t total_length = use_long_packet ? 
	                       (1 + 2 + payload_len + 2 + 1) :  // start + len(2) + payload + crc(2) + end
	                       (1 + 1 + payload_len + 2 + 1);   // start + len(1) + payload + crc(2) + end
	
	if (total_length > out_buffer_size) {
		Serial.printf("❌ Output buffer too small: need %d, have %d\n", total_length, out_buffer_size);
		return 0;
	}
	
	uint16_t ind = 0;
	
	// Start byte
	out_buffer[ind++] = use_long_packet ? PACKET_START_BYTE_LONG : PACKET_START_BYTE_SHORT;
	
	// Length
	if (use_long_packet) {
		out_buffer[ind++] = (payload_len >> 8) & 0xFF;
		out_buffer[ind++] = payload_len & 0xFF;
	} else {
		out_buffer[ind++] = payload_len & 0xFF;
	}
	
	// Payload
	memcpy(out_buffer + ind, payload, payload_len);
	ind += payload_len;
	
	// CRC
	uint16_t crc = crc16(payload, payload_len);
	out_buffer[ind++] = (crc >> 8) & 0xFF;
	out_buffer[ind++] = crc & 0xFF;
	
	// End byte
	out_buffer[ind++] = use_long_packet ? PACKET_END_BYTE_LONG : PACKET_END_BYTE_SHORT;
	
	Serial.printf("📦 Built framed packet: %d bytes (payload: %d, CRC: 0x%04X)\n", 
	             ind, payload_len, crc);
	
	return ind;
}

