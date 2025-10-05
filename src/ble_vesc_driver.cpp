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
      Serial.printf("📥 BLE->CAN: received %d bytes: ", rxValue.length());
      for (int i = 0; i < rxValue.length(); i++) {
        Serial.printf("%02X ", (uint8_t)rxValue[i]);
      }
      Serial.println();
      
      // Send data directly to VESC using comm_can_send_buffer protocol
      uint8_t target_vesc_id = VESC_CAN_ID; // Use configured VESC CAN ID
      uint8_t send_type = 0; // 0 = process and send response back
      
      // Send the received data to VESC using proper fragmentation
      VESC_SDK_SendCommandBuffer(target_vesc_id, (uint8_t*)rxValue.data(), rxValue.length(), send_type);
      
      Serial.printf("🔗 BLE->CAN: Forwarded %d bytes to VESC %d\n", rxValue.length(), target_vesc_id);
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
    
    // Set up CAN bridge callback to forward VESC messages to BLE
    VESC_SDK_SetCANBridgeCallback(BLE_SendRawCANMessage);
    
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

// Send VESC data via BLE (now sends both text and CAN format)
void BLE_SendVescData(const vesc_sdk_data_t& data) {
  if (!deviceConnected || !pCharacteristicVescTx) {
    return;
  }

  static bool send_text_format = true; // Alternate between formats for demo
  
  if (send_text_format) {
    // Send human-readable format
    String dataPacket = String("RPM:") + String(data.rpm) + 
                       ",CURR:" + String(data.current) + 
                       ",VOLT:" + String(data.voltage) + 
                       ",TEMP:" + String(data.temp_fet) + 
                       ",DUTY:" + String(data.duty_cycle) + "\n";
    
    if (dataPacket.length() <= PACKET_SIZE) {
      pCharacteristicVescTx->setValue(dataPacket.c_str());
      pCharacteristicVescTx->notify();
      Serial.printf("📤 BLE: Sent VESC text data (%d bytes)\n", dataPacket.length());
    }
  } else {
    // Send CAN STATUS packet format (simulated CAN_PACKET_STATUS)
    ble_can_message_t status_msg;
    status_msg.can_id = (0x09 << 8) | VESC_CAN_ID; // CAN_PACKET_STATUS + configured VESC ID
    status_msg.data_length = 8;
    
    // Pack STATUS data (RPM, current, duty) - same format as real VESC
    int32_t rpm_int = (int32_t)data.rpm;
    int16_t current_int = (int16_t)(data.current * 10.0f);
    int16_t duty_int = (int16_t)(data.duty_cycle * 1000.0f);
    
    // Little endian format
    status_msg.data[0] = rpm_int & 0xFF;
    status_msg.data[1] = (rpm_int >> 8) & 0xFF;
    status_msg.data[2] = (rpm_int >> 16) & 0xFF;
    status_msg.data[3] = (rpm_int >> 24) & 0xFF;
    status_msg.data[4] = current_int & 0xFF;
    status_msg.data[5] = (current_int >> 8) & 0xFF;
    status_msg.data[6] = duty_int & 0xFF;
    status_msg.data[7] = (duty_int >> 8) & 0xFF;
    
    pCharacteristicVescTx->setValue((uint8_t*)&status_msg, sizeof(status_msg));
    pCharacteristicVescTx->notify();
    Serial.printf("📤 BLE: Sent VESC CAN status (%d bytes)\n", sizeof(status_msg));
  }
  
  send_text_format = !send_text_format; // Alternate formats
}

