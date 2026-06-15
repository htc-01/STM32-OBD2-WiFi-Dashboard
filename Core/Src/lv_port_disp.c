#include "lvgl.h"
#include "lv_port_disp.h"
#include "gc9a01.h"
#include <stdint.h>

extern SPI_HandleTypeDef hspi1;

/* GC9A01: 240x240, RGB565 */
#define MY_DISP_HOR_RES 240
#define MY_DISP_VER_RES 240

/* Partial buffer: 240 pixels x 80 lines x 2 bytes = 38400 bytes */
#define DISP_BUF_LINES 80
static uint8_t disp_buf_1[MY_DISP_HOR_RES * DISP_BUF_LINES * 2];

static void disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    uint16_t x1 = (uint16_t)area->x1;
    uint16_t y1 = (uint16_t)area->y1;
    uint16_t x2 = (uint16_t)area->x2;
    uint16_t y2 = (uint16_t)area->y2;

    GC9A01_SetWindow(x1, y1, x2, y2);

    uint32_t w = (uint32_t)(x2 - x1 + 1);
    uint32_t h = (uint32_t)(y2 - y1 + 1);
    uint32_t len = w * h * 2; /* RGB565 = 2 bytes/pixel */

    /* Note: byte-order swap is handled by LVGL via LV_COLOR_16_SWAP=1 */

    GC9A01_DC_HIGH();
    GC9A01_CS_LOW();
    HAL_SPI_Transmit(&hspi1, px_map, (uint16_t)len, 1000);
    GC9A01_CS_HIGH();

    lv_display_flush_ready(disp);
}

void lv_port_disp_init(void)
{
    lv_display_t *disp = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_display_set_flush_cb(disp, disp_flush);
    lv_display_set_buffers(disp, disp_buf_1, NULL,
                           sizeof(disp_buf_1),
                           LV_DISPLAY_RENDER_MODE_PARTIAL);
}
