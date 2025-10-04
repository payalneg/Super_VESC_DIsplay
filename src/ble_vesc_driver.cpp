#include "ble_vesc_driver.h"

// BLE Configuration variables
int MTU_SIZE = 128;
int PACKET_SIZE = MTU_SIZE - 3;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// BLE Objects
static NimBLEServer *pServer = nullptr;
static NimBLEService *pServiceVesc = nullptr;
static NimBLECharacteristic *pCharacteristicVescTx = nullptr;
static NimBLECharacteristic *pCharacteristicVescRx = nullptr;

// Buffer for BLE data (will be used for CAN communication later)
static std::string vescBuffer;
static std::string updateBuffer;

// BLE Server Callbacks Implementation
void MyServerCallbacks::onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc)
{
  Serial.printf("BLE Client connected: %s\n", NimBLEAddress(desc->peer_ota_addr).toString().c_str());
  deviceConnected = true;
  NimBLEDevice::startAdvertising();
}

void MyServerCallbacks::onDisconnect(NimBLEServer *pServer)
{
  Serial.println("BLE Client disconnected - start advertising");
  deviceConnected = false;
  NimBLEDevice::startAdvertising();
}

void MyServerCallbacks::onMTUChange(uint16_t MTU, ble_gap_conn_desc *desc)
{
  Serial.printf("BLE MTU changed - new size %d, peer %s\n", MTU, NimBLEAddress(desc->peer_ota_addr).toString().c_str());
  MTU_SIZE = MTU;
  PACKET_SIZE = MTU_SIZE - 3;
}

// BLE Characteristic Callbacks Implementation
void MyCallbacks::onWrite(BLECharacteristic *pCharacteristic)
{
  Serial.printf("BLE onWrite to characteristics: %s\n", pCharacteristic->getUUID().toString().c_str());
  std::string rxValue = pCharacteristic->getValue();
  if (rxValue.length() > 0)
  {
    if (pCharacteristic->getUUID().equals(pCharacteristicVescRx->getUUID()))
    {
      Serial.printf("BLE/UART => CAN: received %d bytes\n", rxValue.length());
      // TODO: Here we will send data via CAN instead of Serial
      // For now, just store in buffer for future CAN transmission
      for (int i = 0; i < rxValue.length(); i++)
      {
        vescBuffer.push_back(rxValue[i]);
      }
    }
  }
}

// Initialize BLE Server
bool BLE_Init() {
  try {
    // Create the BLE Device
    NimBLEDevice::init("SuperVESCDisplay");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    // Create the BLE Server
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    auto pSecurity = new NimBLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

    // Create the BLE Service
    BLEService *pService = pServer->createService(VESC_SERVICE_UUID);

    // Create a BLE TX Characteristic
    pCharacteristicVescTx = pService->createCharacteristic(
        VESC_CHARACTERISTIC_UUID_TX,
        NIMBLE_PROPERTY::NOTIFY |
            NIMBLE_PROPERTY::READ);

    // Create a BLE RX Characteristic
    pCharacteristicVescRx = pService->createCharacteristic(
        VESC_CHARACTERISTIC_UUID_RX,
        NIMBLE_PROPERTY::WRITE |
            NIMBLE_PROPERTY::WRITE_NR);

    pCharacteristicVescRx->setCallbacks(new MyCallbacks());

    // Start the VESC service
    pService->start();

    // Start advertising
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(VESC_SERVICE_UUID);
    pAdvertising->start();
    
    Serial.println("🔵 BLE Server initialized - waiting for client connection...");
    return true;
  }
  catch (const std::exception& e) {
    Serial.printf("❌ BLE initialization failed: %s\n", e.what());
    return false;
  }
}

// Check if BLE is connected
bool BLE_IsConnected() {
  return deviceConnected;
}

// Send VESC data via BLE
void BLE_SendVescData(const vesc_sdk_data_t& data) {
  if (!deviceConnected || !pCharacteristicVescTx) {
    return;
  }

  // Create a simple data packet (this will be replaced with proper CAN protocol later)
  String dataPacket = String("RPM:") + String(data.rpm) + 
                     ",CURR:" + String(data.current) + 
                     ",VOLT:" + String(data.voltage) + 
                     ",TEMP:" + String(data.temp_fet) + "\n";
  
  if (dataPacket.length() <= PACKET_SIZE) {
    pCharacteristicVescTx->setValue(dataPacket.c_str());
    pCharacteristicVescTx->notify();
    Serial.printf("📤 BLE: Sent VESC data (%d bytes)\n", dataPacket.length());
  }
}

// Process received BLE data (placeholder for future CAN integration)
void BLE_ProcessReceivedData() {
  if (vescBuffer.length() > 0) {
    Serial.printf("📥 BLE: Processing %d bytes of received data\n", vescBuffer.length());
    // TODO: Process received data and send via CAN
    // For now, just clear the buffer
    vescBuffer.clear();
  }
}

// Main BLE processing loop
void BLE_Loop() {
  // Process any received data
  BLE_ProcessReceivedData();
  
  // Send VESC data if connected and VESC is available
  if (deviceConnected && VESC_SDK_IsConnected()) {
    static unsigned long last_ble_update = 0;
    if (millis() - last_ble_update > 1000) { // Send data every second
      vesc_sdk_data_t data = VESC_SDK_GetData();
      BLE_SendVescData(data);
      last_ble_update = millis();
    }
  }

  // Handle BLE connection state changes
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);                  // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("🔵 BLE: Restarted advertising");
    oldDeviceConnected = deviceConnected;
  }
  if (deviceConnected && !oldDeviceConnected) {
    Serial.println("🔵 BLE: Client connected and ready");
    oldDeviceConnected = deviceConnected;
  }
}
