#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "shared/state_manager.h"

#define BUTTON_GPIO GPIO_NUM_0
#define DEBOUNCE_DELAY_MS 50
#define TRIPLE_PRESS_TIME_MS 1000
#define PRESS_COUNT_THRESHOLD 3

void setup_button_boot(void) {
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << BUTTON_GPIO);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
}

void button_boot_task(void *arg) {
    static int press_count = 0;
    static TickType_t last_press_time = 0;
    static bool button_was_pressed = false;

    while (1) {
        int button_state = gpio_get_level(BUTTON_GPIO);

        TickType_t now = xTaskGetTickCount();
        bool button_is_pressed = (button_state == 0);  // Assuming active-low button

        if (button_is_pressed) {
            if (!button_was_pressed) {  // Detect rising edge
                // Button was just pressed
                if (now - last_press_time < TRIPLE_PRESS_TIME_MS / portTICK_PERIOD_MS) {
                    press_count++;
                } else {
                    press_count = 1;  // Reset press count if time exceeded
                }
                last_press_time = now;
                button_was_pressed = true;  // Update state to pressed
            }
        } else {
            if (button_was_pressed) {  // Detect falling edge
                button_was_pressed = false;  // Update state to not pressed
            }
        }

        if (press_count >= PRESS_COUNT_THRESHOLD) {
            state_t current_state = load_state();
            printf("Current state changed to: %d\n", current_state);
            state_t new_state = !current_state;
            save_state(new_state);
            printf("State changed to: %d\n", new_state);
            press_count = 0;  // Reset press count after state change
            esp_restart();
        }

        vTaskDelay(DEBOUNCE_DELAY_MS / portTICK_PERIOD_MS);
    }
}