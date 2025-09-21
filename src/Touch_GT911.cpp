#include "Touch_GT911.h"

struct GT911_Touch touch_data = {0};
uint8_t GT911_Current_Address = GT911_SLAVE_ADDRESS_L;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// I2C Communication Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Touch_I2C_Read(uint8_t Driver_addr, uint16_t Reg_addr, uint8_t *Reg_data, uint32_t Length)
{
  Wire.beginTransmission(Driver_addr);
  Wire.write((uint8_t)(Reg_addr >> 8)); 
  Wire.write((uint8_t)Reg_addr);        
  if (Wire.endTransmission(false)) {
    printf("The I2C transmission fails. - Touch I2C Read\r\n");
    return false;
  }
  
  Wire.requestFrom(Driver_addr, Length);
  for (int i = 0; i < Length; i++) {
    if (Wire.available()) {
      *Reg_data++ = Wire.read();
    } else {
      return false;
    }
  }
  return true;
}

bool Touch_I2C_Write(uint8_t Driver_addr, uint16_t Reg_addr, const uint8_t *Reg_data, uint32_t Length)
{
  Wire.beginTransmission(Driver_addr);
  Wire.write((uint8_t)(Reg_addr >> 8)); 
  Wire.write((uint8_t)Reg_addr);        
  for (int i = 0; i < Length; i++) {
    Wire.write(*Reg_data++);
  }
  if (Wire.endTransmission(true)) {
    printf("The I2C transmission fails. - Touch I2C Write\r\n");
    return false;
  }
  return true;
}

