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
// BLE Operating Mode Configuration (Runtime)
// ============================================================================

// BLE operating modes
typedef enum {
  BLE_MODE_BRIDGE = 0,  // BLE-CAN Bridge (like vesc_express)
  BLE_MODE_DIRECT = 1   // Direct Processing (standalone device)
} ble_mode_t;

// ============================================================================
// Mode Descriptions
// ============================================================================

/*
 * BLE_MODE_BRIDGE - BLE-CAN Bridge Mode:
 * - Mobile app → BLE → Parse → CAN bus → VESC controllers
 * - VESC responses → CAN → BLE → Mobile app
 * - Local commands (ID=2) processed by device
 * - Compatible with VESC Tool and official mobile apps
 */

/*
 * BLE_MODE_DIRECT - Direct Processing Mode:
 * - Mobile app → BLE → Parse → Local vesc_handler
 * - Responses generated locally → BLE → Mobile app
 * - All commands handled by this device
 * - No CAN forwarding (device acts as standalone VESC)
 */

// ============================================================================
// Configuration API
// ============================================================================

/**
 * Get current BLE operating mode
 * @return Current mode (BLE_MODE_BRIDGE or BLE_MODE_DIRECT)
 */
ble_mode_t ble_config_get_mode(void);

/**
 * Set BLE operating mode
 * @param mode New mode to set
 */
void ble_config_set_mode(ble_mode_t mode);

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

