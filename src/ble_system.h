/*
	Copyright 2025 Super VESC Display

	Centralized BLE System Initialization
	Manages single NimBLEDevice instance and server
*/

#ifndef BLE_SYSTEM_H_
#define BLE_SYSTEM_H_

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEAdvertising.h>

// BLE driver callback function types
typedef void (*ble_driver_on_connect_cb_t)(void);
typedef void (*ble_driver_on_disconnect_cb_t)(void);
typedef void (*ble_driver_on_mtu_change_cb_t)(uint16_t MTU);

// General BLE Server Callbacks
class BLESystemCallbacks : public NimBLEServerCallbacks
{
public:
    void onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc);
    void onDisconnect(NimBLEServer *pServer);
    void onMTUChange(uint16_t MTU, ble_gap_conn_desc *desc);
};

// Function declarations
bool ble_system_init(const char* device_name);
void ble_system_start_advertising(void);
void ble_system_restart_advertising(void);  // Restart advertising after disconnect
NimBLEServer* ble_system_get_server(void);
bool ble_system_is_initialized(void);

// Register BLE driver callbacks (called from drivers to register their handlers)
void ble_system_register_connect_callback(ble_driver_on_connect_cb_t cb);
void ble_system_register_disconnect_callback(ble_driver_on_disconnect_cb_t cb);
void ble_system_register_mtu_change_callback(ble_driver_on_mtu_change_cb_t cb);

#endif /* BLE_SYSTEM_H_ */