// Send raw CAN message via BLE
void BLE_SendRawCANMessage(uint32_t can_id, uint8_t* data, uint8_t len) {
  if (!deviceConnected || !pCharacteristicVescTx) {
    return;
  }

  // Create BLE CAN message structure
  ble_can_message_t can_msg;
  can_msg.can_id = can_id;
  can_msg.data_length = len;
  memcpy(can_msg.data, data, len < 8 ? len : 8);
  
  // Send as binary data
  if (sizeof(can_msg) <= PACKET_SIZE) {
    pCharacteristicVescTx->setValue((uint8_t*)&can_msg, sizeof(can_msg));
    pCharacteristicVescTx->notify();
    Serial.printf("📤 BLE: Sent CAN message ID=0x%03X, Len=%d\n", can_id, len);
  }
}

// Process CAN command received from BLE
bool BLE_ProcessCANCommand(uint8_t* data, uint8_t len) {
  if (len < sizeof(ble_can_message_t)) {
    Serial.printf("❌ BLE: Invalid CAN message size: %d\n", len);
    return false;
  }
  
  ble_can_message_t* can_msg = (ble_can_message_t*)data;
  
  Serial.printf("📥 BLE: Received CAN command ID=0x%03X, Len=%d, Data=", 
                can_msg->can_id, can_msg->data_length);
  for (int i = 0; i < can_msg->data_length; i++) {
    Serial.printf("%02X ", can_msg->data[i]);
  }
  Serial.println();
  
  // Forward to VESC via CAN
  BLE_ForwardCANToVESC(can_msg->data, can_msg->data_length);
  return true;
}

// Forward CAN data to VESC (using existing CAN infrastructure)
void BLE_ForwardCANToVESC(uint8_t* can_data, uint8_t len) {
  if (len < 4) {
    Serial.printf("⚠️  BLE->CAN: Invalid data length %d\n", len);
    return;
  }
  
  // Parse as real VESC CAN packet format
  // First 4 bytes: little-endian int32 value
  int32_t value = (can_data[3] << 24) | (can_data[2] << 16) | (can_data[1] << 8) | can_data[0];
  
  // Check if we have a CAN ID in the message (extended format)
  if (len >= 8) {
    // Extended format: [CAN_ID:4][DATA:4]
    uint32_t can_id = (can_data[7] << 24) | (can_data[6] << 16) | (can_data[5] << 8) | can_data[4];
    uint8_t packet_type = (can_id >> 8) & 0xFF;
    uint8_t vesc_id = can_id & 0xFF;
    
    Serial.printf("🎯 BLE->CAN: Raw packet Type=0x%02X, VESC=%d, Value=%d\n", packet_type, vesc_id, value);
    
    // Send raw CAN message
    uint8_t raw_data[4];
    raw_data[0] = can_data[0];
    raw_data[1] = can_data[1]; 
    raw_data[2] = can_data[2];
    raw_data[3] = can_data[3];
    
    VESC_SDK_SendRawCAN(can_id, raw_data, 4);
  } else {
    // Simple format: interpret first byte as command type
    switch (can_data[0]) {
      case 0x00: // Set duty cycle (CAN_PACKET_SET_DUTY)
        {
          float duty = (float)value / 100000.0f; // VESC duty scaling
          Serial.printf("🎯 BLE->CAN: Setting duty %.3f\n", duty);
          VESC_SDK_SetDuty(duty);
        }
        break;
        
      case 0x01: // Set current (CAN_PACKET_SET_CURRENT)
        {
          float current = (float)value / 1000.0f; // VESC current scaling
          Serial.printf("🎯 BLE->CAN: Setting current %.2fA\n", current);
          VESC_SDK_SetCurrent(current);
        }
        break;
        
      case 0x02: // Set brake current (CAN_PACKET_SET_CURRENT_BRAKE)
        {
          float current = (float)value / 1000.0f;
          Serial.printf("🎯 BLE->CAN: Setting brake current %.2fA\n", current);
          VESC_SDK_SetBrakeCurrent(current);
        }
        break;
        
      case 0x03: // Set RPM (CAN_PACKET_SET_RPM)
        {
          float rpm = (float)value;
          Serial.printf("🎯 BLE->CAN: Setting RPM %.0f\n", rpm);
          VESC_SDK_SetRPM(rpm);
        }
        break;
        
      case 0x11: // Ping (CAN_PACKET_PING)
        Serial.println("🎯 BLE->CAN: Sending ping");
        VESC_SDK_Ping();
        break;
        
      case 0xFF: // Request status (custom command)
        Serial.println("🎯 BLE->CAN: Requesting VESC status");
        VESC_SDK_RequestStatus();
        break;
        
      default:
        Serial.printf("⚠️  BLE->CAN: Unknown command type 0x%02X\n", can_data[0]);
        break;
    }
  }
}

