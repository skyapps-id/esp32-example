#include "helper.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_system.h"
#include <string.h> 
#include "esp_log.h"
#include "ctype.h"

static const char *TAG = "helper";

char* build_html(const char* template_html, const char* placeholder, const char* replacement) {
    size_t template_len = strlen(template_html);
    size_t placeholder_len = strlen(placeholder);
    size_t replacement_len = strlen(replacement);
    size_t new_len = template_len - placeholder_len + replacement_len;

    char* new_html = (char*)malloc(new_len + 1);
    if (new_html == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for new HTML");
        return NULL;
    }

    const char* pos = strstr(template_html, placeholder);
    if (pos == NULL) {
        free(new_html);
        return NULL;
    }

    size_t prefix_len = pos - template_html;
    memcpy(new_html, template_html, prefix_len);

    memcpy(new_html + prefix_len, replacement, replacement_len);

    const char* suffix = pos + placeholder_len;
    strcpy(new_html + prefix_len + replacement_len, suffix);

    return new_html;
}

// Function to convert a single hex digit character to its integer value
int hex_digit_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

// Function to unescape a URL-encoded string
void url_decode(char *dst, const char *src) {
    char a, b;
    while (*src) {
        if ((*src == '%') &&
            ((a = src[1]) && (b = src[2])) &&
            (isxdigit(a) && isxdigit(b))) {
            *dst++ = (char)(hex_digit_to_int(a) * 16 + hex_digit_to_int(b));
            src += 3;
        } else if (*src == '+') {
            *dst++ = ' '; // Convert '+' to space
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

void restart_task(void *pvParameter) {
    vTaskDelay(pdMS_TO_TICKS(500));
    esp_restart();
    vTaskDelete(NULL);
}