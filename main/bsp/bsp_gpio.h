#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#include "driver/gpio.h"


uint8_t bsp_gpio_read_pin       (gpio_num_t gpio_num);
void    bsp_gpio_toggle_pin     (gpio_num_t gpio_num);
void    bsp_gpio_write_pin      (gpio_num_t gpio_num, uint32_t level); 

void    bsp_gpio_set_direction  (gpio_num_t gpio_num, gpio_mode_t mode);


#endif