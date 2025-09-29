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



void setup_scr_dashboard(lv_ui *ui)
{
    //Write codes dashboard
    ui->dashboard = lv_obj_create(NULL);
    lv_obj_set_size(ui->dashboard, 480, 480);
    lv_obj_set_scrollbar_mode(ui->dashboard, LV_SCROLLBAR_MODE_OFF);

    //Write style for dashboard, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->dashboard, lv_color_hex(0x1f1f1f), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->dashboard, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->dashboard, &_back_480x480, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->dashboard, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes dashboard_tileview_1
    ui->dashboard_tileview_1 = lv_tileview_create(ui->dashboard);
    ui->dashboard_tileview_1_tile = lv_tileview_add_tile(ui->dashboard_tileview_1, 0, 0, LV_DIR_BOTTOM);
    lv_obj_set_pos(ui->dashboard_tileview_1, 4, 4);
    lv_obj_set_size(ui->dashboard_tileview_1, 300, 300);
    lv_obj_set_scrollbar_mode(ui->dashboard_tileview_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for dashboard_tileview_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_tileview_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_tileview_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->dashboard_tileview_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for dashboard_tileview_1, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_tileview_1, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->dashboard_tileview_1, lv_color_hex(0xeaeff3), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->dashboard_tileview_1, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_tileview_1, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);



    //Write codes dashboard_Current_meter
    ui->dashboard_Current_meter = lv_meter_create(ui->dashboard_tileview_1_tile);
    // add scale ui->dashboard_Current_meter_scale_0
    ui->dashboard_Current_meter_scale_0 = lv_meter_add_scale(ui->dashboard_Current_meter);
    lv_meter_set_scale_ticks(ui->dashboard_Current_meter, ui->dashboard_Current_meter_scale_0, 51, 0, 0, lv_color_hex(0x000000));
    lv_meter_set_scale_major_ticks(ui->dashboard_Current_meter, ui->dashboard_Current_meter_scale_0, 10, 2, 8, lv_color_hex(0xffffff), 6);
    lv_meter_set_scale_range(ui->dashboard_Current_meter, ui->dashboard_Current_meter_scale_0, 0, 50, 270, 135);

    // add arc for ui->dashboard_Current_meter_scale_0
    ui->dashboard_Current_meter_scale_0_arc_0 = lv_meter_add_arc(ui->dashboard_Current_meter, ui->dashboard_Current_meter_scale_0, 2, lv_color_hex(0xffffff), 1);
    lv_meter_set_indicator_start_value(ui->dashboard_Current_meter, ui->dashboard_Current_meter_scale_0_arc_0, 0);
    lv_meter_set_indicator_end_value(ui->dashboard_Current_meter, ui->dashboard_Current_meter_scale_0_arc_0, 50);

    // add needle line for ui->dashboard_Current_meter_scale_0.
    ui->dashboard_Current_meter_scale_0_ndline_0 = lv_meter_add_needle_line(ui->dashboard_Current_meter, ui->dashboard_Current_meter_scale_0, 3, lv_color_hex(0x3eb3df), -3);
    lv_meter_set_indicator_value(ui->dashboard_Current_meter, ui->dashboard_Current_meter_scale_0_ndline_0, 20);
    lv_obj_set_pos(ui->dashboard_Current_meter, 0, 0);
    lv_obj_set_size(ui->dashboard_Current_meter, 290, 290);

    //Write style for dashboard_Current_meter, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_Current_meter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_Current_meter, 159, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->dashboard_Current_meter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->dashboard_Current_meter, 17, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->dashboard_Current_meter, 17, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->dashboard_Current_meter, 17, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->dashboard_Current_meter, 17, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->dashboard_Current_meter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for dashboard_Current_meter, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->dashboard_Current_meter, lv_color_hex(0xffffff), LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->dashboard_Current_meter, &lv_font_montserratMedium_10, LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->dashboard_Current_meter, 255, LV_PART_TICKS|LV_STATE_DEFAULT);

    //Write style for dashboard_Current_meter, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_Current_meter, 0, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes dashboard_Current_arc
    ui->dashboard_Current_arc = lv_arc_create(ui->dashboard_tileview_1_tile);
    lv_arc_set_mode(ui->dashboard_Current_arc, LV_ARC_MODE_NORMAL);
    lv_arc_set_range(ui->dashboard_Current_arc, 0, 50);
    lv_arc_set_bg_angles(ui->dashboard_Current_arc, 135, 45);
    lv_arc_set_value(ui->dashboard_Current_arc, 20);
    lv_arc_set_rotation(ui->dashboard_Current_arc, 0);
    lv_obj_set_pos(ui->dashboard_Current_arc, 0, 0);
    lv_obj_set_size(ui->dashboard_Current_arc, 290, 290);

    //Write style for dashboard_Current_arc, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_Current_arc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->dashboard_Current_arc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui->dashboard_Current_arc, 12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->dashboard_Current_arc, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->dashboard_Current_arc, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_Current_arc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->dashboard_Current_arc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->dashboard_Current_arc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->dashboard_Current_arc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->dashboard_Current_arc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->dashboard_Current_arc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for dashboard_Current_arc, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_arc_width(ui->dashboard_Current_arc, 12, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->dashboard_Current_arc, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->dashboard_Current_arc, lv_color_hex(0x3eb3df), LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for dashboard_Current_arc, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_Current_arc, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->dashboard_Current_arc, lv_color_hex(0x3eb3df), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->dashboard_Current_arc, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->dashboard_Current_arc, 0, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes dashboard_arc_1
    ui->dashboard_arc_1 = lv_arc_create(ui->dashboard_tileview_1_tile);
    lv_arc_set_mode(ui->dashboard_arc_1, LV_ARC_MODE_NORMAL);
    lv_arc_set_range(ui->dashboard_arc_1, 0, 100);
    lv_arc_set_bg_angles(ui->dashboard_arc_1, 0, 360);
    lv_arc_set_value(ui->dashboard_arc_1, 0);
    lv_arc_set_rotation(ui->dashboard_arc_1, 0);
    lv_obj_set_style_arc_rounded(ui->dashboard_arc_1, 0,  LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_rounded(ui->dashboard_arc_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_pos(ui->dashboard_arc_1, 45, 45);
    lv_obj_set_size(ui->dashboard_arc_1, 200, 200);

    //Write style for dashboard_arc_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_arc_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->dashboard_arc_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui->dashboard_arc_1, 100, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->dashboard_arc_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->dashboard_arc_1, lv_color_hex(0x1f1f1f), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_arc_1, 112, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->dashboard_arc_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->dashboard_arc_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->dashboard_arc_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->dashboard_arc_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->dashboard_arc_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui->dashboard_arc_1, lv_color_hex(0x2FCADA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui->dashboard_arc_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui->dashboard_arc_1, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_x(ui->dashboard_arc_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_y(ui->dashboard_arc_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for dashboard_arc_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_arc_opa(ui->dashboard_arc_1, 0, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for dashboard_arc_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_arc_1, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->dashboard_arc_1, 0, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes dashboard_Speed_text
    ui->dashboard_Speed_text = lv_textarea_create(ui->dashboard_tileview_1_tile);
    lv_textarea_set_text(ui->dashboard_Speed_text, "40");
    lv_textarea_set_placeholder_text(ui->dashboard_Speed_text, "");
    lv_textarea_set_password_bullet(ui->dashboard_Speed_text, "*");
    lv_textarea_set_password_mode(ui->dashboard_Speed_text, false);
    lv_textarea_set_one_line(ui->dashboard_Speed_text, true);
    lv_textarea_set_accepted_chars(ui->dashboard_Speed_text, "");
    lv_textarea_set_max_length(ui->dashboard_Speed_text, 2);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->dashboard_Speed_text, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->dashboard_Speed_text, 70, 75);
    lv_obj_set_size(ui->dashboard_Speed_text, 150, 110);

    //Write style for dashboard_Speed_text, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->dashboard_Speed_text, lv_color_hex(0x2FCADA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->dashboard_Speed_text, &lv_font_Gotham_Bold_90, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->dashboard_Speed_text, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->dashboard_Speed_text, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->dashboard_Speed_text, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->dashboard_Speed_text, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->dashboard_Speed_text, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->dashboard_Speed_text, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->dashboard_Speed_text, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->dashboard_Speed_text, LV_BORDER_SIDE_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->dashboard_Speed_text, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->dashboard_Speed_text, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->dashboard_Speed_text, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->dashboard_Speed_text, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_Speed_text, 4, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for dashboard_Speed_text, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_Speed_text, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->dashboard_Speed_text, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->dashboard_Speed_text, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_Speed_text, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes dashboard_ta_2
    ui->dashboard_ta_2 = lv_textarea_create(ui->dashboard_tileview_1_tile);
    lv_textarea_set_text(ui->dashboard_ta_2, "Km/h");
    lv_textarea_set_placeholder_text(ui->dashboard_ta_2, "");
    lv_textarea_set_password_bullet(ui->dashboard_ta_2, "*");
    lv_textarea_set_password_mode(ui->dashboard_ta_2, false);
    lv_textarea_set_one_line(ui->dashboard_ta_2, true);
    lv_textarea_set_accepted_chars(ui->dashboard_ta_2, "");
    lv_textarea_set_max_length(ui->dashboard_ta_2, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->dashboard_ta_2, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->dashboard_ta_2, 122, 157);
    lv_obj_set_size(ui->dashboard_ta_2, 62, 34);

    //Write style for dashboard_ta_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->dashboard_ta_2, lv_color_hex(0xFFFFFF), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->dashboard_ta_2, &lv_font_montserratMedium_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->dashboard_ta_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->dashboard_ta_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->dashboard_ta_2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->dashboard_ta_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->dashboard_ta_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->dashboard_ta_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->dashboard_ta_2, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->dashboard_ta_2, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->dashboard_ta_2, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_ta_2, 4, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for dashboard_ta_2, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_ta_2, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->dashboard_ta_2, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->dashboard_ta_2, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_ta_2, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes dashboard_ta_3
    ui->dashboard_ta_3 = lv_textarea_create(ui->dashboard_tileview_1_tile);
    lv_textarea_set_text(ui->dashboard_ta_3, "Total Distance (Km):");
    lv_textarea_set_placeholder_text(ui->dashboard_ta_3, "");
    lv_textarea_set_password_bullet(ui->dashboard_ta_3, "*");
    lv_textarea_set_password_mode(ui->dashboard_ta_3, false);
    lv_textarea_set_one_line(ui->dashboard_ta_3, true);
    lv_textarea_set_accepted_chars(ui->dashboard_ta_3, "");
    lv_textarea_set_max_length(ui->dashboard_ta_3, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->dashboard_ta_3, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->dashboard_ta_3, 46, 199);
    lv_obj_set_size(ui->dashboard_ta_3, 200, 60);

    //Write style for dashboard_ta_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->dashboard_ta_3, lv_color_hex(0xFFFFFF), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->dashboard_ta_3, &lv_font_montserratMedium_10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->dashboard_ta_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->dashboard_ta_3, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->dashboard_ta_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->dashboard_ta_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->dashboard_ta_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->dashboard_ta_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->dashboard_ta_3, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->dashboard_ta_3, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->dashboard_ta_3, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_ta_3, 4, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for dashboard_ta_3, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_ta_3, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->dashboard_ta_3, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->dashboard_ta_3, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_ta_3, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes dashboard_odo_text
    ui->dashboard_odo_text = lv_textarea_create(ui->dashboard_tileview_1_tile);
    lv_textarea_set_text(ui->dashboard_odo_text, "1234.5");
    lv_textarea_set_placeholder_text(ui->dashboard_odo_text, "");
    lv_textarea_set_password_bullet(ui->dashboard_odo_text, "*");
    lv_textarea_set_password_mode(ui->dashboard_odo_text, false);
    lv_textarea_set_one_line(ui->dashboard_odo_text, true);
    lv_textarea_set_accepted_chars(ui->dashboard_odo_text, "");
    lv_textarea_set_max_length(ui->dashboard_odo_text, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->dashboard_odo_text, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->dashboard_odo_text, 47, 213);
    lv_obj_set_size(ui->dashboard_odo_text, 200, 60);

    //Write style for dashboard_odo_text, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->dashboard_odo_text, lv_color_hex(0xFFFFFF), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->dashboard_odo_text, &lv_font_montserratMedium_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->dashboard_odo_text, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->dashboard_odo_text, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->dashboard_odo_text, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->dashboard_odo_text, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->dashboard_odo_text, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->dashboard_odo_text, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->dashboard_odo_text, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->dashboard_odo_text, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->dashboard_odo_text, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_odo_text, 4, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for dashboard_odo_text, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_odo_text, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->dashboard_odo_text, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->dashboard_odo_text, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_odo_text, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes dashboard_tileview_2
    ui->dashboard_tileview_2 = lv_tileview_create(ui->dashboard);
    ui->dashboard_tileview_2_tile = lv_tileview_add_tile(ui->dashboard_tileview_2, 0, 0, LV_DIR_BOTTOM);
    lv_obj_set_pos(ui->dashboard_tileview_2, 300, 14);
    lv_obj_set_size(ui->dashboard_tileview_2, 175, 175);
    lv_obj_set_scrollbar_mode(ui->dashboard_tileview_2, LV_SCROLLBAR_MODE_OFF);

    //Write style for dashboard_tileview_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_tileview_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_tileview_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->dashboard_tileview_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for dashboard_tileview_2, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_tileview_2, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_tileview_2, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);



    //Write codes dashboard_Battery_arc
    ui->dashboard_Battery_arc = lv_arc_create(ui->dashboard_tileview_2_tile);
    lv_arc_set_mode(ui->dashboard_Battery_arc, LV_ARC_MODE_NORMAL);
    lv_arc_set_range(ui->dashboard_Battery_arc, 0, 100);
    lv_arc_set_bg_angles(ui->dashboard_Battery_arc, 180, 0);
    lv_arc_set_value(ui->dashboard_Battery_arc, 75);
    lv_arc_set_rotation(ui->dashboard_Battery_arc, 0);
    lv_obj_set_pos(ui->dashboard_Battery_arc, 20, 20);
    lv_obj_set_size(ui->dashboard_Battery_arc, 130, 130);

    //Write style for dashboard_Battery_arc, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_Battery_arc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->dashboard_Battery_arc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui->dashboard_Battery_arc, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->dashboard_Battery_arc, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->dashboard_Battery_arc, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_Battery_arc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->dashboard_Battery_arc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->dashboard_Battery_arc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->dashboard_Battery_arc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->dashboard_Battery_arc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->dashboard_Battery_arc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for dashboard_Battery_arc, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_arc_width(ui->dashboard_Battery_arc, 10, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->dashboard_Battery_arc, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->dashboard_Battery_arc, lv_color_hex(0x3eb3df), LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for dashboard_Battery_arc, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_Battery_arc, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->dashboard_Battery_arc, lv_color_hex(0x3eb3df), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->dashboard_Battery_arc, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->dashboard_Battery_arc, 0, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes dashboard_meter_1
    ui->dashboard_meter_1 = lv_meter_create(ui->dashboard_tileview_2_tile);
    // add scale ui->dashboard_meter_1_scale_0
    ui->dashboard_meter_1_scale_0 = lv_meter_add_scale(ui->dashboard_meter_1);
    lv_meter_set_scale_ticks(ui->dashboard_meter_1, ui->dashboard_meter_1_scale_0, 28, 0, 0, lv_color_hex(0x000000));
    lv_meter_set_scale_major_ticks(ui->dashboard_meter_1, ui->dashboard_meter_1_scale_0, 12, 2, 16, lv_color_hex(0x1f1f1f), -28);
    lv_meter_set_scale_range(ui->dashboard_meter_1, ui->dashboard_meter_1_scale_0, 25, 82, 103, 223);
    lv_obj_set_pos(ui->dashboard_meter_1, -1, 1);
    lv_obj_set_size(ui->dashboard_meter_1, 170, 170);

    //Write style for dashboard_meter_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_meter_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_meter_1, 160, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->dashboard_meter_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->dashboard_meter_1, 15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->dashboard_meter_1, 15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->dashboard_meter_1, 15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->dashboard_meter_1, 15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->dashboard_meter_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for dashboard_meter_1, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->dashboard_meter_1, lv_color_hex(0xffffff), LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->dashboard_meter_1, &lv_font_montserratMedium_10, LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->dashboard_meter_1, 255, LV_PART_TICKS|LV_STATE_DEFAULT);

    //Write style for dashboard_meter_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_meter_1, 0, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes dashboard_ta_1
    ui->dashboard_ta_1 = lv_textarea_create(ui->dashboard_tileview_2_tile);
    lv_textarea_set_text(ui->dashboard_ta_1, "13");
    lv_textarea_set_placeholder_text(ui->dashboard_ta_1, "");
    lv_textarea_set_password_bullet(ui->dashboard_ta_1, "*");
    lv_textarea_set_password_mode(ui->dashboard_ta_1, false);
    lv_textarea_set_one_line(ui->dashboard_ta_1, true);
    lv_textarea_set_accepted_chars(ui->dashboard_ta_1, "");
    lv_textarea_set_max_length(ui->dashboard_ta_1, 2);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->dashboard_ta_1, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->dashboard_ta_1, 50, 41);
    lv_obj_set_size(ui->dashboard_ta_1, 69, 60);

    //Write style for dashboard_ta_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->dashboard_ta_1, lv_color_hex(0x2FCADA), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->dashboard_ta_1, &lv_font_Gotham_Bold_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->dashboard_ta_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->dashboard_ta_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->dashboard_ta_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->dashboard_ta_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->dashboard_ta_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->dashboard_ta_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->dashboard_ta_1, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->dashboard_ta_1, LV_BORDER_SIDE_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->dashboard_ta_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->dashboard_ta_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->dashboard_ta_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->dashboard_ta_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_ta_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for dashboard_ta_1, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_ta_1, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->dashboard_ta_1, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->dashboard_ta_1, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_ta_1, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes dashboard_ta_4
    ui->dashboard_ta_4 = lv_textarea_create(ui->dashboard_tileview_2_tile);
    lv_textarea_set_text(ui->dashboard_ta_4, "% Battery Left");
    lv_textarea_set_placeholder_text(ui->dashboard_ta_4, "");
    lv_textarea_set_password_bullet(ui->dashboard_ta_4, "*");
    lv_textarea_set_password_mode(ui->dashboard_ta_4, false);
    lv_textarea_set_one_line(ui->dashboard_ta_4, false);
    lv_textarea_set_accepted_chars(ui->dashboard_ta_4, "");
    lv_textarea_set_max_length(ui->dashboard_ta_4, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->dashboard_ta_4, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->dashboard_ta_4, 33, 82);
    lv_obj_set_size(ui->dashboard_ta_4, 118, 37);

    //Write style for dashboard_ta_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->dashboard_ta_4, lv_color_hex(0xFFFFFF), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->dashboard_ta_4, &lv_font_montserratMedium_10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->dashboard_ta_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->dashboard_ta_4, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->dashboard_ta_4, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->dashboard_ta_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->dashboard_ta_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->dashboard_ta_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->dashboard_ta_4, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->dashboard_ta_4, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->dashboard_ta_4, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_ta_4, 4, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for dashboard_ta_4, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_ta_4, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->dashboard_ta_4, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->dashboard_ta_4, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_ta_4, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes dashboard_slider_1
    ui->dashboard_slider_1 = lv_slider_create(ui->dashboard);
    lv_slider_set_range(ui->dashboard_slider_1, 0, 100);
    lv_slider_set_mode(ui->dashboard_slider_1, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->dashboard_slider_1, 50, LV_ANIM_OFF);
    lv_obj_set_pos(ui->dashboard_slider_1, 128, 330);
    lv_obj_set_size(ui->dashboard_slider_1, 160, 8);

    //Write style for dashboard_slider_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_slider_1, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->dashboard_slider_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->dashboard_slider_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_slider_1, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->dashboard_slider_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->dashboard_slider_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for dashboard_slider_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_slider_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->dashboard_slider_1, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->dashboard_slider_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_slider_1, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for dashboard_slider_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_slider_1, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->dashboard_slider_1, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->dashboard_slider_1, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_slider_1, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //The custom code of dashboard.


    //Update current screen layout.
    lv_obj_update_layout(ui->dashboard);

    //Init events for screen.
    events_init_dashboard(ui);
}
