#include "Touch_GT911.h"
#include <Wire.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/*
 * IMPORTANT (ESP32-S3-Touch-LCD-4 / Waveshare):
 * - Do NOT call Wire.begin() here. Wire is initialized once in main.cpp
 *   (board_display_powerup_sequence) on the correct pins (SDA=15, SCL=7).
 * - All I2C transactions must be protected by the global I2C mutex to avoid
 *   collisions with EXIO(TCA9554) and other I2C users (LVGL/touch).
 */

// Global I2C mutex created in main.cpp
extern SemaphoreHandle_t g_i2c_mutex;

static inline void i2c_lock() {
  if (g_i2c_mutex) xSemaphoreTake(g_i2c_mutex, portMAX_DELAY);
}
static inline void i2c_unlock() {
  if (g_i2c_mutex) xSemaphoreGive(g_i2c_mutex);
}

struct GT911_Touch touch_data = {0};
uint8_t GT911_Current_Address = GT911_SLAVE_ADDRESS_L;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// I2C Communication Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Touch_I2C_Read(uint8_t Driver_addr, uint16_t Reg_addr, uint8_t *Reg_data, uint32_t Length)
{
  if (!Reg_data || Length == 0) return false;

  i2c_lock();

  Wire.beginTransmission(Driver_addr);
  Wire.write((uint8_t)(Reg_addr >> 8));
  Wire.write((uint8_t)(Reg_addr & 0xFF));

  if (Wire.endTransmission(false) != 0) {
    i2c_unlock();
    // printf("The I2C transmission fails. - Touch I2C Read\r\n");
    return false;
  }

  uint32_t got = Wire.requestFrom((int)Driver_addr, (int)Length, (int)true);
  if (got != Length) {
    while (Wire.available()) (void)Wire.read();
    i2c_unlock();
    return false;
  }

  for (uint32_t i = 0; i < Length; i++) {
    if (!Wire.available()) {
      i2c_unlock();
      return false;
    }
    *Reg_data++ = Wire.read();
  }

  i2c_unlock();
  return true;
}

bool Touch_I2C_Write(uint8_t Driver_addr, uint16_t Reg_addr, const uint8_t *Reg_data, uint32_t Length)
{
  if ((Length > 0) && !Reg_data) return false;

  i2c_lock();

  Wire.beginTransmission(Driver_addr);
  Wire.write((uint8_t)(Reg_addr >> 8));
  Wire.write((uint8_t)(Reg_addr & 0xFF));

  for (uint32_t i = 0; i < Length; i++) {
    Wire.write(*Reg_data++);
  }

  bool ok = (Wire.endTransmission(true) == 0);
  i2c_unlock();

  if (!ok) {
    // printf("The I2C transmission fails. - Touch I2C Write\r\n");
    return false;
  }
  return true;
}

uint8_t GT911_Read_Register(uint16_t reg)
{
  uint8_t value = 0x00;
  (void)Touch_I2C_Read(GT911_Current_Address, reg, &value, 1);
  return value;
}

bool GT911_Write_Register(uint16_t reg, uint8_t value)
{
  return Touch_I2C_Write(GT911_Current_Address, reg, &value, 1);
}

uint8_t GT911_Detect_Address(void)
{
  uint8_t id_buf[4] = {0};

  // Try default address first (0x5D)
  GT911_Current_Address = GT911_SLAVE_ADDRESS_L;
  if (Touch_I2C_Read(GT911_Current_Address, GT911_PRODUCT_ID, id_buf, 4)) {
    printf("GT911 detected at address 0x%02X\r\n", GT911_Current_Address);
    return GT911_Current_Address;
  }

  // Try alternative address (0x14)
  GT911_Current_Address = GT911_SLAVE_ADDRESS_H;
  if (Touch_I2C_Read(GT911_Current_Address, GT911_PRODUCT_ID, id_buf, 4)) {
    printf("GT911 detected at address 0x%02X\r\n", GT911_Current_Address);
    return GT911_Current_Address;
  }

  printf("GT911 not detected at any address!\r\n");
  return 0;
}

