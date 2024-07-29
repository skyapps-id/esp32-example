#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "esp_err.h"
#include "esp_http_server.h"

esp_err_t start_http_server(httpd_handle_t *server);
void stop_http_server(httpd_handle_t server);

#endif // HTTP_SERVER_H