uint8_t GT911_Read_Register(uint16_t reg)
{
  uint8_t value = 0x00;
  Touch_I2C_Read(GT911_Current_Address, reg, &value, 1);
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

uint8_t Touch_Init(void) 
{
  printf("Initializing GT911 touch controller...\r\n");
  
  Wire.begin(GT911_SDA_PIN, GT911_SCL_PIN, I2C_MASTER_FREQ_HZ);
  delay(100);  // Give I2C time to initialize
  
  if (GT911_INT_PIN != -1) {
    pinMode(GT911_INT_PIN, INPUT);
  }
  
  if (GT911_RST_PIN != -1) {
    pinMode(GT911_RST_PIN, OUTPUT);
    GT911_Touch_Reset();
  }

  // Scan I2C bus first
  I2C_Scanner();

  // Detect GT911 I2C address
  if (GT911_Detect_Address() == 0) {
    printf("Touch initialization failed - GT911 not found!\r\n");
    printf("Check I2C connections and power supply.\r\n");
    return false;
  }

  // Read product ID with detected address
  uint32_t product_id = 0;
  uint8_t id_buf[4] = {0};
  
  if (Touch_I2C_Read(GT911_Current_Address, GT911_PRODUCT_ID, id_buf, 4)) {
    product_id = (id_buf[3] << 24) | (id_buf[2] << 16) | (id_buf[1] << 8) | id_buf[0];
    printf("GT911 Product ID: %c%c%c%c (Address: 0x%02X)\r\n", id_buf[0], id_buf[1], id_buf[2], id_buf[3], GT911_Current_Address);
  } else {
    printf("Failed to read GT911 product ID!\r\n");
    return false;
  }

  // Get firmware version
  uint8_t fw_ver[2] = {0};
  if (Touch_I2C_Read(GT911_Current_Address, GT911_FIRMWARE_VERSION, fw_ver, 2)) {
    uint16_t version = fw_ver[0] | (fw_ver[1] << 8);
    printf("GT911 Firmware Version: 0x%04X\r\n", version);
  }

  // Get resolution
  uint8_t x_res[2] = {0}, y_res[2] = {0};
  if (Touch_I2C_Read(GT911_Current_Address, GT911_X_RESOLUTION, x_res, 2) &&
      Touch_I2C_Read(GT911_Current_Address, GT911_Y_RESOLUTION, y_res, 2)) {
    uint16_t x_resolution = x_res[0] | (x_res[1] << 8);
    uint16_t y_resolution = y_res[0] | (y_res[1] << 8);
    printf("GT911 Resolution: %dx%d\r\n", x_resolution, y_resolution);
  }

  if (GT911_INT_PIN != -1) {
    attachInterrupt(GT911_INT_PIN, Touch_GT911_ISR, interrupt);
  }

  return true;
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
  
  // Clear buffer status first
  uint8_t clear = 0x00;
  GT911_Write_Register(GT911_POINT_INFO, clear);
  
  if (touch_cnt == 0 || touch_cnt > GT911_LCD_TOUCH_MAX_POINTS) {
    touch_data.points = 0;
    return true;
  }
  
  // Read all touch points
  if (!Touch_I2C_Read(GT911_Current_Address, GT911_POINT_1, buffer, touch_cnt * 8)) {
    return false;
  }
  
  noInterrupts();
  touch_data.points = touch_cnt;
  
  for (uint8_t i = 0; i < touch_cnt; i++) {
    uint8_t *point_data = &buffer[i * 8];
    
    touch_data.coords[i].x = point_data[1] | (point_data[2] << 8);
    touch_data.coords[i].y = point_data[3] | (point_data[4] << 8);
    touch_data.coords[i].strength = point_data[5] | (point_data[6] << 8);
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

void Touch_Loop(void)
{
  if (Touch_interrupts) {
    Touch_interrupts = false;
    example_touchpad_read();
  }
  
  // Periodic touch test (every 1 second) - remove this in production
  static unsigned long last_test = 0;
  if (millis() - last_test > 1000) {
    last_test = millis();
    
    // Try to read touch data periodically for debugging
    uint8_t point_info = GT911_Read_Register(GT911_POINT_INFO);
    if (point_info != 0) {
      printf("Touch status: 0x%02X (points: %d, buffer: %s)\r\n", 
             point_info, GT911_GET_POINT(point_info), 
             GT911_GET_BUFFER_STATUS(point_info) ? "ready" : "not ready");
    }
  }
}

void example_touchpad_read(void)
{
  uint16_t touchpad_x[5] = {0};
  uint16_t touchpad_y[5] = {0};
  uint16_t strength[5] = {0};
  uint8_t touchpad_cnt = 0;
  
  Touch_Read_Data();
  uint8_t touchpad_pressed = Touch_Get_XY(touchpad_x, touchpad_y, strength, &touchpad_cnt, GT911_LCD_TOUCH_MAX_POINTS);
  
  if (touchpad_pressed && touchpad_cnt > 0) {
    printf("Touch: X=%u Y=%u num=%d\r\n", touchpad_x[0], touchpad_y[0], touchpad_cnt);
  }
}

uint8_t Touch_interrupts = 0;

void IRAM_ATTR Touch_GT911_ISR(void) 
{
  Touch_interrupts = true;
}

void I2C_Scanner(void)
{
  printf("\n=== I2C Scanner ===\r\n");
  printf("Scanning I2C bus (SDA=%d, SCL=%d)...\r\n", GT911_SDA_PIN, GT911_SCL_PIN);
  
  int devices_found = 0;
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
  printf("SDA Pin: %d, SCL Pin: %d\r\n", GT911_SDA_PIN, GT911_SCL_PIN);
  printf("INT Pin: %d, RST Pin: %d\r\n", GT911_INT_PIN, GT911_RST_PIN);
  printf("I2C Frequency: %d Hz\r\n", I2C_MASTER_FREQ_HZ);
  
  // Try to read some basic registers
  uint8_t point_info = GT911_Read_Register(GT911_POINT_INFO);
  printf("Point Info Register: 0x%02X\r\n", point_info);
  
  uint8_t config_version = GT911_Read_Register(GT911_CONFIG_VERSION);
  printf("Config Version: 0x%02X\r\n", config_version);
  
  printf("Interrupt counter: %d\r\n", Touch_interrupts);
  printf("===============================\r\n\n");
}
