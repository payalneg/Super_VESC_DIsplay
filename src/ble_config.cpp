/*
	Copyright 2025 Super VESC Display

	BLE Configuration Implementation
*/

#include "ble_config.h"

// ============================================================================
// Public API Implementation
// ============================================================================

ble_mode_t ble_config_get_mode(void) {
  return BLE_MODE_BRIDGE;
}

const char* ble_config_get_mode_name(ble_mode_t mode) {
  return "BLE-CAN Bridge";
}

const char* ble_config_get_mode_desc(ble_mode_t mode) {
  return "Commands forwarded to CAN bus, responses bridged back";
}

