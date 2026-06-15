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
#include <string.h>
#include "lvgl.h"
#include "custom.h"
#include "gc9a01.h"
#include "at24c02.h"

/*********************
 *      DEFINES
 *********************/
#define CHART1_POINTS 100

/* Arc geometry - matching STM32 gauge (outer=108, inner=82) */
#define ARC_START_ANGLE     135
#define ARC_END_ANGLE       405
#define ARC_SIZE            216     /* 108 * 2 */
#define ARC_WIDTH           26      /* 108 - 82 */
#define ARC_BG_COLOR        0x424242  /* GC9A01_DARKGRAY 0x4208 in RGB888 */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
void update_gauge_display(lv_ui *ui);
static void speed_screen_click_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

/* Gauge configurations - matching the STM32 carscreen project */
static const gauge_cfg_t gauge_cfg[GAUGE_COUNT] = {
    {"LOAD",  "%",    0,   100,  0x00FF00, 0xFF0000, 80},   /* GC9A01_GREEN  -> GC9A01_RED >80   */
    {"TMP",   "C",    0,   150,  0x00FFFF, 0xFF0000, 110},  /* GC9A01_CYAN   -> GC9A01_RED >110  */
    {"RPM",   "rpm",  0,   8000, 0xFFFF00, 0xFF0000, 6000}, /* GC9A01_YELLOW -> GC9A01_RED >6000 */
    {"SPD",   "km/h", 0,   240,  0x00FF00, 0xFF0000, 120},  /* GC9A01_GREEN  -> GC9A01_RED >120  */
    {"MAF",   "g/s",  0,   500,  0xFF00FF, 0xFF0000, 400},  /* GC9A01_MAGENTA-> GC9A01_RED >400  */
    {"THR",   "%",    0,   100,  0xFFD200, 0xFF0000, 80},   /* GC9A01_ORANGE -> GC9A01_RED >80   */
};

static int16_t gauge_values[GAUGE_COUNT] = {0};
static uint8_t current_gauge = 0;

/* Custom gauge widgets */
static lv_obj_t * arc_gauge = NULL;
static lv_obj_t * label_title = NULL;
static lv_obj_t * label_value = NULL;
static lv_obj_t * label_unit = NULL;

/* Chart demo variables */
static int16_t spd_chart[CHART1_POINTS] = {0};
static bool is_up = true;

/* Over-limit flash effect */
static lv_timer_t *flash_timer = NULL;
static bool flash_state = false;

/* Lazy-save flag for AT24C02 (avoid I2C blocking inside LVGL callbacks) */
static bool gauge_save_pending = false;

/* Power state */
static bool power_state = true;

/**********************
 *  STATIC FUNCTIONS
 **********************/

static void gauge_anim_exec_cb(void *var, int32_t v)
{
    (void)var;
    if (arc_gauge) {
        int32_t min = lv_arc_get_min_value(arc_gauge);
        int32_t max = lv_arc_get_max_value(arc_gauge);
        int32_t arc_v = v;
        if (arc_v < min) arc_v = min;
        if (arc_v > max) arc_v = max;
        lv_arc_set_value(arc_gauge, arc_v);
    }
    if (label_value) {
        lv_label_set_text_fmt(label_value, "%d", (int)v);
    }
}

static void flash_timer_cb(lv_timer_t *t)
{
    (void)t;
    flash_state = !flash_state;
    if (flash_state) {
        lv_obj_set_style_bg_color(guider_ui.speed, lv_color_hex(0xFF0000), LV_PART_MAIN);
    } else {
        lv_obj_set_style_bg_color(guider_ui.speed, lv_color_hex(0x000000), LV_PART_MAIN);
    }
}

static void speed_screen_click_event_cb(lv_event_t * e)
{
    (void)e;
    current_gauge = (current_gauge + 1) % GAUGE_COUNT;
    gauge_save_pending = true;
    update_gauge_display(NULL);
}

