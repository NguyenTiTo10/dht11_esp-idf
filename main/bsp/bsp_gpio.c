#include "bsp_gpio.h"

static volatile bool isr_flag = false;

uint8_t bsp_gpio_read_pin  (gpio_num_t gpio_num)
{
  return gpio_get_level(gpio_num);
}

void bsp_gpio_write_pin   (gpio_num_t gpio_num, uint32_t level)
{
  gpio_set_level(gpio_num, level);
}

void bsp_gpio_toggle_pin  (gpio_num_t gpio_num)
{
  uint32_t state = !(gpio_get_level(gpio_num));
  gpio_set_level(gpio_num, state);
}

void bsp_gpio_set_direction (gpio_num_t gpio_num, gpio_mode_t mode)
{
  gpio_set_direction(gpio_num, mode);
}
