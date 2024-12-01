#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h" // For ets_delay_us
#include "esp_rom_gpio.h" // For esp_rom_gpio_pad_select_gpio
#include "rom/ets_sys.h"

#define DHT11_PIN GPIO_NUM_4  // Change this to the GPIO pin you are using

typedef struct {
    float temperature;
    float humidity;
} dht11_data_t;

static const char *TAG = "DHT11";

static void dht11_delay_us(uint32_t us) {
    // Use the ESP-IDF function for microsecond delay
    ets_delay_us(us);
}

static void dht11_gpio_init() {
    // Use esp_rom_gpio_pad_select_gpio instead of gpio_pad_select_gpio
    esp_rom_gpio_pad_select_gpio(DHT11_PIN);
    gpio_set_direction(DHT11_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(DHT11_PIN, 1);
}

bool dht11_read(dht11_data_t *data) {
    uint8_t buf[5] = {0};
    
    // Start signal
    dht11_gpio_init();
    gpio_set_level(DHT11_PIN, 0);
    dht11_delay_us(18000); // 18ms
    gpio_set_level(DHT11_PIN, 1);
    dht11_delay_us(20); // 20us
    gpio_set_direction(DHT11_PIN, GPIO_MODE_INPUT);

    // Wait for DHT11 response
    dht11_delay_us(40);
    if (gpio_get_level(DHT11_PIN) == 1) {
        ESP_LOGE(TAG, "DHT11 did not respond");
        return false;
    }
    dht11_delay_us(80);
    if (gpio_get_level(DHT11_PIN) == 0) {
        ESP_LOGE(TAG, "DHT11 response timing error");
        return false;
    }
    dht11_delay_us(80);
    
    // Read data
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 8; j++) {
            while (gpio_get_level(DHT11_PIN) == 0); // Wait for the pin to go high
            dht11_delay_us(30); // Wait for 30us
            if (gpio_get_level(DHT11_PIN) == 1) {
                buf[i] |= (1 << (7 - j)); // Set the bit
            }
            while (gpio_get_level(DHT11_PIN) == 1); // Wait for the pin to go low
        }
    }

    // Verify checksum
    if (buf[0] + buf[1] + buf[2] + buf[3] != buf[4]) {
        ESP_LOGE(TAG, "Checksum error");
        return false;
    }

    // Assign values to the output data structure
    data->humidity = buf[0] + buf[1] * 0.1; // Humidity value
    data->temperature = buf[2] + buf[3] * 0.1; // Temperature value

    return true;
}

void app_main(void) {
    dht11_data_t data;
    const int max_retries = 5; // Maximum number of retries
    const int retry_delay = 500; // Delay between retries in milliseconds

    // Initial delay before the first read
    vTaskDelay(pdMS_TO_TICKS(2000));

    while (1) {
        bool success = false;
        for (int i = 0; i < max_retries; i++) {
            if (dht11_read(&data)) {
                printf("Temperature: %.1f °C, Humidity: %.1f %%\n", data.temperature, data.humidity);
                success = true;
                break; // Exit the retry loop on success
            } else {
                printf("Failed to read from DHT11 sensor, attempt %d\n", i + 1);
                vTaskDelay(pdMS_TO_TICKS(retry_delay)); // Wait before retrying
            }
        }

        if (!success) {
            printf("Failed to read from DHT11 sensor after %d attempts\n", max_retries);
        }

        vTaskDelay(pdMS_TO_TICKS(3000)); // Read every 3 seconds
    }
}