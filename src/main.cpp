/*Using LVGL with Arduino requires some extra steps:
 *Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html  */

// CAN Logger Configuration
#define CAN_LOGGER_ENABLED      1     // Enable/disable CAN logger
#define CAN_LOGGER_INTERVAL_MS  5000  // Log interval in milliseconds
// #define DEBUG_CAN  // Uncomment for detailed CAN debugging

/*
 * BLE-CAN Bridge Integration:
 * - Full BLE-CAN bridge using FIFO queue + VESC fragmentation protocol! 🎉🔥⚡
 * - BLE service acts as a bidirectional bridge between mobile apps and VESC
 * - Non-blocking FIFO queue for BLE commands (processed in main loop)
 * - Uses comm_can_send_buffer protocol for proper VESC command transmission
 * - Real-time CAN message forwarding: VESC -> BLE and BLE -> VESC
 * - Automatic fragmentation for large commands (>6 bytes)
 * - Compatible with official VESC Tool and mobile apps
 * - Device name: "SuperVESCDisplay"
 *
 * Architecture:
 * - BLE callback -> FIFO queue (non-blocking)
 * - Main loop -> Process FIFO queue -> Send to VESC
 * - VESC responses (CAN_PACKET_PROCESS_*) -> BLE TX (real-time)
 * - Automatic response type detection (FW_VERSION, GET_VALUES, etc.)
 *
 * Supported BLE Commands:
 * - Text: "DUTY:0.5", "CURR:10.0", "RPM:5000", "STATUS", "FW_VERSION", "GET_VALUES"
 * - Binary: Full VESC COMM_* command support with proper fragmentation
 * - CAN: Raw CAN packet format support
 * - All commands routed through FIFO queue + VESC fragmentation protocol
 */

#include "Display_ST7701.h"
#include "LVGL_Driver.h"
#include "Touch_GT911.h"
#include "I2C_Driver.h"

#include "ble_config.h"
#include "comm_can.h"
#include "ble_system.h"
#include "ble_vesc_driver.h"
#include "bluetooth_client.h"
#include "ble_keyboard.h"
#include "ota_update.h"
#include "media_control.h"
#include "music_server.h"

#include "buffer.h"
#include "datatypes.h"
#include "vesc_rt_data.h"
#include "ui_updater.h"
#include "vesc_lisp_poll.h"
#include "debug_log.h"
#include "dev_settings.h"

#include <Wire.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

SemaphoreHandle_t g_i2c_mutex = nullptr;

static inline void i2c_lock()   { if (g_i2c_mutex) xSemaphoreTake(g_i2c_mutex, portMAX_DELAY); }
static inline void i2c_unlock() { if (g_i2c_mutex) xSemaphoreGive(g_i2c_mutex); }

// ------------------------------
// Waveshare ESP32-S3-Touch-LCD-4 board specifics
// ------------------------------
static constexpr uint8_t  TCA_ADDR     = 0x24;
static constexpr uint8_t  EXIO_I2C_SDA = 15;
static constexpr uint8_t  EXIO_I2C_SCL = 7;

// TCA9554 registers
static constexpr uint8_t TCA_REG_INPUT  = 0x00;
static constexpr uint8_t TCA_REG_OUTPUT = 0x01;
static constexpr uint8_t TCA_REG_POLINV = 0x02;
static constexpr uint8_t TCA_REG_CONFIG = 0x03;

// ------------------------------
// Brightness monitoring variables
// ------------------------------
static uint8_t last_brightness = 0;
static unsigned long last_brightness_check = 0;
#define BRIGHTNESS_CHECK_INTERVAL_MS 1000  // Check brightness changes every 1 second