// Process received BLE data (now with proper VESC command buffer support!)
void BLE_ProcessReceivedData() {
  if (vescBuffer.length() > 0) {
    Serial.printf("📥 BLE: Processing buffered data (%d bytes)\n", vescBuffer.length());
    
    // Try to process as binary VESC command first
    if (vescBuffer.length() >= 1) {
      uint8_t first_byte = vescBuffer[0];
      
      // Check if it looks like a VESC command (COMM_* commands are typically 0x00-0x50)
      if (first_byte <= 0x50) {
        Serial.printf("📥 BLE: Processing as VESC command (0x%02X)\n", first_byte);
        
        uint8_t target_vesc_id = VESC_CAN_ID; // Use configured VESC CAN ID
        uint8_t send_type = 0; // 0 = process and send response back
        
        // Send using proper VESC fragmentation protocol
        VESC_SDK_SendCommandBuffer(target_vesc_id, (uint8_t*)vescBuffer.data(), vescBuffer.length(), send_type);
        
        Serial.printf("🔗 BLE->CAN: Sent VESC command buffer (%d bytes) to VESC %d\n", vescBuffer.length(), target_vesc_id);
        vescBuffer.clear();
        return;
      }
    }
    
    // Try to process as CAN message structure
    if (vescBuffer.length() >= sizeof(ble_can_message_t)) {
      Serial.println("📥 BLE: Processing as CAN message structure");
      BLE_ProcessCANCommand((uint8_t*)vescBuffer.data(), vescBuffer.length());
      vescBuffer.clear();
      return;
    }
    
    // Process as text command (for debugging/testing)
    String command = String(vescBuffer.c_str());
    command.trim();
    
    if (command.length() > 0) {
      Serial.printf("📥 BLE: Processing as text command: '%s'\n", command.c_str());
      
      if (command.startsWith("DUTY:")) {
        float duty = command.substring(5).toFloat();
        Serial.printf("🎯 BLE: Text command - Setting duty %.3f\n", duty);
        VESC_SDK_SetDuty(duty);
      } else if (command.startsWith("CURR:")) {
        float current = command.substring(5).toFloat();
        Serial.printf("🎯 BLE: Text command - Setting current %.2fA\n", current);
        VESC_SDK_SetCurrent(current);
      } else if (command.startsWith("RPM:")) {
        float rpm = command.substring(4).toFloat();
        Serial.printf("🎯 BLE: Text command - Setting RPM %.0f\n", rpm);
        VESC_SDK_SetRPM(rpm);
      } else if (command == "STATUS") {
        Serial.println("🎯 BLE: Text command - Requesting status");
        VESC_SDK_RequestStatus();
      } else if (command == "FW_VERSION") {
        Serial.println("🎯 BLE: Text command - Requesting firmware version");
        uint8_t fw_cmd = 0x00; // COMM_FW_VERSION
        VESC_SDK_SendCommandBuffer(VESC_CAN_ID, &fw_cmd, 1, 0);
      } else if (command == "GET_VALUES") {
        Serial.println("🎯 BLE: Text command - Requesting values");
        uint8_t get_cmd = 0x04; // COMM_GET_VALUES
        VESC_SDK_SendCommandBuffer(VESC_CAN_ID, &get_cmd, 1, 0);
      } else {
        Serial.printf("⚠️  BLE: Unknown text command: %s\n", command.c_str());
      }
    }
    
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
