#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define DHT11_PIN GPIO_NUM_4 // Replace with your GPIO pin

// Timing definitions (in microseconds)
#define DHT_START_SIGNAL_LOW_TIME 18000 // MCU start signal low duration
#define DHT_START_SIGNAL_WAIT_TIME 20   // MCU wait time after pulling high
#define DHT_RESPONSE_LOW_TIME 80        // DHT response low duration
#define DHT_RESPONSE_HIGH_TIME 80       // DHT response high duration
#define DHT_BIT_DURATION 50             // Each bit starts with 50µs low

static const char *TAG = "DHT11";

static esp_err_t dht11_start_signal(void) {
    gpio_set_direction(DHT11_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(DHT11_PIN, 0);
    ets_delay_us(DHT_START_SIGNAL_LOW_TIME); // Hold low for 18ms
    gpio_set_level(DHT11_PIN, 1);
    ets_delay_us(DHT_START_SIGNAL_WAIT_TIME); // Wait 20-40µs
    gpio_set_direction(DHT11_PIN, GPIO_MODE_INPUT);
    return ESP_OK;
}

static esp_err_t dht11_read_data(uint8_t *data) {
    uint32_t low_time, high_time;
    for (int i = 0; i < 40; ++i) {
        while (!gpio_get_level(DHT11_PIN)) {}; // Wait for low signal
        low_time = esp_timer_get_time();
        while (gpio_get_level(DHT11_PIN)) {}; // Wait for high signal
        high_time = esp_timer_get_time() - low_time;

        data[i / 8] <<= 1; // Shift data
        if (high_time > DHT_BIT_DURATION) {
            data[i / 8] |= 1; // Set bit if high duration > 50µs
        }
    }
    return ESP_OK;
}

esp_err_t dht11_read(uint8_t *humidity, uint8_t *temperature) {
    uint8_t data[5] = {0};
    esp_err_t ret = dht11_start_signal();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Start signal failed");
        return ret;
    }

    // Read 40-bit data
    ret = dht11_read_data(data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read data");
        return ret;
    }

    // Verify checksum
    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    if (checksum != data[4]) {
        ESP_LOGE(TAG, "Checksum error");
        return ESP_ERR_INVALID_CRC;
    }

    *humidity = data[0];
    *temperature = data[2];
    return ESP_OK;
}

void app_main(void) {
    uint8_t humidity = 0, temperature = 0;

    while (1) {
        if (dht11_read(&humidity, &temperature) == ESP_OK) {
            ESP_LOGI(TAG, "Humidity: %d%%, Temperature: %d°C", humidity, temperature);
        } else {
            ESP_LOGE(TAG, "Failed to read DHT11");
        }
        vTaskDelay(pdMS_TO_TICKS(2000)); // Delay 2 seconds
    }
}
