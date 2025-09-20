#pragma once

#include "Arduino.h"
#include <Wire.h>

// GT911 I2C addresses
#define GT911_SLAVE_ADDRESS_L   0x5D  // Default address
#define GT911_SLAVE_ADDRESS_H   0x14  // Alternative address

// GT911 pins for ESP32-S3-Touch-LCD-4
#define GT911_SDA_PIN       15
#define GT911_SCL_PIN       7
#define GT911_INT_PIN       4
#define GT911_RST_PIN       -1  // Not connected on this board

#define I2C_MASTER_FREQ_HZ  400000

// GT911 registers
#define GT911_POINT_INFO    0x814E
#define GT911_POINT_1       0x814F
#define GT911_CONFIG_VERSION 0x8047
#define GT911_PRODUCT_ID    0x8140
#define GT911_FIRMWARE_VERSION 0x8144
#define GT911_X_RESOLUTION  0x8146
#define GT911_Y_RESOLUTION  0x8148

#define GT911_LCD_TOUCH_MAX_POINTS  5

// GT911 macros
#define GT911_GET_POINT(x)          (x & 0x0F)
#define GT911_GET_BUFFER_STATUS(x)  (x & 0x80)
#define GT911_GET_HAVE_KEY(x)       (x & 0x10)

// Interrupt modes
#define RISING    0x01
#define FALLING   0x02
#define CHANGE    0x03
#define ONLOW     0x04
#define ONHIGH    0x05
#define ONLOW_WE  0x0C
#define ONHIGH_WE 0x0D

#define interrupt FALLING

extern uint8_t Touch_interrupts;

struct GT911_Touch{
  uint8_t points;    // Number of touch points
  struct {
    uint16_t x; /*!< X coordinate */
    uint16_t y; /*!< Y coordinate */
    uint16_t strength; /*!< Strength */
  }coords[GT911_LCD_TOUCH_MAX_POINTS];
};

// Function declarations
uint8_t Touch_Init();
uint8_t GT911_Touch_Reset(void);
uint16_t GT911_Read_cfg(void);
uint8_t Touch_Read_Data(void);
uint8_t Touch_Get_XY(uint16_t *x, uint16_t *y, uint16_t *strength, uint8_t *point_num, uint8_t max_point_num);
void example_touchpad_read(void);
void IRAM_ATTR Touch_GT911_ISR(void);
void Touch_Loop(void);
