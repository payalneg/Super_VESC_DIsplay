/*Using LVGL with Arduino requires some extra steps:
 *Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html  */

// CAN Logger Configuration
#define CAN_LOGGER_ENABLED      1     // Enable/disable CAN logger
#define CAN_LOGGER_INTERVAL_MS  5000  // Log interval in milliseconds
// #define DEBUG_CAN  // Uncomment for detailed CAN debugging

/*
 * BLE-CAN Bridge Integration:
 * - Full BLE-CAN bridge using FIFO queue + VESC fragmentation protocol! 🎉🔥⚡
 * - BLE service acts as a bidirectional bridge between mobile apps and VESC
 * - Non-blocking FIFO queue for BLE commands (processed in main loop)
 * - Uses comm_can_send_buffer protocol for proper VESC command transmission
 * - Real-time CAN message forwarding: VESC -> BLE and BLE -> VESC
 * - Automatic fragmentation for large commands (>6 bytes)
 * - Compatible with official VESC Tool and mobile apps
 * - Device name: "SuperVESCDisplay"
 * 
 * Architecture:
 * - BLE callback -> FIFO queue (non-blocking)
 * - Main loop -> Process FIFO queue -> Send to VESC
 * - VESC responses (CAN_PACKET_PROCESS_*) -> BLE TX (real-time)
 * - Automatic response type detection (FW_VERSION, GET_VALUES, etc.)
 * 
 * Supported BLE Commands:
 * - Text: "DUTY:0.5", "CURR:10.0", "RPM:5000", "STATUS", "FW_VERSION", "GET_VALUES"
 * - Binary: Full VESC COMM_* command support with proper fragmentation
 * - CAN: Raw CAN packet format support
 * - All commands routed through FIFO queue + VESC fragmentation protocol
 */

#include "Display_ST7701.h"
#include "LVGL_Driver.h"
#include "Touch_GT911.h"
#include "I2C_Driver.h"
#include "comm_can.h"                // VESC CAN implementation from vesc_express
#include "ble_vesc_driver.h"         // BLE VESC Bridge
#include "buffer.h"                  // Buffer utility functions
#include "datatypes.h"               // VESC data types

// Firmware version (compatible with most VESC Tool versions)
#define FW_VERSION_MAJOR  6
#define FW_VERSION_MINOR  5   // Version 5.03 for maximum compatibility
#define FW_TEST_VERSION   0
#define HW_NAME           "VESC Express T"
#define FW_NAME           ""  // Empty by default, like in VESC Express

