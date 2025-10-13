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
#include "vesc_rt_data.h"            // RT data module
#include "ui_updater.h"              // UI updater module
#include "debug_log.h"               // Logging system
#include "settings.h"                // Settings system

void setup()
{
  Serial.begin(115200);
  //delay(5000);
  LOG_INFO(SYSTEM, "VESC Display Starting...");
  
  // Initialize settings system first (loads from NVS)
  settings_init();
  
  // Initialize display and backlight
  Backlight_Init();
  LCD_Init();
  
  // Apply saved brightness setting
  settings_apply_brightness();
  LOG_INFO(SYSTEM, "Screen brightness set to %d%%", settings_get_screen_brightness());
  
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
  
  // Initialize CAN communication with settings
  uint8_t vesc_can_id = settings_get_controller_id();
  int can_speed = (int)settings_get_can_speed();
  comm_can_start(GPIO_NUM_6, GPIO_NUM_0, vesc_can_id, can_speed);
  
  // Initialize RT data module with target VESC ID from settings
  vesc_rt_data_init();
  
  // Set CAN packet handler for Bridge mode
  // ============================================================================
  // BLE-CAN Bridge Mode: Forward CAN responses to BLE + Process RT data
  // ============================================================================
  auto packet_handler_wrapper = [](unsigned char *data, unsigned int len) {
    // Process RT data responses
    vesc_rt_data_process_response(data, len);
    
    // Forward CAN responses to BLE
    BLE_OnCANResponse(data, len);
  };
  comm_can_set_packet_handler(packet_handler_wrapper);
  
  LOG_RAW("\n╔════════════════════════════════════════════════╗\n");
  LOG_RAW("║      🚀 CAN Communication Started 🚀          ║\n");
  LOG_RAW("╠════════════════════════════════════════════════╣\n");
  LOG_RAW("║ Hardware:           %-25s ║\n", HW_NAME);
  LOG_RAW("║ Firmware:           v%d.%02d                     ║\n", FW_VERSION_MAJOR, FW_VERSION_MINOR);
  LOG_RAW("║ Device CAN ID:      %3d                       ║\n", vesc_can_id);
  LOG_RAW("║ Target VESC ID:     %3d                       ║\n", settings_get_target_vesc_id());
  LOG_RAW("║ Device Type:        HW_TYPE_CUSTOM_MODULE     ║\n");
  LOG_RAW("║ CAN Speed:          %4d kbps                 ║\n", can_speed);
  LOG_RAW("║ Screen Brightness:  %3d%%                      ║\n", settings_get_screen_brightness());
  LOG_RAW("║ TX Pin:             GPIO 6                    ║\n");
  LOG_RAW("║ RX Pin:             GPIO 0                    ║\n");
  LOG_RAW("╠════════════════════════════════════════════════╣\n");
  
  // Display BLE Bridge mode info
  LOG_RAW("║ 🌉 BLE Mode:        BRIDGE (vesc_express)     ║\n");
  LOG_RAW("║ 📱 BLE Device:      SuperVESCDisplay          ║\n");
  LOG_RAW("║ 📋 Local Commands:  ENABLED (ID=%d)            ║\n", vesc_can_id);
  LOG_RAW("║ 🔄 CAN Forwarding:  ENABLED (all other IDs)   ║\n");
  
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
  
  // Initialize LVGL with dashboard (creates UI elements)
  Lvgl_Init();
  
  // Set all UI elements to zero (before any updates start)
  ui_updater_set_zeros();
  
  // Initialize UI updater timer
  ui_updater_init();
  
  // Start RT data requests
  vesc_rt_data_start();
  LOG_INFO(SYSTEM, "RT data requests started");
  
  // Start UI automatic updates
  ui_updater_start();
  LOG_INFO(SYSTEM, "UI updates started");

  // Start the VESC display and communication task
  LOG_INFO(SYSTEM, "VESC Display Ready!");
}

void loop()
{
  BLE_Loop();              // Process BLE communication
  vesc_rt_data_loop();     // Process RT data requests
  ui_updater_update();     // Update UI with VESC data (checks 50ms interval internally)
  ui_updater_update_fps(); // Update FPS counter independently
  Lvgl_Loop();
  //Serial.println("LVGL loop");
  //Touch_Loop();  // Process touch interrupts
  vTaskDelay(pdMS_TO_TICKS(5));
}
