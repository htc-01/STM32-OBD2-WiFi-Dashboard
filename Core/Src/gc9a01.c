#include "gc9a01.h"
#include "font.h"
#include <math.h>
#include <stdlib.h>

extern SPI_HandleTypeDef hspi1;

static void GC9A01_WriteByte(uint8_t byte)
{
    if (HAL_SPI_Transmit(&hspi1, &byte, 1, 100) != HAL_OK) {
        Error_Handler();
    }
}

void GC9A01_Reset(void)
{
    GC9A01_RST_HIGH();
    HAL_Delay(100);
    GC9A01_RST_LOW();
    HAL_Delay(100);
    GC9A01_RST_HIGH();
    HAL_Delay(100);
}

void GC9A01_WriteCommand(uint8_t cmd)
{
    GC9A01_DC_LOW();
    GC9A01_CS_LOW();
    GC9A01_WriteByte(cmd);
    GC9A01_CS_HIGH();
}

void GC9A01_WriteData(uint8_t data)
{
    GC9A01_DC_HIGH();
    GC9A01_CS_LOW();
    GC9A01_WriteByte(data);
    GC9A01_CS_HIGH();
}

void GC9A01_WriteData16(uint16_t data)
{
    GC9A01_DC_HIGH();
    GC9A01_CS_LOW();
    GC9A01_WriteByte(data >> 8);
    GC9A01_WriteByte(data & 0xFF);
    GC9A01_CS_HIGH();
}

void GC9A01_WriteDataMulti(uint8_t *data, uint16_t len)
{
    GC9A01_DC_HIGH();
    GC9A01_CS_LOW();
    HAL_SPI_Transmit(&hspi1, data, len, 1000);
    GC9A01_CS_HIGH();
}

void GC9A01_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t buf[4];

    GC9A01_CS_LOW();

    /* Column Address Set */
    GC9A01_DC_LOW();
    buf[0] = 0x2A;
    HAL_SPI_Transmit(&hspi1, buf, 1, 100);
    GC9A01_DC_HIGH();
    buf[0] = x0 >> 8;
    buf[1] = x0 & 0xFF;
    buf[2] = x1 >> 8;
    buf[3] = x1 & 0xFF;
    HAL_SPI_Transmit(&hspi1, buf, 4, 100);

    /* Page Address Set */
    GC9A01_DC_LOW();
    buf[0] = 0x2B;
    HAL_SPI_Transmit(&hspi1, buf, 1, 100);
    GC9A01_DC_HIGH();
    buf[0] = y0 >> 8;
    buf[1] = y0 & 0xFF;
    buf[2] = y1 >> 8;
    buf[3] = y1 & 0xFF;
    HAL_SPI_Transmit(&hspi1, buf, 4, 100);

    /* Memory Write */
    GC9A01_DC_LOW();
    buf[0] = 0x2C;
    HAL_SPI_Transmit(&hspi1, buf, 1, 100);

    GC9A01_CS_HIGH();
}

void GC9A01_FillScreen(uint16_t color)
{
    GC9A01_FillRect(0, 0, GC9A01_WIDTH, GC9A01_HEIGHT, color);
}

void GC9A01_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if (x >= GC9A01_WIDTH || y >= GC9A01_HEIGHT) return;
    if ((x + w) > GC9A01_WIDTH) w = GC9A01_WIDTH - x;
    if ((y + h) > GC9A01_HEIGHT) h = GC9A01_HEIGHT - y;

    GC9A01_SetWindow(x, y, x + w - 1, y + h - 1);

    GC9A01_DC_HIGH();
    GC9A01_CS_LOW();

    uint8_t color_high = color >> 8;
    uint8_t color_low = color & 0xFF;

    // Use a small buffer for faster transfer
    uint8_t buf[480]; // 240 pixels * 2 bytes
    for (uint16_t i = 0; i < 480; i += 2) {
        buf[i] = color_high;
        buf[i + 1] = color_low;
    }

    uint32_t total = (uint32_t)w * h;
    while (total > 240) {
        HAL_SPI_Transmit(&hspi1, buf, 480, 1000);
        total -= 240;
    }
    if (total > 0) {
        HAL_SPI_Transmit(&hspi1, buf, total * 2, 1000);
    }

    GC9A01_CS_HIGH();
}

void GC9A01_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= GC9A01_WIDTH || y >= GC9A01_HEIGHT) return;
    GC9A01_SetWindow(x, y, x, y);
    GC9A01_WriteData16(color);
}

