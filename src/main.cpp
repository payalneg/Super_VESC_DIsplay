/*Using LVGL with Arduino requires some extra steps:
 *Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html  */

// Uncomment to enable detailed CAN debugging
// #define DEBUG_CAN

/*
 * BLE Bridge Integration:
 * - Added NimBLE server functionality for VESC communication
 * - BLE service acts as a bridge between mobile apps and VESC
 * - Currently sends VESC data via BLE (RPM, Current, Voltage, Temperature)
 * - Prepared for future CAN protocol integration
 * - Device name: "ble_vesc"
 */

#include "Display_ST7701.h"
#include "LVGL_Driver.h"
#include "Touch_GT911.h"
#include "I2C_Driver.h"
#include "VESC_SDK_Driver.h"         // Профессиональный VESC CAN SDK
#include "ble_vesc_driver.h"         // BLE VESC Bridge

void DriverTask(void *parameter) {
  while(1){
    VESC_SDK_Loop();  // Update VESC SDK communication
    BLE_Loop();       // Process BLE communication
    
    // Show VESC data every 2 seconds
    static unsigned long last_print = 0;
    if (millis() - last_print > 2000) {
      if (VESC_SDK_IsConnected()) {
        vesc_sdk_data_t data = VESC_SDK_GetData();
        Serial.printf("🚀 VESC: RPM=%.0f | Current=%.2fA | Voltage=%.1fV | FET=%.1f°C | Motor=%.1f°C | Battery=%.1f%%\n", 
                      data.rpm, data.current, data.voltage, data.temp_fet, data.temp_motor, data.battery_level);
      } else {
        Serial.println("❌ VESC: Disconnected - Check CAN connection and VESC settings");
      }
      last_print = millis();
    }
    
    // Commented out detailed debug for now
    /*
    // Print debug info every 10 seconds
    static unsigned long last_debug = 0;
    if (millis() - last_debug > 10000) {
      VESC_SDK_PrintDebug();
      last_debug = millis();
    }
    */
    
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
  
  // Initialize VESC SDK communication
  if (VESC_SDK_Init(VESC_CAN_ID)) { 
    Serial.println("🚀 VESC SDK initialized successfully!");
    Serial.println("📡 Ready to communicate with VESC via CAN SDK");
    Serial.println("⚙️  Make sure VESC is set to 250 kbps CAN speed and ID 57");
  } else {
    Serial.println("❌ VESC SDK initialization failed!");
  }
  
  // Initialize BLE Server
  BLE_Init();
  
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
