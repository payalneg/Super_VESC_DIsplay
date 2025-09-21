/*
* Copyright 2024 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/


/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#include "custom.h"

/*********************
 *      DEFINES
 *********************/
#define CHART1_POINTS 40
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static int32_t speed = 39;
static int32_t power = 3000;
static float trip = 12.4;
static int32_t ODO = 288;
static int music_status = 0;
static int16_t spd_chart[CHART1_POINTS] = {0};
static bool is_increase = true;
static bool is_up = true;
static uint32_t track_id = 1;
static const char * music_list[] = {
    "Go Fighting",
    "Because Of You",
    "Need a Better Future"
};

extern lv_ui guider_ui;
/**
 * Create a demo application
 */

/* set the digital label and steering lamp image style. */
static void set_position_x(void * gui, int32_t temp)
{
    lv_obj_set_x(gui, temp);
    if(temp >= 193) {
        lv_obj_set_style_img_recolor_opa(guider_ui.home_img_left, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_add_flag(guider_ui.home_img_light, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(guider_ui.home_img_high_light, LV_OBJ_FLAG_HIDDEN);
        if(temp % 4 == 0) {
            lv_obj_set_style_img_recolor_opa(guider_ui.home_img_right, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
        } else {
            lv_obj_set_style_img_recolor_opa(guider_ui.home_img_right, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        }
    } else {
        lv_obj_set_style_img_recolor_opa(guider_ui.home_img_right, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_clear_flag(guider_ui.home_img_light, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(guider_ui.home_img_high_light, LV_OBJ_FLAG_HIDDEN);
        if(temp % 4 == 0) {
            lv_obj_set_style_img_recolor_opa(guider_ui.home_img_left, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
        } else {
            lv_obj_set_style_img_recolor_opa(guider_ui.home_img_left, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
        }
    }
}

static void set_position_y(void * gui, int32_t temp)
{
    lv_obj_set_y(gui, temp);
}

void custom_init(lv_ui *ui)
{
    /* Add your codes here */
}

void speed_meter_timer_cb(lv_timer_t * t)
{
    lv_ui * gui = t->user_data;

    if(speed >= 160) is_increase = false;
    if(speed <= 30) is_increase = true;

    lv_meter_set_indicator_value(gui->home_meter_board, gui->home_meter_board_scale_0_ndline_0, speed);
    lv_label_set_text_fmt(gui->home_label_digit, "%"LV_PRId32, speed);
    // update trip value
    if(trip < 200.0) trip += 0.001;
    lv_label_set_text_fmt(gui->home_label_trip_num, "%4.1f", trip);
    // update power value
    lv_label_set_text_fmt(gui->home_label_power_num, "%"LV_PRId32, power + speed);
    // update ODO value
    lv_label_set_text_fmt(gui->home_label_ODO_num, "%"LV_PRId32, ODO + (int)trip);
    if(is_increase)
    {
        speed += speed / 20;
    }else
    {
        speed -= speed / 10;
    }
    if(speed <= 60) {
        gui->home_meter_board_scale_0_ndline_0->type_data.needle_img.src = &_image_green_alpha_210x210;
        lv_obj_set_style_bg_img_recolor(gui->home_label_digit, lv_color_hex(0x06ff00), LV_PART_MAIN|LV_STATE_DEFAULT);
	    lv_obj_set_style_bg_img_recolor_opa(gui->home_label_digit, 246, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_img_recolor(gui->home_img_moto, lv_color_hex(0xf3ff00), LV_PART_MAIN|LV_STATE_DEFAULT);
    }else if (speed > 60 && speed <=120){
        gui->home_meter_board_scale_0_ndline_0->type_data.needle_img.src = &_needle_alpha_210x210;
        lv_obj_set_style_bg_img_recolor(gui->home_label_digit, lv_color_hex(0x00a7c5), LV_PART_MAIN|LV_STATE_DEFAULT);
	    lv_obj_set_style_bg_img_recolor_opa(gui->home_label_digit, 246, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_img_recolor(gui->home_img_moto, lv_color_hex(0xf3ff00), LV_PART_MAIN|LV_STATE_DEFAULT);
    }
    else {
        gui->home_meter_board_scale_0_ndline_0->type_data.needle_img.src = &_image_red_alpha_210x210;
        lv_obj_set_style_bg_img_recolor(gui->home_label_digit, lv_color_hex(0xff0027), LV_PART_MAIN|LV_STATE_DEFAULT);
	    lv_obj_set_style_bg_img_recolor_opa(gui->home_label_digit, 246, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_img_recolor(gui->home_img_moto, lv_color_hex(0xff0027), LV_PART_MAIN|LV_STATE_DEFAULT);
    }
}

void home_label_digit_animation(lv_ui *ui)
{
    lv_anim_t a, b;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_position_x);
    lv_anim_set_time(&a, 3000);
    lv_anim_set_playback_time(&a, 2000);
    lv_anim_set_delay(&a, 500);
    lv_anim_set_var(&a, ui->home_label_digit);
    lv_anim_set_values(&a, 180, 205);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
    lv_anim_init(&b);
    lv_anim_set_exec_cb(&b, set_position_y);
    lv_anim_set_time(&b, 4000);
    lv_anim_set_playback_time(&b, 3000);
    lv_anim_set_delay(&b, 500);
    lv_anim_set_var(&b, ui->home_label_digit);
    lv_anim_set_values(&b, 90, 110);
    lv_anim_set_repeat_count(&b, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&b);
}

void digital_cluster_chart_timer_cb(lv_timer_t * t)
{
    // lv_obj_t * obj = t->user_data;
    lv_ui * gui = t->user_data;


    lv_chart_series_t * ser = lv_chart_get_series_next(gui->mode_chart_speed, NULL);
    lv_coord_t * ser_array = lv_chart_get_y_array(gui->mode_chart_speed, ser);


    for(int i = 0; i < CHART1_POINTS - 1; i++)
    {
        spd_chart[i] = spd_chart[i+1];
        ser_array[i] = spd_chart[i];
    }

    if(spd_chart[CHART1_POINTS - 1] > 120) is_up = false;
    if(spd_chart[CHART1_POINTS - 1] < 30) is_up = true;

    if(is_up)
    {
        spd_chart[CHART1_POINTS - 1] += lv_rand(0, 5);
    }else
    {
        spd_chart[CHART1_POINTS - 1] -= lv_rand(0, 5);
    }

    ser_array[CHART1_POINTS - 1] = spd_chart[CHART1_POINTS - 1];
    lv_chart_refresh(gui->mode_chart_speed);
    int16_t speed_num = spd_chart[CHART1_POINTS - 1];
    int16_t temp_num = 40 + speed_num * 0.4;
    lv_label_set_text_fmt(gui->mode_label_ekm_num, "%"LV_PRId32, speed_num);
    lv_label_set_text_fmt(gui->mode_label_fkm_num, "%"LV_PRId32, speed_num);
    lv_label_set_text_fmt(gui->mode_label_temp_num, "%"LV_PRId32, temp_num);
    if(temp_num > 80) {
        lv_obj_set_style_img_recolor_opa(gui->mode_img_temper, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
        lv_obj_set_style_img_recolor(gui->mode_img_temper, lv_color_hex(0xf00000), LV_PART_MAIN|LV_STATE_DEFAULT);
    } else {
        lv_obj_set_style_img_recolor_opa(gui->mode_img_temper, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    }
}

void play_music(lv_ui *ui)
{
    if(music_status == 0) {
        lv_label_set_text(ui->mode_label_play, "" LV_SYMBOL_PAUSE "");
        music_status = 1;
        lv_anim_resume(guider_ui.mode_img_music, (lv_anim_exec_xcb_t)lv_img_set_angle);
        lv_label_set_long_mode(guider_ui.mode_label_name, LV_LABEL_LONG_SCROLL_CIRCULAR);
    }else {
        music_status = 0;
        lv_label_set_text(ui->mode_label_play, "" LV_SYMBOL_PLAY "");
        lv_anim_pause(guider_ui.mode_img_music, (lv_anim_exec_xcb_t)lv_img_set_angle);
        lv_label_set_long_mode(guider_ui.mode_label_name, LV_LABEL_LONG_CLIP);
    }
}

static const void * lv_demo_music_get_list_img(uint32_t track_id)
{
    switch (track_id)
    {
    case 0:
        return &_music_cover_1_alpha_110x110;
        break;
    case 1:
        return &_music_cover_2_alpha_110x110;
        break;
    case 2:
        return &_music_cover_3_alpha_110x110;
        break;
    default:
        return NULL;
        break;
    }
}

void music_album_next(bool next)
{
    if(next) {
        track_id++;
        if(track_id >= 3) track_id = 0;
    } else {
        if(track_id == 0) {
            track_id = 2;
        } else {
            track_id--;
        }
    }
    lv_img_set_src(guider_ui.mode_img_music, lv_demo_music_get_list_img(track_id));
    lv_img_set_angle(guider_ui.mode_img_music, 0);
    lv_label_set_text(guider_ui.mode_label_name, music_list[track_id]);
    if(music_status){
        lv_label_set_long_mode(guider_ui.mode_label_name, LV_LABEL_LONG_SCROLL_CIRCULAR);
    } else {
        lv_label_set_long_mode(guider_ui.mode_label_name, LV_LABEL_LONG_CLIP);
    }
}