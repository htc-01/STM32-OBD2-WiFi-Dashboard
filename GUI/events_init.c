/*
* Copyright 2026 NXP
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
static lv_timer_t * task_chart;

static void speed_event_handler (lv_event_t *e)
{
    (void)e;
}

void events_init_speed (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->speed, speed_event_handler, LV_EVENT_ALL, ui);
}

static void record_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOADED:
    {
        task_chart = lv_timer_create(record_chart_timer_cb, 100, guider_ui.record_chart_board);
        break;
    }
    case LV_EVENT_SCREEN_UNLOADED:
    {
        lv_timer_delete(task_chart);
        break;
    }
    default:
        break;
    }
}

static void record_btn_before_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.speed, guider_ui.speed_del, &guider_ui.record_del, setup_scr_speed, LV_SCR_LOAD_ANIM_FADE_ON, 100, 0, true, false);
        break;
    }
    default:
        break;
    }
}

void events_init_record (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->record, record_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->record_btn_before, record_btn_before_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui *ui)
{

}
