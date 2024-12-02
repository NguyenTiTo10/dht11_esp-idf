#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "drv_dht11.h"


void app_main() 
{
    if (!drv_dht11_init())
    {
      ESP_LOGE("DHT11", "DHT11 Initialization failed!");
    }

    uint32_t count = 0;
    float temp = 0.0f, humid = 0.0f;  // Initialize variables to default values


    while(1)
    {
      
      if(drv_dht11_start_read())
      {  
        humid = drv_dht11_get_humid();
        temp = drv_dht11_get_temp();
        count += 1;
      }

      printf("Count: %lu\n", count);
      printf("[Temperature]> %.2f  \n",temp);
      printf("[Humidity]> %.2f \n \n",humid);

      vTaskDelay(2000/portTICK_PERIOD_MS);
    } 
}