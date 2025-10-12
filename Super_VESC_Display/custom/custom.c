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
    static float old_value = -999.0f;
    if (current == old_value) {
        return;
    }
    old_value = current;
    
    int value = current;
    int abs_value = abs(value);
    
    lv_meter_set_indicator_value(guider_ui.dashboard_Current_meter, guider_ui.dashboard_Current_meter_scale_0_ndline_0, abs_value);
    
    if (value>0)
    {
        lv_meter_set_indicator_end_value(guider_ui.dashboard_Current_meter, guider_ui.dashboard_Current_meter_scale_0_arc_1, abs_value);
        lv_meter_set_indicator_end_value(guider_ui.dashboard_Current_meter, guider_ui.dashboard_Current_meter_scale_0_arc_2, 0);
    }
    else
    {
        lv_meter_set_indicator_end_value(guider_ui.dashboard_Current_meter, guider_ui.dashboard_Current_meter_scale_0_arc_1, 0);
        lv_meter_set_indicator_end_value(guider_ui.dashboard_Current_meter, guider_ui.dashboard_Current_meter_scale_0_arc_2, abs_value);
    }
    char text[10];
    sprintf(text,"%.1f", current);
    lv_textarea_set_text(guider_ui.dashboard_Current_text,text);
}

void update_speed(float speed)
{
    static float old_value = -999.0f;
    if (speed == old_value) {
        return;
    }
    old_value = speed;
    
    int value = speed;
    
    lv_meter_set_indicator_value(guider_ui.dashboard_Speed_meter, guider_ui.dashboard_Speed_meter_scale_0_ndline_0, value);
    lv_meter_set_indicator_end_value(guider_ui.dashboard_Speed_meter, guider_ui.dashboard_Speed_meter_scale_0_arc_0, value);
    
    char text[10];
    sprintf(text,"%d", value);
    lv_textarea_set_text(guider_ui.dashboard_Speed_text,text);
}

void update_battery_proc(float battery_proc)
{
    static float old_value = -999.0f;
    if (battery_proc == old_value) {
        return;
    }
    old_value = battery_proc;
    
    int value = battery_proc;
    
    lv_meter_set_indicator_value(guider_ui.dashboard_Battery_meter, guider_ui.dashboard_Battery_meter_scale_0_ndline_0, 100-value);
    lv_meter_set_indicator_start_value(guider_ui.dashboard_Battery_meter, guider_ui.dashboard_Battery_meter_scale_0_arc_1, 100-value);
    
    char text[10];
    sprintf(text,"%d", value>99?99:value);
    lv_textarea_set_text(guider_ui.dashboard_Battery_proc_text,text);
}

void update_trip(float trip_distance)
{
    static float old_value = -999.0f;
    if (trip_distance == old_value) {
        return;
    }
    old_value = trip_distance;
    
    char text[10];
    sprintf(text,"%0.1f", trip_distance);
    lv_textarea_set_text(guider_ui.dashboard_TRIP_text,text);
}

void update_range(float range_distance)
{
    static float old_value = -999.0f;
    if (range_distance == old_value) {
        return;
    }
    old_value = range_distance;
    
    char text[10];
    sprintf(text,"%.1f", range_distance);
    lv_textarea_set_text(guider_ui.dashboard_Range_text,text);
}

void update_temp_fet(float temp_fet)
{
    static float old_value = -999.0f;
    if (temp_fet == old_value) {
        return;
    }
    old_value = temp_fet;
    
    int value = temp_fet;
    char text[10];
    sprintf(text,"%d", value);
    lv_textarea_set_text(guider_ui.dashboard_temp_esc_text,text);
}

void update_temp_motor(float temp_motor)
{
    static float old_value = -999.0f;
    if (temp_motor == old_value) {
        return;
    }
    old_value = temp_motor;
    
    int value = temp_motor;
    char text[10];
    sprintf(text,"%d", value);
    lv_textarea_set_text(guider_ui.dashboard_temp_mot_text,text);
}

void update_amp_hours(float amp_hours)
{
    static float old_value = -999.0f;
    if (amp_hours == old_value) {
        return;
    }
    old_value = amp_hours;
    
    char text[10];
    sprintf(text,"%.1f", amp_hours);
    lv_textarea_set_text(guider_ui.dashboard_Ah_text,text);
}

void update_battery_temp(float battery_temp)
{
    static float old_value = -999.0f;
    if (battery_temp == old_value) {
        return;
    }
    old_value = battery_temp;
    
    int value = battery_temp;
    char text[10];
    sprintf(text,"%d", value);

    lv_textarea_set_text(guider_ui.dashboard_temp_bat_text,text);
}

void update_battery_voltage(float battery_voltage)
{
    static float old_value = -999.0f;
    if (battery_voltage == old_value) {
        return;
    }
    old_value = battery_voltage;
    
    char text[10];
    sprintf(text,"%.1f", battery_voltage);

    lv_textarea_set_text(guider_ui.dashboard_Voltage_text,text);
}


void update_odometer(float odometer)
{
    static float old_value = -999.0f;
    if (odometer == old_value) {
        return;
    }
    old_value = odometer;
    
    int value = odometer;   
    char text[10];
    sprintf(text,"%05d", value);

    lv_textarea_set_text(guider_ui.dashboard_odo_text,text);
}

void update_fps(int fps)
{
    static int old_value = -999;
    static int min_fps = 400;
    if (fps == old_value) {
        return;
    }
    old_value = fps;
    if (fps < min_fps) {
        min_fps = fps;
    }
    char text[10];
    sprintf(text,"FPS:%d, Min:%d", fps, min_fps);
    lv_textarea_set_text(guider_ui.dashboard_fps_text,text);
}
z