// Function to check for brightness changes and apply them
void check_brightness_changes(void) {
  unsigned long current_time = millis();

  // Check brightness changes every BRIGHTNESS_CHECK_INTERVAL_MS
  //if (current_time - last_brightness_check >= BRIGHTNESS_CHECK_INTERVAL_MS) {
    uint8_t current_brightness = settings_get_screen_brightness();

    if (current_brightness != last_brightness) {
      LOG_INFO(SYSTEM, "Brightness changed from %d%% to %d%%", last_brightness, current_brightness);

      // Apply the new brightness setting
      settings_apply_brightness();

      // Update the last known brightness
      last_brightness = current_brightness;
    }

    last_brightness_check = current_time;
  //}
}

// ------------------------------
// I2C guard (prevents display/touch from breaking)
// ------------------------------
static bool g_wire_started = false;

static void wire_begin_once(int sda, int scl) {
  if (g_wire_started) return;
  Wire.begin(sda, scl);
  Wire.setTimeOut(50);      // helps avoid "sticky" bus
  // Wire.setClock(100000); // максимально надёжно, если нужно
  g_wire_started = true;
}

// ------------------------------
// TCA9554 helpers
// ------------------------------
static bool g_tca_ok = false;

static bool tca_write(uint8_t reg, uint8_t val) {
  i2c_lock();
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(reg);
  Wire.write(val);
  bool ok = (Wire.endTransmission() == 0);
  i2c_unlock();
  return ok;
}

static uint8_t tca_read(uint8_t reg) {
  i2c_lock();
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) {
    i2c_unlock();
    return 0xFF;
  }
  Wire.requestFrom(TCA_ADDR, (uint8_t)1);
  uint8_t v = Wire.available() ? Wire.read() : 0xFF;
  i2c_unlock();
  return v;
}

static void dumpTCA(const char* tag) {
  if (!g_tca_ok) return;
  Serial.printf("[TCA %s] IN=0x%02X OUT=0x%02X POL=0x%02X CFG=0x%02X\n",
                tag,
                tca_read(TCA_REG_INPUT),
                tca_read(TCA_REG_OUTPUT),
                tca_read(TCA_REG_POLINV),
                tca_read(TCA_REG_CONFIG));
}

// ------------------------------
// Power-up sequence (improved)
// ------------------------------
static constexpr uint8_t BIT_P0 = (1u << 0);
static constexpr uint8_t BIT_P1 = (1u << 1);
static constexpr uint8_t BIT_P2 = (1u << 2);

// We keep P0..P2 as outputs (common: LCD_RST, TP_RST, BL_EN). Others remain inputs for safety.
static constexpr uint8_t TCA_OUTPUT_MASK = (BIT_P0 | BIT_P1 | BIT_P2);

// Baseline outputs: all selected outputs HIGH (BL on + resets released)
static constexpr uint8_t TCA_BASELINE_OUT = TCA_OUTPUT_MASK;

static void tca_apply_safe_config() {
  // Polarity: no inversion
  tca_write(TCA_REG_POLINV, 0x00);

  // Config: 1=input, 0=output
  // Make only P0..P2 outputs, all others inputs.
  uint8_t cfg = 0xFF;
  cfg &= ~TCA_OUTPUT_MASK;
  tca_write(TCA_REG_CONFIG, cfg);

  // Baseline outputs
  uint8_t out = tca_read(TCA_REG_OUTPUT);
  if (out == 0xFF) out = 0x00;

  out &= ~TCA_OUTPUT_MASK;
  out |=  TCA_BASELINE_OUT;
  tca_write(TCA_REG_OUTPUT, out);
}

/**
 * Power-up sequence that fixes "works after warm reset, black after USB power-cycle".
 * The key is to initialize EXIO (TCA9554) BEFORE LCD/LVGL init.
 *
 * We do:
 * - start I2C on SDA=15/SCL=7
 * - set safe expander config
 * - pulse candidate reset lines
 * - wait long enough for cold boot stabilization
 */
