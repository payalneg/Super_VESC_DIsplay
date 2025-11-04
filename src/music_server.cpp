/*
	Copyright 2025 Super VESC Display

	Music Server Module Implementation
	Handles BLE music and navigation service
*/

#include "music_server.h"
#include "custom.h"
#include "debug_log.h"
#include <cstdlib>
#include <cstring>
#include <vector>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// Include lodepng for PNG decoding
// Include lvgl.h first to ensure LV_USE_PNG is defined
#include "lodepng.h"

// BLE Server and Service pointers
static NimBLEServer *pMusicServer = nullptr;
static NimBLEService *pMusicService = nullptr;
static NimBLECharacteristic *pCharacteristicSong = nullptr;
static NimBLECharacteristic *pCharacteristicNav = nullptr;
static NimBLECharacteristic *pCharacteristicIcon = nullptr;

static bool music_server_connected = false;
static bool music_server_old_connected = false;

// Data reception state for each characteristic
struct DataReceptionState {
    uint32_t expected_size;      // Expected data size (from first 4 bytes)
    size_t current_size;          // Current received data size
    std::vector<uint8_t> buffer;  // Buffer for accumulating data
    bool is_receiving;            // Flag indicating data reception in progress
    unsigned long last_receive_time; // Timestamp of last received data chunk
    
    DataReceptionState() : expected_size(0), current_size(0), is_receiving(false), last_receive_time(0) {}
    
    void reset() {
        expected_size = 0;
        current_size = 0;
        buffer.clear();
        is_receiving = false;
        last_receive_time = 0;
    }
};

static DataReceptionState song_state;
static DataReceptionState nav_state;
static DataReceptionState icon_state;

// Queue for processing complete data packets (to avoid blocking BLE stack)
typedef enum {
    MUSIC_DATA_TYPE_SONG,
    MUSIC_DATA_TYPE_NAV,
    MUSIC_DATA_TYPE_ICON
} music_data_type_t;

typedef struct {
    music_data_type_t type;
    uint8_t* data;
    size_t data_size;
} music_data_packet_t;

#define MUSIC_QUEUE_SIZE 3
static QueueHandle_t music_data_queue = nullptr;

// Forward declaration
extern "C" {
    void update_music_text(const char *text);
    void update_navigation_text(const char *text);
    void update_navigation_icon(const uint8_t *img_data, uint32_t data_size, uint16_t width, uint16_t height, lv_img_cf_t color_format);
}

// BLE Server Callbacks for Music Server
class MusicServerCallbacks : public BLEServerCallbacks
{
public:
    void onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc) override
    {
        LOG_INFO(MUSIC, "Music server client connected: %s", NimBLEAddress(desc->peer_ota_addr).toString().c_str());
        music_server_connected = true;
        // Reset data reception states on new connection
        song_state.reset();
        nav_state.reset();
        icon_state.reset();
        // Advertising will be restarted by server callbacks if needed
    }

    void onDisconnect(NimBLEServer *pServer) override
    {
        LOG_INFO(MUSIC, "Music server client disconnected");
        music_server_connected = false;
        // Reset data reception states on disconnect
        song_state.reset();
        nav_state.reset();
        icon_state.reset();
        // Advertising will be restarted by server callbacks if needed
    }

    void onMTUChange(uint16_t MTU, ble_gap_conn_desc *desc) override
    {
        LOG_INFO(MUSIC, "Music server MTU changed - new size %d", MTU);
    }
};

// Maximum allowed data sizes (protection against malformed data)
#define MAX_TEXT_SIZE      (1024)   // 1MB for text data
#define MAX_ICON_SIZE      (5 * 1024) // 5MB for icon data
#define DATA_RECEIVE_TIMEOUT_MS 100  // Timeout after which reception state is reset

