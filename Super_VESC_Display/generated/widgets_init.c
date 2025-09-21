/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include "gui_guider.h"
#include "widgets_init.h"
#include <stdlib.h>
#include <string.h>


__attribute__((unused)) void kb_event_cb (lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *kb = lv_event_get_target(e);
    if(code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

__attribute__((unused)) void ta_event_cb (lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
#if LV_USE_KEYBOARD || LV_USE_ZH_KEYBOARD
    lv_obj_t *ta = lv_event_get_target(e);
#endif
    lv_obj_t *kb = lv_event_get_user_data(e);
    if (code == LV_EVENT_FOCUSED || code == LV_EVENT_CLICKED)
    {
#if LV_USE_ZH_KEYBOARD != 0
        lv_zh_keyboard_set_textarea(kb, ta);
#endif
#if LV_USE_KEYBOARD != 0
        lv_keyboard_set_textarea(kb, ta);
#endif
        lv_obj_move_foreground(kb);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
    if (code == LV_EVENT_CANCEL || code == LV_EVENT_DEFOCUSED)
    {

#if LV_USE_ZH_KEYBOARD != 0
        lv_zh_keyboard_set_textarea(kb, ta);
#endif
#if LV_USE_KEYBOARD != 0
        lv_keyboard_set_textarea(kb, ta);
#endif
        lv_obj_move_background(kb);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

#if LV_USE_ANALOGCLOCK != 0
void clock_count(int *hour, int *min, int *sec)
{
    (*sec)++;
    if(*sec == 60)
    {
        *sec = 0;
        (*min)++;
    }
    if(*min == 60)
    {
        *min = 0;
        if(*hour < 12)
        {
            (*hour)++;
        } else {
            (*hour)++;
            *hour = *hour %12;
        }
    }
}
#endif


extern int home_digital_clock_time_hour_value;
extern int home_digital_clock_time_min_value;
extern int home_digital_clock_time_sec_value;
extern char home_digital_clock_time_meridiem[];

void home_digital_clock_time_timer(lv_timer_t *timer)
{
    clock_count_12(&home_digital_clock_time_hour_value, &home_digital_clock_time_min_value, &home_digital_clock_time_sec_value, home_digital_clock_time_meridiem);
    if (lv_obj_is_valid(guider_ui.home_digital_clock_time))
    {
        lv_dclock_set_text_fmt(guider_ui.home_digital_clock_time, "%d:%02d %s", home_digital_clock_time_hour_value, home_digital_clock_time_min_value, home_digital_clock_time_meridiem);
    }
}

const lv_img_dsc_t * mode_animimg_map_imgs[7] = {
    &mode_animimg_mapmap_0,
    &mode_animimg_mapmap_01,
    &mode_animimg_mapmap_02,
    &mode_animimg_mapmap_03,
    &mode_animimg_mapmap_04,
    &mode_animimg_mapmap_05,
    &mode_animimg_mapmap_06,
};
extern int mode_digital_clock_hour_value;
extern int mode_digital_clock_min_value;
extern int mode_digital_clock_sec_value;
extern char mode_digital_clock_meridiem[];

void mode_digital_clock_timer(lv_timer_t *timer)
{
    clock_count_12(&mode_digital_clock_hour_value, &mode_digital_clock_min_value, &mode_digital_clock_sec_value, mode_digital_clock_meridiem);
    if (lv_obj_is_valid(guider_ui.mode_digital_clock))
    {
        lv_dclock_set_text_fmt(guider_ui.mode_digital_clock, "%d:%02d %s", mode_digital_clock_hour_value, mode_digital_clock_min_value, mode_digital_clock_meridiem);
    }
}

