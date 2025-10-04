#ifndef BLE_VESC_DRIVER_H
#define BLE_VESC_DRIVER_H

#include <Arduino.h>
#include <NimBLEDevice.h>
#include "VESC_SDK_Driver.h"

// BLE Configuration
extern int MTU_SIZE;
extern int PACKET_SIZE;
extern bool deviceConnected;
extern bool oldDeviceConnected;

// BLE Service and Characteristic UUIDs for VESC communication
#define VESC_SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define VESC_CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define VESC_CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// BLE Server Callbacks
class MyServerCallbacks : public BLEServerCallbacks
{
public:
  void onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc);
  void onDisconnect(NimBLEServer *pServer);
  void onMTUChange(uint16_t MTU, ble_gap_conn_desc *desc);
};

// BLE Characteristic Callbacks
class MyCallbacks : public BLECharacteristicCallbacks
{
public:
  void onWrite(BLECharacteristic *pCharacteristic);
};

// Function declarations
bool BLE_Init();
void BLE_Loop();
bool BLE_IsConnected();
void BLE_SendVescData(const vesc_sdk_data_t& data);
void BLE_ProcessReceivedData();

#endif // BLE_VESC_DRIVER_H