// Helper function to clean text from special Unicode characters
// Replaces non-breaking space (U+00A0, UTF-8: 0xC2 0xA0) with regular space
// Removes soft hyphen (U+00AD, UTF-8: 0xC2 0xAD)
static void cleanTextData(uint8_t* data, size_t* data_size)
{
    if (data == nullptr || data_size == nullptr || *data_size == 0) {
        return;
    }
    
    size_t read_pos = 0;
    size_t write_pos = 0;
    size_t size = *data_size;
    
    // Process data excluding the null terminator (if present)
    size_t data_end = size;
    if (size > 0 && data[size - 1] == 0) {
        data_end = size - 1; // Exclude null terminator from processing
    }
    
    while (read_pos < data_end) {
        // Check for non-breaking space (0xC2 0xA0)
        if (read_pos + 1 < data_end && data[read_pos] == 0xC2 && data[read_pos + 1] == 0xA0) {
            // Replace with regular space
            data[write_pos++] = 0x20; // Regular space
            read_pos += 2;
        }
        // Check for soft hyphen (0xC2 0xAD)
        else if (read_pos + 1 < data_end && data[read_pos] == 0xC2 && data[read_pos + 1] == 0xAD) {
            // Skip soft hyphen (remove it)
            read_pos += 2;
        }
        // Regular character - copy as is
        else {
            data[write_pos++] = data[read_pos++];
        }
    }
    
    // Add null terminator at the end
    data[write_pos] = 0;
    
    // Update size (include null terminator)
    *data_size = write_pos + 1;
}

// Helper function to process data chunks
// Returns true if complete data received and processed
static bool processDataChunk(DataReceptionState& state, const uint8_t* data, size_t len, const char* characteristic_name, uint32_t max_size)
{
    if (len == 0) {
        return false;
    }
    
    // If not receiving, this is the first chunk - read size from first 4 bytes
    if (!state.is_receiving) {
        if (len < 4) {
            LOG_WARN(MUSIC, "%s: First chunk too small (%zu bytes), need at least 4 bytes for size", characteristic_name, len);
            state.reset();
            return false;
        }
        
        // Read expected size (little-endian uint32_t)
        state.expected_size = (uint32_t)data[0] | 
                             ((uint32_t)data[1] << 8) | 
                             ((uint32_t)data[2] << 16) | 
                             ((uint32_t)data[3] << 24);
        
        // Validate size
        if (state.expected_size == 0 || state.expected_size > max_size) {
            LOG_ERROR(MUSIC, "%s: Invalid data size: %u (max: %u)", characteristic_name, state.expected_size, max_size);
            state.reset();
            return false;
        }
        
        LOG_DEBUG(MUSIC, "%s: Starting reception, expected size: %u bytes", characteristic_name, state.expected_size);
        
        // Allocate buffer for expected data
        state.buffer.resize(state.expected_size);
        state.current_size = 0;
        state.is_receiving = true;
        state.last_receive_time = millis();
        
        // Copy remaining data (skip first 4 bytes)
        if (len > 4) {
            size_t data_len = len - 4;
            if (data_len > state.expected_size) {
                LOG_WARN(MUSIC, "%s: Received more data than expected (%zu > %u)", characteristic_name, data_len, state.expected_size);
                data_len = state.expected_size;
            }
            memcpy(state.buffer.data(), &data[4], data_len);
            state.current_size = data_len;
        }
    } else {
        // Continuing reception - append data to buffer
        size_t remaining = state.expected_size - state.current_size;
        size_t copy_len = (len < remaining) ? len : remaining;
        
        if (copy_len > 0) {
            memcpy(state.buffer.data() + state.current_size, data, copy_len);
            state.current_size += copy_len;
        }
        // Update last receive time
        state.last_receive_time = millis();
    }
    
    // Check if all data received
    if (state.current_size >= state.expected_size) {
        LOG_DEBUG(MUSIC, "%s: Complete data received (%zu bytes)", characteristic_name, state.current_size);
        return true;
    }
    
    LOG_DEBUG(MUSIC, "%s: Received chunk: %zu bytes, total: %zu/%u", characteristic_name, len, state.current_size, state.expected_size);
    return false;
}

