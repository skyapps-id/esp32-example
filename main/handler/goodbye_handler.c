#include "goodbye_handler.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "blink.h"

static const char *TAG = "goodbye_handler";

esp_err_t goodbye_get_handler(httpd_req_t *req) {
    led_on();
    const char resp[] = "Goodbye, World!";
    ESP_LOGI(TAG, "Http Request /goodbye");
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    led_off();
    return ESP_OK;
}
