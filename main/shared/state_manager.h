#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include <stdbool.h>

// Provisioning
typedef bool state_t;

void save_state(state_t state);
state_t load_state(void);
void delete_state(void);

// Wifi
void store_wifi_credentials(const char *ssid, const char *pass);
esp_err_t get_wifi_credentials(char *ssid, size_t ssid_len, char *pass, size_t pass_len);

#endif // STATE_MANAGER_H