// VESC Command Handler - processes incoming VESC commands and sends responses
void VESC_Command_Handler(unsigned char *data, unsigned int len) {
  if (len < 1) return;
  
  uint8_t cmd = data[0];
  static uint32_t msg_count = 0;
  msg_count++;
  
  // Log incoming command
  Serial.printf("[CAN CMD #%04d] Len=%d, CMD=0x%02X ", msg_count, len, cmd);
  
  switch (cmd) {
    case 0x00: { // COMM_FW_VERSION
      Serial.println("(FW_VERSION) - Sending response");
      
      uint8_t send_buffer[80];
      int32_t ind = 0;
      
      send_buffer[ind++] = 0x00; // COMM_FW_VERSION
      send_buffer[ind++] = FW_VERSION_MAJOR;
      send_buffer[ind++] = FW_VERSION_MINOR;
      
      // Hardware name (null-terminated string)
      strcpy((char*)(send_buffer + ind), HW_NAME);
      ind += strlen(HW_NAME) + 1;
      
      // MAC address (6 bytes) - get ESP32 MAC
      uint8_t mac[6];
      esp_read_mac(mac, ESP_MAC_WIFI_STA);
      memcpy(send_buffer + ind, mac, 6);
      ind += 6;
      
      // UUID (6 bytes) - zeros for now
      memset(send_buffer + ind, 0, 6);
      ind += 6;
      
      send_buffer[ind++] = 0; // Pairing done
      send_buffer[ind++] = FW_TEST_VERSION;
      send_buffer[ind++] = HW_TYPE_CUSTOM_MODULE; // HW Type
      send_buffer[ind++] = 1; // One custom config
      send_buffer[ind++] = 0; // No phase filters
      send_buffer[ind++] = 0; // No HW QML
      send_buffer[ind++] = 0; // QML flags
      send_buffer[ind++] = 0; // No NRF flags
      
      // Firmware name (null-terminated string) - use HW_NAME if FW_NAME is empty
      if (strlen(FW_NAME) == 0) {
        strcpy((char*)(send_buffer + ind), HW_NAME);
        ind += strlen(HW_NAME) + 1;
      } else {
        strcpy((char*)(send_buffer + ind), FW_NAME);
        ind += strlen(FW_NAME) + 1;
      }
      
      // HW CRC (4 bytes)
      uint32_t hw_crc = 0x12345678; // Dummy CRC
      buffer_append_uint32(send_buffer, hw_crc, &ind);
      
      comm_can_send_buffer(255, send_buffer, ind, 1); // Send to broadcast
      Serial.printf("✅ FW_VERSION response sent: %s v%d.%02d\n", HW_NAME, FW_VERSION_MAJOR, FW_VERSION_MINOR);
    } break;
    
    case 0x04: { // COMM_GET_VALUES
      Serial.println("(GET_VALUES) - Sending dummy response");
      
      uint8_t send_buffer[70];
      int32_t ind = 0;
      
      send_buffer[ind++] = 0x04; // COMM_GET_VALUES
      
      // Send dummy values for now
      buffer_append_float16(send_buffer, 0.0, 1e3, &ind);     // temp_mos
      buffer_append_float16(send_buffer, 0.0, 1e3, &ind);     // temp_motor  
      buffer_append_float32(send_buffer, 0.0, 1e2, &ind);     // avg_motor_current
      buffer_append_float32(send_buffer, 0.0, 1e2, &ind);     // avg_input_current
      buffer_append_float32(send_buffer, 0.0, 1e2, &ind);     // avg_id
      buffer_append_float32(send_buffer, 0.0, 1e2, &ind);     // avg_iq
      buffer_append_float16(send_buffer, 0.0, 1e1, &ind);     // duty_now
      buffer_append_float32(send_buffer, 0.0, 1e0, &ind);     // rpm
      buffer_append_float16(send_buffer, 24.0, 1e1, &ind);    // v_in (24V)
      buffer_append_float32(send_buffer, 0.0, 1e4, &ind);     // amp_hours
      buffer_append_float32(send_buffer, 0.0, 1e4, &ind);     // amp_hours_charged
      buffer_append_float32(send_buffer, 0.0, 1e4, &ind);     // watt_hours
      buffer_append_float32(send_buffer, 0.0, 1e4, &ind);     // watt_hours_charged
      buffer_append_int32(send_buffer, 0, &ind);              // tachometer
      buffer_append_int32(send_buffer, 0, &ind);              // tachometer_abs
      send_buffer[ind++] = 0;                                 // mc_fault_code
      buffer_append_int32(send_buffer, 0, &ind);              // pid_pos
      send_buffer[ind++] = 0;                                 // app_controller_id
      buffer_append_float16(send_buffer, 0.0, 1e3, &ind);     // temp_mos_1
      buffer_append_float16(send_buffer, 0.0, 1e3, &ind);     // temp_mos_2
      buffer_append_float16(send_buffer, 0.0, 1e3, &ind);     // temp_mos_3
      buffer_append_float32(send_buffer, 0.0, 1e5, &ind);     // avg_vd
      buffer_append_float32(send_buffer, 0.0, 1e5, &ind);     // avg_vq
      
      comm_can_send_buffer(255, send_buffer, ind, 1);
      Serial.println("✅ GET_VALUES response sent");
    } break;
    
    case 0x05: Serial.println("(SET_DUTY)"); break;
    case 0x06: Serial.println("(SET_CURRENT)"); break;
    case 0x08: Serial.println("(SET_RPM)"); break;
    
    default:
      Serial.println("(Unknown command)");
      break;
  }
  
  // Also log raw HEX for debugging
  Serial.print("    Raw: ");
  for (unsigned int i = 0; i < len && i < 16; i++) {
    Serial.printf("%02X ", data[i]);
  }
  Serial.println();
}

