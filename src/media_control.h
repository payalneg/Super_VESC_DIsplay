/*
	Copyright 2025 Super VESC Display

	Media Control Module
	Handles current song display and media control via BLE keyboard
*/

#ifndef MEDIA_CONTROL_H_
#define MEDIA_CONTROL_H_

//#ifdef __cplusplus
//extern "C" {
//#endif

#include <Arduino.h>

// Media control commands
typedef enum {
    MEDIA_PLAY_PAUSE = 0xCD,
    MEDIA_NEXT_TRACK = 0xB5,
    MEDIA_PREV_TRACK = 0xB6,
    MEDIA_STOP = 0xB7,
    MEDIA_VOLUME_UP = 0xE9,
    MEDIA_VOLUME_DOWN = 0xEA,
    MEDIA_MUTE = 0xE2
} media_command_t;

// Song information structure
typedef struct {
    char title[128];
    char artist[64];
    char album[64];
    uint32_t duration_ms;
    uint32_t position_ms;
    bool is_playing;
    uint8_t volume;
} song_info_t;

// Function declarations
bool media_control_init(void);
void media_control_loop(void);
void media_control_send_command(media_command_t command);
void media_control_set_song_info(const char* title, const char* artist, const char* album);
void media_control_set_playback_state(bool is_playing, uint32_t position_ms, uint32_t duration_ms);
void media_control_set_volume(uint8_t volume);
song_info_t* media_control_get_song_info(void);

// UI update functions (to be implemented in ui_updater)
void update_current_song_title(const char* title);
void update_current_song_artist(const char* artist);
void update_current_song_album(const char* album);
void update_playback_progress(uint32_t position_ms, uint32_t duration_ms);
void update_playback_state(bool is_playing);
void update_volume_level(uint8_t volume);

//#ifdef __cplusplus
//}
//#endif

#endif /* MEDIA_CONTROL_H_ */
