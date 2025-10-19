/*
	Copyright 2025 Super VESC Display

	OTA Update Module Implementation
	Handles firmware updates over BLE
*/

#include "ota_update.h"
#include "debug_log.h"
#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>

// Global OTA state
static ota_state_t ota_state = {0};
static bool ota_initialized = false;
static BLECharacteristic* pCharacteristicTX = nullptr;
static BLECharacteristic* pCharacteristicRX = nullptr;

// Initialize OTA update module
bool ota_update_init(void) {
    LOG_INFO(OTA, "Initializing OTA update module...");
    
    // Initialize OTA state
    memset(&ota_state, 0, sizeof(ota_state));
    ota_state.deviceConnected = false;
    ota_state.sendMode = false;
    ota_state.sendSize = true;
    ota_state.writeFile = false;
    ota_state.request = false;
    ota_state.writeLen = 0;
    ota_state.writeLen2 = 0;
    ota_state.current = true;
    ota_state.parts = 0;
    ota_state.next = 0;
    ota_state.cur = 0;
    ota_state.MTU = 0;
    ota_state.MODE = NORMAL_MODE;
    ota_state.rParts = 0;
    ota_state.tParts = 0;
    
    // Initialize SPIFFS
    if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
        LOG_ERROR(OTA, "SPIFFS Mount Failed");
        return false;
    }
    
    ota_initialized = true;
    LOG_INFO(OTA, "OTA update module initialized successfully");
    return true;
}

// Set BLE characteristics for OTA communication
void ota_update_set_characteristics(BLECharacteristic* tx, BLECharacteristic* rx) {
    pCharacteristicTX = tx;
    pCharacteristicRX = rx;
    LOG_INFO(OTA, "OTA characteristics set");
}

// Set connection status
void ota_update_set_connected(bool connected) {
    ota_state.deviceConnected = connected;
    LOG_DEBUG(OTA, "OTA connection status: %s", connected ? "connected" : "disconnected");
}

// Check if OTA is connected
bool ota_update_is_connected(void) {
    return ota_state.deviceConnected;
}

// Process received OTA data
void ota_update_process_data(uint8_t* data, int len) {
    if (!ota_initialized || len == 0) {
        return;
    }
    
    LOG_DEBUG(OTA, "Processing OTA data (%d bytes)", len);
    
    if (data[0] == 0xFB) {
        int pos = data[1];
        for (int x = 0; x < len - 2; x++) {
            if (ota_state.current) {
                ota_state.updater[(pos * ota_state.MTU) + x] = data[x + 2];
            } else {
                ota_state.updater2[(pos * ota_state.MTU) + x] = data[x + 2];
            }
        }
        LOG_DEBUG(OTA, "Received data chunk at position %d", pos);
    } else if (data[0] == 0xFC) {
        if (ota_state.current) {
            ota_state.writeLen = (data[1] * 256) + data[2];
        } else {
            ota_state.writeLen2 = (data[1] * 256) + data[2];
        }
        ota_state.current = !ota_state.current;
        ota_state.cur = (data[3] * 256) + data[4];
        ota_state.writeFile = true;
        if (ota_state.cur < ota_state.parts - 1) {
            ota_state.request = !FASTMODE;
        }
        LOG_INFO(OTA, "Received chunk info: len=%d, cur=%d/%d", 
                 ota_state.current ? ota_state.writeLen : ota_state.writeLen2, 
                 ota_state.cur, ota_state.parts);
    } else if (data[0] == 0xFD) {
        ota_state.sendMode = true;
        if (FLASH.exists("/update.bin")) {
            FLASH.remove("/update.bin");
        }
        LOG_INFO(OTA, "Starting firmware transfer");
    } else if (data[0] == 0xFE) {
        ota_state.rParts = 0;
        ota_state.tParts = (data[1] * 256 * 256 * 256) + (data[2] * 256 * 256) + (data[3] * 256) + data[4];
        LOG_INFO(OTA, "Available space: %lu", FLASH.totalBytes() - FLASH.usedBytes());
        LOG_INFO(OTA, "File Size: %lu", ota_state.tParts);
    } else if (data[0] == 0xFF) {
        ota_state.parts = (data[1] * 256) + data[2];
        ota_state.MTU = (data[3] * 256) + data[4];
        ota_state.MODE = UPDATE_MODE;
        LOG_INFO(OTA, "Starting OTA update: %d parts, MTU: %d", ota_state.parts, ota_state.MTU);
    } else if (data[0] == 0xEF) {
        FLASH.format();
        ota_state.sendSize = true;
        LOG_INFO(OTA, "Flash formatted");
    }
}

// Send OTA result
void ota_update_send_result(const char* result) {
    if (!ota_initialized || !ota_state.deviceConnected || !pCharacteristicTX) {
        return;
    }
    
    byte arr[strlen(result)];
    memcpy(arr, result, strlen(result));
    pCharacteristicTX->setValue(arr, strlen(result));
    pCharacteristicTX->notify();
    delay(200);
    LOG_INFO(OTA, "Sent OTA result: %s", result);
}

// Write binary data to file
void writeBinary(fs::FS &fs, const char * path, uint8_t *dat, int len) {
    LOG_INFO(OTA, "Write binary file %s (%d bytes)", path, len);
    File file = fs.open(path, FILE_APPEND);
    if (!file) {
        LOG_ERROR(OTA, "Failed to open file for writing");
        return;
    }
    file.write(dat, len);
    file.close();
    ota_state.writeFile = false;
    ota_state.rParts += len;
    LOG_DEBUG(OTA, "Written %d bytes, total: %lu", len, ota_state.rParts);
}

