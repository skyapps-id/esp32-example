#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "OTA";

void ota_update_task(void *pvParameter)
{
    esp_err_t err;
    esp_ota_handle_t ota_handle;
    const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);
    const esp_partition_t *running_partition = esp_ota_get_running_partition();

    // Validate that the running partition is an OTA partition
    if (running_partition->type != ESP_PARTITION_TYPE_APP ||
        (running_partition->subtype != ESP_PARTITION_SUBTYPE_APP_OTA_0 &&
         running_partition->subtype != ESP_PARTITION_SUBTYPE_APP_OTA_1)) {
        ESP_LOGE(TAG, "Current running partition is not an OTA partition, aborting OTA update");
        vTaskDelete(NULL);
    }

    if (update_partition == NULL) {
        ESP_LOGE(TAG, "No OTA partition found");
        vTaskDelete(NULL);
    }

    // HTTP Client Configuration
    esp_http_client_config_t http_config = {
        .url = "http://192.168.100.98:8080/firmware",
        .timeout_ms = 5000,
    };

    esp_http_client_handle_t http_client = esp_http_client_init(&http_config);
    err = esp_http_client_open(http_client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(http_client);
        vTaskDelete(NULL);
    }

    int content_length = esp_http_client_fetch_headers(http_client);
    if (content_length <= 0) {
        ESP_LOGE(TAG, "Firmware file not found or invalid content length");
        esp_http_client_cleanup(http_client);
        vTaskDelete(NULL);
    }

    err = esp_ota_begin(update_partition, content_length, &ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin failed: %s", esp_err_to_name(err));
        esp_http_client_cleanup(http_client);
        vTaskDelete(NULL);
    }

    ESP_LOGI(TAG, "Writing firmware to flash...");

    int data_read;
    char buffer[1024];
    while ((data_read = esp_http_client_read(http_client, buffer, sizeof(buffer))) > 0) {
        err = esp_ota_write(ota_handle, buffer, data_read);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_ota_write failed: %s", esp_err_to_name(err));
            esp_http_client_cleanup(http_client);
            vTaskDelete(NULL);
        }
    }

    if (data_read < 0) {
        ESP_LOGE(TAG, "Error reading HTTP response");
    }

    err = esp_ota_end(ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_end failed: %s", esp_err_to_name(err));
    }

    err = esp_ota_set_boot_partition(update_partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "OTA update successful, rebooting...");
        esp_restart();
    }

    esp_http_client_cleanup(http_client);
    vTaskDelete(NULL);
}

// xTaskCreate(&ota_update_task, "ota_update_task", 8192, NULL, 5, NULL);