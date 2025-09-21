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



int mode_digital_clock_min_value = 25;
int mode_digital_clock_hour_value = 11;
int mode_digital_clock_sec_value = 50;
char mode_digital_clock_meridiem[] = "AM";
void setup_scr_mode(lv_ui *ui)
{
    //Write codes mode
    ui->mode = lv_obj_create(NULL);
    lv_obj_set_size(ui->mode, 480, 480);
    lv_obj_set_scrollbar_mode(ui->mode, LV_SCROLLBAR_MODE_OFF);

    //Write style for mode, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->mode, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->mode, lv_color_hex(0x070707), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->mode, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->mode, &_moto_480x480, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->mode, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->mode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_tileview
    ui->mode_tileview = lv_tileview_create(ui->mode);
    ui->mode_tileview_mode_a = lv_tileview_add_tile(ui->mode_tileview, 0, 0, LV_DIR_RIGHT);
    ui->mode_tileview_mode_f = lv_tileview_add_tile(ui->mode_tileview, 1, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
    ui->mode_tileview_mode_c = lv_tileview_add_tile(ui->mode_tileview, 2, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
    ui->mode_tileview_mode_e = lv_tileview_add_tile(ui->mode_tileview, 3, 0, LV_DIR_LEFT);
    lv_obj_set_pos(ui->mode_tileview, 0, 0);
    lv_obj_set_size(ui->mode_tileview, 480, 272);
    lv_obj_set_scrollbar_mode(ui->mode_tileview, LV_SCROLLBAR_MODE_OFF);

    //Write style for mode_tileview, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->mode_tileview, 246, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->mode_tileview, lv_color_hex(0x070707), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->mode_tileview, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_tileview, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_tileview, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for mode_tileview, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->mode_tileview, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->mode_tileview, lv_color_hex(0xeaeff3), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->mode_tileview, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_tileview, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);



    //Write codes mode_img_abg
    ui->mode_img_abg = lv_img_create(ui->mode_tileview_mode_a);
    lv_obj_add_flag(ui->mode_img_abg, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->mode_img_abg, &_det_bak_alpha_480x272);
    lv_img_set_pivot(ui->mode_img_abg, 50,50);
    lv_img_set_angle(ui->mode_img_abg, 0);
    lv_obj_set_pos(ui->mode_img_abg, 0, 0);
    lv_obj_set_size(ui->mode_img_abg, 480, 272);

    //Write style for mode_img_abg, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->mode_img_abg, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->mode_img_abg, lv_color_hex(0xd0ff00), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->mode_img_abg, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_img_abg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->mode_img_abg, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_cont
    ui->mode_cont = lv_obj_create(ui->mode_tileview_mode_a);
    lv_obj_set_pos(ui->mode_cont, 169, 96);
    lv_obj_set_size(ui->mode_cont, 45, 74);
    lv_obj_set_scrollbar_mode(ui->mode_cont, LV_SCROLLBAR_MODE_OFF);

    //Write style for mode_cont, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_cont, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_cont, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->mode_cont, lv_color_hex(0x90a907), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->mode_cont, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_cont, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_mode
    ui->mode_label_mode = lv_label_create(ui->mode_cont);
    lv_label_set_text(ui->mode_label_mode, "Mode\n");
    lv_label_set_long_mode(ui->mode_label_mode, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_mode, 0, 6);
    lv_obj_set_size(ui->mode_label_mode, 46, 13);

    //Write style for mode_label_mode, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_mode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_mode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_mode, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_mode, &lv_font_Amiko_Regular_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_mode, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_mode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_mode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_mode, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_mode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_mode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_mode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_mode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_mode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_mode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_A
    ui->mode_label_A = lv_label_create(ui->mode_cont);
    lv_label_set_text(ui->mode_label_A, "A\n");
    lv_label_set_long_mode(ui->mode_label_A, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_A, 4, 25);
    lv_obj_set_size(ui->mode_label_A, 35, 49);

    //Write style for mode_label_A, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_A, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_A, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_A, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_A, &lv_font_Amiko_Regular_50, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_A, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_A, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_A, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_A, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_A, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_A, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_A, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_A, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_A, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_A, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_img_nav
    ui->mode_img_nav = lv_img_create(ui->mode_tileview_mode_a);
    lv_obj_add_flag(ui->mode_img_nav, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->mode_img_nav, &_img_nav_1_alpha_70x70);
    lv_img_set_pivot(ui->mode_img_nav, 50,50);
    lv_img_set_angle(ui->mode_img_nav, 0);
    lv_obj_set_pos(ui->mode_img_nav, 69, 69);
    lv_obj_set_size(ui->mode_img_nav, 70, 70);

    //Write style for mode_img_nav, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->mode_img_nav, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->mode_img_nav, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_img_nav, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->mode_img_nav, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_spangroup_1
    ui->mode_spangroup_1 = lv_spangroup_create(ui->mode_tileview_mode_a);
    lv_spangroup_set_align(ui->mode_spangroup_1, LV_TEXT_ALIGN_LEFT);
    lv_spangroup_set_overflow(ui->mode_spangroup_1, LV_SPAN_OVERFLOW_CLIP);
    lv_spangroup_set_mode(ui->mode_spangroup_1, LV_SPAN_MODE_BREAK);
    //create span
    ui->mode_spangroup_1_span = lv_spangroup_new_span(ui->mode_spangroup_1);
    lv_span_set_text(ui->mode_spangroup_1_span, "100 m");
    lv_style_set_text_color(&ui->mode_spangroup_1_span->style, lv_color_hex(0xffffff));
    lv_style_set_text_decor(&ui->mode_spangroup_1_span->style, LV_TEXT_DECOR_NONE);
    lv_style_set_text_font(&ui->mode_spangroup_1_span->style, &lv_font_montserratMedium_26);
    ui->mode_spangroup_1_span = lv_spangroup_new_span(ui->mode_spangroup_1);
    lv_span_set_text(ui->mode_spangroup_1_span, "turn right");
    lv_style_set_text_color(&ui->mode_spangroup_1_span->style, lv_color_hex(0xd3d3d3));
    lv_style_set_text_decor(&ui->mode_spangroup_1_span->style, LV_TEXT_DECOR_NONE);
    lv_style_set_text_font(&ui->mode_spangroup_1_span->style, &lv_font_FontAwesome5_15);
    lv_obj_set_pos(ui->mode_spangroup_1, 59, 153);
    lv_obj_set_size(ui->mode_spangroup_1, 96, 50);

    //Write style state: LV_STATE_DEFAULT for &style_mode_spangroup_1_main_main_default
    static lv_style_t style_mode_spangroup_1_main_main_default;
    ui_init_style(&style_mode_spangroup_1_main_main_default);

    lv_style_set_border_width(&style_mode_spangroup_1_main_main_default, 0);
    lv_style_set_radius(&style_mode_spangroup_1_main_main_default, 0);
    lv_style_set_bg_opa(&style_mode_spangroup_1_main_main_default, 0);
    lv_style_set_pad_top(&style_mode_spangroup_1_main_main_default, 0);
    lv_style_set_pad_right(&style_mode_spangroup_1_main_main_default, 0);
    lv_style_set_pad_bottom(&style_mode_spangroup_1_main_main_default, 0);
    lv_style_set_pad_left(&style_mode_spangroup_1_main_main_default, 0);
    lv_style_set_shadow_width(&style_mode_spangroup_1_main_main_default, 0);
    lv_obj_add_style(ui->mode_spangroup_1, &style_mode_spangroup_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_spangroup_refr_mode(ui->mode_spangroup_1);

    //Write codes mode_animimg_map
    ui->mode_animimg_map = lv_animimg_create(ui->mode_tileview_mode_a);
    lv_animimg_set_src(ui->mode_animimg_map, (const void **) mode_animimg_map_imgs, 7, false);
    lv_animimg_set_duration(ui->mode_animimg_map, 250*7);
    lv_animimg_set_repeat_count(ui->mode_animimg_map, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_set_playback_time(ui->mode_animimg_map, 1000);
    lv_animimg_set_playback_delay(ui->mode_animimg_map, 500);
    lv_animimg_start(ui->mode_animimg_map);
    lv_obj_set_pos(ui->mode_animimg_map, 222, 27);
    lv_obj_set_size(ui->mode_animimg_map, 210, 210);

    //Write codes mode_label_dot
    ui->mode_label_dot = lv_label_create(ui->mode_tileview_mode_a);
    lv_label_set_text(ui->mode_label_dot, "");
    lv_label_set_long_mode(ui->mode_label_dot, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_dot, 311, 121);
    lv_obj_set_size(ui->mode_label_dot, 22, 22);

    //Write style for mode_label_dot, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_dot, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->mode_label_dot, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->mode_label_dot, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->mode_label_dot, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_dot, 22, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_dot, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_dot, &lv_font_arial_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_dot, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_dot, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_dot, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_dot, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_dot, 208, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->mode_label_dot, lv_color_hex(0x495be3), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->mode_label_dot, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_dot, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_dot, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_dot, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_dot, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_dot, 0, LV_PART_MAIN|LV_STATE_DEFAULT);



    //Write codes mode_label_fkm_num
    ui->mode_label_fkm_num = lv_label_create(ui->mode_tileview_mode_f);
    lv_label_set_text(ui->mode_label_fkm_num, "26");
    lv_label_set_long_mode(ui->mode_label_fkm_num, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_fkm_num, 267, 87);
    lv_obj_set_size(ui->mode_label_fkm_num, 146, 61);

    //Write style for mode_label_fkm_num, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_fkm_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_fkm_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_fkm_num, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_fkm_num, &lv_font_Antonio_Regular_60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_fkm_num, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_fkm_num, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_fkm_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_fkm_num, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_fkm_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_fkm_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_fkm_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_fkm_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_fkm_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_fkm_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_fkm
    ui->mode_label_fkm = lv_label_create(ui->mode_tileview_mode_f);
    lv_label_set_text(ui->mode_label_fkm, "Km/h");
    lv_label_set_long_mode(ui->mode_label_fkm, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_fkm, 306, 165);
    lv_obj_set_size(ui->mode_label_fkm, 68, 16);

    //Write style for mode_label_fkm, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_fkm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_fkm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_fkm, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_fkm, &lv_font_Abel_regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_fkm, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_fkm, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_fkm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_fkm, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_fkm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_fkm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_fkm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_fkm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_fkm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_fkm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_contf
    ui->mode_contf = lv_obj_create(ui->mode_tileview_mode_f);
    lv_obj_set_pos(ui->mode_contf, 169, 96);
    lv_obj_set_size(ui->mode_contf, 45, 74);
    lv_obj_set_scrollbar_mode(ui->mode_contf, LV_SCROLLBAR_MODE_OFF);

    //Write style for mode_contf, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_contf, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_contf, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_contf, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->mode_contf, lv_color_hex(0x90a907), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->mode_contf, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_contf, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_contf, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_contf, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_contf, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_contf, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_fmode
    ui->mode_label_fmode = lv_label_create(ui->mode_contf);
    lv_label_set_text(ui->mode_label_fmode, "Mode\n");
    lv_label_set_long_mode(ui->mode_label_fmode, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_fmode, 0, 6);
    lv_obj_set_size(ui->mode_label_fmode, 46, 13);

    //Write style for mode_label_fmode, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_fmode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_fmode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_fmode, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_fmode, &lv_font_Amiko_Regular_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_fmode, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_fmode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_fmode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_fmode, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_fmode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_fmode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_fmode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_fmode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_fmode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_fmode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_F
    ui->mode_label_F = lv_label_create(ui->mode_contf);
    lv_label_set_text(ui->mode_label_F, "F\n");
    lv_label_set_long_mode(ui->mode_label_F, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_F, 4, 20);
    lv_obj_set_size(ui->mode_label_F, 35, 49);

    //Write style for mode_label_F, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_F, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_F, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_F, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_F, &lv_font_Amiko_Regular_60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_F, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_F, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_F, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_F, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_F, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_F, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_F, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_F, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_F, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_F, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_speed
    ui->mode_label_speed = lv_label_create(ui->mode_tileview_mode_f);
    lv_label_set_text(ui->mode_label_speed, "Speed");
    lv_label_set_long_mode(ui->mode_label_speed, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_speed, 122, 45);
    lv_obj_set_size(ui->mode_label_speed, 139, 48);

    //Write style for mode_label_speed, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_speed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_speed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_speed, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_speed, &lv_font_Antonio_Regular_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_speed, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_speed, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_speed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_speed, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_speed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_speed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_speed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_speed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_speed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_speed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_temp_num
    ui->mode_label_temp_num = lv_label_create(ui->mode_tileview_mode_f);
    lv_label_set_text(ui->mode_label_temp_num, "24");
    lv_label_set_long_mode(ui->mode_label_temp_num, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_temp_num, 84, 129);
    lv_obj_set_size(ui->mode_label_temp_num, 50, 36);

    //Write style for mode_label_temp_num, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_temp_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_temp_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_temp_num, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_temp_num, &lv_font_Acme_Regular_30, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_temp_num, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_temp_num, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_temp_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_temp_num, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_temp_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_temp_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_temp_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_temp_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_temp_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_temp_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_img_temper
    ui->mode_img_temper = lv_img_create(ui->mode_tileview_mode_f);
    lv_obj_add_flag(ui->mode_img_temper, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->mode_img_temper, &_tem_alpha_55x70);
    lv_img_set_pivot(ui->mode_img_temper, 50,50);
    lv_img_set_angle(ui->mode_img_temper, 0);
    lv_obj_set_pos(ui->mode_img_temper, 40, 102);
    lv_obj_set_size(ui->mode_img_temper, 55, 70);

    //Write style for mode_img_temper, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->mode_img_temper, 208, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->mode_img_temper, lv_color_hex(0x0dff51), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->mode_img_temper, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_img_temper, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->mode_img_temper, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_img_fbg
    ui->mode_img_fbg = lv_img_create(ui->mode_tileview_mode_f);
    lv_obj_add_flag(ui->mode_img_fbg, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->mode_img_fbg, &_det_bak_alpha_480x272);
    lv_img_set_pivot(ui->mode_img_fbg, 50,50);
    lv_img_set_angle(ui->mode_img_fbg, 0);
    lv_obj_set_pos(ui->mode_img_fbg, 0, 0);
    lv_obj_set_size(ui->mode_img_fbg, 480, 272);

    //Write style for mode_img_fbg, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->mode_img_fbg, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->mode_img_fbg, lv_color_hex(0xfff000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->mode_img_fbg, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_img_fbg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->mode_img_fbg, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_tempter
    ui->mode_label_tempter = lv_label_create(ui->mode_tileview_mode_f);
    lv_label_set_text(ui->mode_label_tempter, "°C");
    lv_label_set_long_mode(ui->mode_label_tempter, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_tempter, 122, 128);
    lv_obj_set_size(ui->mode_label_tempter, 25, 18);

    //Write style for mode_label_tempter, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_tempter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_tempter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_tempter, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_tempter, &lv_font_Acme_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_tempter, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_tempter, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_tempter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_tempter, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_tempter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_tempter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_tempter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_tempter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_tempter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_tempter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);



    //Write codes mode_cont_4
    ui->mode_cont_4 = lv_obj_create(ui->mode_tileview_mode_c);
    lv_obj_set_pos(ui->mode_cont_4, 169, 96);
    lv_obj_set_size(ui->mode_cont_4, 45, 74);
    lv_obj_set_scrollbar_mode(ui->mode_cont_4, LV_SCROLLBAR_MODE_OFF);

    //Write style for mode_cont_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_cont_4, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_cont_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->mode_cont_4, lv_color_hex(0x90a907), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->mode_cont_4, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_cont_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_cmode
    ui->mode_label_cmode = lv_label_create(ui->mode_cont_4);
    lv_label_set_text(ui->mode_label_cmode, "Mode\n");
    lv_label_set_long_mode(ui->mode_label_cmode, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_cmode, 0, 6);
    lv_obj_set_size(ui->mode_label_cmode, 46, 13);

    //Write style for mode_label_cmode, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_cmode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_cmode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_cmode, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_cmode, &lv_font_Amiko_Regular_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_cmode, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_cmode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_cmode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_cmode, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_cmode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_cmode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_cmode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_cmode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_cmode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_cmode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_C
    ui->mode_label_C = lv_label_create(ui->mode_cont_4);
    lv_label_set_text(ui->mode_label_C, "C\n");
    lv_label_set_long_mode(ui->mode_label_C, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_C, 4, 25);
    lv_obj_set_size(ui->mode_label_C, 35, 49);

    //Write style for mode_label_C, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_C, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_C, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_C, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_C, &lv_font_Amiko_Regular_50, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_C, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_C, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_C, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_C, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_C, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_C, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_C, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_C, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_C, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_C, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_ctemp_num
    ui->mode_label_ctemp_num = lv_label_create(ui->mode_tileview_mode_c);
    lv_label_set_text(ui->mode_label_ctemp_num, "20");
    lv_label_set_long_mode(ui->mode_label_ctemp_num, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_ctemp_num, 59, 78);
    lv_obj_set_size(ui->mode_label_ctemp_num, 66, 47);

    //Write style for mode_label_ctemp_num, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_ctemp_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_ctemp_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_ctemp_num, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_ctemp_num, &lv_font_Antonio_Regular_50, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_ctemp_num, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_ctemp_num, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_ctemp_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_ctemp_num, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_ctemp_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_ctemp_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_ctemp_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_ctemp_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_ctemp_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_ctemp_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_temperature
    ui->mode_label_temperature = lv_label_create(ui->mode_tileview_mode_c);
    lv_label_set_text(ui->mode_label_temperature, "°C");
    lv_label_set_long_mode(ui->mode_label_temperature, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_temperature, 112, 74);
    lv_obj_set_size(ui->mode_label_temperature, 24, 17);

    //Write style for mode_label_temperature, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_temperature, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_temperature, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_temperature, lv_color_hex(0xfeefef), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_temperature, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_temperature, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_temperature, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_temperature, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_temperature, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_temperature, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_temperature, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_temperature, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_temperature, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_temperature, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_temperature, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_day
    ui->mode_label_day = lv_label_create(ui->mode_tileview_mode_c);
    lv_label_set_text(ui->mode_label_day, "Wednesday\n");
    lv_label_set_long_mode(ui->mode_label_day, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_day, 54, 177);
    lv_obj_set_size(ui->mode_label_day, 102, 18);

    //Write style for mode_label_day, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_day, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_day, &lv_font_SourceHanSerifSC_Regular_13, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_day, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_day, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_day, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_img_weather
    ui->mode_img_weather = lv_img_create(ui->mode_tileview_mode_c);
    lv_obj_add_flag(ui->mode_img_weather, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->mode_img_weather, &_sunny_alpha_40x40);
    lv_img_set_pivot(ui->mode_img_weather, 50,50);
    lv_img_set_angle(ui->mode_img_weather, 0);
    lv_obj_set_pos(ui->mode_img_weather, 119, 130);
    lv_obj_set_size(ui->mode_img_weather, 40, 40);

    //Write style for mode_img_weather, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->mode_img_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->mode_img_weather, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_img_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->mode_img_weather, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_img_cbg
    ui->mode_img_cbg = lv_img_create(ui->mode_tileview_mode_c);
    lv_obj_add_flag(ui->mode_img_cbg, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->mode_img_cbg, &_det_bak_alpha_480x272);
    lv_img_set_pivot(ui->mode_img_cbg, 50,50);
    lv_img_set_angle(ui->mode_img_cbg, 0);
    lv_obj_set_pos(ui->mode_img_cbg, 0, 0);
    lv_obj_set_size(ui->mode_img_cbg, 480, 272);

    //Write style for mode_img_cbg, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->mode_img_cbg, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->mode_img_cbg, lv_color_hex(0xf900ff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->mode_img_cbg, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_img_cbg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->mode_img_cbg, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_name
    ui->mode_label_name = lv_label_create(ui->mode_tileview_mode_c);
    lv_label_set_text(ui->mode_label_name, "Go Fighting");
    lv_label_set_long_mode(ui->mode_label_name, LV_LABEL_LONG_CLIP);
    lv_obj_set_pos(ui->mode_label_name, 286, 151);
    lv_obj_set_size(ui->mode_label_name, 90, 23);

    //Write style for mode_label_name, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_name, lv_color_hex(0xadff00), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_name, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_name, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_name, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_name, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_contc
    ui->mode_contc = lv_obj_create(ui->mode_tileview_mode_c);
    lv_obj_set_pos(ui->mode_contc, 254, 178);
    lv_obj_set_size(ui->mode_contc, 153, 31);
    lv_obj_set_scrollbar_mode(ui->mode_contc, LV_SCROLLBAR_MODE_OFF);

    //Write style for mode_contc, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_contc, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->mode_contc, 46, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->mode_contc, lv_color_hex(0xf3f9fe), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->mode_contc, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_contc, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_contc, 88, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->mode_contc, lv_color_hex(0x332a2a), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->mode_contc, LV_GRAD_DIR_HOR, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui->mode_contc, lv_color_hex(0xffcef4), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui->mode_contc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui->mode_contc, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_contc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_contc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_contc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_contc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_contc, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_prev
    ui->mode_label_prev = lv_label_create(ui->mode_contc);
    lv_label_set_text(ui->mode_label_prev, "" LV_SYMBOL_PREV " ");
    lv_label_set_long_mode(ui->mode_label_prev, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_prev, 30, 5);
    lv_obj_set_size(ui->mode_label_prev, 22, 22);
    lv_obj_add_flag(ui->mode_label_prev, LV_OBJ_FLAG_CLICKABLE);

    //Write style for mode_label_prev, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_prev, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_prev, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_prev, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_prev, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_prev, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_prev, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_prev, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_prev, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_prev, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_prev, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_prev, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_prev, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_prev, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_prev, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_play
    ui->mode_label_play = lv_label_create(ui->mode_contc);
    lv_label_set_text(ui->mode_label_play, "" LV_SYMBOL_PLAY "");
    lv_label_set_long_mode(ui->mode_label_play, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_play, 72, 5);
    lv_obj_set_size(ui->mode_label_play, 22, 22);
    lv_obj_add_flag(ui->mode_label_play, LV_OBJ_FLAG_CLICKABLE);

    //Write style for mode_label_play, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_play, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_play, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_play, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_play, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_play, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_play, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_play, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_play, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_play, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_play, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_play, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_play, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_play, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_play, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_next
    ui->mode_label_next = lv_label_create(ui->mode_contc);
    lv_label_set_text(ui->mode_label_next, "" LV_SYMBOL_NEXT " ");
    lv_label_set_long_mode(ui->mode_label_next, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_next, 112, 5);
    lv_obj_set_size(ui->mode_label_next, 22, 22);
    lv_obj_add_flag(ui->mode_label_next, LV_OBJ_FLAG_CLICKABLE);

    //Write style for mode_label_next, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_next, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_next, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_next, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_next, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_next, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_next, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_next, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_next, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_next, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_next, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_next, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_next, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_next, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_next, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_img_music
    ui->mode_img_music = lv_img_create(ui->mode_tileview_mode_c);
    lv_obj_add_flag(ui->mode_img_music, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->mode_img_music, &_music_cover_1_alpha_110x110);
    lv_img_set_pivot(ui->mode_img_music, 55,55);
    lv_img_set_angle(ui->mode_img_music, 0);
    lv_obj_set_pos(ui->mode_img_music, 275, 33);
    lv_obj_set_size(ui->mode_img_music, 110, 110);

    //Write style for mode_img_music, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->mode_img_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->mode_img_music, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_img_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->mode_img_music, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_img_1
    ui->mode_img_1 = lv_img_create(ui->mode_tileview_mode_c);
    lv_obj_add_flag(ui->mode_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->mode_img_1, &_music_cover_2_alpha_110x110);
    lv_img_set_pivot(ui->mode_img_1, 55,55);
    lv_img_set_angle(ui->mode_img_1, 0);
    lv_obj_set_pos(ui->mode_img_1, 297, 65);
    lv_obj_set_size(ui->mode_img_1, 110, 110);
    lv_obj_add_flag(ui->mode_img_1, LV_OBJ_FLAG_HIDDEN);

    //Write style for mode_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->mode_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->mode_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->mode_img_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_img_2
    ui->mode_img_2 = lv_img_create(ui->mode_tileview_mode_c);
    lv_obj_add_flag(ui->mode_img_2, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->mode_img_2, &_music_cover_3_alpha_110x110);
    lv_img_set_pivot(ui->mode_img_2, 55,55);
    lv_img_set_angle(ui->mode_img_2, 0);
    lv_obj_set_pos(ui->mode_img_2, 314, 65);
    lv_obj_set_size(ui->mode_img_2, 110, 110);
    lv_obj_add_flag(ui->mode_img_2, LV_OBJ_FLAG_HIDDEN);

    //Write style for mode_img_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->mode_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->mode_img_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_img_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->mode_img_2, true, LV_PART_MAIN|LV_STATE_DEFAULT);



    //Write codes mode_img_ebg
    ui->mode_img_ebg = lv_img_create(ui->mode_tileview_mode_e);
    lv_obj_add_flag(ui->mode_img_ebg, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->mode_img_ebg, &_det_bak_alpha_480x272);
    lv_img_set_pivot(ui->mode_img_ebg, 50,50);
    lv_img_set_angle(ui->mode_img_ebg, 0);
    lv_obj_set_pos(ui->mode_img_ebg, 0, 0);
    lv_obj_set_size(ui->mode_img_ebg, 480, 272);

    //Write style for mode_img_ebg, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->mode_img_ebg, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor(ui->mode_img_ebg, lv_color_hex(0xf900ff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->mode_img_ebg, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_img_ebg, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->mode_img_ebg, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_ekm_num
    ui->mode_label_ekm_num = lv_label_create(ui->mode_tileview_mode_e);
    lv_label_set_text(ui->mode_label_ekm_num, "26");
    lv_label_set_long_mode(ui->mode_label_ekm_num, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_ekm_num, 51, 91);
    lv_obj_set_size(ui->mode_label_ekm_num, 104, 60);

    //Write style for mode_label_ekm_num, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_ekm_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_ekm_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_ekm_num, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_ekm_num, &lv_font_Antonio_Regular_50, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_ekm_num, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_ekm_num, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_ekm_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_ekm_num, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_ekm_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_ekm_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_ekm_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_ekm_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_ekm_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_ekm_num, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_conte
    ui->mode_conte = lv_obj_create(ui->mode_tileview_mode_e);
    lv_obj_set_pos(ui->mode_conte, 169, 96);
    lv_obj_set_size(ui->mode_conte, 45, 74);
    lv_obj_set_scrollbar_mode(ui->mode_conte, LV_SCROLLBAR_MODE_OFF);

    //Write style for mode_conte, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_conte, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_conte, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_conte, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->mode_conte, lv_color_hex(0x90a907), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->mode_conte, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_conte, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_conte, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_conte, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_conte, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_conte, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_emode
    ui->mode_label_emode = lv_label_create(ui->mode_conte);
    lv_label_set_text(ui->mode_label_emode, "Mode\n");
    lv_label_set_long_mode(ui->mode_label_emode, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_emode, 0, 6);
    lv_obj_set_size(ui->mode_label_emode, 46, 13);

    //Write style for mode_label_emode, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_emode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_emode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_emode, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_emode, &lv_font_Amiko_Regular_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_emode, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_emode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_emode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_emode, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_emode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_emode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_emode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_emode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_emode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_emode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_E
    ui->mode_label_E = lv_label_create(ui->mode_conte);
    lv_label_set_text(ui->mode_label_E, "E\n");
    lv_label_set_long_mode(ui->mode_label_E, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_E, 4, 25);
    lv_obj_set_size(ui->mode_label_E, 35, 49);

    //Write style for mode_label_E, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_E, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_E, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_E, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_E, &lv_font_Amiko_Regular_50, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_E, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_E, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_E, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_E, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_E, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_E, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_E, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_E, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_E, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_E, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_ekm
    ui->mode_label_ekm = lv_label_create(ui->mode_tileview_mode_e);
    lv_label_set_text(ui->mode_label_ekm, "Km/h");
    lv_label_set_long_mode(ui->mode_label_ekm, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_ekm, 74, 161);
    lv_obj_set_size(ui->mode_label_ekm, 59, 20);

    //Write style for mode_label_ekm, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_ekm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_ekm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_ekm, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_ekm, &lv_font_SourceHanSerifSC_Regular_13, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_ekm, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_ekm, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_ekm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_ekm, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_ekm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_ekm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_ekm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_ekm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_ekm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_ekm, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_chart_speed
    ui->mode_chart_speed = lv_chart_create(ui->mode_tileview_mode_e);
    lv_chart_set_type(ui->mode_chart_speed, LV_CHART_TYPE_LINE);
    lv_chart_set_div_line_count(ui->mode_chart_speed, 4, 5);
    lv_chart_set_point_count(ui->mode_chart_speed, 40);
    lv_chart_set_range(ui->mode_chart_speed, LV_CHART_AXIS_PRIMARY_Y, 0, 200);
    lv_chart_set_axis_tick(ui->mode_chart_speed, LV_CHART_AXIS_PRIMARY_Y, 10, 6, 5, 4, true, 40);
    lv_chart_set_range(ui->mode_chart_speed, LV_CHART_AXIS_SECONDARY_Y, 0, 100);
    lv_chart_set_zoom_x(ui->mode_chart_speed, 256);
    lv_chart_set_zoom_y(ui->mode_chart_speed, 256);
    lv_obj_set_style_size(ui->mode_chart_speed, 0, LV_PART_INDICATOR);
    ui->mode_chart_speed_0 = lv_chart_add_series(ui->mode_chart_speed, lv_color_hex(0x00ed0d), LV_CHART_AXIS_PRIMARY_Y);
#if LV_USE_FREEMASTER == 0
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 5);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 10);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 15);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 20);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 30);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
    lv_chart_set_next_value(ui->mode_chart_speed, ui->mode_chart_speed_0, 0);