static void board_display_powerup_sequence() {
  wire_begin_once(EXIO_I2C_SDA, EXIO_I2C_SCL);
  delay(10);

  // Probe TCA once
  g_tca_ok = tca_write(TCA_REG_POLINV, 0x00);
  if (!g_tca_ok) {
    Serial.println("[TCA] no ACK. Skipping EXIO powerup.");
    return;
  }

  // Apply safe config
  tca_apply_safe_config();

  // Cold-boot settle time for panel power rails
  delay(250);

  // Use cached OUT image (avoid readback during cold boot)
  uint8_t out = tca_read(TCA_REG_OUTPUT);
  if (out == 0xFF) out = 0x00;
  out &= ~TCA_OUTPUT_MASK;
  out |=  TCA_BASELINE_OUT;
  tca_write(TCA_REG_OUTPUT, out);

  // LCD_RST pulse (P0)
  tca_write(TCA_REG_OUTPUT, (uint8_t)(out & ~BIT_P0));
  delay(20);
  tca_write(TCA_REG_OUTPUT, (uint8_t)(out |  BIT_P0));
  delay(150);

  // TP_RST pulse (P1)
  tca_write(TCA_REG_OUTPUT, (uint8_t)(out & ~BIT_P1));
  delay(10);
  tca_write(TCA_REG_OUTPUT, (uint8_t)(out |  BIT_P1));
  delay(100);

  dumpTCA("after_powerup");
}

// ------------------------------
// Banner printing (aligned + emoji-safe-ish)
// ------------------------------
// NOTE: Emoji width differs between terminals. Best practice:
// keep emoji on their own lines only (we do that).
static void print_can_banner(uint8_t vesc_can_id, int can_speed) {
   LOG_RAW("\n╔═══════════════════════════════════════════════╗\n");
  LOG_RAW("║      🚀 CAN Communication Started 🚀          ║\n");
  LOG_RAW("╠═══════════════════════════════════════════════╣\n");
  LOG_RAW("║ Hardware:           %-25s ║\n", HW_NAME);
  LOG_RAW("║ Firmware:           v%d.%02d                     ║\n", FW_VERSION_MAJOR, FW_VERSION_MINOR);
  LOG_RAW("║ Device CAN ID:      %3d                       ║\n", vesc_can_id);
  LOG_RAW("║ Target VESC ID:     %3d                       ║\n", settings_get_target_vesc_id());
  LOG_RAW("║ Device Type:        HW_TYPE_CUSTOM_MODULE     ║\n");
  LOG_RAW("║ CAN Speed:          %4d kbps                 ║\n", can_speed);
  LOG_RAW("║ Screen Brightness:  %3d%%                      ║\n", settings_get_screen_brightness());
  LOG_RAW("║ TX Pin:             GPIO 6                    ║\n");
  LOG_RAW("║ RX Pin:             GPIO 0                    ║\n");
  LOG_RAW("╠═══════════════════════════════════════════════╣\n");
  
  // Display BLE Bridge mode info
  LOG_RAW("║ 🌉 BLE Mode:        BRIDGE (vesc_express)     ║\n");
  LOG_RAW("║ 📱 BLE Device:      SuperVESCDisplay          ║\n");
  LOG_RAW("║ 📋 Local Commands:  ENABLED (ID=%3d)          ║\n", vesc_can_id);
  LOG_RAW("║ 🔄 CAN Forwarding:  ENABLED (all other IDs)   ║\n");
  
  LOG_RAW("╚═══════════════════════════════════════════════╝\n");
  LOG_RAW("\n⏳ Waiting for BLE/CAN messages...\n\n");
}

