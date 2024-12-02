#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "drv_dht11.h"


void app_main() 
{
    drv_dht11_init();

    uint32_t count = 0;
    float temp, humid;

    while(1)
    {
      
      if(drv_dht11_start_read())
      {  
        humid = drv_dht11_get_humid();
        temp = drv_dht11_get_temp();
        count += 1;
      }

      printf("Count: %d", count);
      printf("[Temperature]> %.2f \n",temp);
      printf("[Humidity]> %.2f \n",humid);
      
      vTaskDelay(2000/portTICK_PERIOD_MS);
    } 
}