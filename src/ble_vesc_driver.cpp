#include "ble_vesc_driver.h"
#include "ble_config.h"
#include "comm_can.h"
#include "packet_parser.h"
#include "vesc_handler.h"

// BLE Configuration variables
int MTU_SIZE = 23;
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

// Packet parser for framed VESC protocol
static packet_parser_t ble_packet_parser;

// FIFO Command Queue
#define BLE_QUEUE_SIZE 10
static QueueHandle_t ble_command_queue = NULL;

// BLE Server Callbacks Implementation
void MyServerCallbacks::onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc)
{
  Serial.printf("🔵 BLE Client connected: %s\n", NimBLEAddress(desc->peer_ota_addr).toString().c_str());
  deviceConnected = true;
  NimBLEDevice::startAdvertising();
}

void MyServerCallbacks::onDisconnect(NimBLEServer *pServer)
{
  Serial.printf("[%lu] 🔵 BLE Client disconnected\n", millis());
  deviceConnected = false;
  NimBLEDevice::startAdvertising();
}

void MyServerCallbacks::onMTUChange(uint16_t MTU, ble_gap_conn_desc *desc)
{
  Serial.printf("[%lu] 🔵 BLE MTU changed - new size %d, peer %s\n", millis(), MTU, NimBLEAddress(desc->peer_ota_addr).toString().c_str());
  Serial.printf("[%lu] 🔵 BLE Packet size adjusted to %d bytes\n", millis(), MTU - 3);
  MTU_SIZE = MTU;
  PACKET_SIZE = MTU_SIZE - 3;
  if (PACKET_SIZE > 120) {
    PACKET_SIZE = 120;
  }
}

#ifdef BLE_MODE_BRIDGE
// Track if we're waiting for a CAN response to forward to BLE (Bridge mode only)
static bool waiting_for_can_response = false;
static uint8_t expected_response_vesc_id = 255;
#endif

// Packet processed callback - called when valid packet is parsed from BLE
void BLE_OnPacketParsed(uint8_t* data, uint16_t len) {
  Serial.printf("[%lu] 📦 BLE: Parsed complete packet (%d bytes)\n", millis(), len);
  
  if (len < 1) {
    return;
  }
  
#ifdef BLE_MODE_BRIDGE
  // ============================================================================
  // BLE-CAN Bridge Mode (like vesc_express)
  // Packet parser has already extracted clean payload (without 02/03 framing)
  // Just forward the entire payload to CAN bus
  // ============================================================================
  
  // The data[] is already parsed payload from BLE packet:
  // Example: BLE receives "02 01 11 02 10 03"
  //          Parser extracts: data[0]=0x11 (payload), len=1
  //          0x11 = 17 = CAN_PACKET_PING
  
  Serial.printf("[%lu] 📦 BLE→CAN: Received payload (%d bytes): ", millis(), len);
  for (int i = 0; i < len && i < 16; i++) {
    Serial.printf("%02X ", data[i]);
  }
  if (len > 16) Serial.printf("... (%d more)", len - 16);
  Serial.println();
  
  // Forward entire payload to CAN bus
  // comm_can_send_buffer handles:
  // - Fragmentation (FILL_RX_BUFFER if >6 bytes)
  // - CRC calculation
  // - PROCESS_RX_BUFFER / PROCESS_SHORT_BUFFER wrapping
  
  uint8_t target_vesc_id = 255; // Broadcast to all VESCs by default
  waiting_for_can_response = true;
  expected_response_vesc_id = target_vesc_id;
  
  Serial.printf("[%lu] 🔄 BLE→CAN: Forwarding to CAN bus (broadcast to all VESCs)\n", millis());
  
  // send_type = 0: commands_send (wait for response and send it back)
  comm_can_send_buffer(target_vesc_id, data, len, 0);
  
#endif // BLE_MODE_BRIDGE

#ifdef BLE_MODE_DIRECT
  // ============================================================================
  // Direct Processing Mode (standalone device)
  // ============================================================================
  
  // Check if this is addressed to our device (default CAN ID = 2)
  uint8_t first_byte = data[0];
  
  // Check if first byte is our device address (02)
  if (first_byte == CONF_CONTROLLER_ID) {
    Serial.printf("📍 BLE→Local: Packet addressed to device (ID=%d)\n", CONF_CONTROLLER_ID);
    
    // Skip the address byte and process the rest as a VESC command
    if (len > 1) {
      vesc_handler_process_command(data + 1, len - 1);
    }
  } else {
    // Otherwise, treat the whole packet as a VESC command
    Serial.printf("📍 BLE→Local: Processing as direct VESC command\n");
    vesc_handler_process_command(data, len);
  }
  
#endif // BLE_MODE_DIRECT
}

