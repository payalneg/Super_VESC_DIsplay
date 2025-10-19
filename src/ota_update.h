/*
	Copyright 2025 Super VESC Display

	OTA Update Module
	Handles firmware updates over BLE
*/

#ifndef OTA_UPDATE_H_
#define OTA_UPDATE_H_


#include <Arduino.h>
#include <Update.h>
#include "FS.h"
#include "SPIFFS.h"

// OTA Update modes
#define NORMAL_MODE   0   // normal operation
#define UPDATE_MODE   1   // receiving firmware
#define OTA_MODE      2   // installing firmware

// File system configuration
#define FORMAT_SPIFFS_IF_FAILED true
#define FORMAT_FFAT_IF_FAILED true
#define USE_SPIFFS  // comment to use FFat

#ifdef USE_SPIFFS
#define FLASH SPIFFS
#define FASTMODE false    //SPIFFS write is slow
#else
#define FLASH FFat
#define FASTMODE true    //FFat is faster
#endif

// Service and characteristic UUIDs for OTA
#define OTA_SERVICE_UUID              "fb1e4001-54ae-4a28-9f74-dfccb248601d"
#define OTA_CHARACTERISTIC_UUID_RX    "fb1e4002-54ae-4a28-9f74-dfccb248601d"
#define OTA_CHARACTERISTIC_UUID_TX    "fb1e4003-54ae-4a28-9f74-dfccb248601d"

// OTA state variables
typedef struct {
    uint8_t updater[16384];
    uint8_t updater2[16384];
    bool deviceConnected;
    bool sendMode;
    bool sendSize;
    bool writeFile;
    bool request;
    int writeLen;
    int writeLen2;
    bool current;
    int parts;
    int next;
    int cur;
    int MTU;
    int MODE;
    unsigned long rParts;
    unsigned long tParts;
} ota_state_t;

// Function declarations
bool ota_update_init(void);
void ota_update_loop(void);
bool ota_update_is_connected(void);
void ota_update_set_connected(bool connected);
void ota_update_process_data(uint8_t* data, int len);
void ota_update_send_result(const char* result);

// Helper functions
void performUpdate(Stream &updateSource, size_t updateSize);
void updateFromFS(fs::FS &fs);
void writeBinary(fs::FS &fs, const char * path, uint8_t *dat, int len);

#endif /* OTA_UPDATE_H_ */