// BLE Characteristic Callbacks for Music Server
class MusicCharacteristicCallbacks : public BLECharacteristicCallbacks
{
public:
    void onWrite(BLECharacteristic *pCharacteristic) override
    {
        std::string rxValue = pCharacteristic->getValue();
        
        if (rxValue.length() == 0) {
            return;
        }

        const uint8_t* data = (const uint8_t*)rxValue.data();
        size_t data_len = rxValue.length();
        NimBLEUUID charUuid = pCharacteristic->getUUID();
        
        // Handle Song characteristic
        if (charUuid.equals(NimBLEUUID(MUSIC_CHARACTERISTIC_UUID_SONG))) {
            if (processDataChunk(song_state, data, data_len, "Song", MAX_TEXT_SIZE)) {
                // Complete data received - queue it for processing in loop
                if (song_state.buffer.size() > 0 && music_data_queue != nullptr) {
                    music_data_packet_t packet;
                    packet.type = MUSIC_DATA_TYPE_SONG;
                    packet.data_size = song_state.buffer.size() + 1; // +1 for null terminator
                    packet.data = (uint8_t*)std::malloc(packet.data_size);
                    if (packet.data != nullptr) {
                        memcpy(packet.data, song_state.buffer.data(), song_state.buffer.size());
                        packet.data[packet.data_size - 1] = 0; // Add null terminator
                        
                        // Clean text data (remove special Unicode characters)
                        cleanTextData(packet.data, &packet.data_size);
                        
                        BaseType_t result = xQueueSend(music_data_queue, &packet, 0); // Non-blocking
                        if (result != pdTRUE) {
                            LOG_WARN(MUSIC, "Music data queue full, dropping song data");
                            std::free(packet.data);
                        } else {
                            LOG_DEBUG(MUSIC, "Queued song data for processing (%zu bytes)", packet.data_size);
                        }
                    } else {
                        LOG_ERROR(MUSIC, "Failed to allocate memory for song data packet");
                    }
                }
                song_state.reset();
            }
        }
        // Handle Navigation characteristic
        else if (charUuid.equals(NimBLEUUID(MUSIC_CHARACTERISTIC_UUID_NAV))) {
            if (processDataChunk(nav_state, data, data_len, "Navigation", MAX_TEXT_SIZE)) {
                // Complete data received - queue it for processing in loop
                if (nav_state.buffer.size() > 0 && music_data_queue != nullptr) {
                    music_data_packet_t packet;
                    packet.type = MUSIC_DATA_TYPE_NAV;
                    packet.data_size = nav_state.buffer.size() + 1; // +1 for null terminator
                    packet.data = (uint8_t*)std::malloc(packet.data_size);
                    if (packet.data != nullptr) {
                        memcpy(packet.data, nav_state.buffer.data(), nav_state.buffer.size());
                        packet.data[packet.data_size - 1] = 0; // Add null terminator
                        
                        // Clean text data (remove special Unicode characters)
                        cleanTextData(packet.data, &packet.data_size);
                        
                        BaseType_t result = xQueueSend(music_data_queue, &packet, 0); // Non-blocking
                        if (result != pdTRUE) {
                            LOG_WARN(MUSIC, "Music data queue full, dropping navigation data");
                            std::free(packet.data);
                        } else {
                            LOG_DEBUG(MUSIC, "Queued navigation data for processing (%zu bytes)", packet.data_size);
                        }
                    } else {
                        LOG_ERROR(MUSIC, "Failed to allocate memory for navigation data packet");
                    }
                }
                nav_state.reset();
            }
        }
        // Handle Icon characteristic
        else if (charUuid.equals(NimBLEUUID(MUSIC_CHARACTERISTIC_UUID_ICON))) {
            if (processDataChunk(icon_state, data, data_len, "Icon", MAX_ICON_SIZE)) {
                // Complete data received - queue it for processing in loop
                if (icon_state.buffer.size() >= 8 && music_data_queue != nullptr) {
                    music_data_packet_t packet;
                    packet.type = MUSIC_DATA_TYPE_ICON;
                    packet.data_size = icon_state.buffer.size();
                    packet.data = (uint8_t*)std::malloc(packet.data_size);
                    if (packet.data != nullptr) {
                        memcpy(packet.data, icon_state.buffer.data(), packet.data_size);
                        
                        BaseType_t result = xQueueSend(music_data_queue, &packet, 0); // Non-blocking
                        if (result != pdTRUE) {
                            LOG_WARN(MUSIC, "Music data queue full, dropping icon data");
                            std::free(packet.data);
                        } else {
                            LOG_DEBUG(MUSIC, "Queued icon data for processing (%zu bytes)", packet.data_size);
                        }
                    } else {
                        LOG_ERROR(MUSIC, "Failed to allocate memory for icon data packet");
                    }
                } else {
                    if (icon_state.buffer.size() < 8) {
                        LOG_WARN(MUSIC, "Icon data too short: %zu bytes (minimum 8 for PNG)", icon_state.buffer.size());
                    }
                }
                icon_state.reset();
            }
        }
    }

    void onSubscribe(NimBLECharacteristic* pCharacteristic, ble_gap_conn_desc* desc, uint16_t subValue) override
    {
        LOG_INFO(MUSIC, "Client subscribed to music characteristic: %s, value: %d", 
                 pCharacteristic->getUUID().toString().c_str(), subValue);
    }
};

