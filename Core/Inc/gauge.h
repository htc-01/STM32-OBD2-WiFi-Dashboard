#ifndef __GAUGE_H
#define __GAUGE_H

#include <stdint.h>

/* Number of gauge pages */
#define GAUGE_COUNT 6

/* Gauge parameter indices */
#define GAUGE_LOAD  0
#define GAUGE_TMP   1
#define GAUGE_RPM   2
#define GAUGE_SPD   3
#define GAUGE_MAF   4
#define GAUGE_THR   5

/* Screen center */
#define GAUGE_CX    120
#define GAUGE_CY    120

void Gauge_Draw(uint8_t index, int16_t value);
void Gauge_DrawPageIndicator(uint8_t current_page);

#endif /* __GAUGE_H */
