/*
	Copyright 2025 Super VESC Display

	Music Server Module
	Handles BLE music and navigation service
*/

#ifndef MUSIC_SERVER_H_
#define MUSIC_SERVER_H_

#include <Arduino.h>
#include <NimBLEDevice.h>

// Music Server UUIDs
#define MUSIC_SERVICE_UUID              "55c1ef40-6155-47cf-929a-c994c915ca22"
#define MUSIC_CHARACTERISTIC_UUID_SONG  "55c1ef41-6155-47cf-929a-c994c915ca22"
#define MUSIC_CHARACTERISTIC_UUID_NAV   "55c1ef42-6155-47cf-929a-c994c915ca22"
#define MUSIC_CHARACTERISTIC_UUID_ICON  "55c1ef43-6155-47cf-929a-c994c915ca22"

// Forward declarations
class MusicServerCallbacks;
class MusicCharacteristicCallbacks;

// Function declarations
bool music_server_init(NimBLEServer* pServer);  // Now accepts server as parameter
void music_server_loop(void);
bool music_server_is_connected(void);

#endif /* MUSIC_SERVER_H_ */

