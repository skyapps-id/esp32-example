#include "http_server.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "router.h"

static const char *TAG = "http_server";

esp_err_t start_http_server(httpd_handle_t *server) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    
    ESP_LOGI(TAG, "Starting HTTP Server");
    if (httpd_start(server, &config) == ESP_OK) {
        register_routes(*server);
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return ESP_FAIL;
}

void stop_http_server(httpd_handle_t server) {
    if (server) {
        httpd_stop(server);
    }
}
