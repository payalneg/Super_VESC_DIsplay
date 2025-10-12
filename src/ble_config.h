/*
	Copyright 2025 Super VESC Display

	BLE Configuration
	Operating mode can be changed at runtime (e.g., from menu)
*/

#ifndef BLE_CONFIG_H_
#define BLE_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// BLE Operating Mode Configuration
// ============================================================================

// BLE operating mode (Bridge mode only)
typedef enum {
  BLE_MODE_BRIDGE = 0  // BLE-CAN Bridge (like vesc_express)
} ble_mode_t;

// ============================================================================
// Mode Description
// ============================================================================

/*
 * BLE_MODE_BRIDGE - BLE-CAN Bridge Mode:
 * - Mobile app → BLE → Parse → CAN bus → VESC controllers
 * - VESC responses → CAN → BLE → Mobile app
 * - Local commands (ID=2) processed by device
 * - Compatible with VESC Tool and official mobile apps
 */

// ============================================================================
// Configuration API
// ============================================================================

/**
 * Get current BLE operating mode
 * @return Current mode (always BLE_MODE_BRIDGE)
 */
ble_mode_t ble_config_get_mode(void);

/**
 * Get mode name string
 * @param mode Mode to get name for
 * @return Mode name string
 */
const char* ble_config_get_mode_name(ble_mode_t mode);

/**
 * Get mode description string
 * @param mode Mode to get description for
 * @return Mode description string
 */
const char* ble_config_get_mode_desc(ble_mode_t mode);

#ifdef __cplusplus
}
#endif

#endif /* BLE_CONFIG_H_ */

