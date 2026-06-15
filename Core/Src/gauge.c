#include "gauge.h"
#include "gc9a01.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/* Parameter configurations */
typedef struct {
    const char *name;
    const char *unit;
    int16_t min;
    int16_t max;
    uint16_t color;
    uint16_t warn_color;
    int16_t warn_threshold;
} GaugeCfg_t;

static const GaugeCfg_t gauge_cfg[GAUGE_COUNT] = {
    {"LOAD",  "%",    0,   100,  GC9A01_GREEN,   GC9A01_RED,  80},
    {"TMP",   "C",    0,   150,  GC9A01_CYAN,    GC9A01_RED,  110},
    {"RPM",   "rpm",  0,   8000, GC9A01_YELLOW,  GC9A01_RED,  6000},
    {"SPD",   "km/h", 0,   240,  GC9A01_GREEN,   GC9A01_RED,  120},
    {"MAF",   "g/s",  0,   500,  GC9A01_MAGENTA, GC9A01_RED,  400},
    {"THR",   "%",    0,   100,  GC9A01_ORANGE,  GC9A01_RED,  80},
};

/* Ring geometry */
#define RING_OUTER  108
#define RING_INNER   82
#define RING_TRACK_COLOR  GC9A01_DARKGRAY

/* Angle range for the gauge arc */
#define ANGLE_START 135.0f
#define ANGLE_END   405.0f

static void polar_to_xy(int16_t r, float angle_deg, int16_t *x, int16_t *y)
{
    float rad = angle_deg * 3.14159265f / 180.0f;
    *x = GAUGE_CX + (int16_t)(r * cosf(rad));
    *y = GAUGE_CY + (int16_t)(r * sinf(rad));
}

static float value_to_angle(int16_t value, const GaugeCfg_t *cfg)
{
    float ratio = (float)(value - cfg->min) / (float)(cfg->max - cfg->min);
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    return ANGLE_START + ratio * (ANGLE_END - ANGLE_START);
}

/* Draw a thick arc using radial lines (0.5° step).  
 * Smaller step eliminates the visible radial gap between 1px lines. */
static void draw_arc_thick(int16_t r_in, int16_t r_out, float start_deg, float end_deg, uint16_t color)
{
    float step = 0.5f;
    int16_t x0, y0, x1, y1;
    for (float a = start_deg; a <= end_deg; a += step) {
        polar_to_xy(r_in, a, &x0, &y0);
        polar_to_xy(r_out, a, &x1, &y1);
        GC9A01_DrawLine(x0, y0, x1, y1, color);
    }
}

/* Full redraw of the static background (called on page switch) */
static void draw_static_background(const GaugeCfg_t *cfg)
{
    GC9A01_FillScreen(GC9A01_BLACK);

    /* Grey background ring */
    GC9A01_FillCircle(GAUGE_CX, GAUGE_CY, RING_OUTER, RING_TRACK_COLOR);
    GC9A01_FillCircle(GAUGE_CX, GAUGE_CY, RING_INNER, GC9A01_BLACK);

    /* Title (inside the ring, upper area) */
    uint8_t title_len = strlen(cfg->name);
    GC9A01_DrawStringRotated(GAUGE_CX - title_len * 8 * 2 / 2, 55,
                             cfg->name, GC9A01_WHITE, GC9A01_BLACK, 2);

    /* Unit (inside the ring, lower area) */
    uint8_t unit_len = strlen(cfg->unit);
    GC9A01_DrawStringRotated(GAUGE_CX - unit_len * 8 * 2 / 2, 150,
                             cfg->unit, GC9A01_GRAY, GC9A01_BLACK, 2);
}

/* Draw the large centre value.
 * To avoid flicker we never blindly clear the whole box.
 * If the digit count shrinks we clear the maximal possible area once,
 * otherwise we rely on DrawStringRotated's own background pixels to
 * overwrite the old digits. */
