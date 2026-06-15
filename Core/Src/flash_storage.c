#include "flash_storage.h"
#include "custom.h"
#include "stm32f4xx_hal.h"

#define GAUGE_MAGIC  0xA5U

uint8_t flash_storage_read_gauge(void)
{
    uint16_t *p = (uint16_t *)FLASH_STORAGE_ADDR;
    uint16_t *end = (uint16_t *)(FLASH_STORAGE_ADDR + FLASH_STORAGE_SIZE);
    uint16_t *last_valid = NULL;

    while (p < end) {
        uint16_t val = *p;
        if (val == 0xFFFFU) {
            break;
        }
        if ((uint8_t)(val >> 8) == GAUGE_MAGIC) {
            last_valid = p;
        }
        p++;
    }

    if (last_valid != NULL) {
        uint8_t gauge = (uint8_t)(*last_valid & 0xFFU);
        if (gauge < GAUGE_COUNT) {
            return gauge;
        }
    }
    return 0;
}

int flash_storage_save_gauge(uint8_t gauge)
{
    uint16_t *p = (uint16_t *)FLASH_STORAGE_ADDR;
    uint16_t *end = (uint16_t *)(FLASH_STORAGE_ADDR + FLASH_STORAGE_SIZE);

    /* Find first empty slot */
    while (p < end) {
        if (*p == 0xFFFFU) {
            break;
        }
        p++;
    }

    if (p >= end) {
        return 1; /* Flash storage full */
    }

    HAL_FLASH_Unlock();

    /* Clear any previous error flags that would block programming */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR |
                           FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
                           FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    uint16_t data = (uint16_t)((GAUGE_MAGIC << 8) | (gauge & 0xFFU));
    HAL_StatusTypeDef status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,
                                                  (uint32_t)p,
                                                  (uint64_t)data);
    HAL_FLASH_Lock();

    return (status == HAL_OK) ? 0 : 1;
}

int flash_storage_init(void)
{
    uint16_t first = *(uint16_t *)FLASH_STORAGE_ADDR;

    /* Empty flash after erase -> all 0xFFFF */
    if (first == 0xFFFFU) {
        return 0;
    }

    /* If first word has valid magic, area is likely valid */
    if ((uint8_t)(first >> 8) == GAUGE_MAGIC) {
        return 0;
    }

    /* Flash area is not empty and does not start with valid data.
     * This usually means the flash was not erased before programming.
     */
    return 1;
}
