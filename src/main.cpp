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
#include "debug_log.h"               // Logging system

void DriverTask(void *parameter) {
  LOG_INFO(SYSTEM, "🚀 DriverTask started");
  LOG_INFO(SYSTEM, "🔧 BLE Mode: %s", ble_config_get_mode_name(ble_config_get_mode()));
  LOG_INFO(SYSTEM, "📋 Description: %s", ble_config_get_mode_desc(ble_config_get_mode()));
  LOG_INFO(SYSTEM, "📡 CAN Bus: TX=GPIO6, RX=GPIO0, Speed=250kbps, Device ID=%d\n", CONF_CONTROLLER_ID);
  
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
  LOG_INFO(SYSTEM, "VESC Display Starting...");
  
  // Initialize display and backlight first
  Backlight_Init();
  LCD_Init();
  
  // Initialize touch screen (it will initialize its own I2C)
  if (Touch_Init()) {
    LOG_INFO(SYSTEM, "Touch screen initialized successfully!");
    //Touch_Debug_Info();  // Print debug information
  } else {
    LOG_ERROR(SYSTEM, "Touch screen initialization failed!");
  }
  
  // Initialize I2C for other peripherals on different pins
  I2C_Init();
  
  // Initialize VESC handler
  vesc_handler_init();
  
  // Initialize CAN communication
  uint8_t vesc_can_id = CONF_CONTROLLER_ID;
  comm_can_start(GPIO_NUM_6, GPIO_NUM_0, vesc_can_id);
  
  // Set CAN packet handler based on current mode
  if (ble_config_get_mode() == BLE_MODE_BRIDGE) {
    // ============================================================================
    // BLE-CAN Bridge Mode: Forward CAN responses to BLE
    // ============================================================================
    auto packet_handler_wrapper = [](unsigned char *data, unsigned int len) {
      // Forward CAN responses to BLE
      BLE_OnCANResponse(data, len);
    };
    comm_can_set_packet_handler(packet_handler_wrapper);
  } else {
    // ============================================================================
    // Direct Mode: Process CAN messages locally
    // ============================================================================
    comm_can_set_packet_handler(vesc_handler_process_command);
  }
  
  LOG_RAW("\n╔════════════════════════════════════════════════╗\n");
  LOG_RAW("║      🚀 CAN Communication Started 🚀          ║\n");
  LOG_RAW("╠════════════════════════════════════════════════╣\n");
  LOG_RAW("║ Hardware:           %-25s ║\n", HW_NAME);
  LOG_RAW("║ Firmware:           v%d.%02d                     ║\n", FW_VERSION_MAJOR, FW_VERSION_MINOR);
  LOG_RAW("║ Device CAN ID:      %3d                       ║\n", vesc_can_id);
  LOG_RAW("║ Device Type:        HW_TYPE_CUSTOM_MODULE     ║\n");
  LOG_RAW("║ CAN Speed:          250 kbps                  ║\n");
  LOG_RAW("║ TX Pin:             GPIO 6                    ║\n");
  LOG_RAW("║ RX Pin:             GPIO 0                    ║\n");
  LOG_RAW("╠════════════════════════════════════════════════╣\n");
  
  // Display mode-specific info based on current runtime mode
  if (ble_config_get_mode() == BLE_MODE_BRIDGE) {
    LOG_RAW("║ 🌉 BLE Mode:        BRIDGE (vesc_express)     ║\n");
    LOG_RAW("║ 📱 BLE Device:      SuperVESCDisplay          ║\n");
    LOG_RAW("║ 📋 Local Commands:  ENABLED (ID=2)            ║\n");
    LOG_RAW("║ 🔄 CAN Forwarding:  ENABLED (all other IDs)   ║\n");
  } else {
    LOG_RAW("║ 🌉 BLE Mode:        DIRECT (standalone)       ║\n");
    LOG_RAW("║ 📱 BLE Device:      SuperVESCDisplay          ║\n");
    LOG_RAW("║ 📋 Local Commands:  ALL commands processed    ║\n");
    LOG_RAW("║ 🔄 CAN Forwarding:  DISABLED                  ║\n");
  }
  
  LOG_RAW("╚════════════════════════════════════════════════╝\n");
  LOG_RAW("\n⏳ Waiting for BLE/CAN messages...\n\n");
  
  // Initialize BLE Server
  if (BLE_Init()) {
    LOG_INFO(SYSTEM, "BLE initialized successfully");
    
    // Register BLE response callback in vesc_handler
    vesc_handler_set_response_callback(BLE_SendFramedResponse);
    LOG_INFO(SYSTEM, "BLE response callback registered in VESC handler");
  } else {
    LOG_ERROR(SYSTEM, "BLE initialization failed");
  }
  
  // Initialize LVGL with dashboard
  Lvgl_Init();

  // Start the VESC display and communication task
  LOG_INFO(SYSTEM, "VESC Display Ready!");
  Driver_Loop();
}

void loop()
{
  Lvgl_Loop();
  //Serial.println("LVGL loop");
  //Touch_Loop();  // Process touch interrupts
  vTaskDelay(pdMS_TO_TICKS(5));
}