static void draw_center_value(int16_t value, uint16_t color, uint8_t force_clear)
{
    char buf[16];
    sprintf(buf, "%d", value);
    uint8_t val_len = strlen(buf);
    uint8_t size = 5;
    uint8_t tw = val_len * 8 * size;
    uint8_t th = 8 * size;
    uint16_t x = GAUGE_CX - tw / 2;
    uint16_t y = 95;

    static uint8_t last_len = 0;

    /* Only need a hard clear when the string gets shorter (e.g. 100 -> 99).
     * In that case wipe the maximal width we ever use (5 digits * 8 * 5 = 200 px). */
    if (force_clear || val_len < last_len) {
        uint8_t max_tw = 5 * 8 * size;          /* 200 px */
        uint16_t clear_x = GAUGE_CX - max_tw / 2;
        GC9A01_FillRect(clear_x, y, max_tw, th, GC9A01_BLACK);
    }

    GC9A01_DrawStringRotated(x, y, buf, color, GC9A01_BLACK, size);
    last_len = val_len;
}

void Gauge_Draw(uint8_t index, int16_t value)
{
    if (index >= GAUGE_COUNT) return;
    const GaugeCfg_t *cfg = &gauge_cfg[index];

    static uint8_t  last_page  = 0xFF;
    static int16_t  last_value = -1;
    static uint16_t last_color = 0;

    uint8_t full_redraw = (index != last_page);
    uint16_t arc_color  = (value >= cfg->warn_threshold) ? cfg->warn_color : cfg->color;

    if (full_redraw) {
        last_page  = index;
        last_value = -1;   /* force full arc redraw */
        last_color = 0;
        draw_static_background(cfg);
        Gauge_DrawPageIndicator(index);
    }

    float angle     = value_to_angle(value, cfg);
    float old_angle = value_to_angle(last_value < 0 ? 0 : last_value, cfg);

    if (full_redraw || last_value < 0) {
        /* First draw on this page – draw entire coloured arc */
        draw_arc_thick(RING_INNER, RING_OUTER, ANGLE_START, angle, arc_color);
    } else if (value > last_value) {
        /* Value increased – extend the coloured arc */
        if (arc_color != last_color) {
            draw_arc_thick(RING_INNER, RING_OUTER, ANGLE_START, angle, arc_color);
        } else {
            draw_arc_thick(RING_INNER, RING_OUTER, old_angle, angle, arc_color);
        }
    } else if (value < last_value) {
        /* Value decreased – erase the tail with track colour */
        if (arc_color != last_color) {
            draw_arc_thick(RING_INNER, RING_OUTER, ANGLE_START, old_angle, RING_TRACK_COLOR);
            draw_arc_thick(RING_INNER, RING_OUTER, ANGLE_START, angle, arc_color);
        } else {
            draw_arc_thick(RING_INNER, RING_OUTER, angle, old_angle, RING_TRACK_COLOR);
        }
    }
    /* if value == last_value, do nothing for the arc */

    /* Only touch the text when something actually changed.
     * This removes flicker when the value stays constant. */
    if (full_redraw || last_value < 0 || value != last_value || arc_color != last_color) {
        draw_center_value(value, arc_color, full_redraw || last_value < 0);
    }

    last_value = value;
    last_color = arc_color;
}

void Gauge_DrawPageIndicator(uint8_t current_page)
{
    int16_t y = 220;
    int16_t dot_r = 3;
    int16_t spacing = 14;
    int16_t total_w = (GAUGE_COUNT - 1) * spacing;
    int16_t start_x = GAUGE_CX - total_w / 2;

    for (uint8_t i = 0; i < GAUGE_COUNT; i++) {
        int16_t cx = start_x + i * spacing;
        if (i == current_page) {
            GC9A01_FillCircle(cx, y, dot_r, GC9A01_WHITE);
        } else {
            GC9A01_FillCircle(cx, y, dot_r, GC9A01_DARKGRAY);
        }
    }
}