// BLE Characteristic Callbacks Implementation
void MyCallbacks::onWrite(BLECharacteristic *pCharacteristic)
{
  //Serial.printf("BLE onWrite to characteristics: %s\n", pCharacteristic->getUUID().toString().c_str());
  std::string rxValue = pCharacteristic->getValue();
  if (rxValue.length() > 0)
  {
    if (pCharacteristic->getUUID().equals(pCharacteristicVescRx->getUUID()))
    {
      Serial.printf("[%lu] 📥 BLE: received %d bytes: ", millis(), rxValue.length());
      for (int i = 0; i < rxValue.length(); i++) {
        Serial.printf("%02X ", (uint8_t)rxValue[i]);
      }
      Serial.println();
      
      // Process each byte through the packet parser
      for (size_t i = 0; i < rxValue.length(); i++) {
        bool packet_complete = packet_parser_process_byte(&ble_packet_parser, 
                                                          (uint8_t)rxValue[i], 
                                                          BLE_OnPacketParsed);
        
        if (packet_complete) {
          Serial.printf("[%lu] ✅ BLE: Packet successfully parsed and processed\n", millis());
        }
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
    
    Serial.printf("[%lu] 🔵 BLE Server initialized - waiting for client connection...\n", millis());
    
    // Initialize command queue
    if (!BLE_InitCommandQueue()) {
      return false;
    }
    
    // Initialize packet parser
    packet_parser_init(&ble_packet_parser);
    Serial.printf("[%lu] 🔵 BLE: Packet parser initialized\n", millis());
    
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
      //Serial.printf("📤 BLE: Sent VESC text data (%d bytes)\n", dataPacket.length());
    }
  } else {
    // Send CAN STATUS packet format (simulated CAN_PACKET_STATUS)
    ble_can_message_t status_msg;
    status_msg.can_id = (0x09 << 8) | 255; // CAN_PACKET_STATUS + VESC ID
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
    //Serial.printf("📤 BLE: Sent VESC CAN status (%d bytes)\n", sizeof(status_msg));
  }
  
  send_text_format = !send_text_format; // Alternate formats
}

// Send raw CAN message via BLE
void BLE_SendRawCANMessage(uint32_t can_id, uint8_t* data, uint8_t len) {
  if (!deviceConnected || !pCharacteristicVescTx) {
    return;
  }

  uint8_t packet_type = (can_id >> 8) & 0xFF;
  uint8_t vesc_id = can_id & 0xFF;
  
  // Check if this is a VESC response packet
  if (packet_type == 0x07 || packet_type == 0x08) { // CAN_PACKET_PROCESS_RX_BUFFER or CAN_PACKET_PROCESS_SHORT_BUFFER
    BLE_SendVESCResponse(can_id, data, len);
    return;
  }

  // Create BLE CAN message structure for other packets
  ble_can_message_t can_msg;
  can_msg.can_id = can_id;
  can_msg.data_length = len;
  memcpy(can_msg.data, data, len < 8 ? len : 8);
  
  // Send as binary data
  if (sizeof(can_msg) <= PACKET_SIZE) {
    pCharacteristicVescTx->setValue((uint8_t*)&can_msg, sizeof(can_msg));
    pCharacteristicVescTx->notify();
    //Serial.printf("📤 BLE: Sent CAN message ID=0x%03X, Len=%d\n", can_id, len);
  }
}

// Send VESC response via BLE (for PROCESS_RX_BUFFER and PROCESS_SHORT_BUFFER responses)
void BLE_SendVESCResponse(uint32_t can_id, uint8_t* data, uint8_t len) {
  if (!deviceConnected || !pCharacteristicVescTx) {
    return;
  }

  uint8_t packet_type = (can_id >> 8) & 0xFF;
  uint8_t vesc_id = can_id & 0xFF;
  
  // Identify response type based on first byte (COMM command)
  vesc_response_type_t resp_type = VESC_RESP_UNKNOWN;
  const char* resp_name = "UNKNOWN";
  
  if (len > 0) {
    switch (data[0]) {
      case 0x00: // COMM_FW_VERSION response
        resp_type = VESC_RESP_FW_VERSION;
        resp_name = "FW_VERSION";
        break;
      case 0x04: // COMM_GET_VALUES response
        resp_type = VESC_RESP_GET_VALUES;
        resp_name = "GET_VALUES";
        break;
      case 0x12: // COMM_PING_CAN response
        resp_type = VESC_RESP_PING;
        resp_name = "PING";
        break;
      default:
        resp_type = VESC_RESP_GENERIC;
        resp_name = "GENERIC";
        break;
    }
  }
  
  //Serial.printf("📤 BLE: VESC %s response (%d bytes)\n", resp_name, len);
  
  // For VESC responses, send the raw data directly (this is the actual COMM response)
  if (len <= PACKET_SIZE) {
    pCharacteristicVescTx->setValue(data, len);
    pCharacteristicVescTx->notify();
    
    //Serial.printf("📤 BLE: Sent VESC %s response (%d bytes): ", resp_name, len);
    //for (int i = 0; i < len && i < 16; i++) { // Limit to first 16 bytes for readability
    //  Serial.printf("%02X ", data[i]);
    //}
    //if (len > 16) Serial.printf("... (%d more bytes)", len - 16);
    //Serial.println();
  } else {
    Serial.printf("⚠️  BLE: VESC response too large (%d > %d bytes)\n", len, PACKET_SIZE);
    
    // For large responses, we might need to fragment them
    // For now, just send what fits
    if (PACKET_SIZE > 0) {
      pCharacteristicVescTx->setValue(data, PACKET_SIZE);
      pCharacteristicVescTx->notify();
      Serial.printf("⚠️  BLE: Sent truncated VESC response (%d of %d bytes)\n", PACKET_SIZE, len);
    }
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
    
    comm_can_transmit_eid(can_id, raw_data, 4);
  } else {
    // Simple format: interpret first byte as command type
    switch (can_data[0]) {
      case 0x00: // Set duty cycle (CAN_PACKET_SET_DUTY)
        {
          float duty = (float)value / 100000.0f; // VESC duty scaling
          Serial.printf("[%lu] 🎯 BLE->CAN: Setting duty %.3f\n", millis(), duty);
          comm_can_set_duty(255, duty);
        }
        break;
        
      case 0x01: // Set current (CAN_PACKET_SET_CURRENT)
        {
          float current = (float)value / 1000.0f; // VESC current scaling
          Serial.printf("[%lu] 🎯 BLE->CAN: Setting current %.2fA\n", millis(), current);
          comm_can_set_current(255, current);
        }
        break;
        
      case 0x02: // Set brake current (CAN_PACKET_SET_CURRENT_BRAKE)
        {
          float current = (float)value / 1000.0f;
          Serial.printf("[%lu] 🎯 BLE->CAN: Setting brake current %.2fA\n", millis(), current);
          comm_can_set_current_brake(255, current);
        }
        break;
        
      case 0x03: // Set RPM (CAN_PACKET_SET_RPM)
        {
          float rpm = (float)value;
          Serial.printf("[%lu] 🎯 BLE->CAN: Setting RPM %.0f\n", millis(), rpm);
          comm_can_set_rpm(255, rpm);
        }
        break;
        
      case 0x11: // Ping (CAN_PACKET_PING)
        Serial.printf("[%lu] 🎯 BLE->CAN: Sending ping\n", millis());
        {
          HW_TYPE hw_type;
          comm_can_ping(255, &hw_type);
        }
        break;
        
      case 0xFF: // Request status (custom command)
        Serial.printf("[%lu] 🎯 BLE->CAN: Requesting VESC status\n", millis());
        // Status is received automatically from VESC STATUS packets
        break;
        
      default:
        Serial.printf("[%lu] ⚠️  BLE->CAN: Unknown command type 0x%02X\n", millis(), can_data[0]);
        break;
    }
  }
}

// Process received BLE data (now with proper VESC command buffer support!)
void BLE_ProcessReceivedData() {
  if (vescBuffer.length() > 0) {
    //Serial.printf("[%lu] 📥 BLE: Processing buffered data (%d bytes)\n", millis(), vescBuffer.length());
    
    // Try to process as binary VESC command first
    if (vescBuffer.length() >= 1) {
      uint8_t first_byte = vescBuffer[0];
      
      // Check if it looks like a VESC command (COMM_* commands are typically 0x00-0x50)
      if (first_byte <= 0x50) {
        //Serial.printf("[%lu] 📥 BLE: Processing as VESC command (0x%02X)\n", millis(), first_byte);
        
        uint8_t target_vesc_id = 255; // Send to any VESC
        uint8_t send_type = 0; // 0 = process and send response back
        
        // Queue using proper VESC fragmentation protocol
        if (BLE_QueueCommand((uint8_t*)vescBuffer.data(), vescBuffer.length(), target_vesc_id, send_type)) {
          //Serial.printf("[%lu] 📥 BLE->Queue: Queued VESC command buffer (%d bytes) for VESC %d\n", millis(), vescBuffer.length(), target_vesc_id);
        } else {
          Serial.printf("[%lu] ❌ BLE: Failed to queue VESC command buffer (%d bytes)\n", millis(), vescBuffer.length());
        }
        vescBuffer.clear();
        return;
      }
    }
    
    // Try to process as CAN message structure
    if (vescBuffer.length() >= sizeof(ble_can_message_t)) {
      //Serial.println("[%lu] 📥 BLE: Processing as CAN message structure", millis());
      BLE_ProcessCANCommand((uint8_t*)vescBuffer.data(), vescBuffer.length());
      vescBuffer.clear();
      return;
    }
    
    // Process as text command (for debugging/testing)
    String command = String(vescBuffer.c_str());
    command.trim();
    
    if (command.length() > 0) {
      //Serial.printf("[%lu] 📥 BLE: Processing as text command: '%s'\n", millis(), command.c_str());
      
      if (command.startsWith("DUTY:")) {
        float duty = command.substring(5).toFloat();
        Serial.printf("[%lu] 🎯 BLE: Text command - Setting duty %.3f\n", millis(), duty);
        comm_can_set_duty(255, duty);
      } else if (command.startsWith("CURR:")) {
        float current = command.substring(5).toFloat();
        Serial.printf("[%lu] 🎯 BLE: Text command - Setting current %.2fA\n", millis(), current);
        comm_can_set_current(255, current);
      } else if (command.startsWith("RPM:")) {
        float rpm = command.substring(4).toFloat();
        Serial.printf("[%lu] 🎯 BLE: Text command - Setting RPM %.0f\n", millis(), rpm);
        comm_can_set_rpm(255, rpm);
      } else if (command == "STATUS") {
        Serial.printf("[%lu] 🎯 BLE: Text command - Requesting status\n", millis());
        // Status is received automatically from VESC STATUS packets
      } else if (command == "FW_VERSION") {
        Serial.printf("[%lu] 🎯 BLE: Text command - Requesting firmware version\n", millis());
        uint8_t fw_cmd = 0x00; // COMM_FW_VERSION
        BLE_QueueCommand(&fw_cmd, 1, 255, 0);
      } else if (command == "GET_VALUES") {
        Serial.printf("[%lu] 🎯 BLE: Text command - Requesting values\n", millis());
        uint8_t get_cmd = 0x04; // COMM_GET_VALUES
        BLE_QueueCommand(&get_cmd, 1, 255, 0);
      } else {
        Serial.printf("[%lu] ⚠️  BLE: Unknown text command: %s\n", millis(), command.c_str());
      }
    }
    
    vescBuffer.clear();
  }
}

// Main BLE processing loop
void BLE_Loop() {
  // Process command queue (FIFO)
  BLE_ProcessCommandQueue();
  
  // Process any buffered received data (legacy)
  //BLE_ProcessReceivedData();
  
  // Send VESC data if connected
  /*
  if (deviceConnected) {
    static unsigned long last_ble_update = 0;
    if (millis() - last_ble_update > 1000) { // Send data every second
      can_status_msg *status = comm_can_get_status_msg_id(255);
      if (status && status->id != -1) {
        // Create a compatible data structure
        vesc_sdk_data_t data;
        data.rpm = status->rpm;
        data.current = status->current;
        data.duty_cycle = status->duty;
        
        can_status_msg_5 *status5 = comm_can_get_status_msg_5_id(255);
        if (status5) {
          data.voltage = status5->v_in;
        }
        
        can_status_msg_4 *status4 = comm_can_get_status_msg_4_id(255);
        if (status4) {
          data.temp_fet = status4->temp_fet;
        }
        
        BLE_SendVescData(data);
      }
      last_ble_update = millis();
    }
  }
  */

  // Handle BLE connection state changes
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);                  // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    //Serial.println("[%lu] 🔵 BLE: Restarted advertising", millis());
    oldDeviceConnected = deviceConnected;
  }
  if (deviceConnected && !oldDeviceConnected) {
    //Serial.println("[%lu] 🔵 BLE: Client connected and ready", millis());
    oldDeviceConnected = deviceConnected;
  }
}