// Perform OTA update
void performUpdate(Stream &updateSource, size_t updateSize) {
    char s1 = 0x0F;
    String result = String(s1);
    
    if (Update.begin(updateSize)) {
        size_t written = Update.writeStream(updateSource);
        if (written == updateSize) {
            LOG_INFO(OTA, "Written : %d successfully", written);
        } else {
            LOG_WARN(OTA, "Written only : %d/%d. Retry?", written, updateSize);
        }
        result += "Written : " + String(written) + "/" + String(updateSize) + " [" + String((written / updateSize) * 100) + "%] \n";
        
        if (Update.end()) {
            LOG_INFO(OTA, "OTA done!");
            result += "OTA Done: ";
            if (Update.isFinished()) {
                LOG_INFO(OTA, "Update successfully completed. Rebooting...");
                result += "Success!\n";
            } else {
                LOG_ERROR(OTA, "Update not finished? Something went wrong!");
                result += "Failed!\n";
            }
        } else {
            LOG_ERROR(OTA, "Error Occurred. Error #: %d", Update.getError());
            result += "Error #: " + String(Update.getError());
        }
    } else {
        LOG_ERROR(OTA, "Not enough space to begin OTA");
        result += "Not enough space for OTA";
    }
    
    if (ota_state.deviceConnected) {
        ota_update_send_result(result.c_str());
        delay(5000);
    }
}

// Update from file system
void updateFromFS(fs::FS &fs) {
    File updateBin = fs.open("/update.bin");
    if (updateBin) {
        if (updateBin.isDirectory()) {
            LOG_ERROR(OTA, "Error, update.bin is not a file");
            updateBin.close();
            return;
        }

        size_t updateSize = updateBin.size();
        if (updateSize > 0) {
            LOG_INFO(OTA, "Trying to start update");
            performUpdate(updateBin, updateSize);
        } else {
            LOG_ERROR(OTA, "Error, file is empty");
        }

        updateBin.close();
        LOG_INFO(OTA, "Removing update file");
        fs.remove("/update.bin");
        
        LOG_INFO(OTA, "Rebooting to complete OTA update");
        delay(1000);
        ESP.restart();
    } else {
        LOG_ERROR(OTA, "Could not load update.bin from spiffs root");
    }
}

// Main OTA processing loop
void ota_update_loop(void) {
    if (!ota_initialized) {
        return;
    }
    
    switch (ota_state.MODE) {
        case NORMAL_MODE:		
            if (ota_state.deviceConnected) {
                if (ota_state.sendMode) {
                    LOG_INFO(OTA, "sendMode");
                    uint8_t fMode[] = {0xAA, FASTMODE};
                    if (pCharacteristicTX) {
                        pCharacteristicTX->setValue(fMode, 2);
                        pCharacteristicTX->notify();
                    }
                    delay(50);
                    ota_state.sendMode = false;
                }
                if (ota_state.sendSize) {
                    LOG_INFO(OTA, "sendSize");
                    unsigned long x = FLASH.totalBytes();
                    unsigned long y = FLASH.usedBytes();
                    uint8_t fSize[] = {0xEF, (uint8_t) (x >> 16), (uint8_t) (x >> 8), (uint8_t) x, (uint8_t) (y >> 16), (uint8_t) (y >> 8), (uint8_t) y};
                    if (pCharacteristicTX) {
                        pCharacteristicTX->setValue(fSize, 7);
                        pCharacteristicTX->notify();
                    }
                    delay(50);
                    ota_state.sendSize = false;
                }
            }
            break;

        case UPDATE_MODE:
            if (ota_state.request) {
                uint8_t rq[] = {0xF1, (uint8_t)((ota_state.cur + 1) / 256), (uint8_t)((ota_state.cur + 1) % 256)};
                if (pCharacteristicTX) {
                    pCharacteristicTX->setValue(rq, 3);
                    pCharacteristicTX->notify();
                }
                delay(50);
                ota_state.request = false;
            }

            if (ota_state.writeFile) {
                if (!ota_state.current) {
                    writeBinary(FLASH, "/update.bin", ota_state.updater, ota_state.writeLen);
                } else {
                    writeBinary(FLASH, "/update.bin", ota_state.updater2, ota_state.writeLen2);
                }
                ota_state.writeFile = false;
            }

            if (ota_state.cur + 1 == ota_state.parts) {
                uint8_t com[] = {0xF2, (uint8_t)((ota_state.cur + 1) / 256), (uint8_t)((ota_state.cur + 1) % 256)};
                if (pCharacteristicTX) {
                    pCharacteristicTX->setValue(com, 3);
                    pCharacteristicTX->notify();
                }
                delay(50);
                ota_state.MODE = OTA_MODE;
                LOG_INFO(OTA, "File transfer complete, starting installation");
            }
            break;

        case OTA_MODE:
            if (ota_state.writeFile) {
                if (!ota_state.current) {
                    writeBinary(FLASH, "/update.bin", ota_state.updater, ota_state.writeLen);
                } else {
                    writeBinary(FLASH, "/update.bin", ota_state.updater2, ota_state.writeLen2);
                }
            }

            if (ota_state.rParts == ota_state.tParts) {
                LOG_INFO(OTA, "OTA Complete");
                delay(5000);
                updateFromFS(FLASH);
            } else {
                ota_state.writeFile = true;
                LOG_WARN(OTA, "OTA Incomplete - Expected: %lu, Received: %lu", ota_state.tParts, ota_state.rParts);
                delay(2000);
            }
            break;
    }
}