void GC9A01_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    int16_t dx = abs((int16_t)x1 - (int16_t)x0);
    int16_t dy = abs((int16_t)y1 - (int16_t)y0);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;

    while (1) {
        GC9A01_DrawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int16_t e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void GC9A01_DrawCircle(uint16_t cx, uint16_t cy, uint16_t r, uint16_t color)
{
    int16_t x = 0, y = r;
    int16_t d = 3 - 2 * r;
    while (y >= x) {
        GC9A01_DrawPixel(cx + x, cy + y, color);
        GC9A01_DrawPixel(cx - x, cy + y, color);
        GC9A01_DrawPixel(cx + x, cy - y, color);
        GC9A01_DrawPixel(cx - x, cy - y, color);
        GC9A01_DrawPixel(cx + y, cy + x, color);
        GC9A01_DrawPixel(cx - y, cy + x, color);
        GC9A01_DrawPixel(cx + y, cy - x, color);
        GC9A01_DrawPixel(cx - y, cy - x, color);
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
    }
}

void GC9A01_FillCircle(uint16_t cx, uint16_t cy, uint16_t r, uint16_t color)
{
    for (int16_t dy = -r; dy <= r; dy++) {
        int32_t yy = (int32_t)dy * dy;
        int32_t rr = (int32_t)r * r;
        int16_t h = (int16_t)sqrtf((float)(rr - yy));
        GC9A01_DrawLine(cx - h, cy + dy, cx + h, cy + dy, color);
    }
}

void GC9A01_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size)
{
    if (c < 32 || c > 127) c = 63; // '?' for unknown
    const uint8_t *bitmap = font_8x8[(uint8_t)c - 32];

    for (uint8_t row = 0; row < 8; row++) {
        uint8_t line = bitmap[row];
        for (uint8_t col = 0; col < 8; col++) {
            uint16_t px = x + col * size;
            uint16_t py = y + row * size;
            if ((line >> (7 - col)) & 1) {
                if (size == 1) {
                    GC9A01_DrawPixel(px, py, color);
                } else {
                    GC9A01_FillRect(px, py, size, size, color);
                }
            } else if (bg != color) {
                if (size == 1) {
                    GC9A01_DrawPixel(px, py, bg);
                } else {
                    GC9A01_FillRect(px, py, size, size, bg);
                }
            }
        }
    }
}

void GC9A01_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t size)
{
    while (*str) {
        GC9A01_DrawChar(x, y, *str, color, bg, size);
        x += 8 * size;
        str++;
    }
}

/* Draw a single character rotated 90° clockwise.
 * Use this when the display scanning direction renders normal text sideways.
 * The hardware rotation (e.g. 0x08 -> counter-clockwise 90°) will then
 * make the character appear upright on the physical screen.
 */
void GC9A01_DrawCharRotated(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size)
{
    if (c < 32 || c > 127) c = 63; // '?' for unknown
    const uint8_t *bitmap = font_8x8[(uint8_t)c - 32];

    for (uint8_t row = 0; row < 8; row++) {
        uint8_t line = bitmap[row];
        for (uint8_t col = 0; col < 8; col++) {
            if ((line >> (7 - col)) & 1) {
                /* Clockwise 90°: (row, col) -> (col, 7 - row) */
                uint16_t px = x + row * size;
                uint16_t py = y + (7 - col) * size;
                if (size == 1) {
                    GC9A01_DrawPixel(px, py, color);
                } else {
                    GC9A01_FillRect(px, py, size, size, color);
                }
            } else if (bg != color) {
                uint16_t px = x + row * size;
                uint16_t py = y + (7 - col) * size;
                if (size == 1) {
                    GC9A01_DrawPixel(px, py, bg);
                } else {
                    GC9A01_FillRect(px, py, size, size, bg);
                }
            }
        }
    }
}

void GC9A01_DrawStringRotated(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t size)
{
    while (*str) {
        GC9A01_DrawCharRotated(x, y, *str, color, bg, size);
        x += 8 * size;
        str++;
    }
}

void GC9A01_DrawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    GC9A01_DrawLine(x0, y0, x1, y1, color);
    GC9A01_DrawLine(x1, y1, x2, y2, color);
    GC9A01_DrawLine(x2, y2, x0, y0, color);
}

static void swap(uint16_t *a, uint16_t *b)
{
    uint16_t t = *a; *a = *b; *b = t;
}

void GC9A01_FillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    if (y0 > y1) { swap(&x0, &x1); swap(&y0, &y1); }
    if (y1 > y2) { swap(&x1, &x2); swap(&y1, &y2); }
    if (y0 > y1) { swap(&x0, &x1); swap(&y0, &y1); }

    if (y0 == y2) {
        int16_t a = x0, b = x0;
        if (x1 < a) a = x1; else if (x1 > b) b = x1;
        if (x2 < a) a = x2; else if (x2 > b) b = x2;
        GC9A01_DrawLine(a, y0, b, y0, color);
        return;
    }

    int16_t dx01 = x1 - x0, dy01 = y1 - y0;
    int16_t dx02 = x2 - x0, dy02 = y2 - y0;
    int16_t dx12 = x2 - x1, dy12 = y2 - y1;
    int32_t sa = 0, sb = 0;

    int16_t last = y1 == y2 ? y1 : y1 - 1;

    for (int16_t y = y0; y <= last; y++) {
        int16_t a = x0 + sa / dy01;
        int16_t b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        if (a > b) { int16_t t = a; a = b; b = t; }
        GC9A01_DrawLine(a, y, b, y, color);
    }

    sa = (int32_t)dx12 * (last - y1);
    sb = (int32_t)dx02 * (last - y0);
    for (int16_t y = last + 1; y <= y2; y++) {
        int16_t a = x1 + sa / dy12;
        int16_t b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        if (a > b) { int16_t t = a; a = b; b = t; }
        GC9A01_DrawLine(a, y, b, y, color);
    }
}

