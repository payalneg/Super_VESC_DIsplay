/*
	Copyright 2025 Super VESC Display

	BLE Configuration
	Define operating mode and settings
*/

#ifndef BLE_CONFIG_H_
#define BLE_CONFIG_H_

// ============================================================================
// BLE Operating Mode Configuration
// ============================================================================

// Uncomment ONE of the following modes:

// Mode 1: BLE-CAN Bridge (like vesc_express)
// Commands from BLE are forwarded to CAN bus, responses sent back
#define BLE_MODE_BRIDGE

// Mode 2: Direct Processing (standalone device)
// Commands from BLE are processed locally by this device
// #define BLE_MODE_DIRECT

// ============================================================================

// Validate configuration
#if defined(BLE_MODE_BRIDGE) && defined(BLE_MODE_DIRECT)
  #error "Cannot enable both BLE_MODE_BRIDGE and BLE_MODE_DIRECT at the same time!"
#endif

#if !defined(BLE_MODE_BRIDGE) && !defined(BLE_MODE_DIRECT)
  #error "Must define either BLE_MODE_BRIDGE or BLE_MODE_DIRECT!"
#endif

// ============================================================================
// Mode descriptions
// ============================================================================

#ifdef BLE_MODE_BRIDGE
  #define BLE_MODE_NAME "BLE-CAN Bridge"
  #define BLE_MODE_DESC "Commands forwarded to CAN bus, responses bridged back"
  /*
   * BLE-CAN Bridge Mode:
   * - Mobile app → BLE → Parse → CAN bus → VESC controllers
   * - VESC responses → CAN → BLE → Mobile app
   * - Local commands (ID=2) processed by device
   * - Compatible with VESC Tool and official mobile apps
   */
#endif

#ifdef BLE_MODE_DIRECT
  #define BLE_MODE_NAME "Direct Processing"
  #define BLE_MODE_DESC "Commands processed locally by device"
  /*
   * Direct Processing Mode:
   * - Mobile app → BLE → Parse → Local vesc_handler
   * - Responses generated locally → BLE → Mobile app
   * - All commands handled by this device
   * - No CAN forwarding (device acts as standalone VESC)
   */
#endif

#endif /* BLE_CONFIG_H_ */

