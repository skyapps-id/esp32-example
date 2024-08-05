#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "connection/wifi.h"
#include "server/http_server.h"
#include "blink.h"
#include "sensors.h"
#include "clients/mqtt_client_handler.h"
#include "driver/gpio.h"
#include "button_boot.h"
#include "shared/state_manager.h"

#define BUTTON_GPIO GPIO_NUM_0

void app_main(void) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Handler Button Boot
    setup_button_boot();
    xTaskCreate(&button_boot_task, "button_task", 2048, NULL, 10, NULL);

    // Mode Botting Provisioning
    state_t provisioningMode = load_state();
    printf("Current State Provisioning: %d\n", provisioningMode);
    if (provisioningMode == true) {
        // Blink
        xTaskCreate(&blink_task, "blink_task", 2048, NULL, 5, NULL);
        
        // Start Wifi AP
        wifi_init_ap();

        // Start HTTP server
        httpd_handle_t server = NULL;
        start_http_server(&server);
    } else {
        // Initialize Wi-Fi
        wifi_init_sta();

        /* esp_mqtt_client_config_t mqtt_cfg = {
            .broker.address.uri = "mqtt://192.168.100.98",
            .broker.address.port = 1883,
            .credentials.client_id = "esp32",
            .credentials.username = "esp32"
        };

        esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
        esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
        esp_mqtt_client_start(client); */
        
        // xTaskCreate(&sensors_task, "dht_task", 2048, client, 5, NULL);

        // Start HTTP server
        httpd_handle_t server = NULL;
        start_http_server(&server);
    }
    
}
