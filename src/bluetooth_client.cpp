/*
	Copyright 2025 Super VESC Display

	Bluetooth Client Module Implementation
	Connects to external measurement devices and receives data
*/

#include "bluetooth_client.h"
#include "debug_log.h"
#include "ui_updater.h"
#include "ble_vesc_driver.h"  // For VESC service UUIDs

// Global state variables
static bool isConnected = false;
static bool isScanning = false;
static NimBLEClient* client = nullptr;
static NimBLEScan* scan = nullptr;
static NimBLEAdvertisedDevice* targetDevice = nullptr;
static NimBLERemoteCharacteristic* pCharacteristic = nullptr;
static DeviceState currentState = DEVICE_STATE_SCANNING;

// Client callbacks
class MyClientCallback : public NimBLEClientCallbacks {
    void onConnect(NimBLEClient* pClient) override {
        isConnected = true;
        LOG_INFO(BLE_CLIENT, "Connected to measurement device!");
    }

    void onDisconnect(NimBLEClient* pClient) override {
        isConnected = false;
        LOG_INFO(BLE_CLIENT, "Disconnected from measurement device!");
        targetDevice = nullptr;
        currentState = DEVICE_STATE_SCANNING;
    }
};

// Advertisement callbacks
class MyAdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) override {
        LOG_DEBUG(BLE_CLIENT, "Found device: %s %s", 
                 advertisedDevice->getName().c_str(),
                 advertisedDevice->getAddress().toString().c_str());
        
        if (advertisedDevice->getName() == TARGET_DEVICE_NAME) {
            LOG_INFO(BLE_CLIENT, "Target device found!");
            targetDevice = advertisedDevice;
            scan->stop();
            isScanning = false;
            currentState = DEVICE_STATE_CONNECTING;
        }
    }
};

// Notification callback
void onNotifyCallback(NimBLERemoteCharacteristic* pCharacteristic, uint8_t* data, size_t length, bool isNotify) {
    LOG_DEBUG(BLE_CLIENT, "Received data (%d bytes)", length);
    // Process received data as needed
}

// Initialize Bluetooth client
bool bluetooth_client_init(void) {
    LOG_INFO(BLE_CLIENT, "Initializing Bluetooth client...");
    
    // Initialize NimBLE if not already done
    if (!NimBLEDevice::getInitialized()) {
        LOG_ERROR(BLE_CLIENT, "NimBLE not initialized! Call NimBLEDevice::init() first.");
        return false;
    }
    
    // Create scan object
    scan = NimBLEDevice::getScan();
    if (!scan) {
        LOG_ERROR(BLE_CLIENT, "Failed to get scan object");
        return false;
    }
    
    scan->setActiveScan(true);
    scan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    
    LOG_INFO(BLE_CLIENT, "Bluetooth client initialized successfully");
    return true;
}

// Scan for devices
void scanForDevices() {
    if (!isScanning) {
        LOG_INFO(BLE_CLIENT, "Starting device scan...");
        isScanning = true;
        scan->start(0, nullptr, true);
    }
}

// Connect to target device
void connectToServer() {
    if (targetDevice == nullptr) {
        LOG_WARN(BLE_CLIENT, "No target device to connect to");
        return;
    }

    client = NimBLEDevice::createClient();
    client->setClientCallbacks(new MyClientCallback());

    LOG_INFO(BLE_CLIENT, "Connecting to BLE server...");
    if (!client->connect(targetDevice)) {
        LOG_ERROR(BLE_CLIENT, "Failed to connect to server!");
        targetDevice = nullptr;
        currentState = DEVICE_STATE_SCANNING;
        return;
    }

    NimBLERemoteService* pService = client->getService(VESC_SERVICE_UUID);
    if (pService == nullptr) {
        LOG_ERROR(BLE_CLIENT, "Failed to find service!");
        client->disconnect();
        currentState = DEVICE_STATE_SCANNING;
        return;
    }

    pCharacteristic = pService->getCharacteristic(VESC_CHARACTERISTIC_UUID_TX);
    if (pCharacteristic == nullptr) {
        LOG_ERROR(BLE_CLIENT, "Failed to find characteristic!");
        client->disconnect();
        currentState = DEVICE_STATE_SCANNING;
        return;
    }

    if (pCharacteristic->canNotify()) {
        pCharacteristic->subscribe(true, onNotifyCallback);
        LOG_INFO(BLE_CLIENT, "Subscribed to notifications");
    }
    
    currentState = DEVICE_STATE_CONNECTED;
    LOG_INFO(BLE_CLIENT, "Successfully connected to measurement device");
}


// Main processing loop
void bluetooth_client_loop() {
    switch (currentState) {
        case DEVICE_STATE_SCANNING:
            scanForDevices();
            break;

        case DEVICE_STATE_CONNECTING:
            connectToServer();
            break;

        case DEVICE_STATE_CONNECTED:
            // Connected state - ready for data exchange
            LOG_DEBUG(BLE_CLIENT, "Connected to device");
            break;

        case DEVICE_STATE_IDLE:
            currentState = DEVICE_STATE_SCANNING;
            break;
    }
}

// Get connection status
bool bluetooth_client_is_connected(void) {
    return isConnected;
}

// Get current device state
DeviceState bluetooth_client_get_state(void) {
    return currentState;
}