// ------------------------------
// Production-style setup sequence
// ------------------------------
void setup() {
  Serial.begin(115200);

  g_i2c_mutex = xSemaphoreCreateMutex();

  delay(150);
  LOG_INFO(SYSTEM, "VESC Display Starting...");

  // 1) Board bring-up FIRST (prevents cold-boot black screen)
  board_display_powerup_sequence();

  // 2) Settings system (NVS)
  settings_init();

  // 3) Display init (ST7701)
  LCD_Init();

  // 4) Backlight + brightness
  Backlight_Init();
  settings_apply_brightness();
  last_brightness = settings_get_screen_brightness();
  LOG_INFO(SYSTEM, "Screen brightness set to %d%%", last_brightness);

  // 5) Touch init (GT911)
  if (Touch_Init()) {
    LOG_INFO(SYSTEM, "Touch screen initialized successfully!");
  } else {
    LOG_ERROR(SYSTEM, "Touch screen initialization failed!");
  }

  // 6) Other I2C peripherals
  // IMPORTANT: I2C_Init() MUST NOT call Wire.begin() on different pins.
  I2C_Init();

  // 7) CAN init
  uint8_t vesc_can_id = settings_get_controller_id();
  int can_speed = (int)settings_get_can_speed();
  comm_can_start(GPIO_NUM_6, GPIO_NUM_0, vesc_can_id, can_speed);

  // 8) RT modules
  vesc_rt_data_init();
  vesc_lisp_poll_init();

  // 9) CAN packet handler for Bridge mode
  auto packet_handler_wrapper = [](unsigned char *data, unsigned int len) {
    vesc_rt_data_process_response(data, len);
    vesc_lisp_poll_process_response(data, len);
    BLE_OnCANResponse(data, len);
  };
  comm_can_set_packet_handler(packet_handler_wrapper);

  print_can_banner(vesc_can_id, can_speed);

  // 10) Centralized BLE System Initialization
  if (!ble_system_init("SuperVESCDisplay")) {
    LOG_ERROR(SYSTEM, "BLE system initialization failed");
  } else {
    LOG_INFO(SYSTEM, "BLE system initialized successfully");

    NimBLEServer* pBLEServer = ble_system_get_server();
    if (pBLEServer == nullptr) {
      LOG_ERROR(SYSTEM, "Failed to get BLE server instance");
    } else {
      if (vesc_ble_driver_init(pBLEServer)) {
        LOG_INFO(SYSTEM, "BLE VESC driver initialized successfully");
        LOG_INFO(SYSTEM, "BLE response callback registered in VESC handler");
      } else {
        LOG_ERROR(SYSTEM, "BLE VESC driver initialization failed");
      }

      if (ota_update_init()) {
        LOG_INFO(SYSTEM, "OTA update module initialized successfully");
      } else {
        LOG_ERROR(SYSTEM, "OTA update module initialization failed");
      }

      if (music_server_init(pBLEServer)) {
        LOG_INFO(SYSTEM, "Music server initialized successfully");
      } else {
        LOG_ERROR(SYSTEM, "Music server initialization failed");
      }

      ble_system_start_advertising();
      LOG_INFO(SYSTEM, "BLE advertising started - all services ready");
    }
  }

  // 11) LVGL + UI
  Lvgl_Init();
  ui_updater_set_zeros();
  ui_updater_init();

  vesc_rt_data_start();
  LOG_INFO(SYSTEM, "RT data requests started");

  vesc_lisp_poll_start();
  LOG_INFO(SYSTEM, "LISP poll started (10 Hz)");

  ui_updater_start();
  LOG_INFO(SYSTEM, "UI updates started");

  LOG_INFO(SYSTEM, "VESC Display Ready!");
}

void loop() {
  vesc_ble_driver_loop();   // Process BLE communication
  ota_update_loop();        // Process OTA updates
  media_control_loop();     // Process media control
  music_server_loop();      // Process music server

  if (millis() > 5000) {
    vesc_rt_data_loop();    // Process RT data requests
  }
  vesc_lisp_poll_loop();    // Process LISP poll requests (10 Hz)

  ui_updater_update();      // Update UI with VESC data
  ui_updater_update_fps();  // Update FPS counter

  //check_brightness_changes(); // Check for brightness setting changes

  Lvgl_Loop();

  vTaskDelay(pdMS_TO_TICKS(5));
}
