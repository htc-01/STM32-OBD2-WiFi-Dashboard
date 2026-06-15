#include "at24c02.h"
#include "stm32f4xx_hal.h"
#include "custom.h"

#define AT24C02_DEV_ADDR    0xA0U   /* 7-bit addr 0x50 << 1 (write) */
#define AT24C02_GAUGE_ADDR  0x00U
#define AT24C02_MAGIC_ADDR  0x01U
#define GAUGE_MAGIC         0xA5U

extern I2C_HandleTypeDef hi2c1;

static bool at24c02_ready = false;

int at24c02_init(void)
{
    uint8_t dummy = 0;
    /* Probe the device by reading magic address */
    if (HAL_I2C_Mem_Read(&hi2c1, AT24C02_DEV_ADDR, AT24C02_MAGIC_ADDR,
                         I2C_MEMADD_SIZE_8BIT, &dummy, 1, 100) != HAL_OK) {
        at24c02_ready = false;
        return 1; /* Device not ready */
    }
    at24c02_ready = true;
    return 0;
}

uint8_t at24c02_read_gauge(void)
{
    uint8_t magic = 0;
    uint8_t gauge = 0;

    if (!at24c02_ready) {
        return 0;
    }

    if (HAL_I2C_Mem_Read(&hi2c1, AT24C02_DEV_ADDR, AT24C02_MAGIC_ADDR,
                         I2C_MEMADD_SIZE_8BIT, &magic, 1, 100) != HAL_OK) {
        return 0;
    }

    if (magic != GAUGE_MAGIC) {
        return 0;
    }

    if (HAL_I2C_Mem_Read(&hi2c1, AT24C02_DEV_ADDR, AT24C02_GAUGE_ADDR,
                         I2C_MEMADD_SIZE_8BIT, &gauge, 1, 100) != HAL_OK) {
        return 0;
    }

    if (gauge < GAUGE_COUNT) {
        return gauge;
    }
    return 0;
}

int at24c02_save_gauge(uint8_t gauge)
{
    uint8_t data;

    if (!at24c02_ready) {
        return 1;
    }

    /* Write gauge index first */
    data = gauge;
    if (HAL_I2C_Mem_Write(&hi2c1, AT24C02_DEV_ADDR, AT24C02_GAUGE_ADDR,
                          I2C_MEMADD_SIZE_8BIT, &data, 1, 100) != HAL_OK) {
        return 1;
    }
    HAL_Delay(5); /* AT24C02 write cycle time (max 5 ms) */

    /* Write magic to validate the data */
    data = GAUGE_MAGIC;
    if (HAL_I2C_Mem_Write(&hi2c1, AT24C02_DEV_ADDR, AT24C02_MAGIC_ADDR,
                          I2C_MEMADD_SIZE_8BIT, &data, 1, 100) != HAL_OK) {
        return 1;
    }
    HAL_Delay(5);

    return 0;
}
