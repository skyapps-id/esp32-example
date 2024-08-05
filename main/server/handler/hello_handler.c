#include "hello_handler.h"
#include "esp_log.h"
#include "esp_http_server.h"

static const char *TAG = "hello_handler";

esp_err_t hello_get_handler(httpd_req_t *req) {
    const char resp[] = "Hello, World!";
    ESP_LOGI(TAG, "Http Request /hello");
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}