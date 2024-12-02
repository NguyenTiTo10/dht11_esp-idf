#include "drv_dht11.h"


static int drv_dht11_wait_for_pin_state (uint8_t state, int timeout);
static void drv_dht11_init_transmit     (void);
static bool drv_dht11_checksum_valid    (const uint8_t *data);

static dht11_t dht11_sensor;

/**
 * @brief Wait on pin until it reaches the specified state
 * @return returns either the time waited or -1 in the case of a timeout
 * @param state state to wait for
 * @param timeout if counter reaches timeout the function returns -1
*/
static int drv_dht11_wait_for_pin_state (uint8_t state, int timeout)
{
    bsp_gpio_set_direction(dht11_sensor.pin, GPIO_MODE_INPUT);               // Set the GPIO mode: Input 
                                                                        // Let the DHT11 sends data

    int wait_time = 0;                                                      // Track elapsed time spent waiting for the pin state
    
    while(gpio_get_level(dht11_sensor.pin) != state)                    
    {
        if(wait_time == timeout)                                            // If reached the timeout, return -1.
            return -1;                                  
        wait_time += 2;
        bsp_timer_ets_delay_us(2);                                                // A short delay of 2 microseconds
    }

    return  wait_time;                                                      // Return the Elapsed Time
}



/**
 * @brief Holds the pin low to the specified duration
 * @param hold_time_us time to hold the pin low for in microseconds
*/
static void drv_dht11_init_transmit(void)
{
    bsp_gpio_set_direction(dht11_sensor.pin,GPIO_MODE_OUTPUT);               // Configure GPIO: Output
    bsp_gpio_write_pin(dht11_sensor.pin,0);                                  // Drives the GPIO pin to a low logic level (0) 
                                                                            // To send a "start signal" to the DHT11 sensor.
    bsp_timer_ets_delay_us(TIME_LOW_INIT_SIGNAL);                                         // Delay "hold_time_us"
    bsp_gpio_write_pin(dht11_sensor.pin,1);                                  // Drives the GPIO pin back to a high logic level (1)
                                                                        // After this step, the DHT11 will begin its response.
}



/**
 * @brief Validate the checksum of the received DHT11 data.
 * 
 * @param data Pointer to the array containing the received data (5 bytes).
 *             data[0] = Humidity integer part
 *             data[1] = Humidity decimal part
 *             data[2] = Temperature integer part
 *             data[3] = Temperature decimal part
 *             data[4] = Checksum
 * @return true if checksum is valid, false otherwise.
 */
static bool drv_dht11_checksum_valid(const uint8_t *data) 
{
    if (data == NULL) {
        return false; // Handle null pointer
    }

    uint8_t calculated_checksum = data[0] + data[1] + data[2] + data[3];
    return (calculated_checksum == data[4]);
}



/**
 * @brief Initializes the DHT11 sensor.
 *        This function sets the GPIO pin direction and sends the initial signal to the DHT11.
 * @param dht11 Pointer to the DHT11 instance (contains the GPIO pin info).
 * @return Returns 0 if successful, -1 if initialization failed.
 */
uint8_t drv_dht11_init(void)
{
    // if (&dht11_sensor == NULL) 
    // {
    //     ESP_LOGE("DHT11:", "Invalid DHT11 pointer");
    //     return 0;  // Return error if the pointer is invalid.
    // }

    dht11_sensor.pin = CONFIG_DHT11_PIN;
    
    return 1;  // Initialization successful
}


int drv_dht11_start_read(void)
{
    int waited = 0;                                                     // Tracks the duration for state changes.
    int one_duration = 0;                                               // Measure the high pulse durations for decoding bits.
    int zero_duration = 0;                                              // Measure the low pulse durations for decoding bits.
    int timeout_counter = 0;                                            // Counts the number of retries.

    uint8_t recieved_data[5] =                                          // Stores the raw data received from the sensor
    {
        0x00,
        0x00,
        0x00,
        0x00,
        0x00
    };

    while(timeout_counter < CONNECTION_TIMEOUT)                         
    {
        timeout_counter++;
        bsp_gpio_set_direction(dht11_sensor.pin,GPIO_MODE_INPUT);
        drv_dht11_init_transmit();                                        // drv_dht11_init_transmit 18us, start the signal
        
                                                                        
                                                                        // If any phase fails, the loop retries after a delay of 20 ms.
        waited = drv_dht11_wait_for_pin_state   (0,40);                           // Waits for the sensor to pull the line low.                         

        if(waited == -1)    
        {
            ESP_LOGE("DHT11:","Failed at phase 1");     
            bsp_timer_ets_delay_us(20000);
            continue;
        } 


        waited = drv_dht11_wait_for_pin_state   (1,80);                           // Waits for the sensor to pull the line high.
        if(waited == -1) 
        {
            ESP_LOGE("DHT11:","Failed at phase 2");
            bsp_timer_ets_delay_us(20000);
            continue;
        } 
        
        waited = drv_dht11_wait_for_pin_state   (0,80);                           // Waits for the sensor to pull the line low again.
        if(waited == -1) 
        {
            ESP_LOGE("DHT11:","Failed at phase 3");
            bsp_timer_ets_delay_us(20000);
            continue;
        } 
        break;     
    }
    
    if(timeout_counter == CONNECTION_TIMEOUT)                           // Return -1: timeout
        return -1;

    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            zero_duration   =   drv_dht11_wait_for_pin_state (1,60);                        // Time spent at logic level 1
            one_duration    =   drv_dht11_wait_for_pin_state (0,80);                         // Time spent at logic level 0

            recieved_data[i] |= (one_duration > zero_duration) << (7 - j);      // If one_duration > zero_duration, 
                                                                                    // the bit is 1; 
                                                                                    // otherwise, it’s 0
        }
    }

    // Validate checksum
    if (!drv_dht11_checksum_valid(recieved_data)) 
    {
        ESP_LOGE("DHT11:", "Checksum validation failed");
        // return -2;
    }

    dht11_sensor.humidity = recieved_data[0] + recieved_data[1] /10.0 ;               // recieved_data[0]: Phần đơn vị
                                                                                // recieved_data[1] /10.0: Phần hàng chục

    dht11_sensor.temperature = recieved_data[2] + recieved_data[3] /10.0 ;            // Tương tự với nhiệt độ

    return 1;                                                                   
}



float drv_dht11_get_temp(void)
{
    float ret_temp;
    ret_temp = dht11_sensor.temperature;

    return ret_temp;
}


float drv_dht11_get_humid(void)
{
    float ret_humid;
    ret_humid = dht11_sensor.humidity;

    return ret_humid;
}
