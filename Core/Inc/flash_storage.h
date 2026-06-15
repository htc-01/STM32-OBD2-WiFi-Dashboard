#ifndef __FLASH_STORAGE_H
#define __FLASH_STORAGE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Flash storage for persistent gauge index across power cycles.
 * A 16 KB region is reserved at the end of FLASH (240K of 256K).
 * Uses sequential half-word write (no runtime erase) for wear leveling.
 * IMPORTANT: Download firmware with "Erase Full Chip" to ensure the area
 * is cleared to 0xFF, otherwise writes will fail or read garbage.
 */

#define FLASH_STORAGE_ADDR  0x0803C000
#define FLASH_STORAGE_SIZE  16384

/* Initialize flash storage. Returns 0 if area looks valid, 1 if dirty
 * (not erased before programming).
 */
int flash_storage_init(void);

/* Read the saved gauge index. Returns default (0) if none saved. */
uint8_t flash_storage_read_gauge(void);

/* Save the current gauge index to flash. Uses sequential write.
 * Returns 0 on success, 1 if flash is full.
 */
int flash_storage_save_gauge(uint8_t gauge);

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_STORAGE_H */