uint8_t GT911_Touch_Reset(void)
{
  if (GT911_RST_PIN == -1) return false;

  digitalWrite(GT911_RST_PIN, HIGH);
  delay(10);
  digitalWrite(GT911_RST_PIN, LOW);
  delay(5);
  digitalWrite(GT911_RST_PIN, HIGH);
  delay(50);
  return true;
}

uint8_t Touch_Init(void)
{
  printf("Initializing GT911 touch controller...\r\n");

  // IMPORTANT: Do NOT call Wire.begin() here.
  // Wire is already initialized once in board_display_powerup_sequence().
  delay(20);

  if (GT911_INT_PIN != -1) {
    pinMode(GT911_INT_PIN, INPUT);
  }

  if (GT911_RST_PIN != -1) {
    pinMode(GT911_RST_PIN, OUTPUT);
    GT911_Touch_Reset();
  }

  // DEBUG ONLY:
  // I2C_Scanner();

  // Detect GT911 I2C address
  if (GT911_Detect_Address() == 0) {
    printf("Touch initialization failed - GT911 not found!\r\n");
    printf("Check I2C connections and power supply.\r\n");
    return false;
  }

  // Read product ID with detected address
  uint8_t id_buf[4] = {0};
  if (Touch_I2C_Read(GT911_Current_Address, GT911_PRODUCT_ID, id_buf, 4)) {
    printf("GT911 Product ID: %c%c%c%c (Address: 0x%02X)\r\n",
           id_buf[0], id_buf[1], id_buf[2], id_buf[3], GT911_Current_Address);
  } else {
    printf("Failed to read GT911 product ID!\r\n");
    return false;
  }

  // Get firmware version
  uint8_t fw_ver[2] = {0};
  if (Touch_I2C_Read(GT911_Current_Address, GT911_FIRMWARE_VERSION, fw_ver, 2)) {
    uint16_t version = (uint16_t)fw_ver[0] | ((uint16_t)fw_ver[1] << 8);
    printf("GT911 Firmware Version: 0x%04X\r\n", version);
  }

  // Get resolution
  uint8_t x_res[2] = {0}, y_res[2] = {0};
  if (Touch_I2C_Read(GT911_Current_Address, GT911_X_RESOLUTION, x_res, 2) &&
      Touch_I2C_Read(GT911_Current_Address, GT911_Y_RESOLUTION, y_res, 2)) {
    uint16_t x_resolution = (uint16_t)x_res[0] | ((uint16_t)x_res[1] << 8);
    uint16_t y_resolution = (uint16_t)y_res[0] | ((uint16_t)y_res[1] << 8);
    printf("GT911 Resolution: %dx%d\r\n", x_resolution, y_resolution);
  }

  if (GT911_INT_PIN != -1) {
    // NOTE: "interrupt" comes from your original code; keep as-is for compatibility.
    attachInterrupt(GT911_INT_PIN, Touch_GT911_ISR, interrupt);
  }

  return true;
}

uint8_t Touch_Read_Data(void)
{
  uint8_t point_info = 0;
  uint8_t touch_cnt = 0;
  uint8_t buffer[40];

  // Read point info register
  if (!Touch_I2C_Read(GT911_Current_Address, GT911_POINT_INFO, &point_info, 1)) {
    return false;
  }

  // Check buffer status - only proceed if data is ready
  if (!GT911_GET_BUFFER_STATUS(point_info)) {
    return false;
  }

  touch_cnt = GT911_GET_POINT(point_info);

  // Read all touch points first (then clear buffer status)
  if (touch_cnt > 0 && touch_cnt <= GT911_LCD_TOUCH_MAX_POINTS) {
    if (!Touch_I2C_Read(GT911_Current_Address, GT911_POINT_1, buffer, touch_cnt * 8)) {
      return false;
    }
  }

  // Clear buffer status AFTER reading points
  uint8_t clear = 0x00;
  (void)GT911_Write_Register(GT911_POINT_INFO, clear);

  if (touch_cnt == 0 || touch_cnt > GT911_LCD_TOUCH_MAX_POINTS) {
    touch_data.points = 0;
    return true;
  }

  noInterrupts();
  touch_data.points = touch_cnt;

  for (uint8_t i = 0; i < touch_cnt; i++) {
    uint8_t *point_data = &buffer[i * 8];

    touch_data.coords[i].x = (uint16_t)point_data[1] | ((uint16_t)point_data[2] << 8);
    touch_data.coords[i].y = (uint16_t)point_data[3] | ((uint16_t)point_data[4] << 8);
    touch_data.coords[i].strength = (uint16_t)point_data[5] | ((uint16_t)point_data[6] << 8);
  }
  interrupts();

  return true;
}

