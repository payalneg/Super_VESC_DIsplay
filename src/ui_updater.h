/*
	Copyright 2025 Super VESC Display

	UI Updater Module
	Updates LVGL UI with real-time VESC data
*/

#ifndef UI_UPDATER_H_
#define UI_UPDATER_H_

#include <stdint.h>

// Initialize UI updater (manual mode, no timer)
void ui_updater_init(void);

// Initialize UI with zero values
void ui_updater_set_zeros(void);

// Start/stop UI updates
void ui_updater_start(void);
void ui_updater_stop(void);

// Manual UI update (called by timer automatically)
void ui_updater_update(void);

// Update FPS counter (call independently)
void ui_updater_update_fps(void);

#endif /* UI_UPDATER_H_ */

