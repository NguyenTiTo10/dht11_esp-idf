#include "stdint.h"
#include <rom/ets_sys.h>


#ifndef BSP_TIMER_H
#define BSP_TIMER_H

void bsp_timer_ets_delay_us (uint32_t time_us);

#endif