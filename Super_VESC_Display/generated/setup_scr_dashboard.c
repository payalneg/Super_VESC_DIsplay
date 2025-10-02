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
    lv_obj_set_style_bg_img_src(ui->dashboard, &_dashboard_480x480, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->dashboard, 187, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->dashboard, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

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



    //Write codes dashboard_Battery_meter
    ui->dashboard_Battery_meter = lv_meter_create(ui->dashboard_tileview_1_tile);
    // add scale ui->dashboard_Battery_meter_scale_0
    ui->dashboard_Battery_meter_scale_0 = lv_meter_add_scale(ui->dashboard_Battery_meter);
    lv_meter_set_scale_ticks(ui->dashboard_Battery_meter, ui->dashboard_Battery_meter_scale_0, 20, 0, 12, lv_color_hex(0x8e97a6));
    lv_meter_set_scale_range(ui->dashboard_Battery_meter, ui->dashboard_Battery_meter_scale_0, 0, 100, 100, 310);

    // add arc for ui->dashboard_Battery_meter_scale_0
    ui->dashboard_Battery_meter_scale_0_arc_0 = lv_meter_add_arc(ui->dashboard_Battery_meter, ui->dashboard_Battery_meter_scale_0, 25, lv_color_hex(0x2a3440), 15);
    lv_meter_set_indicator_start_value(ui->dashboard_Battery_meter, ui->dashboard_Battery_meter_scale_0_arc_0, 0);
    lv_meter_set_indicator_end_value(ui->dashboard_Battery_meter, ui->dashboard_Battery_meter_scale_0_arc_0, 60);

    // add arc for ui->dashboard_Battery_meter_scale_0
    ui->dashboard_Battery_meter_scale_0_arc_1 = lv_meter_add_arc(ui->dashboard_Battery_meter, ui->dashboard_Battery_meter_scale_0, 10, lv_color_hex(0x2acb48), 0);
    lv_meter_set_indicator_start_value(ui->dashboard_Battery_meter, ui->dashboard_Battery_meter_scale_0_arc_1, 33);
    lv_meter_set_indicator_end_value(ui->dashboard_Battery_meter, ui->dashboard_Battery_meter_scale_0_arc_1, 100);

    // add needle line for ui->dashboard_Battery_meter_scale_0.
    ui->dashboard_Battery_meter_scale_0_ndline_0 = lv_meter_add_needle_line(ui->dashboard_Battery_meter, ui->dashboard_Battery_meter_scale_0, 4, lv_color_hex(0xffffff), 14);
    lv_meter_set_indicator_value(ui->dashboard_Battery_meter, ui->dashboard_Battery_meter_scale_0_ndline_0, 33);
    lv_obj_set_pos(ui->dashboard_Battery_meter, 64, -25);
    lv_obj_set_size(ui->dashboard_Battery_meter, 336, 336);

    //Write style for dashboard_Battery_meter, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_Battery_meter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_Battery_meter, 175, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->dashboard_Battery_meter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->dashboard_Battery_meter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->dashboard_Battery_meter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->dashboard_Battery_meter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->dashboard_Battery_meter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->dashboard_Battery_meter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for dashboard_Battery_meter, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->dashboard_Battery_meter, lv_color_hex(0xffffff), LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->dashboard_Battery_meter, &lv_font_montserratMedium_13, LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->dashboard_Battery_meter, 255, LV_PART_TICKS|LV_STATE_DEFAULT);

    //Write style for dashboard_Battery_meter, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_Battery_meter, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->dashboard_Battery_meter, lv_color_hex(0x2a3440), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->dashboard_Battery_meter, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes dashboard_Current_meter
    ui->dashboard_Current_meter = lv_meter_create(ui->dashboard_tileview_1_tile);
    // add scale ui->dashboard_Current_meter_scale_0
    ui->dashboard_Current_meter_scale_0 = lv_meter_add_scale(ui->dashboard_Current_meter);
    lv_meter_set_scale_ticks(ui->dashboard_Current_meter, ui->dashboard_Current_meter_scale_0, 20, 0, 12, lv_color_hex(0x8e97a6));
    lv_meter_set_scale_range(ui->dashboard_Current_meter, ui->dashboard_Current_meter_scale_0, 0, 60, 100, 130);

    // add arc for ui->dashboard_Current_meter_scale_0
    ui->dashboard_Current_meter_scale_0_arc_0 = lv_meter_add_arc(ui->dashboard_Current_meter, ui->dashboard_Current_meter_scale_0, 25, lv_color_hex(0x2a3440), 15);
    lv_meter_set_indicator_start_value(ui->dashboard_Current_meter, ui->dashboard_Current_meter_scale_0_arc_0, 0);
    lv_meter_set_indicator_end_value(ui->dashboard_Current_meter, ui->dashboard_Current_meter_scale_0_arc_0, 60);

    // add arc for ui->dashboard_Current_meter_scale_0
    ui->dashboard_Current_meter_scale_0_arc_1 = lv_meter_add_arc(ui->dashboard_Current_meter, ui->dashboard_Current_meter_scale_0, 10, lv_color_hex(0xfed2aa), 0);
    lv_meter_set_indicator_start_value(ui->dashboard_Current_meter, ui->dashboard_Current_meter_scale_0_arc_1, 0);
    lv_meter_set_indicator_end_value(ui->dashboard_Current_meter, ui->dashboard_Current_meter_scale_0_arc_1, 18);

    // add needle line for ui->dashboard_Current_meter_scale_0.
    ui->dashboard_Current_meter_scale_0_ndline_0 = lv_meter_add_needle_line(ui->dashboard_Current_meter, ui->dashboard_Current_meter_scale_0, 4, lv_color_hex(0xffffff), 14);
    lv_meter_set_indicator_value(ui->dashboard_Current_meter, ui->dashboard_Current_meter_scale_0_ndline_0, 18);
    lv_obj_set_pos(ui->dashboard_Current_meter, 64, -25);
    lv_obj_set_size(ui->dashboard_Current_meter, 336, 336);

    //Write style for dashboard_Current_meter, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_Current_meter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_Current_meter, 175, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->dashboard_Current_meter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->dashboard_Current_meter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->dashboard_Current_meter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->dashboard_Current_meter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->dashboard_Current_meter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->dashboard_Current_meter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for dashboard_Current_meter, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->dashboard_Current_meter, lv_color_hex(0xffffff), LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->dashboard_Current_meter, &lv_font_montserratMedium_13, LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->dashboard_Current_meter, 255, LV_PART_TICKS|LV_STATE_DEFAULT);

    //Write style for dashboard_Current_meter, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_Current_meter, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->dashboard_Current_meter, lv_color_hex(0x2a3440), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->dashboard_Current_meter, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes dashboard_Speed_meter
    ui->dashboard_Speed_meter = lv_meter_create(ui->dashboard_tileview_1_tile);
    // add scale ui->dashboard_Speed_meter_scale_0
    ui->dashboard_Speed_meter_scale_0 = lv_meter_add_scale(ui->dashboard_Speed_meter);
    lv_meter_set_scale_ticks(ui->dashboard_Speed_meter, ui->dashboard_Speed_meter_scale_0, 13, 3, 9, lv_color_hex(0x8e97a6));
    lv_meter_set_scale_major_ticks(ui->dashboard_Speed_meter, ui->dashboard_Speed_meter_scale_0, 2, 4, 8, lv_color_hex(0xffffff), 7);
    lv_meter_set_scale_range(ui->dashboard_Speed_meter, ui->dashboard_Speed_meter_scale_0, 0, 60, 282, 129);

    // add arc for ui->dashboard_Speed_meter_scale_0
    ui->dashboard_Speed_meter_scale_0_arc_0 = lv_meter_add_arc(ui->dashboard_Speed_meter, ui->dashboard_Speed_meter_scale_0, 10, lv_color_hex(0x0ab8f7), 14);
    lv_meter_set_indicator_start_value(ui->dashboard_Speed_meter, ui->dashboard_Speed_meter_scale_0_arc_0, 0);
    lv_meter_set_indicator_end_value(ui->dashboard_Speed_meter, ui->dashboard_Speed_meter_scale_0_arc_0, 19);

    // add needle line for ui->dashboard_Speed_meter_scale_0.
    ui->dashboard_Speed_meter_scale_0_ndline_0 = lv_meter_add_needle_line(ui->dashboard_Speed_meter, ui->dashboard_Speed_meter_scale_0, 4, lv_color_hex(0x3eb3df), 14);
    lv_meter_set_indicator_value(ui->dashboard_Speed_meter, ui->dashboard_Speed_meter_scale_0_ndline_0, 19);
    lv_obj_set_pos(ui->dashboard_Speed_meter, 95, 5);
    lv_obj_set_size(ui->dashboard_Speed_meter, 274, 274);

    //Write style for dashboard_Speed_meter, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_Speed_meter, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->dashboard_Speed_meter, lv_color_hex(0x1f1f1f), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->dashboard_Speed_meter, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_Speed_meter, 168, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->dashboard_Speed_meter, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->dashboard_Speed_meter, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->dashboard_Speed_meter, lv_color_hex(0x2a3440), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->dashboard_Speed_meter, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->dashboard_Speed_meter, 14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->dashboard_Speed_meter, 14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->dashboard_Speed_meter, 14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->dashboard_Speed_meter, 14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->dashboard_Speed_meter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for dashboard_Speed_meter, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->dashboard_Speed_meter, lv_color_hex(0xffffff), LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->dashboard_Speed_meter, &lv_font_montserratMedium_13, LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->dashboard_Speed_meter, 255, LV_PART_TICKS|LV_STATE_DEFAULT);

    //Write style for dashboard_Speed_meter, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_Speed_meter, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->dashboard_Speed_meter, lv_color_hex(0x2a3440), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->dashboard_Speed_meter, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes dashboard_odo_text
    ui->dashboard_odo_text = lv_textarea_create(ui->dashboard_tileview_1_tile);
    lv_textarea_set_text(ui->dashboard_odo_text, "019018");
    lv_textarea_set_placeholder_text(ui->dashboard_odo_text, "");
    lv_textarea_set_password_bullet(ui->dashboard_odo_text, "*");
    lv_textarea_set_password_mode(ui->dashboard_odo_text, false);
    lv_textarea_set_one_line(ui->dashboard_odo_text, true);
    lv_textarea_set_accepted_chars(ui->dashboard_odo_text, "");
    lv_textarea_set_max_length(ui->dashboard_odo_text, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->dashboard_odo_text, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->dashboard_odo_text, 150, 235);
    lv_obj_set_size(ui->dashboard_odo_text, 100, 39);

    //Write style for dashboard_odo_text, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->dashboard_odo_text, lv_color_hex(0xFFFFFF), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->dashboard_odo_text, &lv_font_Montserrat_I_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->dashboard_odo_text, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->dashboard_odo_text, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->dashboard_odo_text, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
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

    //Write codes dashboard_canvas_1
    ui->dashboard_canvas_1 = lv_canvas_create(ui->dashboard_tileview_1_tile);
    static lv_color_t buf_dashboard_canvas_1[LV_CANVAS_BUF_SIZE_TRUE_COLOR_ALPHA(300, 200)];
    lv_canvas_set_buffer(ui->dashboard_canvas_1, buf_dashboard_canvas_1, 300, 200, LV_IMG_CF_TRUE_COLOR_ALPHA);
    lv_canvas_fill_bg(ui->dashboard_canvas_1, lv_color_hex(0xffffff), 0);
    //Canvas draw arc
    lv_draw_arc_dsc_t dashboard_canvas_1_arc_dsc_0;
    lv_draw_arc_dsc_init(&dashboard_canvas_1_arc_dsc_0);
    dashboard_canvas_1_arc_dsc_0.color = lv_color_hex(0x8e97a6);
    dashboard_canvas_1_arc_dsc_0.opa = 255;
    dashboard_canvas_1_arc_dsc_0.width = 50;
    lv_canvas_draw_arc(ui->dashboard_canvas_1, 100, 100, 78, 0, 360, &dashboard_canvas_1_arc_dsc_0);

    lv_obj_set_pos(ui->dashboard_canvas_1, 132, 44);
    lv_obj_set_size(ui->dashboard_canvas_1, 300, 200);
    lv_obj_set_scrollbar_mode(ui->dashboard_canvas_1, LV_SCROLLBAR_MODE_OFF);

    //Write codes dashboard_canvas_2
    ui->dashboard_canvas_2 = lv_canvas_create(ui->dashboard_tileview_1_tile);
    static lv_color_t buf_dashboard_canvas_2[LV_CANVAS_BUF_SIZE_TRUE_COLOR_ALPHA(300, 200)];
    lv_canvas_set_buffer(ui->dashboard_canvas_2, buf_dashboard_canvas_2, 300, 200, LV_IMG_CF_TRUE_COLOR_ALPHA);
    lv_canvas_fill_bg(ui->dashboard_canvas_2, lv_color_hex(0xffffff), 0);
    //Canvas draw arc
    lv_draw_arc_dsc_t dashboard_canvas_2_arc_dsc_0;
    lv_draw_arc_dsc_init(&dashboard_canvas_2_arc_dsc_0);
    dashboard_canvas_2_arc_dsc_0.color = lv_color_hex(0x2a3440);
    dashboard_canvas_2_arc_dsc_0.opa = 255;
    dashboard_canvas_2_arc_dsc_0.width = 50;
    lv_canvas_draw_arc(ui->dashboard_canvas_2, 100, 100, 78, 0, 360, &dashboard_canvas_2_arc_dsc_0);

    lv_obj_set_pos(ui->dashboard_canvas_2, 132, 43);
    lv_obj_set_size(ui->dashboard_canvas_2, 300, 200);
    lv_obj_set_scrollbar_mode(ui->dashboard_canvas_2, LV_SCROLLBAR_MODE_OFF);

    //Write codes dashboard_canvas_3
    ui->dashboard_canvas_3 = lv_canvas_create(ui->dashboard_tileview_1_tile);
    static lv_color_t buf_dashboard_canvas_3[LV_CANVAS_BUF_SIZE_TRUE_COLOR_ALPHA(300, 200)];
    lv_canvas_set_buffer(ui->dashboard_canvas_3, buf_dashboard_canvas_3, 300, 200, LV_IMG_CF_TRUE_COLOR_ALPHA);
    lv_canvas_fill_bg(ui->dashboard_canvas_3, lv_color_hex(0xffffff), 0);
    //Canvas draw arc
    lv_draw_arc_dsc_t dashboard_canvas_3_arc_dsc_0;
    lv_draw_arc_dsc_init(&dashboard_canvas_3_arc_dsc_0);
    dashboard_canvas_3_arc_dsc_0.color = lv_color_hex(0x2a3440);
    dashboard_canvas_3_arc_dsc_0.opa = 255;
    dashboard_canvas_3_arc_dsc_0.width = 50;
    lv_canvas_draw_arc(ui->dashboard_canvas_3, 100, 100, 40, 0, 360, &dashboard_canvas_3_arc_dsc_0);

    lv_obj_set_pos(ui->dashboard_canvas_3, 132, 42);
    lv_obj_set_size(ui->dashboard_canvas_3, 300, 200);
    lv_obj_set_scrollbar_mode(ui->dashboard_canvas_3, LV_SCROLLBAR_MODE_OFF);

    //Write codes dashboard_ta_1
    ui->dashboard_ta_1 = lv_textarea_create(ui->dashboard_tileview_1_tile);
    lv_textarea_set_text(ui->dashboard_ta_1, "km");
    lv_textarea_set_placeholder_text(ui->dashboard_ta_1, "");
    lv_textarea_set_password_bullet(ui->dashboard_ta_1, "*");
    lv_textarea_set_password_mode(ui->dashboard_ta_1, false);
    lv_textarea_set_one_line(ui->dashboard_ta_1, true);
    lv_textarea_set_accepted_chars(ui->dashboard_ta_1, "");
    lv_textarea_set_max_length(ui->dashboard_ta_1, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->dashboard_ta_1, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->dashboard_ta_1, 182, 234);
    lv_obj_set_size(ui->dashboard_ta_1, 100, 39);

    //Write style for dashboard_ta_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->dashboard_ta_1, lv_color_hex(0x8e97a6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->dashboard_ta_1, &lv_font_Montserrat_I_15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->dashboard_ta_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->dashboard_ta_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->dashboard_ta_1, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->dashboard_ta_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->dashboard_ta_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->dashboard_ta_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->dashboard_ta_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->dashboard_ta_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->dashboard_ta_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_ta_1, 4, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for dashboard_ta_1, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->dashboard_ta_1, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->dashboard_ta_1, lv_color_hex(0x2195f6), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->dashboard_ta_1, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->dashboard_ta_1, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes dashboard_ta_2
    ui->dashboard_ta_2 = lv_textarea_create(ui->dashboard_tileview_1_tile);
    lv_textarea_set_text(ui->dashboard_ta_2, "km/h");
    lv_textarea_set_placeholder_text(ui->dashboard_ta_2, "");
    lv_textarea_set_password_bullet(ui->dashboard_ta_2, "*");
    lv_textarea_set_password_mode(ui->dashboard_ta_2, false);
    lv_textarea_set_one_line(ui->dashboard_ta_2, true);
    lv_textarea_set_accepted_chars(ui->dashboard_ta_2, "");
    lv_textarea_set_max_length(ui->dashboard_ta_2, 32);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->dashboard_ta_2, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->dashboard_ta_2, 164, 173);
    lv_obj_set_size(ui->dashboard_ta_2, 100, 39);

    //Write style for dashboard_ta_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->dashboard_ta_2, lv_color_hex(0x8e97a6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->dashboard_ta_2, &lv_font_Montserrat_I_15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->dashboard_ta_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->dashboard_ta_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->dashboard_ta_2, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
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

    //Write codes dashboard_Speed_text
    ui->dashboard_Speed_text = lv_textarea_create(ui->dashboard_tileview_1_tile);
    lv_textarea_set_text(ui->dashboard_Speed_text, "38");
    lv_textarea_set_placeholder_text(ui->dashboard_Speed_text, "");
    lv_textarea_set_password_bullet(ui->dashboard_Speed_text, "*");
    lv_textarea_set_password_mode(ui->dashboard_Speed_text, false);
    lv_textarea_set_one_line(ui->dashboard_Speed_text, true);
    lv_textarea_set_accepted_chars(ui->dashboard_Speed_text, "");
    lv_textarea_set_max_length(ui->dashboard_Speed_text, 2);
#if LV_USE_KEYBOARD != 0 || LV_USE_ZH_KEYBOARD != 0
    lv_obj_add_event_cb(ui->dashboard_Speed_text, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif
    lv_obj_set_pos(ui->dashboard_Speed_text, 159, 105);
    lv_obj_set_size(ui->dashboard_Speed_text, 150, 110);

    //Write style for dashboard_Speed_text, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->dashboard_Speed_text, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->dashboard_Speed_text, &lv_font_Montserrat_I_61, LV_PART_MAIN|LV_STATE_DEFAULT);
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

    //The custom code of dashboard.


    //Update current screen layout.
    lv_obj_update_layout(ui->dashboard);

}
