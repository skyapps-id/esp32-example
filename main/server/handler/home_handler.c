#include "home_handler.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "state_manager.h"
#include "helper.h"

static const char *TAG = "home_handler";

const char* form_html = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>ESP32 Device</title><style>body {font-family: 'Open Sans', sans-serif;color: #333;margin: 0;padding: 0;}.container {width: 80%;margin: 0 auto;}.hero {background: linear-gradient(to right, #5D2AEE, #FF00B8);color: #fff;text-align: center;padding: 100px 0;}.hero p {font-size: 30px;margin: 10px 0 30px;}.offerings {padding: 20px 0;text-align: center;background-color: #fff;}.offerings h3 {font-size: 24px;margin-bottom: 25px;}.wifi-form {text-align: center;}.form-group {margin-bottom: 20px !important;text-align: left;width: 100%;max-width: 350px;margin: 0 auto;}.form-group label {display: block;font-weight: 600;margin-bottom: 10px;text-align: left;}.form-group input[type=\"text\"],.form-group input[type=\"password\"] {width: 100%;padding: 15px;border: 1px solid #ccc;border-radius: 5px;font-size: 16px;box-sizing: border-box;}.file-drop-area {width: 100%;border: 2px dashed #5D2AEE;border-radius: 10px;padding: 10px 0px;text-align: center;background-color: #f0f0f0;cursor: pointer;position: relative;overflow: hidden;}.file-drop-area:hover {background-color: #e6e6e6;}.file-drop-area.active {border-color: #FF00B8;background-color: #e0e0ff;}.file-drop-area p {font-size: 16px;color: #666;margin: 0;line-height: 1.5;}.file-drop-area input[type=\"file\"] {position: absolute;width: 100%;height: 100%;opacity: 0;cursor: pointer;}.cta-button {margin-top: 20px;background-color: #FFB400;color: #fff;padding: 15px 30px;border-radius: 25px;text-decoration: none;font-weight: 600;border: none;cursor: pointer;transition: background-color .3s ease;}.cta-button:hover {background-color: #E69A00;}.cta-button.loading {background-color: #cccccc;color: #666666;cursor: not-allowed;pointer-events: none;}.status {margin-top: 20px;font-size: 16px;color: #555;}.tab-container {display: flex;justify-content: center;margin-bottom: 20px;}.tab {padding: 10px 20px;cursor: pointer;border: 1px solid #ccc;border-bottom: none;background-color: #f9f9f9;}.tab.active {background-color: #fff;font-weight: bold;border-top: 2px solid #5D2AEE;}.tab-content {display: none;}.tab-content.active {display: block;}</style><script>document.addEventListener('DOMContentLoaded', function() {const BASE_URL = window.location.origin;const tabs = document.querySelectorAll('.tab');const contents = document.querySelectorAll('.tab-content');tabs.forEach((tab, index) => {tab.addEventListener('click', function() {tabs.forEach(t => t.classList.remove('active'));contents.forEach(c => c.classList.remove('active'));tab.classList.add('active');contents[index].classList.add('active');});});const dropArea = document.querySelector('.file-drop-area');const fileInput = document.querySelector('#firmwareFile');dropArea.addEventListener('click', () => fileInput.click());dropArea.addEventListener('dragover', (event) => {event.preventDefault();dropArea.classList.add('active');});dropArea.addEventListener('dragleave', () => {dropArea.classList.remove('active');});dropArea.addEventListener('drop', (event) => {event.preventDefault();dropArea.classList.remove('active');if (event.dataTransfer.files.length) {fileInput.files = event.dataTransfer.files;dropArea.querySelector('p').textContent = event.dataTransfer.files[0].name;}});fileInput.addEventListener('change', () => {if (fileInput.files.length) {dropArea.querySelector('p').textContent = fileInput.files[0].name;}});function setLoading(button, isLoading) {if (isLoading) {button.classList.add('loading');button.innerHTML = 'Processing';} else {button.classList.remove('loading');button.innerHTML = button.dataset.originalText;}}document.getElementById('wifiForm').addEventListener('submit', function(event) {event.preventDefault();const formData = new FormData(this);const formDataString = new URLSearchParams(formData).toString();const url = `${BASE_URL}/save-config-wifi`;const submitButton = this.querySelector('.cta-button');submitButton.dataset.originalText = submitButton.innerHTML;setLoading(submitButton, true);fetch(url, {method: 'POST',headers: {'Content-Type': 'application/x-www-form-urlencoded'},body: formDataString}).then(response => response.text()).then(data => {alert(data);setLoading(submitButton, false);setTimeout(function() {window.location.reload();}, 3000);}).catch(error => {console.error('Error:', error);alert('Error saving configuration');setLoading(submitButton, false);});});document.getElementById('otaForm').addEventListener('submit', function(event) {event.preventDefault();const formData = new FormData(this);const url = `${BASE_URL}/ota`;const submitButton = this.querySelector('.cta-button');submitButton.dataset.originalText = submitButton.innerHTML;setLoading(submitButton, true);fetch(url, {method: 'POST',body: formData}).then(response => response.text()).then(data => {alert(data);setLoading(submitButton, false);setTimeout(function() {window.location.reload();}, 3000);}).catch(error => {console.error('Error:', error);alert('Error uploading firmware');setLoading(submitButton, false);});});});</script></head><body><section class=\"hero\"><div class=\"container\"><p>ESP32 Device</p></div></section><section class=\"offerings\"><div class=\"container\"><h3>~ DEVICE INFORMATION ~</h3><p>Firmware Version: {{VERSION}}</p><p>Wi-Fi Mode: {{MODE}}</p><p>Wi-Fi Name: {{SSID}}</p></div></section><section class=\"offerings\"><div class=\"container\"><div class=\"tab-container\"><div class=\"tab active\">Wi-Fi Setting</div><div class=\"tab\">OTA Update</div></div><div class=\"tab-content active\"><div class=\"wifi-form\"><form id=\"wifiForm\" method=\"post\" enctype=\"application/x-www-form-urlencoded\"><div class=\"form-group\"><label for=\"ssid\">SSID</label><input type=\"text\" id=\"ssid\" name=\"ssid\" placeholder=\"Enter SSID\" required></div><div class=\"form-group\"><label for=\"password\">Password</label><input type=\"password\" id=\"password\" name=\"password\" placeholder=\"Enter Password\" required></div><button type=\"submit\" value=\"Submit\" class=\"cta-button\">Save</button></form></div></div><div class=\"tab-content\"><div class=\"wifi-form\"><form id=\"otaForm\" method=\"post\" enctype=\"multipart/form-data\"><div class=\"form-group\"><div class=\"file-drop-area\"><p>Click to select a file</p><input type=\"file\" id=\"firmwareFile\" name=\"firmware\"></div></div><button type=\"submit\" value=\"Submit\" class=\"cta-button\">Upload Firmware</button></form></div></div></div></section></body></html>";

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
    httpd_resp_send(req, html_content, strlen(html_content));
    return ESP_OK;
}

    