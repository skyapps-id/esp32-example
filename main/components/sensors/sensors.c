#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "dht.h"
#include "clients/mqtt_client_handler.h"

#define DHT_GPIO GPIO_NUM_16

static const char *TAG = "DHT11";

void sensors_task(void *pvParameter) {
    esp_mqtt_client_handle_t client = (esp_mqtt_client_handle_t)pvParameter;
    int16_t temperature = 0;
    int16_t humidity = 0;
    char combinedStr[50] = "";
    while (1) {
        if (dht_read_data(DHT_TYPE_DHT11, DHT_GPIO, &humidity, &temperature) == ESP_OK) {
            sprintf(combinedStr, "Humidity: %d%% Temp: %dC", humidity / 10, temperature / 10);
            esp_mqtt_client_publish(client, "sensor/data", combinedStr, 0, 1, 0);
            ESP_LOGI(TAG, "sent publish successful, combinedStr=%s", combinedStr);
        } else {
            ESP_LOGE(TAG, "Could not read data from sensor");
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