#endif
    lv_obj_set_pos(ui->mode_chart_speed, 251, 69);
    lv_obj_set_size(ui->mode_chart_speed, 161, 129);
    lv_obj_set_scrollbar_mode(ui->mode_chart_speed, LV_SCROLLBAR_MODE_OFF);

    //Write style for mode_chart_speed, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->mode_chart_speed, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->mode_chart_speed, lv_color_hex(0x141010), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->mode_chart_speed, LV_GRAD_DIR_HOR, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui->mode_chart_speed, lv_color_hex(0x033c2d), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui->mode_chart_speed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui->mode_chart_speed, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->mode_chart_speed, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->mode_chart_speed, 100, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->mode_chart_speed, lv_color_hex(0xe8e8e8), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->mode_chart_speed, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_chart_speed, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_width(ui->mode_chart_speed, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->mode_chart_speed, lv_color_hex(0xf6fff7), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->mode_chart_speed, 38, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_chart_speed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for mode_chart_speed, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->mode_chart_speed, lv_color_hex(0x298d53), LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_chart_speed, &lv_font_montserratMedium_12, LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_chart_speed, 255, LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_line_width(ui->mode_chart_speed, 1, LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->mode_chart_speed, lv_color_hex(0x298d53), LV_PART_TICKS|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->mode_chart_speed, 255, LV_PART_TICKS|LV_STATE_DEFAULT);

    //Write codes mode_img_logo
    ui->mode_img_logo = lv_img_create(ui->mode);
    lv_obj_add_flag(ui->mode_img_logo, LV_OBJ_FLAG_CLICKABLE);
    lv_img_set_src(ui->mode_img_logo, &_logo_alpha_60x20);
    lv_img_set_pivot(ui->mode_img_logo, 50,50);
    lv_img_set_angle(ui->mode_img_logo, 0);
    lv_obj_set_pos(ui->mode_img_logo, 210, 10);
    lv_obj_set_size(ui->mode_img_logo, 60, 20);

    //Write style for mode_img_logo, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->mode_img_logo, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->mode_img_logo, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_img_logo, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->mode_img_logo, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_WIFI
    ui->mode_label_WIFI = lv_label_create(ui->mode);
    lv_label_set_text(ui->mode_label_WIFI, "" LV_SYMBOL_WIFI "");
    lv_label_set_long_mode(ui->mode_label_WIFI, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_WIFI, 400, 8);
    lv_obj_set_size(ui->mode_label_WIFI, 41, 20);

    //Write style for mode_label_WIFI, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_WIFI, lv_color_hex(0x14ff00), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_WIFI, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_WIFI, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_WIFI, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_WIFI, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_WIFI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_bar_battery
    ui->mode_bar_battery = lv_bar_create(ui->mode);
    lv_obj_set_style_anim_time(ui->mode_bar_battery, 1000, 0);
    lv_bar_set_mode(ui->mode_bar_battery, LV_BAR_MODE_SYMMETRICAL);
    lv_bar_set_range(ui->mode_bar_battery, 0, 100);
    lv_bar_set_value(ui->mode_bar_battery, 90, LV_ANIM_OFF);
    lv_obj_set_pos(ui->mode_bar_battery, 441, 9);
    lv_obj_set_size(ui->mode_bar_battery, 26, 14);

    //Write style for mode_bar_battery, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->mode_bar_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_bar_battery, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_bar_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->mode_bar_battery, &_battery_bak_26x14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->mode_bar_battery, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->mode_bar_battery, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for mode_bar_battery, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->mode_bar_battery, 0, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_bar_battery, 10, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui->mode_bar_battery, &_battery_ind_26x14, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_opa(ui->mode_bar_battery, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_recolor_opa(ui->mode_bar_battery, 0, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes mode_digital_clock
    static bool mode_digital_clock_timer_enabled = false;
    ui->mode_digital_clock = lv_dclock_create(ui->mode, "11:25 AM");
    if (!mode_digital_clock_timer_enabled) {
        lv_timer_create(mode_digital_clock_timer, 1000, NULL);
        mode_digital_clock_timer_enabled = true;
    }
    lv_obj_set_pos(ui->mode_digital_clock, 45, 8);
    lv_obj_set_size(ui->mode_digital_clock, 83, 14);

    //Write style for mode_digital_clock, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->mode_digital_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_digital_clock, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_digital_clock, &lv_font_Antonio_Regular_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_digital_clock, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_digital_clock, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_digital_clock, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_digital_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_digital_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_digital_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_digital_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_digital_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_digital_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_img_home
    ui->mode_img_home = lv_img_create(ui->mode);
    lv_img_set_src(ui->mode_img_home, &_home_alpha_26x26);
    lv_img_set_pivot(ui->mode_img_home, 50,50);
    lv_img_set_angle(ui->mode_img_home, 0);
    lv_obj_set_pos(ui->mode_img_home, 13, 3);
    lv_obj_set_size(ui->mode_img_home, 26, 26);
    lv_obj_add_flag(ui->mode_img_home, LV_OBJ_FLAG_CLICKABLE);

    //Write style for mode_img_home, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_img_recolor_opa(ui->mode_img_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui->mode_img_home, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_img_home, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->mode_img_home, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes mode_label_BT
    ui->mode_label_BT = lv_label_create(ui->mode);
    lv_label_set_text(ui->mode_label_BT, "" LV_SYMBOL_BLUETOOTH " ");
    lv_label_set_long_mode(ui->mode_label_BT, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->mode_label_BT, 374, 8);
    lv_obj_set_size(ui->mode_label_BT, 41, 20);

    //Write style for mode_label_BT, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->mode_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->mode_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->mode_label_BT, lv_color_hex(0x3101fe), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->mode_label_BT, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->mode_label_BT, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->mode_label_BT, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->mode_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->mode_label_BT, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->mode_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->mode_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->mode_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->mode_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->mode_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->mode_label_BT, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of mode.


    //Update current screen layout.
    lv_obj_update_layout(ui->mode);

    //Init events for screen.
    events_init_mode(ui);
}
