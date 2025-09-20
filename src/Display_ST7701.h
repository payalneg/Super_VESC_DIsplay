#pragma once
#include <Arduino.h>

#include "Touch_GT911.h"

// Display configuration for ESP32-S3-Touch-LCD-4
#define LCD_WIDTH   480 //LCD width
#define LCD_HEIGHT  480 //LCD height

// This board uses RGB parallel interface, not SPI
// RGB interface pins for ESP32-S3-Touch-LCD-4
#define RGB_DE_PIN      40
#define RGB_VSYNC_PIN   39
#define RGB_HSYNC_PIN   38
#define RGB_PCLK_PIN    41

// RGB data pins
#define RGB_R0_PIN      46
#define RGB_R1_PIN      3
#define RGB_R2_PIN      8
#define RGB_R3_PIN      18
#define RGB_R4_PIN      17

#define RGB_G0_PIN      14
#define RGB_G1_PIN      13
#define RGB_G2_PIN      12
#define RGB_G3_PIN      11
#define RGB_G4_PIN      10
#define RGB_G5_PIN      9

#define RGB_B0_PIN      5
#define RGB_B1_PIN      45
#define RGB_B2_PIN      48
#define RGB_B3_PIN      47
#define RGB_B4_PIN      21

// Control pins
#define LCD_CS_PIN      42
#define LCD_RST_PIN     -1  // Controlled via I2C expander
#define LCD_Backlight_PIN   2
#define PWM_Channel     1       // PWM Channel   
#define Frequency       20000   // PWM frequencyconst        
#define Resolution      10       // PWM resolution ratio     MAX:13
#define Dutyfactor      500     // PWM Dutyfactor      
#define Backlight_MAX   100      

#define VERTICAL   0
#define HORIZONTAL 1

#define Offset_X 0
#define Offset_Y 0


// Forward declaration
class Arduino_RGB_Display;

extern uint8_t LCD_Backlight;

// Function declarations
void LCD_Init(void);
void LCD_SetCursor(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t  Yend);
void LCD_addWindow(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t* color);
Arduino_RGB_Display* LCD_GetGFX(void);

void Backlight_Init(void);
void Set_Backlight(uint8_t Light);
