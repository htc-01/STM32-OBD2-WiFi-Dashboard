#ifndef AT24C02_H
#define AT24C02_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

int at24c02_init(void);
uint8_t at24c02_read_gauge(void);
int at24c02_save_gauge(uint8_t gauge);

#ifdef __cplusplus
}
#endif

#endif /* AT24C02_H */
