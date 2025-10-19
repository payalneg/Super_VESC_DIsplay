/*
	Copyright 2025 Super VESC Display

	Bluetooth Client Module
	Connects to external measurement devices and receives data
*/

#ifndef BLUETOOTH_CLIENT_H_
#define BLUETOOTH_CLIENT_H_

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEClient.h>
#include <NimBLEScan.h>
#include <NimBLEAdvertisedDevice.h>

// Device connection states
typedef enum {
    DEVICE_STATE_SCANNING = 0,
    DEVICE_STATE_CONNECTING,
    DEVICE_STATE_CONNECTED,
    DEVICE_STATE_IDLE
} DeviceState;

// Target device name
#define TARGET_DEVICE_NAME "SuperVESCDisplay"

// Function declarations
bool bluetooth_client_init(void);
void bluetooth_client_loop(void);
bool bluetooth_client_is_connected(void);
DeviceState bluetooth_client_get_state(void);

#endif /* BLUETOOTH_CLIENT_H_ */
