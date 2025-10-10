/*
	Copyright 2025 Super VESC Display

	BLE Configuration Implementation
*/

#include "ble_config.h"

// ============================================================================
// Internal State
// ============================================================================

// Current BLE operating mode (default: Bridge mode)
static ble_mode_t current_ble_mode = BLE_MODE_BRIDGE;

// ============================================================================
// Public API Implementation
// ============================================================================

ble_mode_t ble_config_get_mode(void) {
  return current_ble_mode;
}

void ble_config_set_mode(ble_mode_t mode) {
  if (mode == BLE_MODE_BRIDGE || mode == BLE_MODE_DIRECT) {
    current_ble_mode = mode;
  }
}

const char* ble_config_get_mode_name(ble_mode_t mode) {
  switch (mode) {
    case BLE_MODE_BRIDGE:
      return "BLE-CAN Bridge";
    case BLE_MODE_DIRECT:
      return "Direct Processing";
    default:
      return "Unknown";
  }
}

const char* ble_config_get_mode_desc(ble_mode_t mode) {
  switch (mode) {
    case BLE_MODE_BRIDGE:
      return "Commands forwarded to CAN bus, responses bridged back";
    case BLE_MODE_DIRECT:
      return "Commands processed locally by device";
    default:
      return "Unknown mode";
  }
}

