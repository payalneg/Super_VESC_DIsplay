/*
	Copyright 2025 Super VESC Display

	Centralized BLE System Initialization Implementation
	Manages single NimBLEDevice instance and server
*/

#include "ble_system.h"
#include "debug_log.h"

// Global BLE server instance
static NimBLEServer* pBLE_server = nullptr;
static bool ble_system_initialized = false;

// Registered BLE driver callbacks (set by drivers via registration functions)
static ble_driver_on_connect_cb_t registered_connect_cb = nullptr;
static ble_driver_on_disconnect_cb_t registered_disconnect_cb = nullptr;
static ble_driver_on_mtu_change_cb_t registered_mtu_change_cb = nullptr;

// BLE Server Callbacks Implementation
void BLESystemCallbacks::onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc)
{
  LOG_INFO(BLE, "🔵 Client connected: %s", NimBLEAddress(desc->peer_ota_addr).toString().c_str());
  NimBLEDevice::startAdvertising(); // restart advertising using centralized function
  
  // Notify registered driver about connection
  if (registered_connect_cb != nullptr) {
    registered_connect_cb();
  }
}

void BLESystemCallbacks::onDisconnect(NimBLEServer *pServer)
{
  LOG_INFO(BLE, "🔵 Client disconnected");
  ble_system_restart_advertising();  // Use centralized function
  
  // Notify registered driver about disconnection
  if (registered_disconnect_cb != nullptr) {
    registered_disconnect_cb();
  }
}

void BLESystemCallbacks::onMTUChange(uint16_t MTU, ble_gap_conn_desc *desc)
{
  LOG_INFO(BLE, "🔵 MTU changed - new size %d, peer %s", MTU, NimBLEAddress(desc->peer_ota_addr).toString().c_str());
  
  // Notify registered driver about MTU change
  if (registered_mtu_change_cb != nullptr) {
    registered_mtu_change_cb(MTU);
  }
}

// Initialize centralized BLE system
bool ble_system_init(const char* device_name) {
    if (ble_system_initialized) {
        LOG_WARN(BLE, "BLE system already initialized");
        return true;
    }
    
    try {
        LOG_INFO(BLE, "Initializing centralized BLE system...");
        
        // Initialize the BLE Device
        NimBLEDevice::init(device_name);
        NimBLEDevice::setPower(ESP_PWR_LVL_P9);
        
        // Create the BLE Server
        pBLE_server = NimBLEDevice::createServer();
        
        if (pBLE_server == nullptr) {
            LOG_ERROR(BLE, "Failed to create BLE server");
            return false;
        }
        
        // Set general server callbacks
        pBLE_server->setCallbacks(new BLESystemCallbacks());
        
        // Set security
        auto pSecurity = new NimBLESecurity();
        //pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);
        
        ble_system_initialized = true;
        LOG_INFO(BLE, "BLE system initialized successfully (device: %s)", device_name);
        
        return true;
    }
    catch (const std::exception& e) {
        LOG_ERROR(BLE, "BLE system initialization failed: %s", e.what());
        return false;
    }
}

// Start advertising (call after all services are added)
void ble_system_start_advertising(void) {
    if (!ble_system_initialized) {
        LOG_ERROR(BLE, "BLE system not initialized");
        return;
    }
    
    try {
        NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
        if (pAdvertising == nullptr) {
            LOG_ERROR(BLE, "Failed to get advertising object");
            return;
        }
        
        // Start advertising
        pAdvertising->start();
        LOG_INFO(BLE, "BLE advertising started");
    }
    catch (const std::exception& e) {
        LOG_ERROR(BLE, "Failed to start advertising: %s", e.what());
    }
}

// Restart advertising (called from callbacks after disconnect)
void ble_system_restart_advertising(void) {
    if (!ble_system_initialized) {
        return;
    }
    
    try {
        NimBLEDevice::startAdvertising();
    }
    catch (const std::exception& e) {
        LOG_ERROR(BLE, "Failed to restart advertising: %s", e.what());
    }
}

// Get the BLE server instance
NimBLEServer* ble_system_get_server(void) {
    return pBLE_server;
}

// Check if BLE system is initialized
bool ble_system_is_initialized(void) {
    return ble_system_initialized;
}

// Register BLE driver callbacks
void ble_system_register_connect_callback(ble_driver_on_connect_cb_t cb) {
    registered_connect_cb = cb;
}

void ble_system_register_disconnect_callback(ble_driver_on_disconnect_cb_t cb) {
    registered_disconnect_cb = cb;
}

void ble_system_register_mtu_change_callback(ble_driver_on_mtu_change_cb_t cb) {
    registered_mtu_change_cb = cb;
}

