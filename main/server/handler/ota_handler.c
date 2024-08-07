#include "ota_handler.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_system.h"
#include "helper.h"

static const char *TAG = "ota_handler";

esp_err_t ota_post_handler(httpd_req_t *req) {
    char buf[1024];
    int len;
    bool is_writing = false;

    const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);
    if (update_partition == NULL) {
        ESP_LOGE(TAG, "Failed to get OTA update partition");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    esp_ota_handle_t ota_handle;
    esp_err_t err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin failed, error=%d", err);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    while ((len = httpd_req_recv(req, buf, sizeof(buf))) > 0) {
        if (!is_writing) {
            char *file_start = strstr(buf, "Content-Disposition: form-data; name=\"file\"");
            if (file_start) {
                char *file_content_start = strstr(file_start, "\r\n\r\n");
                if (file_content_start) {
                    file_content_start += 4;
                    size_t file_content_len = len - (file_content_start - buf);
                    
                    err = esp_ota_write(ota_handle, file_content_start, file_content_len);
                    if (err != ESP_OK) {
                        ESP_LOGE(TAG, "OTA write failed");
                        esp_ota_end(ota_handle);
                        httpd_resp_send_500(req);
                        return ESP_FAIL;
                    }
                    is_writing = true;
                    continue;
                }
            }
        } else {
            err = esp_ota_write(ota_handle, buf, len);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "OTA write failed");
                esp_ota_end(ota_handle);
                httpd_resp_send_500(req);
                return ESP_FAIL;
            }
        }
    }

    if (len < 0) {
        ESP_LOGE(TAG, "File reception failed!");
        esp_ota_end(ota_handle);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    err = esp_ota_end(ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_end failed, error=%d", err);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    err = esp_ota_set_boot_partition(update_partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed, error=%d", err);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "OTA Update successful, rebooting...");
    httpd_resp_sendstr(req, "OTA Update successful, rebooting...");

    xTaskCreate(restart_task, "restart_task", 2048, NULL, 5, NULL);
    return ESP_OK;
}