/*Using LVGL with Arduino requires some extra steps:
 *Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html  */

#include "Display_ST7701.h"
#include "LVGL_Driver.h"
#include "Touch_GT911.h"
#include "I2C_Driver.h"
#include "VESC_Driver.h"
#include "VESC_CAN_Driver.h"

void DriverTask(void *parameter) {
  while(1){
    VESC_CAN_Loop();  // Update VESC communication
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
  delay(2000);
  Serial.println("VESC Display Starting...");
  
  // Initialize display and backlight first
  Backlight_Init();
  LCD_Init();
  
  // Initialize touch screen (it will initialize its own I2C)
  if (Touch_Init()) {
    Serial.println("Touch screen initialized successfully!");
    Touch_Debug_Info();  // Print debug information
  } else {
    Serial.println("Touch screen initialization failed!");
  }
  
  // Initialize I2C for other peripherals on different pins
  I2C_Init();
  
  // Initialize VESC communication
  //VESC_CAN_Init();
  
  // Initialize LVGL with dashboard
  Lvgl_Init();

  // Start the VESC display
  Serial.println("VESC Display Ready!");
  //Driver_Loop();
}

void loop()
{
  Lvgl_Loop();
  //Touch_Loop();  // Process touch interrupts
  vTaskDelay(pdMS_TO_TICKS(5));
}
