#ifndef BLE_VESC_DRIVER_H
#define BLE_VESC_DRIVER_H

#include <Arduino.h>
#include <NimBLEDevice.h>
#include "comm_can.h"
#include "datatypes.h"

// Compatibility structure for BLE
typedef struct {
    float rpm;
    float current;
    float duty_cycle;
    float voltage;
    float temp_fet;
} vesc_sdk_data_t;
#include "driver/twai.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// CAN Message Structure for BLE transmission
typedef struct {
    uint32_t can_id;
    uint8_t data_length;
    uint8_t data[8];
} ble_can_message_t;

// BLE Command Buffer Structure for FIFO
#define BLE_CMD_BUFFER_SIZE 256
typedef struct {
    uint8_t data[BLE_CMD_BUFFER_SIZE];
    uint16_t length;
    uint8_t target_vesc_id;
    uint8_t send_type;
} ble_command_t;

// VESC Response Types (for better response handling)
typedef enum {
    VESC_RESP_UNKNOWN = 0,
    VESC_RESP_FW_VERSION,
    VESC_RESP_GET_VALUES,
    VESC_RESP_PING,
    VESC_RESP_GENERIC
} vesc_response_type_t;

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

// CAN Bridge Functions
void BLE_SendRawCANMessage(uint32_t can_id, uint8_t* data, uint8_t len);
bool BLE_ProcessCANCommand(uint8_t* data, uint8_t len);
void BLE_ForwardCANToVESC(uint8_t* can_data, uint8_t len);
void BLE_SendVESCResponse(uint32_t can_id, uint8_t* data, uint8_t len);

// FIFO Buffer Functions
bool BLE_InitCommandQueue();
bool BLE_QueueCommand(uint8_t* data, uint16_t length, uint8_t target_vesc_id, uint8_t send_type);
void BLE_ProcessCommandQueue();

#endif // BLE_VESC_DRIVER_H
