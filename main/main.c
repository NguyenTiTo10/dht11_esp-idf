#include "drv_dht11.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


void app_main() {
    

    // Read data
    while(1)
    {
      // if(!drv_dht11_start_read(&dht11_sensor, CONFIG_CONNECTION_TIMEOUT))
      // {  
      //   printf("[Temperature]> %.2f \n",dht11_sensor.temperature);
      //   printf("[Humidity]> %.2f \n",dht11_sensor.humidity);
      // }
      vTaskDelay(2000/portTICK_PERIOD_MS);
    } 
}