// Initialize FIFO command queue
bool BLE_InitCommandQueue() {
  ble_command_queue = xQueueCreate(BLE_QUEUE_SIZE, sizeof(ble_command_t));
  if (ble_command_queue == NULL) {
    Serial.printf("[%lu] ❌ BLE: Failed to create command queue\n", millis());
    return false;
  }
  //Serial.printf("[%lu] ✅ BLE: Command queue initialized (size: %d)\n", millis(), BLE_QUEUE_SIZE);
  return true;
}

// Queue a command for processing in main loop
bool BLE_QueueCommand(uint8_t* data, uint16_t length, uint8_t target_vesc_id, uint8_t send_type) {
  if (ble_command_queue == NULL) {
    Serial.printf("[%lu] ❌ BLE: Command queue not initialized\n", millis());
    return false;
  }
  
  if (length > BLE_CMD_BUFFER_SIZE) {
    Serial.printf("[%lu] ❌ BLE: Command too large (%d > %d bytes)\n", millis(), length, BLE_CMD_BUFFER_SIZE);
    return false;
  }
  
  ble_command_t cmd;
  memcpy(cmd.data, data, length);
  cmd.length = length;
  cmd.target_vesc_id = target_vesc_id;
  cmd.send_type = send_type;
  
  BaseType_t result = xQueueSend(ble_command_queue, &cmd, 0); // Non-blocking
  if (result != pdTRUE) {
    Serial.printf("[%lu] ⚠️  BLE: Command queue full, dropping command\n", millis());
    return false;
  }
  
  //Serial.printf("[%lu] 📥 BLE: Queued command (%d bytes) for VESC %d\n", millis(), length, target_vesc_id);
  return true;
}