void update_gauge_display(lv_ui *ui)
{
    (void)ui;
    const gauge_cfg_t *cfg = &gauge_cfg[current_gauge];
    int16_t raw_value = gauge_values[current_gauge];

    static int16_t last_value = 0x7FFF;
    static uint8_t last_gauge = 0xFF;
    static uint32_t last_color = 0xFFFFFFFF;

    /* Determine color based on actual (unclamped) value */
    uint32_t color = (raw_value >= cfg->warn_threshold) ? cfg->warn_color : cfg->color;

    bool gauge_changed = (current_gauge != last_gauge);
    bool value_changed = (raw_value != last_value);
    bool color_changed = (color != last_color);

    /* Only update widgets when something actually changed */
    if (gauge_changed) {
        lv_arc_set_range(arc_gauge, cfg->min, cfg->max);
        lv_label_set_text_fmt(label_title, "%s", cfg->name);
        lv_label_set_text_fmt(label_unit, "%s", cfg->unit);
    }

    if (gauge_changed || value_changed || color_changed) {
        lv_obj_set_style_arc_color(arc_gauge, lv_color_hex(color), LV_PART_INDICATOR);
        lv_obj_set_style_text_color(label_value, lv_color_hex(color), LV_PART_MAIN);

        int32_t arc_v = raw_value;
        if (arc_v < cfg->min) arc_v = cfg->min;
        if (arc_v > cfg->max) arc_v = cfg->max;
        lv_arc_set_value(arc_gauge, arc_v);
        lv_label_set_text_fmt(label_value, "%d", (int)raw_value);
    }

    /* Over-limit flash effect: when value exceeds gauge max, flash red/black */
    if (raw_value > cfg->max) {
        if (flash_timer == NULL) {
            flash_timer = lv_timer_create(flash_timer_cb, 200, NULL);
        }
    } else {
        if (flash_timer != NULL) {
            lv_timer_delete(flash_timer);
            flash_timer = NULL;
            /* Restore black background */
            lv_obj_set_style_bg_color(guider_ui.speed, lv_color_hex(0x000000), LV_PART_MAIN);
        }
    }

    last_value = raw_value;
    last_gauge = current_gauge;
    last_color = color;
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void custom_init(lv_ui *ui)
{
    /* Restore last gauge index from flash (persistent across power cycles) */
    uint8_t saved_gauge = at24c02_read_gauge();
    if (saved_gauge < GAUGE_COUNT) {
        current_gauge = saved_gauge;
    }

    /* Hide GUI Guider generated speed widgets – we draw our own gauge */
    lv_obj_add_flag(ui->speed_meter_board, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->speed_label_speed_unit, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->speed_label_digit, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->speed_label_title, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->speed_cont_box, LV_OBJ_FLAG_HIDDEN);

    /* Set speed screen to pure black (matching STM32 GC9A01_BLACK background) */
    lv_obj_set_style_bg_color(ui->speed, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_dir(ui->speed, LV_GRAD_DIR_NONE, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui->speed, LV_OPA_COVER, LV_PART_MAIN);

    /* Create arc gauge (single arc: MAIN = grey track, INDICATOR = coloured progress) */
    arc_gauge = lv_arc_create(ui->speed);
    lv_obj_set_size(arc_gauge, ARC_SIZE, ARC_SIZE);
    lv_obj_center(arc_gauge);
    lv_arc_set_bg_angles(arc_gauge, ARC_START_ANGLE, ARC_END_ANGLE);
    lv_arc_set_range(arc_gauge, 0, 100);
    lv_arc_set_value(arc_gauge, 0);

    /* Grey background track */
    lv_obj_set_style_arc_color(arc_gauge, lv_color_hex(ARC_BG_COLOR), LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_gauge, ARC_WIDTH, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(arc_gauge, false, LV_PART_MAIN);

    /* Coloured progress indicator */
    lv_obj_set_style_arc_color(arc_gauge, lv_color_hex(0x00FF00), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc_gauge, ARC_WIDTH, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(arc_gauge, false, LV_PART_INDICATOR);

    /* Hide knob (blue dot at arc end) */
    lv_obj_set_style_size(arc_gauge, 0, 0, LV_PART_KNOB);
    lv_obj_set_style_opa(arc_gauge, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_set_style_bg_opa(arc_gauge, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_set_style_arc_opa(arc_gauge, LV_OPA_TRANSP, LV_PART_KNOB);

    /* Title label (inside ring, upper area) - y=55 in STM32 (120-65) */
    label_title = lv_label_create(ui->speed);
    lv_obj_align(label_title, LV_ALIGN_CENTER, 0, -65);
    lv_obj_set_style_text_font(label_title, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_align(label_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_label_set_text(label_title, "LOAD");

    /* Value label (large, centre) - y=95 in STM32 (120-25) */
    label_value = lv_label_create(ui->speed);
    lv_obj_align(label_value, LV_ALIGN_CENTER, 0, -25);
    lv_obj_set_style_text_font(label_value, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_value, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_align(label_value, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_label_set_text(label_value, "0");

    /* Unit label (inside ring, lower area) - y=150 in STM32 (120+30) */
    label_unit = lv_label_create(ui->speed);
    lv_obj_align(label_unit, LV_ALIGN_CENTER, 0, 30);
    lv_obj_set_style_text_font(label_unit, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_unit, lv_color_hex(0xAAAAAA), LV_PART_MAIN);
    lv_obj_set_style_text_align(label_unit, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_label_set_text(label_unit, "%");

    /* Force initial draw */
    update_gauge_display(ui);

    /* Click anywhere on speed screen to switch gauge page */
    lv_obj_add_event_cb(ui->speed, speed_screen_click_event_cb, LV_EVENT_CLICKED, NULL);
}

void gauge_set_value(uint8_t index, int16_t value)
{
    if (index >= GAUGE_COUNT) return;
    gauge_values[index] = value;
}

void gauge_next_page(void)
{
    current_gauge = (current_gauge + 1) % GAUGE_COUNT;
    gauge_save_pending = true;
}

void gauge_process_pending_save(void)
{
    if (gauge_save_pending) {
        at24c02_save_gauge(current_gauge);
        gauge_save_pending = false;
    }
}

void record_chart_timer_cb(lv_timer_t * t)
{
    lv_obj_t * obj = lv_timer_get_user_data(t);

    lv_chart_series_t * ser = lv_chart_get_series_next(obj, NULL);
    int32_t * ser_array = lv_chart_get_y_array(obj, ser);

    for(int i = 0; i < CHART1_POINTS - 1; i++)
    {
        spd_chart[i] = spd_chart[i+1];
        ser_array[i] = spd_chart[i];
    }

    if(spd_chart[CHART1_POINTS - 1] > 100) is_up = false;
    if(spd_chart[CHART1_POINTS - 1] < 70) is_up = true;

    if(is_up)
    {
        spd_chart[CHART1_POINTS - 1] += lv_rand(0, 5);
    }else
    {
        spd_chart[CHART1_POINTS - 1] -= lv_rand(0, 5);
    }
    ser_array[CHART1_POINTS - 1] = spd_chart[CHART1_POINTS - 1];
    lv_chart_refresh(obj);
}

bool is_power_on(void)
{
    return power_state;
}

void power_off(void)
{
    if (!power_state) return;
    power_state = false;
    if (at24c02_save_gauge(current_gauge) != 0) {
        /* Flash save failed (area dirty or full): blink red screen to warn */
        GC9A01_FillScreen(GC9A01_RED);
        HAL_Delay(200);
        GC9A01_FillScreen(GC9A01_BLACK);
        HAL_Delay(200);
        GC9A01_FillScreen(GC9A01_RED);
        HAL_Delay(200);
    }
    GC9A01_Sleep();
}

void power_on(void)
{
    if (power_state) return;
    GC9A01_Wake();
    update_gauge_display(NULL);
    lv_obj_invalidate(guider_ui.speed);
    power_state = true;
}

void power_toggle(void)
{
    if (power_state) {
        power_off();
    } else {
        power_on();
    }
}