void GC9A01_Init(void)
{
    GC9A01_Reset();

    GC9A01_WriteCommand(0xEF);
    GC9A01_WriteCommand(0xEB);
    GC9A01_WriteData(0x14);

    GC9A01_WriteCommand(0xFE);
    GC9A01_WriteCommand(0xEF);

    GC9A01_WriteCommand(0xEB);
    GC9A01_WriteData(0x14);

    GC9A01_WriteCommand(0x84);
    GC9A01_WriteData(0x40);

    GC9A01_WriteCommand(0x85);
    GC9A01_WriteData(0xFF);

    GC9A01_WriteCommand(0x86);
    GC9A01_WriteData(0xFF);

    GC9A01_WriteCommand(0x87);
    GC9A01_WriteData(0xFF);

    GC9A01_WriteCommand(0x88);
    GC9A01_WriteData(0x0A);

    GC9A01_WriteCommand(0x89);
    GC9A01_WriteData(0x21);

    GC9A01_WriteCommand(0x8A);
    GC9A01_WriteData(0x00);

    GC9A01_WriteCommand(0x8B);
    GC9A01_WriteData(0x80);

    GC9A01_WriteCommand(0x8C);
    GC9A01_WriteData(0x01);

    GC9A01_WriteCommand(0x8D);
    GC9A01_WriteData(0x01);

    GC9A01_WriteCommand(0x8E);
    GC9A01_WriteData(0xFF);

    GC9A01_WriteCommand(0x8F);
    GC9A01_WriteData(0xFF);

    GC9A01_WriteCommand(0xB6);
    GC9A01_WriteData(0x00);
    GC9A01_WriteData(0x20);

    GC9A01_WriteCommand(0x3A);
    GC9A01_WriteData(0x05); // 16-bit/pixel

    GC9A01_WriteCommand(0x90);
    GC9A01_WriteData(0x08);
    GC9A01_WriteData(0x08);
    GC9A01_WriteData(0x08);
    GC9A01_WriteData(0x08);

    GC9A01_WriteCommand(0xBD);
    GC9A01_WriteData(0x06);

    GC9A01_WriteCommand(0xBC);
    GC9A01_WriteData(0x00);

    GC9A01_WriteCommand(0xFF);
    GC9A01_WriteData(0x60);
    GC9A01_WriteData(0x01);
    GC9A01_WriteData(0x04);

    GC9A01_WriteCommand(0xC3);
    GC9A01_WriteData(0x13);
    GC9A01_WriteCommand(0xC4);
    GC9A01_WriteData(0x13);

    GC9A01_WriteCommand(0xC9);
    GC9A01_WriteData(0x22);

    GC9A01_WriteCommand(0xBE);
    GC9A01_WriteData(0x11);

    GC9A01_WriteCommand(0xE1);
    GC9A01_WriteData(0x10);
    GC9A01_WriteData(0x0E);

    GC9A01_WriteCommand(0xDF);
    GC9A01_WriteData(0x21);
    GC9A01_WriteData(0x0c);
    GC9A01_WriteData(0x02);

    GC9A01_WriteCommand(0xF0);
    GC9A01_WriteData(0x45);
    GC9A01_WriteData(0x09);
    GC9A01_WriteData(0x08);
    GC9A01_WriteData(0x08);
    GC9A01_WriteData(0x26);
    GC9A01_WriteData(0x2A);

    GC9A01_WriteCommand(0xF1);
    GC9A01_WriteData(0x43);
    GC9A01_WriteData(0x70);
    GC9A01_WriteData(0x72);
    GC9A01_WriteData(0x36);
    GC9A01_WriteData(0x37);
    GC9A01_WriteData(0x6F);

    GC9A01_WriteCommand(0xF2);
    GC9A01_WriteData(0x45);
    GC9A01_WriteData(0x09);
    GC9A01_WriteData(0x08);
    GC9A01_WriteData(0x08);
    GC9A01_WriteData(0x26);
    GC9A01_WriteData(0x2A);

    GC9A01_WriteCommand(0xF3);
    GC9A01_WriteData(0x43);
    GC9A01_WriteData(0x70);
    GC9A01_WriteData(0x72);
    GC9A01_WriteData(0x36);
    GC9A01_WriteData(0x37);
    GC9A01_WriteData(0x6F);

    GC9A01_WriteCommand(0xED);
    GC9A01_WriteData(0x1B);
    GC9A01_WriteData(0x0B);

    GC9A01_WriteCommand(0xAE);
    GC9A01_WriteData(0x77);

    GC9A01_WriteCommand(0xCD);
    GC9A01_WriteData(0x63);

    GC9A01_WriteCommand(0x70);
    GC9A01_WriteData(0x07);
    GC9A01_WriteData(0x07);
    GC9A01_WriteData(0x04);
    GC9A01_WriteData(0x0E);
    GC9A01_WriteData(0x0F);
    GC9A01_WriteData(0x09);
    GC9A01_WriteData(0x07);
    GC9A01_WriteData(0x08);
    GC9A01_WriteData(0x03);

    GC9A01_WriteCommand(0xE8);
    GC9A01_WriteData(0x34);

    GC9A01_WriteCommand(0x62);
    GC9A01_WriteData(0x18);
    GC9A01_WriteData(0x0D);
    GC9A01_WriteData(0x71);
    GC9A01_WriteData(0xED);
    GC9A01_WriteData(0x70);
    GC9A01_WriteData(0x70);
    GC9A01_WriteData(0x18);
    GC9A01_WriteData(0x0F);
    GC9A01_WriteData(0x71);
    GC9A01_WriteData(0xEF);
    GC9A01_WriteData(0x70);
    GC9A01_WriteData(0x70);

    GC9A01_WriteCommand(0x63);
    GC9A01_WriteData(0x18);
    GC9A01_WriteData(0x11);
    GC9A01_WriteData(0x71);
    GC9A01_WriteData(0xF1);
    GC9A01_WriteData(0x70);
    GC9A01_WriteData(0x70);
    GC9A01_WriteData(0x18);
    GC9A01_WriteData(0x13);
    GC9A01_WriteData(0x71);
    GC9A01_WriteData(0xF3);
    GC9A01_WriteData(0x70);
    GC9A01_WriteData(0x70);

    GC9A01_WriteCommand(0x64);
    GC9A01_WriteData(0x28);
    GC9A01_WriteData(0x29);
    GC9A01_WriteData(0xF1);
    GC9A01_WriteData(0x01);
    GC9A01_WriteData(0xF1);
    GC9A01_WriteData(0x00);
    GC9A01_WriteData(0x07);

    GC9A01_WriteCommand(0x66);
    GC9A01_WriteData(0x3C);
    GC9A01_WriteData(0x00);
    GC9A01_WriteData(0xCD);
    GC9A01_WriteData(0x67);
    GC9A01_WriteData(0x45);
    GC9A01_WriteData(0x45);
    GC9A01_WriteData(0x10);
    GC9A01_WriteData(0x00);
    GC9A01_WriteData(0x00);
    GC9A01_WriteData(0x00);

    GC9A01_WriteCommand(0x67);
    GC9A01_WriteData(0x00);
    GC9A01_WriteData(0x3C);
    GC9A01_WriteData(0x00);
    GC9A01_WriteData(0x00);
    GC9A01_WriteData(0x00);
    GC9A01_WriteData(0x01);
    GC9A01_WriteData(0x54);
    GC9A01_WriteData(0x10);
    GC9A01_WriteData(0x32);
    GC9A01_WriteData(0x98);

    GC9A01_WriteCommand(0x74);
    GC9A01_WriteData(0x10);
    GC9A01_WriteData(0x85);
    GC9A01_WriteData(0x80);
    GC9A01_WriteData(0x00);
    GC9A01_WriteData(0x00);
    GC9A01_WriteData(0x4E);
    GC9A01_WriteData(0x00);

    GC9A01_WriteCommand(0x98);
    GC9A01_WriteData(0x3e);
    GC9A01_WriteData(0x07);

    GC9A01_WriteCommand(0x35); // Tearing effect line on
    GC9A01_WriteCommand(0x21); // Display inversion on

    GC9A01_WriteCommand(0x11); // Sleep Out
    HAL_Delay(150);
    GC9A01_WriteCommand(0x29); // Display On
    HAL_Delay(50);
}

void GC9A01_Sleep(void)
{
    GC9A01_WriteCommand(0x28); // Display Off
    HAL_Delay(50);
    GC9A01_WriteCommand(0x10); // Sleep In
    HAL_Delay(100);
}

void GC9A01_Wake(void)
{
    GC9A01_WriteCommand(0x11); // Sleep Out
    HAL_Delay(150);
    GC9A01_WriteCommand(0x29); // Display On
    HAL_Delay(50);
}
