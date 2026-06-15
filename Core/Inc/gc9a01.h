#ifndef __GC9A01_H
#define __GC9A01_H

#include "main.h"
#include <stdint.h>

/* GC9A01 display parameters */
#define GC9A01_WIDTH   240
#define GC9A01_HEIGHT  240

/* Color definitions (RGB565) */
#define GC9A01_BLACK   0x0000
#define GC9A01_WHITE   0xFFFF
#define GC9A01_RED     0xF800
#define GC9A01_GREEN   0x07E0
#define GC9A01_BLUE    0x001F
#define GC9A01_CYAN    0x07FF
#define GC9A01_MAGENTA 0xF81F
#define GC9A01_YELLOW  0xFFE0
#define GC9A01_ORANGE  0xFD20
#define GC9A01_GRAY    0x8410
#define GC9A01_DARKGRAY 0x4208

/* Display rotation (MADCTL value for 0x36 command)
 * Common values for GC9A01 round displays:
 *   0x08 -> default (used here; text is drawn rotated in software)
 *   0x68 -> portrait (upright) – rotates whole screen, not just text
 *   0xC8 -> inverted portrait (180°)
 *   0xA8 -> inverted landscape (270°)
 */
#define GC9A01_ROTATION  0x08

/* Hardware control macros */
#define GC9A01_CS_LOW()      HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET)
#define GC9A01_CS_HIGH()     HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET)
#define GC9A01_DC_LOW()      HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET)
#define GC9A01_DC_HIGH()     HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET)
#define GC9A01_RST_LOW()     HAL_GPIO_WritePin(screen_RST_GPIO_Port, screen_RST_Pin, GPIO_PIN_RESET)
#define GC9A01_RST_HIGH()    HAL_GPIO_WritePin(screen_RST_GPIO_Port, screen_RST_Pin, GPIO_PIN_SET)

void GC9A01_Init(void);
void GC9A01_Reset(void);
void GC9A01_WriteCommand(uint8_t cmd);
void GC9A01_WriteData(uint8_t data);
void GC9A01_WriteData16(uint16_t data);
void GC9A01_WriteDataMulti(uint8_t *data, uint16_t len);
void GC9A01_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void GC9A01_FillScreen(uint16_t color);
void GC9A01_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void GC9A01_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void GC9A01_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void GC9A01_DrawCircle(uint16_t cx, uint16_t cy, uint16_t r, uint16_t color);
void GC9A01_FillCircle(uint16_t cx, uint16_t cy, uint16_t r, uint16_t color);
void GC9A01_DrawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size);
void GC9A01_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t size);
void GC9A01_DrawCharRotated(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size);
void GC9A01_DrawStringRotated(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t size);
void GC9A01_DrawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void GC9A01_FillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);

/* Power control */
void GC9A01_Sleep(void);
void GC9A01_Wake(void);

#endif /* __GC9A01_H */
