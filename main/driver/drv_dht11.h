#ifndef _DHT_11
#define _DHT_11
#include <driver/gpio.h>
#include <stdio.h>
#include <string.h>
#include <rom/ets_sys.h>
#include "esp_log.h"
#include "esp_err.h"

#include "bsp_timer.h"
#include "bsp_gpio.h"

#include <stdint.h>
#include <stdbool.h>

/**
 * Structure containing readings and info about the dht11
 * @var pin the pin associated with the dht11
 * @var temperature last temperature reading
 * @var humidity last humidity reading 
*/


#define     CONFIG_DHT11_PIN            GPIO_NUM_4
#define     TIME_LOW_INIT_SIGNAL        18000
#define     CONNECTION_TIMEOUT          5

typedef struct
{
    int     pin;
    float temperature;
    float humidity;
} dht11_t;

uint8_t  drv_dht11_init (void);

int drv_dht11_start_read(void);

float drv_dht11_get_temp(void);

float drv_dht11_get_humid(void);
#endif