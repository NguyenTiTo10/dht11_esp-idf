#include "bsp_timer.h"


void bsp_timer_ets_delay_us (uint32_t time_us)
{
  ets_delay_us(time_us);
}
