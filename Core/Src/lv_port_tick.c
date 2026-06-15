#include "lvgl.h"
#include "lv_port_tick.h"
#include "stm32f4xx_hal.h"

void lv_port_tick_init(void)
{
    /* LVGL tick is provided by HAL_GetTick() in the SysTick ISR */
}

/* This function is called in the SysTick_Handler (1ms interval) */
void lv_tick_handler(void)
{
    lv_tick_inc(1);
}
