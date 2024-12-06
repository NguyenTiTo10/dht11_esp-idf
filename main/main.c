#include "drv_dht11.h"
#include "bsp_timer.h"

#define DHT11_INTERVAL_US 3000000  // 3 seconds interval in microseconds

void app_main() 
{
    if (!drv_dht11_init())
    {
        ESP_LOGE("DHT11", "DHT11 Initialization failed!");
        return;
    }

    int64_t last_time = esp_timer_get_time();  // Initialize the time counter
    uint32_t count = 0;
    float temp = 0.0f, humid = 0.0f;  // Initialize variables to default values

    char temp_str[30];  // Buffer to hold the temperature string
    char humid_str[30]; // Buffer to hold the humidity string

    while (1)
    {
        int64_t current_time = esp_timer_get_time();

        // Check if 2 seconds have passed
        if ((current_time - last_time) >= DHT11_INTERVAL_US)
        {
            last_time = current_time;  // Update the last time

            if (drv_dht11_start_read())  // Fetch new data from the DHT11
            {
                humid = drv_dht11_get_humid();  // Get the latest humidity
                temp = drv_dht11_get_temp();    // Get the latest temperature
                count += 1;

                // Format strings
                snprintf(temp_str, sizeof(temp_str), "Temp    : %.1f C        ", temp);
                snprintf(humid_str, sizeof(humid_str), "Humid   : %.1f %%       ", humid);

                // Print the results
                printf("Count: %lu\n", count);
                printf("[Temperature]> %.2f  \n", temp);
                printf("[Humidity]> %.2f \n\n", humid);

                // Print the formatted strings
                printf("%s\n", temp_str);
                printf("%s\n\n", humid_str);
            }
            else
            {
                ESP_LOGE("DHT11", "Failed to read data from DHT11!");
            }
        }

        bsp_delay(10);

        // Perform other tasks here
        // Example: Handle communication, control other peripherals, etc.
    }
}
