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


/**
 * @brief The function for reading temperature and humidity values from the dht11
 * @note  This function is blocking, ie: it forces the cpu to busy wait for the duration necessary to finish comms with the sensor.
 * @note  Wait for atleast 2 seconds between reads 
 * @param connection_timeout the number of connection attempts before declaring a timeout
*/
int drv_dht11_start_read(dht11_t *dht11,int connection_timeout);
#endif