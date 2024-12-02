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
 * @var dht11_pin the pin associated with the dht11
 * @var temperature last temperature reading
 * @var humidity last humidity reading 
*/
typedef struct
{
    int dht11_pin;
    float temperature;
    float humidity;
} dht11_t;

int drv_dht11_start_read(dht11_t *dht11,int connection_timeout);

float drv_dht11_get_temp(void);

float drv_dht11_get_humid(void);
#endif