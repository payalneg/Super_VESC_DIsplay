# VESC Display Adaptation for ESP32-S3-Touch-LCD-4

This project has been adapted from ESP32-S3-Touch-LCD-2.8 to work with the Waveshare ESP32-S3-Touch-LCD-4.

## Hardware Changes

### Display
- **From**: ST7789 (320x240) SPI interface
- **To**: ST7701 (480x480) RGB parallel interface

### Touch Controller
- **From**: CST328 
- **To**: GT911

### Pin Configuration

#### Display (RGB Parallel Interface)
```
RGB_DE_PIN      = 40
RGB_VSYNC_PIN   = 39
RGB_HSYNC_PIN   = 38
RGB_PCLK_PIN    = 41

RGB_R0_PIN      = 46
RGB_R1_PIN      = 3
RGB_R2_PIN      = 8
RGB_R3_PIN      = 18
RGB_R4_PIN      = 17

RGB_G0_PIN      = 14
RGB_G1_PIN      = 13
RGB_G2_PIN      = 12
RGB_G3_PIN      = 11
RGB_G4_PIN      = 10
RGB_G5_PIN      = 9

RGB_B0_PIN      = 5
RGB_B1_PIN      = 45
RGB_B2_PIN      = 48
RGB_B3_PIN      = 47
RGB_B4_PIN      = 21

LCD_CS_PIN      = 42
LCD_Backlight_PIN = 2
```

#### Touch Controller (GT911)
```
GT911_SDA_PIN   = 15
GT911_SCL_PIN   = 7
GT911_INT_PIN   = 4
GT911_RST_PIN   = -1  // Not connected
```

## Software Changes

### New Files Created
- `src/Touch_GT911.h` - GT911 touch controller header
- `src/Touch_GT911.cpp` - GT911 touch controller implementation

### Modified Files
- `src/Display_ST7701.h` - Updated for RGB interface and new pins
- `src/Display_ST7701.cpp` - Completely rewritten for Arduino_GFX library
- `src/LVGL_Arduino.ino` - Updated includes for new drivers
- `src/LVGL_Driver.h` - Updated includes
- `src/LVGL_Driver.cpp` - Updated touch reading for GT911
- `platformio.ini` - Updated project name and added Arduino_GFX library
- `User_Setup1.h` - Added comments about RGB interface

### Key Technical Changes

1. **Display Interface**: Changed from SPI to RGB parallel interface using Arduino_GFX library
2. **Touch Controller**: Implemented GT911 driver with I2C communication
3. **Resolution**: Updated from 320x240 to 480x480
4. **Library Dependencies**: Added Arduino_GFX library for RGB display support

## Libraries Required

- `lvgl/lvgl@^8.3.10`
- `moononournation/GFX Library for Arduino@^1.4.7`

## Compilation Notes

The project uses:
- Arduino_GFX library for RGB display management
- Custom GT911 driver for touch input
- LVGL for GUI framework
- Original VESC communication drivers (unchanged)

## Testing Checklist

- [ ] Display initialization
- [ ] Touch input functionality  
- [ ] Backlight control
- [ ] VESC CAN communication
- [ ] LVGL dashboard rendering

## Hardware Notes

This board (ESP32-S3-Touch-LCD-4) features:
- 4-inch 480x480 RGB display with ST7701 controller
- GT911 5-point capacitive touch
- ESP32-S3-N16R8 module (16MB Flash, 8MB PSRAM)
- I2C expander for additional GPIO
- CAN and RS485 interfaces
- MicroSD card slot
- RTC with battery backup

For more information, see: https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4