// Process commands from FIFO queue (called from main loop)
void BLE_ProcessCommandQueue() {
  if (ble_command_queue == NULL) {
    return;
  }
  
  ble_command_t cmd;
  while (xQueueReceive(ble_command_queue, &cmd, 0) == pdTRUE) { // Non-blocking
    //Serial.printf("[%lu] 🔄 BLE: Processing queued command (%d bytes) to VESC %d\n", 
    //              millis(), cmd.length, cmd.target_vesc_id);
    
    // Send command using VESC fragmentation protocol
    comm_can_send_buffer(cmd.target_vesc_id, cmd.data, cmd.length, cmd.send_type);
    
    //Serial.printf("[%lu] ✅ BLE: Sent queued command (%d bytes) to VESC %d\n", 
    //              millis(), cmd.length, cmd.target_vesc_id);
  }
}

// Send framed response via BLE (callback for vesc_handler - local commands)
void BLE_SendFramedResponse(uint8_t* data, unsigned int len) {
  if (!deviceConnected || !pCharacteristicVescTx) {
    Serial.printf("[%lu] ⚠️  BLE: Cannot send response - not connected\n", millis());
    return;
  }
  
  // Build framed packet
  uint8_t framed_buffer[600]; // payload + framing overhead
  uint16_t framed_len = packet_build_frame(data, len, framed_buffer, sizeof(framed_buffer));
  
  if (framed_len == 0) {
    Serial.printf("[%lu] ❌ BLE: Failed to build framed packet\n", millis());
    return;
  }
  
  // Send via BLE
  if (framed_len <= PACKET_SIZE) {
    pCharacteristicVescTx->setValue(framed_buffer, framed_len);
    pCharacteristicVescTx->notify();
    
    Serial.printf("[%lu] 📤 BLE←Local: Sent framed response (%d bytes total, %d payload): ", millis(), framed_len, len);
    for (int i = 0; i < framed_len && i < 20; i++) {
      Serial.printf("%02X ", framed_buffer[i]);
    }
    if (framed_len > 20) Serial.printf("... (%d more)", framed_len - 20);
    Serial.println();
  } else {
    // Need to fragment the response
    Serial.printf("[%lu] ⚠️  BLE←Local: Response too large (%d > %d), fragmenting...\n", millis(), framed_len, PACKET_SIZE);
    
    int offset = 0;
    while (offset < framed_len) {
      int chunk_size = (framed_len - offset > PACKET_SIZE) ? PACKET_SIZE : (framed_len - offset);
      
      pCharacteristicVescTx->setValue(framed_buffer + offset, chunk_size);
      pCharacteristicVescTx->notify();
      
      Serial.printf("[%lu] 📤 BLE←Local: Sent fragment %d bytes (offset %d)\n", millis(), chunk_size, offset);
      offset += chunk_size;
      
      // Small delay between fragments
      delay(10);
    }
  }
}

