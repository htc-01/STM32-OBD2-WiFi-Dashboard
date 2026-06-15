#ifndef __GAUGE_FLASH_DATA_H
#define __GAUGE_FLASH_DATA_H

#include <stdint.h>

/* Pre-computed arc coordinates for every 0.5 degrees (0 .. 359.5).
 * Index 0 = 0.0 deg, index 720 wraps back to 0.0 deg.
 * Layout per entry: [outer_x, outer_y, inner_x, inner_y] */
extern const uint8_t ARC_COORDS[720][4];

#endif /* __GAUGE_FLASH_DATA_H */
