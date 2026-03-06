#include "I2C_Driver.h"
#include <Wire.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// Use the same mutex as main.cpp (must NOT be static there)
extern SemaphoreHandle_t g_i2c_mutex;

static inline void i2c_lock() {
  if (g_i2c_mutex) xSemaphoreTake(g_i2c_mutex, portMAX_DELAY);
}
static inline void i2c_unlock() {
  if (g_i2c_mutex) xSemaphoreGive(g_i2c_mutex);
}

// IMPORTANT:
// On ESP32-S3-Touch-LCD-4 the display/touch already owns Wire with SDA=15 SCL=7.
// Re-calling Wire.begin() with other pins can break panel/touch.
// So keep I2C_Init() as a safe no-op.
void I2C_Init(void) {
  // Do NOT call Wire.begin() here.
  // Wire is initialized once in board_display_powerup_sequence().
}

bool I2C_Read(uint8_t Driver_addr, uint8_t Reg_addr, uint8_t *Reg_data, uint32_t Length) {
  if (!Reg_data || Length == 0) return -1;

  i2c_lock();

  Wire.beginTransmission(Driver_addr);
  Wire.write(Reg_addr);

  // Use repeated start (false) because most register reads expect it.
  if (Wire.endTransmission(false) != 0) {
    // NACK / bus issue
    i2c_unlock();
    // printf("The I2C transmission fails. - I2C Read\r\n");
    return -1;
  }

  // Request bytes; check how many we actually got
  uint32_t got = Wire.requestFrom((int)Driver_addr, (int)Length, (int)true);
  if (got != Length) {
    // Drain whatever is available to keep bus clean
    while (Wire.available()) (void)Wire.read();
    i2c_unlock();
    return -1;
  }

  for (uint32_t i = 0; i < Length; i++) {
    if (!Wire.available()) {
      i2c_unlock();
      return -1;
    }
    *Reg_data++ = Wire.read();
  }

  i2c_unlock();
  return 0;
}

bool I2C_Write(uint8_t Driver_addr, uint8_t Reg_addr, const uint8_t *Reg_data, uint32_t Length) {
  if ((Length > 0) && !Reg_data) return -1;

  i2c_lock();

  Wire.beginTransmission(Driver_addr);
  Wire.write(Reg_addr);

  for (uint32_t i = 0; i < Length; i++) {
    Wire.write(*Reg_data++);
  }

  if (Wire.endTransmission(true) != 0) {
    i2c_unlock();
    // printf("The I2C transmission fails. - I2C Write\r\n");
    return -1;
  }

  i2c_unlock();
  return 0;
}
