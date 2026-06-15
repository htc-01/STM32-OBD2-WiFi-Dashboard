#ifndef __CUSTOM_H_
#define __CUSTOM_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define GAUGE_COUNT 6

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

void custom_init(void);
void custom_refresh(void);
void gauge_set_value(uint8_t index, int16_t value);
void gauge_next_page(void);

#ifdef __cplusplus
}
#endif
#endif /* __CUSTOM_H_ */
