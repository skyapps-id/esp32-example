#include "router.h"
#include "server/handler/hello_handler.h"
#include "server/handler/save_config_wifi_handler.h"
#include "server/handler/index_handler.h"

static const httpd_uri_t home = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t hello = {
    .uri       = "/hello",
    .method    = HTTP_GET,
    .handler   = hello_get_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t saveConfigWifi = {
    .uri       = "/save-config-wifi",
    .method    = HTTP_POST,
    .handler   = save_config_wifi_handler,
    .user_ctx  = NULL
};

void register_routes(httpd_handle_t server) {
    httpd_register_uri_handler(server, &home);
    httpd_register_uri_handler(server, &hello);
    httpd_register_uri_handler(server, &saveConfigWifi);
}