uint8_t Touch_Get_XY(uint16_t *x, uint16_t *y, uint16_t *strength, uint8_t *point_num, uint8_t max_point_num)
{
  if (!x || !y || !point_num || max_point_num == 0) {
    return false;
  }

  noInterrupts();

  uint8_t points = touch_data.points;
  if (points > max_point_num) {
    points = max_point_num;
  }

  for (uint8_t i = 0; i < points; i++) {
    x[i] = touch_data.coords[i].x;
    y[i] = touch_data.coords[i].y;
    if (strength) {
      strength[i] = touch_data.coords[i].strength;
    }
  }

  *point_num = points;
  touch_data.points = 0;  // Clear data after reading

  interrupts();

  return (points > 0);
}

uint8_t Touch_interrupts = 0;

void IRAM_ATTR Touch_GT911_ISR(void)
{
  Touch_interrupts = true;
}

void example_touchpad_read(void)
{
  uint16_t touchpad_x[5] = {0};
  uint16_t touchpad_y[5] = {0};
  uint16_t strength[5] = {0};
  uint8_t touchpad_cnt = 0;

  (void)Touch_Read_Data();
  uint8_t touchpad_pressed = Touch_Get_XY(touchpad_x, touchpad_y, strength, &touchpad_cnt, GT911_LCD_TOUCH_MAX_POINTS);

  if (touchpad_pressed && touchpad_cnt > 0) {
    printf("Touch: X=%u Y=%u num=%d\r\n", touchpad_x[0], touchpad_y[0], touchpad_cnt);
  }
}

void Touch_Loop(void)
{
  if (Touch_interrupts) {
    Touch_interrupts = false;
    example_touchpad_read();
  }

  // DEBUG ONLY (disable in production to reduce I2C traffic):
  /*
  static unsigned long last_test = 0;
  if (millis() - last_test > 1000) {
    last_test = millis();
    uint8_t point_info = GT911_Read_Register(GT911_POINT_INFO);
    if (point_info != 0) {
      printf("Touch status: 0x%02X (points: %d, buffer: %s)\r\n",
             point_info, GT911_GET_POINT(point_info),
             GT911_GET_BUFFER_STATUS(point_info) ? "ready" : "not ready");
    }
  }
  */
}

void I2C_Scanner(void)
{
  printf("\n=== I2C Scanner ===\r\n");
  printf("Scanning I2C bus...\r\n");

  int devices_found = 0;

  i2c_lock();
  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();

    if (error == 0) {
      printf("I2C device found at address 0x%02X\r\n", address);
      devices_found++;
    } else if (error == 4) {
      printf("Unknown error at address 0x%02X\r\n", address);
    }
  }
  i2c_unlock();

  if (devices_found == 0) {
    printf("No I2C devices found!\r\n");
  } else {
    printf("Found %d I2C device(s)\r\n", devices_found);
  }
  printf("==================\r\n\n");
}

void Touch_Debug_Info(void)
{
  printf("\n=== GT911 Touch Debug Info ===\r\n");
  printf("Current I2C Address: 0x%02X\r\n", GT911_Current_Address);
  printf("INT Pin: %d, RST Pin: %d\r\n", GT911_INT_PIN, GT911_RST_PIN);
  printf("I2C Frequency: %d Hz\r\n", I2C_MASTER_FREQ_HZ);

  uint8_t point_info = GT911_Read_Register(GT911_POINT_INFO);
  printf("Point Info Register: 0x%02X\r\n", point_info);

  uint8_t config_version = GT911_Read_Register(GT911_CONFIG_VERSION);
  printf("Config Version: 0x%02X\r\n", config_version);

  printf("Interrupt counter: %d\r\n", Touch_interrupts);
  printf("===============================\r\n\n");
}
