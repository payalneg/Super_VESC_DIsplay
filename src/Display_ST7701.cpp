#include "Display_ST7701.h"
#include "Arduino_GFX_Library.h"
#include "display/Arduino_RGB_Display.h"

// Arduino_GFX setup for ESP32-S3-Touch-LCD-4
Arduino_DataBus *bus = new Arduino_SWSPI(
  GFX_NOT_DEFINED /* DC */, LCD_CS_PIN /* CS */,
  2 /* SCK */, 1 /* MOSI */, GFX_NOT_DEFINED /* MISO */);

Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
  RGB_DE_PIN /* DE */, RGB_VSYNC_PIN /* VSYNC */, RGB_HSYNC_PIN /* HSYNC */, RGB_PCLK_PIN /* PCLK */,
  RGB_R0_PIN /* R0 */, RGB_R1_PIN /* R1 */, RGB_R2_PIN /* R2 */, RGB_R3_PIN /* R3 */, RGB_R4_PIN /* R4 */,
  RGB_G0_PIN /* G0 */, RGB_G1_PIN /* G1 */, RGB_G2_PIN /* G2 */, RGB_G3_PIN /* G3 */, RGB_G4_PIN /* G4 */, RGB_G5_PIN /* G5 */,
  RGB_B0_PIN /* B0 */, RGB_B1_PIN /* B1 */, RGB_B2_PIN /* B2 */, RGB_B3_PIN /* B3 */, RGB_B4_PIN /* B4 */,
  1 /* hsync_polarity */, 10 /* hsync_front_porch */, 8 /* hsync_pulse_width */, 50 /* hsync_back_porch */,
  1 /* vsync_polarity */, 10 /* vsync_front_porch */, 8 /* vsync_pulse_width */, 20 /* vsync_back_porch */);

Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
  LCD_WIDTH /* width */, LCD_HEIGHT /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
  bus, GFX_NOT_DEFINED /* RST */, st7701_type1_init_operations, sizeof(st7701_type1_init_operations));

void LCD_Init(void)
{
  Serial.println("Initializing ST7701 Display...");
  
  // Initialize I2C for touch
  Wire.begin(15, 7);
  
  // Initialize the display
  if (!gfx->begin()) {
    Serial.println("Failed to initialize display!");
    return;
  }
  
  Serial.println("Display initialized successfully");
  
  // Initialize touch
  Touch_Init();
  
  // Initialize backlight
  Backlight_Init();
  
  // Clear screen
  gfx->fillScreen(BLACK);
  
  Serial.println("ST7701 Display initialization complete");
}

// Wrapper function for LVGL compatibility
void LCD_addWindow(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t* color)
{
  uint16_t width = Xend - Xstart + 1;
  uint16_t height = Yend - Ystart + 1;
  
  gfx->draw16bitRGBBitmap(Xstart, Ystart, color, width, height);
}

// Legacy function for compatibility - now uses Arduino_GFX
void LCD_SetCursor(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend)
{
  // This function is not needed with Arduino_GFX but kept for compatibility
  // Arduino_GFX handles drawing area internally
}

// Get the GFX object for direct access
Arduino_RGB_Display* LCD_GetGFX(void)
{
  return gfx;
}

// Backlight control
uint8_t LCD_Backlight = 50;

void Backlight_Init()
{
  //No backlight on this display
  return;

  ledcAttachPin(LCD_Backlight_PIN, PWM_Channel);
  ledcSetup(PWM_Channel, Frequency, Resolution);   
  ledcWrite(PWM_Channel, Dutyfactor);  
  Set_Backlight(LCD_Backlight);      //0~100                 
}

void Set_Backlight(uint8_t Light)                     
{
  //No backlight on this display
  return;
  if(Light > Backlight_MAX || Light < 0) {
    printf("Set Backlight parameters in the range of 0 to 100 \r\n");
  } else {
    uint32_t Backlight = Light * 10;
    if(Backlight == 1000) {
      Backlight = 1024;
    }
    ledcWrite(PWM_Channel, Backlight);
  }
}