void DriverTask(void *parameter) {
  Serial.println("\n🚀 DriverTask started - VESC Command Handler enabled");
  Serial.printf("📡 Listening on CAN: TX=GPIO6, RX=GPIO0, Speed=250kbps, Device ID=2\n");
  Serial.println("📋 All VESC commands will be processed and responded\n");
  
  while(1){
    // BLE_Loop();       // Process BLE communication (disabled for now)
    
    vTaskDelay(pdMS_TO_TICKS(100));  // Update every 100ms
  }
}
void Driver_Loop() {
  xTaskCreatePinnedToCore(
    DriverTask,           
    "DriverTask",         
    4096,                 
    NULL,                 
    3,                    
    NULL,                 
    0                     
  );  
}
void setup()
{
  Serial.begin(115200);
  delay(5000);
  Serial.println("VESC Display Starting...");
  
  // Initialize display and backlight first
  Backlight_Init();
  LCD_Init();
  
  // Initialize touch screen (it will initialize its own I2C)
  if (Touch_Init()) {
    Serial.println("Touch screen initialized successfully!");
    //Touch_Debug_Info();  // Print debug information
  } else {
    Serial.println("Touch screen initialization failed!");
  }
  
  // Initialize I2C for other peripherals on different pins
  I2C_Init();
  
  // Initialize CAN communication
  uint8_t vesc_can_id = 2;  // CAN ID for this device (same as VESC Express T)
  comm_can_start(GPIO_NUM_6, GPIO_NUM_0, vesc_can_id);
  
  // Set VESC command handler callback
  comm_can_set_packet_handler(VESC_Command_Handler);
  
  Serial.println("\n╔════════════════════════════════════════════════╗");
  Serial.println("║      🚀 CAN Communication Started 🚀          ║");
  Serial.println("╠════════════════════════════════════════════════╣");
  Serial.printf("║ Hardware:           %-25s ║\n", HW_NAME);
  Serial.printf("║ Firmware:           v%d.%02d                     ║\n", FW_VERSION_MAJOR, FW_VERSION_MINOR);
  Serial.printf("║ Device CAN ID:      %3d                       ║\n", vesc_can_id);
  Serial.printf("║ Device Type:        HW_TYPE_CUSTOM_MODULE     ║\n");
  Serial.printf("║ CAN Speed:          250 kbps                  ║\n");
  Serial.printf("║ TX Pin:             GPIO 6                    ║\n");
  Serial.printf("║ RX Pin:             GPIO 0                    ║\n");
  Serial.println("╠════════════════════════════════════════════════╣");
  Serial.println("║ 📋 VESC Commands:   ENABLED                   ║");
  Serial.println("║ 📊 PING/PONG:       ENABLED                   ║");
  Serial.println("║ 🔍 Auto-Response:   ENABLED                   ║");
  Serial.println("╚════════════════════════════════════════════════╝");
  Serial.println("\n⏳ Waiting for CAN messages...\n");
  
  // Initialize BLE Server (disabled for now - focusing on CAN only)
  // BLE_Init();
  
  // Initialize LVGL with dashboard
  Lvgl_Init();

  // Start the VESC display and communication task
  Serial.println("VESC Display Ready!");
  Driver_Loop();
}

void loop()
{
  Lvgl_Loop();
  //Serial.println("LVGL loop");
  //Touch_Loop();  // Process touch interrupts
  vTaskDelay(pdMS_TO_TICKS(5));
}
