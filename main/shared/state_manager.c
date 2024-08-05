#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "state_manager.h"

static const char *TAG = "state-manager";

#define NVS_NAMESPACE "storage"
#define STATE_KEY "state"

void save_state(state_t state) {
    nvs_handle my_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err == ESP_OK) {
        // Convert boolean to int (true -> 1, false -> 0)
        int32_t state_int = state ? 1 : 0;
        err = nvs_set_i32(my_handle, STATE_KEY, state_int);
        if (err == ESP_OK) {
            nvs_commit(my_handle);
        }
        nvs_close(my_handle);
    } else {
        // Handle error
        printf("Failed to open NVS handle: %s\n", esp_err_to_name(err));
    }
}

state_t load_state(void) {
    nvs_handle my_handle;
    state_t state = false;  // Default to false
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &my_handle);
    if (err == ESP_OK) {
        int32_t stored_state;
        err = nvs_get_i32(my_handle, STATE_KEY, &stored_state);
        if (err == ESP_OK) {
            // Convert int to boolean (1 -> true, 0 -> false)
            state = stored_state != 0;
        }
        nvs_close(my_handle);
    } else {
        // Handle error
        printf("Failed to open NVS handle: %s\n", esp_err_to_name(err));
    }
    return state;
}

void delete_state(void) {
    nvs_handle my_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err == ESP_OK) {
        err = nvs_erase_key(my_handle, STATE_KEY);
        if (err == ESP_OK) {
            nvs_commit(my_handle);
        }
        nvs_close(my_handle);
    } else {
        printf("Failed to open NVS handle: %s\n", esp_err_to_name(err));
    }
}

void store_wifi_credentials(const char *ssid, const char *pass) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    } else {
        err = nvs_set_str(nvs_handle, "wifi_ssid", ssid);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) setting SSID!", esp_err_to_name(err));
        }
        err = nvs_set_str(nvs_handle, "wifi_pass", pass);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) setting password!", esp_err_to_name(err));
        }
        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) committing to NVS!", esp_err_to_name(err));
        }
        nvs_close(nvs_handle);
    }
}

esp_err_t get_wifi_credentials(char *ssid, size_t ssid_len, char *pass, size_t pass_len) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return err;
    }
    err = nvs_get_str(nvs_handle, "wifi_ssid", ssid, &ssid_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) getting SSID!", esp_err_to_name(err));
    }
    err = nvs_get_str(nvs_handle, "wifi_pass", pass, &pass_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) getting password!", esp_err_to_name(err));
    }
    nvs_close(nvs_handle);
    return err;
}