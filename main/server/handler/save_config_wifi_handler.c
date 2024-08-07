#include "save_config_wifi_handler.h"
#include <string.h>
#include "esp_log.h"
#include "esp_http_server.h"
#include "blink.h"
#include "state_manager.h"
#include "helper.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static const char *TAG = "save_config_wifi";

const char* resp_html = "Successfully changing the configuration of Wi-Fi to {{SSID}}";

esp_err_t save_config_wifi_handler(httpd_req_t *req) {
    led_on();
    ESP_LOGI(TAG, "Http Request /save-config-wifi");

    char buf[1024]; // Adjust buffer size as needed
    int ret, remaining = req->content_len;
    int total_received = 0;

    while (remaining > 0) {
        ret = httpd_req_recv(req, buf + total_received, MIN(remaining, sizeof(buf) - total_received));
        if (ret <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                continue; // Retry receiving if timeout occurred
            }
            return ESP_FAIL;
        }
        total_received += ret;
        remaining -= ret;
    }
    buf[total_received] = '\0';

    ESP_LOGI(TAG, "Received data: %s", buf);

    char ssid[64], password[64], decodedSsid[64], decodedPassword[64];
    if (sscanf(buf, "ssid=%63[^&]&password=%63s", ssid, password) == 2) {
        url_decode(decodedSsid, ssid);
        ESP_LOGI(TAG, "Parsed SSID: %s", decodedSsid);
        url_decode(decodedPassword, password);
        ESP_LOGI(TAG, "Parsed Password: %s", decodedPassword);
        store_wifi_credentials(decodedSsid, decodedPassword);
    } else {
        ESP_LOGE(TAG, "Failed to parse POST data");
        httpd_resp_send_408(req);
        led_off();
        return ESP_FAIL;
    }

    char* html_content = build_html(resp_html, "{{SSID}}", decodedSsid);
    if (html_content == NULL) {
        ESP_LOGE(TAG, "Failed to replace template in HTML");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_send(req, html_content, strlen(html_content));
    led_off();
    xTaskCreate(restart_task, "restart_task", 2048, NULL, 5, NULL);
    return ESP_OK;
}
