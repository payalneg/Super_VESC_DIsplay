/*Using LVGL with Arduino requires some extra steps:
 *Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html  */

#include "Display_ST7701.h"
#include "LVGL_Driver.h"
#include "Touch_GT911.h"
#include "I2C_Driver.h"
// #include "VESC_Driver.h"           // Отключено - конфликт с SDK
// #include "VESC_CAN_Driver.h"       // Отключено - конфликт с SDK  
// #include "VESC_TWAI_Driver.h"      // Отключено - конфликт с SDK
// #include "VESC_TWAI_Interface.h"   // Отключено - конфликт с SDK
#include "VESC_SDK_Driver.h"

void DriverTask(void *parameter) {
  while(1){
    VESC_SDK_Loop();  // Update VESC SDK communication
    
    // Print VESC status every 2 seconds
    static unsigned long last_print = 0;
    if (millis() - last_print > 2000) {
      if (VESC_SDK_IsConnected()) {
        float speed = VESC_SDK_GetSpeed();
        float voltage = VESC_SDK_GetBatteryVoltage();
        float percentage = VESC_SDK_GetBatteryPercentage();
        float power = VESC_SDK_GetPower();
        float temp_fet = VESC_SDK_GetTempFET();
        float temp_motor = VESC_SDK_GetTempMotor();
        
        Serial.printf("VESC SDK: %.1f km/h | %.1fV (%.0f%%) | %.0fW | FET:%.0f°C Motor:%.0f°C\n", 
                      speed, voltage, percentage, power, temp_fet, temp_motor);
      } else {
        Serial.println("VESC SDK: Disconnected - Check CAN bus connection");
      }
      last_print = millis();
    }
    
    vTaskDelay(pdMS_TO_TICKS(50));  // Faster updates for VESC data
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
  if (VESC_SDK_Init(0)) {  // VESC ID = 0
    Serial.println("VESC SDK initialized successfully!");
    
    // Configuration removed - using VESC controller settings
    // Can be added later if needed for speed/battery calculations
    
  } else {
    Serial.println("VESC SDK initialization failed!");
  }
  
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
