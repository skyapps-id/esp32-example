#include "hello_handler.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "state_manager.h"
#include "helper.h"

static const char *TAG = "index_handler";

const char* form_html = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>ESP32 Device</title><style>body{font-family:'Open Sans',sans-serif;color:#333;margin:0;padding:0}.container{width:80%;margin:0 auto}.hero{background:linear-gradient(to right,#5D2AEE,#FF00B8);color:#fff;text-align:center;padding:100px 0}.hero p{font-size:30px;margin:10px 0 30px}.offerings{padding:20px 0;text-align:center}.offerings h3{font-size:24px;margin-bottom:25px}.wifi-form{text-align:center}.form-group{margin-bottom:20px!important;text-align:left;width:100%;max-width:400px;margin:0 auto}.form-group label{display:block;font-weight:600;margin-bottom:10px;text-align:left}.form-group input{width:100%;padding:15px;border:1px solid #ccc;border-radius:5px;font-size:16px;box-sizing:border-box}.cta-button{margin-top:20px;background-color:#FFB400;color:#fff;padding:15px 30px;border-radius:25px;text-decoration:none;font-weight:600;border:none;cursor:pointer;transition:background-color .3s ease}.cta-button:hover{background-color:#E69A00}</style><script>document.addEventListener('DOMContentLoaded',function(){const BASE_URL=window.location.origin;document.querySelector('form').addEventListener('submit',function(event){event.preventDefault();const formData=new FormData(this);const formDataString=new URLSearchParams(formData).toString();const url=`${BASE_URL}/save-config-wifi`;fetch(url,{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:formDataString}).then(response=>response.text()).then(data=>{alert(data);setTimeout(function(){window.location.reload();},3000);}).catch(error=>{console.error('Error:',error);alert('Error saving configuration');});});});</script></head><body><section class=\"hero\"><div class=\"container\"><p>ESP32 Device</p></div></section><section class=\"offerings\"><div class=\"container\"><h3>Device Info</h3><p>Firmware Version : {{VERSION}}</p><p>Wi-Fi Mode : {{MODE}}</p><p>Wi-Fi Name : {{SSID}}</p></div></section><section class=\"offerings\"><div class=\"container\"><h3>Wi-Fi Configuration</h3><div class=\"wifi-form\"><form method=\"post\" enctype=\"application/x-www-form-urlencoded\"><div class=\"form-group\"><label for=\"ssid\">SSID</label><input type=\"text\" id=\"ssid\" name=\"ssid\" placeholder=\"Enter SSID\" required></div><div class=\"form-group\"><label for=\"password\">Password</label><input type=\"password\" id=\"password\" name=\"password\" placeholder=\"Enter Password\" required></div><button type=\"submit\" value=\"Submit\" class=\"cta-button\">Save</button></form></div></div></section></body></html>";

static const char *FIRMWARE_VERSION = "v1.2.3";

esp_err_t index_handler(httpd_req_t *req) {
    char ssid[32] = {0};
    char pass[64] = {0};
    get_wifi_credentials(ssid, sizeof(ssid), pass, sizeof(pass));

    const char *wifiMode = "Station";
    state_t provisioningMode = load_state();
    if (provisioningMode == true) {
        wifiMode = "Access Point";
        strcpy(ssid, "ESP32_AP");
    }

    char* html_content = build_html(form_html, "{{VERSION}}", FIRMWARE_VERSION);
    if (html_content == NULL) {
        ESP_LOGE(TAG, "Failed to replace template in HTML");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    html_content = build_html(html_content, "{{MODE}}", wifiMode);
    if (html_content == NULL) {
        ESP_LOGE(TAG, "Failed to replace template in HTML");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    html_content = build_html(html_content, "{{SSID}}", ssid);
    if (html_content == NULL) {
        ESP_LOGE(TAG, "Failed to replace template in HTML");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Http Request /index");
    httpd_resp_send(req, html_content, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

    