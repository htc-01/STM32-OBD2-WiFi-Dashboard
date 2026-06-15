/*
* Copyright 2024 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef __CUSTOM_H_
#define __CUSTOM_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_guider.h"

#define GAUGE_COUNT 6

/* Gauge parameter indices */
#define GAUGE_LOAD  0
#define GAUGE_TMP   1
#define GAUGE_RPM   2
#define GAUGE_SPD   3
#define GAUGE_MAF   4
#define GAUGE_THR   5

typedef struct {
    const char *name;
    const char *unit;
    int16_t min;
    int16_t max;
    uint32_t color;
    uint32_t warn_color;
    int16_t warn_threshold;
} gauge_cfg_t;

void custom_init(lv_ui *ui);
void update_gauge_display(lv_ui *ui);

void record_chart_timer_cb(lv_timer_t * t);

/* Update a single gauge value (typically called from UART/parser) */
void gauge_set_value(uint8_t index, int16_t value);

/* Switch to the next gauge page (simulates key press) */
void gauge_next_page(void);

/* Flush any pending gauge save to AT24C02 (call in main loop) */
void gauge_process_pending_save(void);

/* Power management */
#include <stdbool.h>
bool is_power_on(void);
void power_on(void);
void power_off(void);
void power_toggle(void);

#ifdef __cplusplus
}
#endif
#endif /* EVENT_CB_H_ */
