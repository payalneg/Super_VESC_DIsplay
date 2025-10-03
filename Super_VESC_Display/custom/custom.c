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

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

extern lv_ui guider_ui;
/**
 * Create a demo application
 */

/* set the digital label and steering lamp image style. */
static void set_position_x(void * gui, int32_t temp)
{
    
}

static void set_position_y(void * gui, int32_t temp)
{
  
}

void custom_init(lv_ui *ui)
{
    /* Add your codes here */
}

void speed_meter_timer_cb(lv_timer_t * t)
{
    
}

void home_label_digit_animation(lv_ui *ui)
{

}

void digital_cluster_chart_timer_cb(lv_timer_t * t)
{
    
}

void play_music(lv_ui *ui)
{
   
}

static const void * lv_demo_music_get_list_img(uint32_t track_id)
{
 
}

void music_album_next(bool next)
{
   
}

void update_current(float current)
{
    lv_meter_set_indicator_value(guider_ui.dashboard_Current_meter, guider_ui.dashboard_Current_meter_scale_0_ndline_0, (int)abs(current));
    char text[10];
    sprintf(text,"%.1f", current);
    lv_textarea_set_text(guider_ui.dashboard_Current_text,text);
}