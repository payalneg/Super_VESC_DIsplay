/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif

#include "custom.h"
static lv_timer_t  * task_meter;
#include "custom.h"
static lv_timer_t * task_chart;

static void home_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        task_meter = lv_timer_create(speed_meter_timer_cb, 100, &guider_ui);
        home_label_digit_animation(&guider_ui);
        break;
    }
    case LV_EVENT_SCREEN_UNLOADED:
    {
        lv_timer_del(task_meter);
        break;
    }
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch(dir) {
        case LV_DIR_LEFT:
        {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.mode, guider_ui.mode_del, &guider_ui.home_del, setup_scr_mode, LV_SCR_LOAD_ANIM_NONE, 0, 100, false, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void home_img_light_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        break;
    }
    default:
        break;
    }
}

static void home_btn_mode_a_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.mode, guider_ui.mode_del, &guider_ui.home_del, setup_scr_mode, LV_SCR_LOAD_ANIM_OVER_LEFT, 100, 100, false, true);
        lv_obj_set_tile(guider_ui.mode_tileview, guider_ui.mode_tileview_mode_a, LV_ANIM_OFF);
        break;
    }
    default:
        break;
    }
}

static void home_btn_mode_f_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.mode, guider_ui.mode_del, &guider_ui.home_del, setup_scr_mode, LV_SCR_LOAD_ANIM_OVER_LEFT, 100, 100, false, true);
        lv_obj_set_tile(guider_ui.mode_tileview, guider_ui.mode_tileview_mode_f, LV_ANIM_OFF);
        break;
    }
    default:
        break;
    }
}

static void home_btn_mode_c_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.mode, guider_ui.mode_del, &guider_ui.home_del, setup_scr_mode, LV_SCR_LOAD_ANIM_OVER_LEFT, 100, 100, false, true);
        lv_obj_set_tile(guider_ui.mode_tileview, guider_ui.mode_tileview_mode_c, LV_ANIM_OFF);
        break;
    }
    default:
        break;
    }
}

static void home_btn_mode_e_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.mode, guider_ui.mode_del, &guider_ui.home_del, setup_scr_mode, LV_SCR_LOAD_ANIM_OVER_LEFT, 100, 100, false, true);
        lv_obj_set_tile(guider_ui.mode_tileview, guider_ui.mode_tileview_mode_e, LV_ANIM_OFF);
        break;
    }
    default:
        break;
    }
}

void events_init_home (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->home, home_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->home_img_light, home_img_light_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->home_btn_mode_a, home_btn_mode_a_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->home_btn_mode_f, home_btn_mode_f_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->home_btn_mode_c, home_btn_mode_c_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->home_btn_mode_e, home_btn_mode_e_event_handler, LV_EVENT_ALL, ui);
}

static void mode_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        lv_obj_clear_flag(guider_ui.mode_tileview, LV_OBJ_FLAG_CLICKABLE);
        task_chart = lv_timer_create(digital_cluster_chart_timer_cb, 100, &guider_ui);
        lv_img_set_pivot(guider_ui.mode_img_music, 55, 55);
        ui_animation(guider_ui.mode_img_music, 4000, 1, lv_img_get_angle(guider_ui.mode_img_music), 360*10, &lv_anim_path_linear, -1, 0, 0, 0, (lv_anim_exec_xcb_t)lv_img_set_angle, NULL, NULL, NULL);
        lv_anim_pause(guider_ui.mode_img_music, (lv_anim_exec_xcb_t)lv_img_set_angle);
        lv_img_set_pivot(guider_ui.mode_img_nav, 50, 50);
        ui_animation(guider_ui.mode_img_nav, 1000, 0, lv_img_get_angle(guider_ui.mode_img_nav), 25*10, &lv_anim_path_linear, -1, 0, 500, 0, (lv_anim_exec_xcb_t)lv_img_set_angle, NULL, NULL, NULL);
        break;
    }
    case LV_EVENT_SCREEN_UNLOADED:
    {
        lv_timer_del(task_chart);

        break;
    }
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        switch(dir) {
        case LV_DIR_RIGHT:
        {
            lv_indev_wait_release(lv_indev_get_act());
            ui_load_scr_animation(&guider_ui, &guider_ui.home, guider_ui.home_del, &guider_ui.mode_del, setup_scr_home, LV_SCR_LOAD_ANIM_NONE, 0, 100, false, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void mode_label_prev_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        music_album_next(false);
        break;
    }
    default:
        break;
    }
}

static void mode_label_play_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        play_music(&guider_ui);
        break;
    }
    default:
        break;
    }
}

static void mode_label_next_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        music_album_next(true);
        break;
    }
    default:
        break;
    }
}

static void mode_img_home_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.home, guider_ui.home_del, &guider_ui.mode_del, setup_scr_home, LV_SCR_LOAD_ANIM_NONE, 100, 0, false, true);
        break;
    }
    default:
        break;
    }
}

void events_init_mode (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->mode, mode_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->mode_label_prev, mode_label_prev_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->mode_label_play, mode_label_play_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->mode_label_next, mode_label_next_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->mode_img_home, mode_img_home_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui *ui)
{

}
