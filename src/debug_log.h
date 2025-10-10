#ifndef DEBUG_LOG_H
#define DEBUG_LOG_H

#include <Arduino.h>

// ============================================================================
// LOGGING CONFIGURATION
// ============================================================================

// Master logging switch - set to 0 to disable ALL logging
#define DEBUG_LOGGING_ENABLED 1

// Undefine conflicting NimBLE macros if they exist
#ifdef LOG_LEVEL_ERROR
#undef LOG_LEVEL_ERROR
#endif
#ifdef LOG_LEVEL_WARN
#undef LOG_LEVEL_WARN
#endif
#ifdef LOG_LEVEL_INFO
#undef LOG_LEVEL_INFO
#endif
#ifdef LOG_LEVEL_DEBUG
#undef LOG_LEVEL_DEBUG
#endif

// Individual log level controls (comment out or set to 0 to disable)
#define LOG_LEVEL_ERROR   1  // Critical errors
#define LOG_LEVEL_WARN    1  // Warnings
#define LOG_LEVEL_INFO    1  // Important information
#define LOG_LEVEL_DEBUG   1  // Debug information
#define LOG_LEVEL_VERBOSE 0  // Verbose/detailed logs (disabled by default)

// Module-specific logging (set to 0 to disable specific modules)
#define LOG_CAN_ENABLED     1
#define LOG_BLE_ENABLED     1
#define LOG_PACKET_ENABLED  1
#define LOG_VESC_ENABLED    1
#define LOG_SYSTEM_ENABLED  1

// ============================================================================
// LOGGING MACROS - DO NOT MODIFY BELOW THIS LINE
// ============================================================================

#if DEBUG_LOGGING_ENABLED

  // Helper macro to check if logging is enabled for a module and level
  #define LOG_ENABLED(module, level) (LOG_##module##_ENABLED && LOG_LEVEL_##level)

  // Error logs (always with ❌ emoji)
  #if LOG_LEVEL_ERROR
    #define LOG_ERROR(module, ...) \
      if (LOG_##module##_ENABLED) { \
        Serial.printf("[%lu] ❌ [%s] ", millis(), #module); \
        Serial.printf(__VA_ARGS__); \
        Serial.println(); \
      }
  #else
    #define LOG_ERROR(module, ...)
  #endif

  // Warning logs (with ⚠️ emoji)
  #if LOG_LEVEL_WARN
    #define LOG_WARN(module, ...) \
      if (LOG_##module##_ENABLED) { \
        Serial.printf("[%lu] ⚠️  [%s] ", millis(), #module); \
        Serial.printf(__VA_ARGS__); \
        Serial.println(); \
      }
  #else
    #define LOG_WARN(module, ...)
  #endif

  // Info logs (with ✅ or 📋 emoji)
  #if LOG_LEVEL_INFO
    #define LOG_INFO(module, ...) \
      if (LOG_##module##_ENABLED) { \
        Serial.printf("[%lu] ✅ [%s] ", millis(), #module); \
        Serial.printf(__VA_ARGS__); \
        Serial.println(); \
      }
  #else
    #define LOG_INFO(module, ...)
  #endif

  // Debug logs (with 🔍 emoji)
  #if LOG_LEVEL_DEBUG
    #define LOG_DEBUG(module, ...) \
      if (LOG_##module##_ENABLED) { \
        Serial.printf("[%lu] 🔍 [%s] ", millis(), #module); \
        Serial.printf(__VA_ARGS__); \
        Serial.println(); \
      }
  #else
    #define LOG_DEBUG(module, ...)
  #endif

  // Verbose logs (with 📝 emoji)
  #if LOG_LEVEL_VERBOSE
    #define LOG_VERBOSE(module, ...) \
      if (LOG_##module##_ENABLED) { \
        Serial.printf("[%lu] 📝 [%s] ", millis(), #module); \
        Serial.printf(__VA_ARGS__); \
        Serial.println(); \
      }
  #else
    #define LOG_VERBOSE(module, ...)
  #endif

  // Special macro for data dumps (hex output)
  #define LOG_HEX(module, data, len, prefix) \
    if (LOG_##module##_ENABLED && LOG_LEVEL_VERBOSE) { \
      Serial.printf("[%lu] 📊 [%s] %s", millis(), #module, prefix); \
      for (int i = 0; i < len && i < 16; i++) { \
        Serial.printf("%02X ", data[i]); \
      } \
      if (len > 16) Serial.printf("... (%d more)", len - 16); \
      Serial.println(); \
    }

  // Raw printf without module tag (for custom formatting)
  #define LOG_RAW(...) Serial.printf(__VA_ARGS__)

#else
  // All logging disabled
  #define LOG_ERROR(module, ...)
  #define LOG_WARN(module, ...)
  #define LOG_INFO(module, ...)
  #define LOG_DEBUG(module, ...)
  #define LOG_VERBOSE(module, ...)
  #define LOG_HEX(module, data, len, prefix)
  #define LOG_RAW(...)
#endif

// Legacy compatibility macro (if you want to keep some direct Serial.printf)
#define LOG_PRINT(...) Serial.printf(__VA_ARGS__)

#endif // DEBUG_LOG_H

