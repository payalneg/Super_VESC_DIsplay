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
#include "ble_config.h"              // BLE mode configuration
#include "comm_can.h"                // VESC CAN implementation from vesc_express
#include "ble_vesc_driver.h"         // BLE VESC Bridge
#include "buffer.h"                  // Buffer utility functions
#include "datatypes.h"               // VESC data types
#include "vesc_handler.h"            // VESC command handler

void DriverTask(void *parameter) {
  Serial.printf("[%lu] 🚀 DriverTask started\n", millis());
  Serial.printf("[%lu] 🔧 BLE Mode: %s\n", millis(), BLE_MODE_NAME);
  Serial.printf("[%lu] 📋 Description: %s\n", millis(), BLE_MODE_DESC);
  Serial.printf("[%lu] 📡 CAN Bus: TX=GPIO6, RX=GPIO0, Speed=250kbps, Device ID=%d\n\n", millis(), CONF_CONTROLLER_ID);
  
  while(1){
    BLE_Loop();       // Process BLE communication
    
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
  Serial.printf("[%lu] VESC Display Starting...\n", millis());
  
  // Initialize display and backlight first
  Backlight_Init();
  LCD_Init();
  
  // Initialize touch screen (it will initialize its own I2C)
  if (Touch_Init()) {
    Serial.printf("[%lu] Touch screen initialized successfully!\n", millis());
    //Touch_Debug_Info();  // Print debug information
  } else {
    Serial.printf("[%lu] Touch screen initialization failed!\n", millis());
  }
  
  // Initialize I2C for other peripherals on different pins
  I2C_Init();
  
  // Initialize VESC handler
  vesc_handler_init();
  
  // Initialize CAN communication
  uint8_t vesc_can_id = CONF_CONTROLLER_ID;
  comm_can_start(GPIO_NUM_6, GPIO_NUM_0, vesc_can_id);
  
#ifdef BLE_MODE_BRIDGE
  // ============================================================================
  // BLE-CAN Bridge Mode: Forward CAN responses to BLE
  // ============================================================================
  auto packet_handler_wrapper = [](unsigned char *data, unsigned int len) {
    // Forward CAN responses to BLE
    BLE_OnCANResponse(data, len);
  };
  comm_can_set_packet_handler(packet_handler_wrapper);
  
#else // BLE_MODE_DIRECT
  // ============================================================================
  // Direct Mode: Process CAN messages locally
  // ============================================================================
  comm_can_set_packet_handler(vesc_handler_process_command);
  
#endif
  
  Serial.println("\n╔════════════════════════════════════════════════╗");
  Serial.println("║      🚀 CAN Communication Started 🚀          ║");
  Serial.println("╠════════════════════════════════════════════════╣");
  Serial.printf("[%lu] ║ Hardware:           %-25s ║\n", millis(), HW_NAME);
  Serial.printf("[%lu] ║ Firmware:           v%d.%02d                     ║\n", millis(), FW_VERSION_MAJOR, FW_VERSION_MINOR);
  Serial.printf("[%lu] ║ Device CAN ID:      %3d                       ║\n", millis(), vesc_can_id);
  Serial.printf("[%lu] ║ Device Type:        HW_TYPE_CUSTOM_MODULE     ║\n", millis());
  Serial.printf("[%lu] ║ CAN Speed:          250 kbps                  ║\n", millis());
  Serial.printf("[%lu] ║ TX Pin:             GPIO 6                    ║\n", millis());
  Serial.printf("[%lu] ║ RX Pin:             GPIO 0                    ║\n", millis());
  Serial.println("╠════════════════════════════════════════════════╣");
#ifdef BLE_MODE_BRIDGE
  Serial.println("║ 🌉 BLE Mode:        BRIDGE (vesc_express)     ║");
  Serial.println("║ 📱 BLE Device:      SuperVESCDisplay          ║");
  Serial.println("║ 📋 Local Commands:  ENABLED (ID=2)            ║");
  Serial.println("║ 🔄 CAN Forwarding:  ENABLED (all other IDs)   ║");
#else
  Serial.println("║ 🌉 BLE Mode:        DIRECT (standalone)       ║");
  Serial.println("║ 📱 BLE Device:      SuperVESCDisplay          ║");
  Serial.println("║ 📋 Local Commands:  ALL commands processed    ║");
  Serial.println("║ 🔄 CAN Forwarding:  DISABLED                  ║");
#endif
  Serial.println("╚════════════════════════════════════════════════╝");
  Serial.println("\n⏳ Waiting for BLE/CAN messages...\n");
  
  // Initialize BLE Server
  if (BLE_Init()) {
    Serial.printf("[%lu] ✅ BLE initialized successfully\n", millis());
    
    // Register BLE response callback in vesc_handler
    vesc_handler_set_response_callback(BLE_SendFramedResponse);
    Serial.printf("[%lu] ✅ BLE response callback registered in VESC handler\n", millis());
  } else {
    Serial.printf("[%lu] ❌ BLE initialization failed\n", millis());
  }
  
  // Initialize LVGL with dashboard
  Lvgl_Init();

  // Start the VESC display and communication task
  Serial.printf("[%lu] VESC Display Ready!\n", millis());
  Driver_Loop();
}

void loop()
{
  Lvgl_Loop();
  //Serial.println("LVGL loop");
  //Touch_Loop();  // Process touch interrupts
  vTaskDelay(pdMS_TO_TICKS(5));
}