// Initialize Music Server (now accepts server as parameter)
bool music_server_init(NimBLEServer* pServer) {
    if (pServer == nullptr) {
        LOG_ERROR(MUSIC, "BLE server is null");
        return false;
    }
    
    try {
        // Store server reference
        pMusicServer = pServer;

        // Create the Music Service
        pMusicService = pMusicServer->createService(MUSIC_SERVICE_UUID);
        
        if (pMusicService == nullptr) {
            LOG_ERROR(MUSIC, "Failed to create music service");
            return false;
        }

        // Create Song Characteristic (Write/Notify)
        pCharacteristicSong = pMusicService->createCharacteristic(
            MUSIC_CHARACTERISTIC_UUID_SONG,
            NIMBLE_PROPERTY::WRITE |
            NIMBLE_PROPERTY::WRITE_NR |
            NIMBLE_PROPERTY::NOTIFY |
            NIMBLE_PROPERTY::READ
        );

        if (pCharacteristicSong == nullptr) {
            LOG_ERROR(MUSIC, "Failed to create song characteristic");
            return false;
        }

        // Create Navigation Characteristic (Write/Notify)
        pCharacteristicNav = pMusicService->createCharacteristic(
            MUSIC_CHARACTERISTIC_UUID_NAV,
            NIMBLE_PROPERTY::WRITE |
            NIMBLE_PROPERTY::WRITE_NR |
            NIMBLE_PROPERTY::NOTIFY |
            NIMBLE_PROPERTY::READ
        );

        if (pCharacteristicNav == nullptr) {
            LOG_ERROR(MUSIC, "Failed to create navigation characteristic");
            return false;
        }

        // Create Icon Characteristic (Write/Notify)
        pCharacteristicIcon = pMusicService->createCharacteristic(
            MUSIC_CHARACTERISTIC_UUID_ICON,
            NIMBLE_PROPERTY::WRITE |
            NIMBLE_PROPERTY::WRITE_NR |
            NIMBLE_PROPERTY::NOTIFY |
            NIMBLE_PROPERTY::READ
        );

        if (pCharacteristicIcon == nullptr) {
            LOG_ERROR(MUSIC, "Failed to create icon characteristic");
            return false;
        }

        // Set callbacks
        pCharacteristicSong->setCallbacks(new MusicCharacteristicCallbacks());
        pCharacteristicNav->setCallbacks(new MusicCharacteristicCallbacks());
        pCharacteristicIcon->setCallbacks(new MusicCharacteristicCallbacks());

        // Start the Music service
        pMusicService->start();

        // Add service UUID to advertising (but don't start advertising yet)
        NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
        if (pAdvertising != nullptr) {
            pAdvertising->addServiceUUID(MUSIC_SERVICE_UUID);
        }

        // Initialize processing queue
        music_data_queue = xQueueCreate(MUSIC_QUEUE_SIZE, sizeof(music_data_packet_t));
        if (music_data_queue == nullptr) {
            LOG_ERROR(MUSIC, "Failed to create music data queue");
            return false;
        }

        LOG_INFO(MUSIC, "Music server initialized successfully");

        return true;
    }
    catch (const std::exception& e) {
        LOG_ERROR(MUSIC, "Music server initialization failed: %s", e.what());
        return false;
    }
}

