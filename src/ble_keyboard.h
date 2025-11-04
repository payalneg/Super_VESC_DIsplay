/*
	Copyright 2025 Super VESC Display

	BLE Keyboard Module with OTA Update Support
	Provides HID keyboard functionality and firmware update capabilities
*/

#ifndef BLE_KEYBOARD_H_
#define BLE_KEYBOARD_H_

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>
#include <NimBLEHIDDevice.h>
#include "HIDTypes.h"
#include <driver/adc.h>
#include "sdkconfig.h"

// Report IDs for HID
#define KEYBOARD_ID 0x01
#define MEDIA_KEYS_ID 0x02

// Key report structure
typedef struct {
    uint8_t modifiers;
    uint8_t reserved;
    uint8_t keys[6];
} KeyReport;

// Media key report structure
typedef struct {
    uint8_t keys[2];
} MediaKeyReport;

// BLE Keyboard class
class BleKeyboardModule : public NimBLEServerCallbacks, public NimBLECharacteristicCallbacks {
public:
    BleKeyboardModule(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel);
    void begin(NimBLEServer* pServer);  // Now accepts server as parameter
    void end(void);
    bool isConnected(void);
    void setBatteryLevel(uint8_t level);
    void setName(std::string deviceName);
    void setDelay(uint32_t ms);
    void set_vendor_id(uint16_t vid);
    void set_product_id(uint16_t pid);
    void set_version(uint16_t version);
    
    // Keyboard functions
    size_t press(uint8_t k);
    size_t press(const MediaKeyReport k);
    size_t release(uint8_t k);
    size_t release(const MediaKeyReport k);
    void releaseAll(void);
    size_t write(uint8_t c);
    size_t write(const MediaKeyReport c);
    size_t write(const uint8_t *buffer, size_t size);
    
    // Server callbacks
    void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc) override;
    void onDisconnect(NimBLEServer* pServer) override;
    void onWrite(NimBLECharacteristic* pCharacteristic) override;
    
private:
    void sendReport(KeyReport* keys);
    void sendReport(MediaKeyReport* keys);
    void delay_ms(uint64_t ms);
    
    NimBLEHIDDevice* hid;
    NimBLECharacteristic* inputKeyboard;
    NimBLECharacteristic* outputKeyboard;
    NimBLECharacteristic* inputMediaKeys;
    NimBLEAdvertising* advertising;
    
    std::string deviceName;
    std::string deviceManufacturer;
    uint8_t batteryLevel;
    bool connected;
    uint32_t _delay_ms;
    uint16_t vid;
    uint16_t pid;
    uint16_t version;
    
    KeyReport _keyReport;
    MediaKeyReport _mediaKeyReport;
};

// Function declarations
bool ble_keyboard_init(NimBLEServer* pServer);  // Now accepts server as parameter
void ble_keyboard_loop(void);
bool ble_keyboard_is_connected(void);
void ble_keyboard_send_text(const char* text);
void ble_keyboard_send_media_key(uint8_t key);

#endif /* BLE_KEYBOARD_H_ */