#ifdef BLE_MODE_BRIDGE
// CAN response handler - called when CAN response is received (Bridge mode only)
void BLE_OnCANResponse(uint8_t* data, unsigned int len) {
  // Only forward if we're waiting for a response and BLE is connected
  if (!waiting_for_can_response || !deviceConnected || !pCharacteristicVescTx) {
    return;
  }
  
  Serial.printf("[%lu] 📦 CAN→BLE: Received CAN response (%d bytes), forwarding to BLE\n", millis(), len);
  
  // Forward the response to BLE with framing
  waiting_for_can_response = false; // Reset flag
  
  // Build framed packet
  uint8_t framed_buffer[600];
  uint16_t framed_len = packet_build_frame(data, len, framed_buffer, sizeof(framed_buffer));
  
  if (framed_len == 0) {
    Serial.printf("[%lu] ❌ BLE←CAN: Failed to build framed packet\n", millis());
    return;
  }
  
  // Send via BLE
  if (framed_len <= PACKET_SIZE) {
    pCharacteristicVescTx->setValue(framed_buffer, framed_len);
    pCharacteristicVescTx->notify();
    
    Serial.printf("[%lu] 📤 BLE←CAN: Sent framed response (%d bytes total, %d payload): ", millis(), framed_len, len);
    for (int i = 0; i < framed_len && i < 20; i++) {
      Serial.printf("%02X ", framed_buffer[i]);
    }
    if (framed_len > 20) Serial.printf("... (%d more)", framed_len - 20);
    Serial.println();
  } else {
    // Need to fragment the response
    Serial.printf("[%lu] ⚠️  BLE←CAN: Response too large (%d > %d), fragmenting...\n", millis(), framed_len, PACKET_SIZE);
    
    int offset = 0;
    while (offset < framed_len) {
      int chunk_size = (framed_len - offset > PACKET_SIZE) ? PACKET_SIZE : (framed_len - offset);
      
      pCharacteristicVescTx->setValue(framed_buffer + offset, chunk_size);
      pCharacteristicVescTx->notify();
      
      Serial.printf("[%lu] 📤 BLE←CAN: Sent fragment %d bytes (offset %d)\n", millis(), chunk_size, offset);
      offset += chunk_size;
      
      // Small delay between fragments
      delay(10);
    }
  }
}
#endif // BLE_MODE_BRIDGE
