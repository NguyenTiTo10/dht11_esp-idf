#include "esp32-dht11.h"


/*
    Relies on precise timing to distinguish between 0 and 1 bits in the data stream.
    For example:
        A short high pulse (26–28 µs) represents a 0.
        A long high pulse (70 µs) represents a 1.

    By measuring how long it takes for the pin to reach a state, this function enables the driver to decode the transmitted bits.
*/
int wait_for_state(dht11_t dht11,int state,int timeout)
{
    gpio_set_direction(dht11.dht11_pin, GPIO_MODE_INPUT);               // Set the GPIO mode: Input 
                                                                        // Let the DHT11 sends data

    int count = 0;                                                      // Track elapsed time spent waiting for the pin state
    
    while(gpio_get_level(dht11.dht11_pin) != state)                    
    {
        if(count == timeout)                                            // If reached the timeout, return -1 (fail).
            return -1;                                  
        count += 2;
        ets_delay_us(2);                                                // A short delay of 2 microseconds
    }

    return  count;                                                      // Return the Elapsed Time
}


/*
    Send a signal from the ESP32 to the DHT11 sensor:
        - By pulling a GPIO pin low (logic level 0) for a specified amount of time.
        - Then returning the pin to high (logic level 1). 
        
    Make a initialization sequence for communication with the DHT11 sensor.
*/

void hold_low(dht11_t dht11, int hold_time_us)
{
    gpio_set_direction(dht11.dht11_pin,GPIO_MODE_OUTPUT);
    gpio_set_level(dht11.dht11_pin,0);
    ets_delay_us(hold_time_us);
    gpio_set_level(dht11.dht11_pin,1);
}

int dht11_read(dht11_t *dht11,int connection_timeout)
{
    int waited = 0;
    int one_duration = 0;
    int zero_duration = 0;
    int timeout_counter = 0;

    uint8_t recieved_data[5] = 
    {
        0x00,
        0x00,
        0x00,
        0x00,
        0x00
    };

    while(timeout_counter < connection_timeout)
    {
        timeout_counter++;
        gpio_set_direction(dht11->dht11_pin,GPIO_MODE_INPUT);
        hold_low(*dht11, 18000);
        
        waited = wait_for_state(*dht11,0,40);

        if(waited == -1)
        {
            ESP_LOGE("DHT11:","Failed at phase 1");
            ets_delay_us(20000);
            continue;
        } 


        waited = wait_for_state(*dht11,1,80);
        if(waited == -1) 
        {
            ESP_LOGE("DHT11:","Failed at phase 2");
            ets_delay_us(20000);
            continue;
        } 
        
        waited = wait_for_state(*dht11,0,80);
        if(waited == -1) 
        {
            ESP_LOGE("DHT11:","Failed at phase 3");
            ets_delay_us(20000);
            continue;
        } 
        break;
        
    }
    
    if(timeout_counter == connection_timeout) return -1;

    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            zero_duration = wait_for_state(*dht11,1,60);         
            one_duration = wait_for_state(*dht11,0,80);
            recieved_data[i] |= (one_duration > zero_duration) << (7 - j);
        }
    }
    dht11->humidity = recieved_data[0] + recieved_data[1] /10.0 ;
    dht11->temperature = recieved_data[2] + recieved_data[3] /10.0 ;
    return 0;
}
