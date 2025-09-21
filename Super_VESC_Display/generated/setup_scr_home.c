/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



int home_digital_clock_time_min_value = 25;
int home_digital_clock_time_hour_value = 11;
int home_digital_clock_time_sec_value = 50;
char home_digital_clock_time_meridiem[] = "AM";
void setup_scr_home(lv_ui *ui)
{
    //Write codes home
    ui->home = lv_obj_create(NULL);
    lv_obj_set_size(ui->home, 480, 480);
    lv_obj_set_scrollbar_mode(ui->home, LV_SCROLLBAR_MODE_OFF);

    //Write style for home, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->home, &_moto_480x480, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->home, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_meter_board
    ui->home_meter_board = lv_meter_create(ui->home);
    // add scale ui->home_meter_board_scale_0
    ui->home_meter_board_scale_0 = lv_meter_add_scale(ui->home_meter_board);
    lv_meter_set_scale_ticks(ui->home_meter_board, ui->home_meter_board_scale_0, 41, 2, 6, lv_color_hex(0xc8b8b8));
    lv_meter_set_scale_major_ticks(ui->home_meter_board, ui->home_meter_board_scale_0, 5, 2, 10, lv_color_hex(0x00a7c5), 3);
    lv_meter_set_scale_range(ui->home_meter_board, ui->home_meter_board_scale_0, 0, 200, 275, 135);

    // add scale line for ui->home_meter_board_scale_0
    ui->home_meter_board_scale_0_scaleline_0 = lv_meter_add_scale_lines(ui->home_meter_board, ui->home_meter_board_scale_0, lv_color_hex(0x00ff47), lv_color_hex(0x01b51b), true, 0);
    lv_meter_set_indicator_start_value(ui->home_meter_board, ui->home_meter_board_scale_0_scaleline_0, 0);
    lv_meter_set_indicator_end_value(ui->home_meter_board, ui->home_meter_board_scale_0_scaleline_0, 60);

    // add scale line for ui->home_meter_board_scale_0
    ui->home_meter_board_scale_0_scaleline_1 = lv_meter_add_scale_lines(ui->home_meter_board, ui->home_meter_board_scale_0, lv_color_hex(0xffff00), lv_color_hex(0xbc9c00), true, 0);
    lv_meter_set_indicator_start_value(ui->home_meter_board, ui->home_meter_board_scale_0_scaleline_1, 60);
    lv_meter_set_indicator_end_value(ui->home_meter_board, ui->home_meter_board_scale_0_scaleline_1, 120);

    // add scale line for ui->home_meter_board_scale_0
    ui->home_meter_board_scale_0_scaleline_2 = lv_meter_add_scale_lines(ui->home_meter_board, ui->home_meter_board_scale_0, lv_color_hex(0xd8c500), lv_color_hex(0xd80021), true, 0);
    lv_meter_set_indicator_start_value(ui->home_meter_board, ui->home_meter_board_scale_0_scaleline_2, 120);
    lv_meter_set_indicator_end_value(ui->home_meter_board, ui->home_meter_board_scale_0_scaleline_2, 200);

    // add needle images for ui->home_meter_board_scale_0.
    ui->home_meter_board_scale_0_ndline_0 = lv_meter_add_needle_img(ui->home_meter_board, ui->home_meter_board_scale_0, &_needle_alpha_210x210, 105, 105);
    lv_meter_set_indicator_value(ui->home_meter_board, ui->home_meter_board_scale_0_ndline_0, 39);
    lv_obj_set_pos(ui->home_meter_board, 140, 54);
    lv_obj_set_size(ui->home_meter_board, 200, 200);

    //Write style for home_meter_board, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->home_meter_board, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_meter_board, 100, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->home_meter_board, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->home_meter_board, 14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->home_meter_board, 14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->home_meter_board, 14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->home_meter_board, 14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_meter_board, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for home_meter_board, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->home_meter_board, lv_color_hex(0x3affe7), LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_meter_board, &lv_font_montserratMedium_10, LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_meter_board, 255, LV_PART_TICKS|LV_STATE_DEFAULT);

    //Write style for home_meter_board, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->home_meter_board, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->home_meter_board, lv_color_hex(0x000000), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->home_meter_board, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes home_label_digit
    ui->home_label_digit = lv_label_create(ui->home);
    lv_label_set_text(ui->home_label_digit, "40");
    lv_label_set_long_mode(ui->home_label_digit, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->home_label_digit, 190, 102);
    lv_obj_set_size(ui->home_label_digit, 100, 100);

    //Write style for home_label_digit, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->home_label_digit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_label_digit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->home_label_digit, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_label_digit, &lv_font_Antonio_Regular_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_label_digit, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->home_label_digit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->home_label_digit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->home_label_digit, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->home_label_digit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->home_label_digit, 40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->home_label_digit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->home_label_digit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->home_label_digit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->home_label_digit, &_kmbg_100x100, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->home_label_digit, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor(ui->home_label_digit, lv_color_hex(0x3d5340), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->home_label_digit, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_label_digit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_label_trip_num
    ui->home_label_trip_num = lv_label_create(ui->home);
    lv_label_set_text(ui->home_label_trip_num, "12.4");
    lv_label_set_long_mode(ui->home_label_trip_num, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->home_label_trip_num, 43, 85);
    lv_obj_set_size(ui->home_label_trip_num, 104, 35);

    //Write style for home_label_trip_num, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->home_label_trip_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_label_trip_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->home_label_trip_num, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_label_trip_num, &lv_font_Antonio_Regular_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_label_trip_num, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->home_label_trip_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->home_label_trip_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->home_label_trip_num, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->home_label_trip_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->home_label_trip_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->home_label_trip_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->home_label_trip_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->home_label_trip_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_label_trip_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_label_power_num
    ui->home_label_power_num = lv_label_create(ui->home);
    lv_label_set_text(ui->home_label_power_num, "3000");
    lv_label_set_long_mode(ui->home_label_power_num, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->home_label_power_num, 363, 179);
    lv_obj_set_size(ui->home_label_power_num, 81, 35);

    //Write style for home_label_power_num, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->home_label_power_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_label_power_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->home_label_power_num, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_label_power_num, &lv_font_Antonio_Regular_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_label_power_num, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->home_label_power_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->home_label_power_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->home_label_power_num, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->home_label_power_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->home_label_power_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->home_label_power_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->home_label_power_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->home_label_power_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_label_power_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_digital_clock_time
    static bool home_digital_clock_time_timer_enabled = false;
    ui->home_digital_clock_time = lv_dclock_create(ui->home, "11:25 AM");
    if (!home_digital_clock_time_timer_enabled) {
        lv_timer_create(home_digital_clock_time_timer, 1000, NULL);
        home_digital_clock_time_timer_enabled = true;
    }
    lv_obj_set_pos(ui->home_digital_clock_time, 10, 11);
    lv_obj_set_size(ui->home_digital_clock_time, 94, 17);

    //Write style for home_digital_clock_time, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->home_digital_clock_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->home_digital_clock_time, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_digital_clock_time, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_digital_clock_time, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->home_digital_clock_time, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->home_digital_clock_time, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->home_digital_clock_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->home_digital_clock_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->home_digital_clock_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->home_digital_clock_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->home_digital_clock_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_digital_clock_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_bar_battery
    ui->home_bar_battery = lv_bar_create(ui->home);
    lv_obj_set_style_anim_time(ui->home_bar_battery, 1000, 0);
    lv_bar_set_mode(ui->home_bar_battery, LV_BAR_MODE_NORMAL);
    lv_bar_set_range(ui->home_bar_battery, 0, 100);
    lv_bar_set_value(ui->home_bar_battery, 90, LV_ANIM_OFF);
    lv_obj_set_pos(ui->home_bar_battery, 441, 8);
    lv_obj_set_size(ui->home_bar_battery, 26, 14);

    //Write style for home_bar_battery, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->home_bar_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_bar_battery, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_bar_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->home_bar_battery, &_battery_bak_26x14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->home_bar_battery, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->home_bar_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for home_bar_battery, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->home_bar_battery, 0, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_bar_battery, 10, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->home_bar_battery, &_battery_ind_26x14, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->home_bar_battery, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->home_bar_battery, 0, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes home_label_ODO_num
    ui->home_label_ODO_num = lv_label_create(ui->home);
    lv_label_set_text(ui->home_label_ODO_num, "300");
    lv_label_set_long_mode(ui->home_label_ODO_num, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->home_label_ODO_num, 46, 176);
    lv_obj_set_size(ui->home_label_ODO_num, 96, 35);

    //Write style for home_label_ODO_num, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->home_label_ODO_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_label_ODO_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->home_label_ODO_num, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_label_ODO_num, &lv_font_Antonio_Regular_32, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_label_ODO_num, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->home_label_ODO_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->home_label_ODO_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->home_label_ODO_num, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->home_label_ODO_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->home_label_ODO_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->home_label_ODO_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->home_label_ODO_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->home_label_ODO_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_label_ODO_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_img_right
    ui->home_img_right = lv_img_create(ui->home);
    lv_obj_add_flag(ui->home_img_right, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->home_img_right, &_direction_alpha_30x24);
    lv_img_set_pivot(ui->home_img_right, 50,50);
    lv_img_set_angle(ui->home_img_right, 0);
    lv_obj_set_pos(ui->home_img_right, 317, 32);
    lv_obj_set_size(ui->home_img_right, 30, 24);

    //Write style for home_img_right, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->home_img_right, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->home_img_right, lv_color_hex(0x04cf00), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->home_img_right, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_img_right, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->home_img_right, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_img_left
    ui->home_img_left = lv_img_create(ui->home);
    lv_obj_add_flag(ui->home_img_left, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->home_img_left, &_direction_alpha_30x24);
    lv_img_set_pivot(ui->home_img_left, 12,12);
    lv_img_set_angle(ui->home_img_left, 1800);
    lv_obj_set_pos(ui->home_img_left, 130, 32);
    lv_obj_set_size(ui->home_img_left, 30, 24);

    //Write style for home_img_left, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->home_img_left, 239, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->home_img_left, lv_color_hex(0x04cf00), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->home_img_left, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_img_left, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->home_img_left, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_label_WIFI
    ui->home_label_WIFI = lv_label_create(ui->home);
    lv_label_set_text(ui->home_label_WIFI, "" LV_SYMBOL_WIFI "");
    lv_label_set_long_mode(ui->home_label_WIFI, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->home_label_WIFI, 400, 8);
    lv_obj_set_size(ui->home_label_WIFI, 41, 20);

    //Write style for home_label_WIFI, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->home_label_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_label_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->home_label_WIFI, lv_color_hex(0x14ff00), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_label_WIFI, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_label_WIFI, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->home_label_WIFI, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->home_label_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->home_label_WIFI, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->home_label_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->home_label_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->home_label_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->home_label_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->home_label_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_label_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_label_BT
    ui->home_label_BT = lv_label_create(ui->home);
    lv_label_set_text(ui->home_label_BT, "" LV_SYMBOL_BLUETOOTH " ");
    lv_label_set_long_mode(ui->home_label_BT, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->home_label_BT, 374, 8);
    lv_obj_set_size(ui->home_label_BT, 41, 20);

    //Write style for home_label_BT, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->home_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->home_label_BT, lv_color_hex(0x3101fe), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_label_BT, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_label_BT, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->home_label_BT, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->home_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->home_label_BT, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->home_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->home_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->home_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->home_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->home_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_img_moto
    ui->home_img_moto = lv_img_create(ui->home);
    lv_img_set_src(ui->home_img_moto, &_monitor_alpha_25x21);
    lv_img_set_pivot(ui->home_img_moto, 50,50);
    lv_img_set_angle(ui->home_img_moto, 0);
    lv_obj_set_pos(ui->home_img_moto, 366, 102);
    lv_obj_set_size(ui->home_img_moto, 25, 21);
    lv_obj_add_flag(ui->home_img_moto, LV_OBJ_FLAG_CLICKABLE);

    //Write style for home_img_moto, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->home_img_moto, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->home_img_moto, lv_color_hex(0xf3ff00), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->home_img_moto, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_img_moto, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->home_img_moto, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_img_light
    ui->home_img_light = lv_img_create(ui->home);
    lv_img_set_src(ui->home_img_light, &_light_alpha_28x28);
    lv_img_set_pivot(ui->home_img_light, 50,50);
    lv_img_set_angle(ui->home_img_light, 0);
    lv_obj_set_pos(ui->home_img_light, 410, 68);
    lv_obj_set_size(ui->home_img_light, 28, 28);
    lv_obj_add_flag(ui->home_img_light, LV_OBJ_FLAG_CLICKABLE);

    //Write style for home_img_light, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->home_img_light, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->home_img_light, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_img_light, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->home_img_light, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_btn_mode_a
    ui->home_btn_mode_a = lv_btn_create(ui->home);
    ui->home_btn_mode_a_label = lv_label_create(ui->home_btn_mode_a);
    lv_label_set_text(ui->home_btn_mode_a_label, "A");
    lv_label_set_long_mode(ui->home_btn_mode_a_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->home_btn_mode_a_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->home_btn_mode_a, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->home_btn_mode_a_label, LV_PCT(100));
    lv_obj_set_pos(ui->home_btn_mode_a, 156, 239);
    lv_obj_set_size(ui->home_btn_mode_a, 40, 25);

    //Write style for home_btn_mode_a, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->home_btn_mode_a, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->home_btn_mode_a, lv_color_hex(0x2d373d), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->home_btn_mode_a, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->home_btn_mode_a, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_btn_mode_a, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_btn_mode_a, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->home_btn_mode_a, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_btn_mode_a, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_btn_mode_a, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->home_btn_mode_a, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_img_red
    ui->home_img_red = lv_img_create(ui->home);
    lv_obj_add_flag(ui->home_img_red, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->home_img_red, &_image_red_alpha_210x210);
    lv_img_set_pivot(ui->home_img_red, 50,50);
    lv_img_set_angle(ui->home_img_red, 0);
    lv_obj_set_pos(ui->home_img_red, 190, 86);
    lv_obj_set_size(ui->home_img_red, 210, 210);
    lv_obj_add_flag(ui->home_img_red, LV_OBJ_FLAG_HIDDEN);

    //Write style for home_img_red, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->home_img_red, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->home_img_red, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_img_red, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->home_img_red, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_img_green
    ui->home_img_green = lv_img_create(ui->home);
    lv_obj_add_flag(ui->home_img_green, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->home_img_green, &_image_green_alpha_210x210);
    lv_img_set_pivot(ui->home_img_green, 50,50);
    lv_img_set_angle(ui->home_img_green, 0);
    lv_obj_set_pos(ui->home_img_green, 213, 99);
    lv_obj_set_size(ui->home_img_green, 210, 210);
    lv_obj_add_flag(ui->home_img_green, LV_OBJ_FLAG_HIDDEN);

    //Write style for home_img_green, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->home_img_green, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->home_img_green, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_img_green, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->home_img_green, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_btn_mode_f
    ui->home_btn_mode_f = lv_btn_create(ui->home);
    ui->home_btn_mode_f_label = lv_label_create(ui->home_btn_mode_f);
    lv_label_set_text(ui->home_btn_mode_f_label, "F");
    lv_label_set_long_mode(ui->home_btn_mode_f_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->home_btn_mode_f_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->home_btn_mode_f, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->home_btn_mode_f_label, LV_PCT(100));
    lv_obj_set_pos(ui->home_btn_mode_f, 199, 239);
    lv_obj_set_size(ui->home_btn_mode_f, 40, 25);

    //Write style for home_btn_mode_f, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->home_btn_mode_f, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->home_btn_mode_f, lv_color_hex(0x018e94), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->home_btn_mode_f, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->home_btn_mode_f, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_btn_mode_f, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_btn_mode_f, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->home_btn_mode_f, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_btn_mode_f, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_btn_mode_f, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->home_btn_mode_f, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_btn_mode_c
    ui->home_btn_mode_c = lv_btn_create(ui->home);
    ui->home_btn_mode_c_label = lv_label_create(ui->home_btn_mode_c);
    lv_label_set_text(ui->home_btn_mode_c_label, "C");
    lv_label_set_long_mode(ui->home_btn_mode_c_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->home_btn_mode_c_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->home_btn_mode_c, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->home_btn_mode_c_label, LV_PCT(100));
    lv_obj_set_pos(ui->home_btn_mode_c, 242, 239);
    lv_obj_set_size(ui->home_btn_mode_c, 40, 25);

    //Write style for home_btn_mode_c, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->home_btn_mode_c, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->home_btn_mode_c, lv_color_hex(0x508602), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->home_btn_mode_c, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->home_btn_mode_c, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_btn_mode_c, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_btn_mode_c, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->home_btn_mode_c, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_btn_mode_c, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_btn_mode_c, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->home_btn_mode_c, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_btn_mode_e
    ui->home_btn_mode_e = lv_btn_create(ui->home);
    ui->home_btn_mode_e_label = lv_label_create(ui->home_btn_mode_e);
    lv_label_set_text(ui->home_btn_mode_e_label, "E");
    lv_label_set_long_mode(ui->home_btn_mode_e_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->home_btn_mode_e_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->home_btn_mode_e, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->home_btn_mode_e_label, LV_PCT(100));
    lv_obj_set_pos(ui->home_btn_mode_e, 285, 239);
    lv_obj_set_size(ui->home_btn_mode_e, 40, 25);

    //Write style for home_btn_mode_e, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->home_btn_mode_e, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->home_btn_mode_e, lv_color_hex(0x7b8a02), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->home_btn_mode_e, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->home_btn_mode_e, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_btn_mode_e, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->home_btn_mode_e, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->home_btn_mode_e, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->home_btn_mode_e, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->home_btn_mode_e, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->home_btn_mode_e, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes home_img_high_light
    ui->home_img_high_light = lv_img_create(ui->home);
    lv_img_set_src(ui->home_img_high_light, &_high_beam_alpha_28x28);
    lv_img_set_pivot(ui->home_img_high_light, 50,50);
    lv_img_set_angle(ui->home_img_high_light, 0);
    lv_obj_set_pos(ui->home_img_high_light, 410, 124);
    lv_obj_set_size(ui->home_img_high_light, 28, 28);
    lv_obj_add_flag(ui->home_img_high_light, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->home_img_high_light, LV_OBJ_FLAG_CLICKABLE);

    //Write style for home_img_high_light, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->home_img_high_light, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->home_img_high_light, lv_color_hex(0x00edfe), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->home_img_high_light, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->home_img_high_light, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->home_img_high_light, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of home.


    //Update current screen layout.
    lv_obj_update_layout(ui->home);

    //Init events for screen.
    events_init_home(ui);
}