// Helper function to process PNG icon data
static void processIconData(const uint8_t* png_data, size_t data_size)
{
    if (data_size < 8) {
        LOG_WARN(MUSIC, "Icon data too short: %zu bytes (minimum 8 for PNG)", data_size);
        return;
    }

    const uint8_t png_signature[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    
    bool is_png = true;
    for (int i = 0; i < 8; i++) {
        if (png_data[i] != png_signature[i]) {
            is_png = false;
            break;
        }
    }
    
    if (!is_png) {
        LOG_WARN(MUSIC, "Icon data does not appear to be PNG format");
        return;
    }

    // Decode PNG data using lodepng
    unsigned char* decoded_img = nullptr;
    unsigned width = 0;
    unsigned height = 0;
    
    // Use lodepng_decode with RGBA color mode
    LodePNGState state;
    lodepng_state_init(&state);
    state.info_raw.colortype = LCT_RGBA;
    state.info_raw.bitdepth = 8;
    unsigned error = lodepng_decode(&decoded_img, &width, &height, &state, png_data, data_size);
    lodepng_state_cleanup(&state);
    
    if (error == 0 && decoded_img != nullptr && width > 0 && height > 0) {
        // lodepng_decode returns RGBA8888 format
        // LVGL with LV_COLOR_DEPTH 16 expects RGB565A8 format (3 bytes per pixel: 2 bytes RGB565 + 1 byte alpha)
        uint32_t pixel_count = width * height;
        uint32_t converted_size = pixel_count * 3; // RGB565A8 = 3 bytes per pixel
        uint8_t* converted_img = (uint8_t*)std::malloc(converted_size);
        
        if (converted_img == nullptr) {
            LOG_ERROR(MUSIC, "Failed to allocate memory for converted image");
            std::free(decoded_img);
        } else {
            // Convert RGBA8888 to RGB565A8
            for (uint32_t i = 0; i < pixel_count; i++) {
                uint8_t* src_pixel = &decoded_img[i * 4];
                uint8_t* dst_pixel = &converted_img[i * 3];
                
                uint8_t r = src_pixel[0];
                uint8_t g = src_pixel[1];
                uint8_t b = src_pixel[2];
                uint8_t a = src_pixel[3];
                
                // Convert RGB888 to RGB565
                uint16_t rgb565 = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
                
                // Store RGB565 as little-endian (low byte, high byte) + alpha
                dst_pixel[0] = rgb565 & 0xFF;        // Low byte
                dst_pixel[1] = (rgb565 >> 8) & 0xFF; // High byte
                dst_pixel[2] = a;                    // Alpha byte
            }
            
            LOG_DEBUG(MUSIC, "PNG decoded: %dx%d, converted size: %u bytes (RGB565A8)", width, height, converted_size);
            
            // Call custom function to update navigation icon with converted data
            update_navigation_icon(converted_img, converted_size, (uint16_t)width, (uint16_t)height, LV_IMG_CF_TRUE_COLOR_ALPHA);
            
            // Free converted image data (update_navigation_icon copies it)
            std::free(converted_img);
            std::free(decoded_img);
        }
    } else {
        LOG_ERROR(MUSIC, "PNG decode failed: %s (error code: %u)", lodepng_error_text(error), error);
    }
}

// Main processing loop
void music_server_loop(void)
{
    // Check connection status
    if (music_server_connected != music_server_old_connected) {
        music_server_old_connected = music_server_connected;
        // Connection status change is handled by callbacks
    }

    // Check for receive timeouts and reset states if timeout exceeded
    unsigned long current_time = millis();
    if (song_state.is_receiving && 
        (current_time - song_state.last_receive_time) > DATA_RECEIVE_TIMEOUT_MS) {
        LOG_WARN(MUSIC, "Song data reception timeout, resetting state");
        song_state.reset();
    }
    if (nav_state.is_receiving && 
        (current_time - nav_state.last_receive_time) > DATA_RECEIVE_TIMEOUT_MS) {
        LOG_WARN(MUSIC, "Navigation data reception timeout, resetting state");
        nav_state.reset();
    }
    if (icon_state.is_receiving && 
        (current_time - icon_state.last_receive_time) > DATA_RECEIVE_TIMEOUT_MS) {
        LOG_WARN(MUSIC, "Icon data reception timeout, resetting state");
        icon_state.reset();
    }

    // Process queued data packets (non-blocking)
    if (music_data_queue != nullptr) {
        music_data_packet_t packet;
        while (xQueueReceive(music_data_queue, &packet, 0) == pdTRUE) { // Non-blocking
            switch (packet.type) {
                case MUSIC_DATA_TYPE_SONG:
                    if (packet.data != nullptr && packet.data_size > 0) {
                        const char* song_text = (const char*)packet.data;
                        LOG_DEBUG(MUSIC, "Processing song data: %s", song_text);
                        update_music_text(song_text);
                    }
                    break;

                case MUSIC_DATA_TYPE_NAV:
                    if (packet.data != nullptr && packet.data_size > 0) {
                        const char* nav_text = (const char*)packet.data;
                        LOG_DEBUG(MUSIC, "Processing navigation data: %s", nav_text);
                        //for (int i = 0; i < packet.data_size; i++) {
                        //    Serial.printf("%02x ", (unsigned char)packet.data[i]);
                        //}
                        update_navigation_text(nav_text);
                    }
                    break;

                case MUSIC_DATA_TYPE_ICON:
                    if (packet.data != nullptr && packet.data_size > 0) {
                        LOG_DEBUG(MUSIC, "Processing icon data (%zu bytes)", packet.data_size);
                        processIconData(packet.data, packet.data_size);
                    }
                    break;
            }

            // Free packet data
            if (packet.data != nullptr) {
                std::free(packet.data);
            }
        }
    }
}

// Check if music server is connected
bool music_server_is_connected(void)
{
    return music_server_connected;
}

