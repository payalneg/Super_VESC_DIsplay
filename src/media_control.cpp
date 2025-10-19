/*
	Copyright 2025 Super VESC Display

	Media Control Module Implementation
	Handles current song display and media control via BLE keyboard
*/

#include "media_control.h"
#include "ble_keyboard.h"
#include "debug_log.h"

// Global song information
static song_info_t current_song = {0};
static bool media_control_initialized = false;

// Initialize media control
bool media_control_init(void) {
    LOG_INFO(MEDIA, "Initializing media control...");
    
    // Initialize song info
    memset(&current_song, 0, sizeof(current_song));
    strcpy(current_song.title, "No Song");
    strcpy(current_song.artist, "Unknown Artist");
    strcpy(current_song.album, "Unknown Album");
    current_song.is_playing = false;
    current_song.volume = 50;
    
    media_control_initialized = true;
    LOG_INFO(MEDIA, "Media control initialized successfully");
    return true;
}

// Main processing loop
void media_control_loop(void) {
    if (!media_control_initialized) {
        return;
    }
    
    // Update UI with current song information
    static unsigned long last_update = 0;
    if ((millis() - last_update) > 1000) { // Update every second
        last_update = millis();
        
        update_current_song_title(current_song.title);
        update_current_song_artist(current_song.artist);
        update_current_song_album(current_song.album);
        update_playback_progress(current_song.position_ms, current_song.duration_ms);
        update_playback_state(current_song.is_playing);
        update_volume_level(current_song.volume);
    }
}

// Send media control command via BLE keyboard
void media_control_send_command(media_command_t command) {
    if (!media_control_initialized) {
        LOG_WARN(MEDIA, "Media control not initialized");
        return;
    }
    
    LOG_INFO(MEDIA, "Sending media command: 0x%02X", command);
    ble_keyboard_send_media_key(command);
}

// Set song information
void media_control_set_song_info(const char* title, const char* artist, const char* album) {
    if (!media_control_initialized) {
        return;
    }
    
    if (title) {
        strncpy(current_song.title, title, sizeof(current_song.title) - 1);
        current_song.title[sizeof(current_song.title) - 1] = '\0';
    }
    
    if (artist) {
        strncpy(current_song.artist, artist, sizeof(current_song.artist) - 1);
        current_song.artist[sizeof(current_song.artist) - 1] = '\0';
    }
    
    if (album) {
        strncpy(current_song.album, album, sizeof(current_song.album) - 1);
        current_song.album[sizeof(current_song.album) - 1] = '\0';
    }
    
    LOG_INFO(MEDIA, "Song info updated: %s - %s (%s)", 
             current_song.artist, current_song.title, current_song.album);
}

// Set playback state
void media_control_set_playback_state(bool is_playing, uint32_t position_ms, uint32_t duration_ms) {
    if (!media_control_initialized) {
        return;
    }
    
    current_song.is_playing = is_playing;
    current_song.position_ms = position_ms;
    current_song.duration_ms = duration_ms;
    
    LOG_DEBUG(MEDIA, "Playback state: %s, %lu/%lu ms", 
              is_playing ? "Playing" : "Paused", position_ms, duration_ms);
}

// Set volume level
void media_control_set_volume(uint8_t volume) {
    if (!media_control_initialized) {
        return;
    }
    
    current_song.volume = volume;
    LOG_DEBUG(MEDIA, "Volume set to: %d%%", volume);
}

// Get current song information
song_info_t* media_control_get_song_info(void) {
    return &current_song;
}
