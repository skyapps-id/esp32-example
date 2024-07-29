#ifndef ROUTER_H
#define ROUTER_H

#include "esp_http_server.h"

void register_routes(httpd_handle_t server);

#endif // ROUTER_H
