#include "router.h"
#include "hello_handler.h"
#include "goodbye_handler.h"

static const httpd_uri_t hello = {
    .uri       = "/hello",
    .method    = HTTP_GET,
    .handler   = hello_get_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t goodbye = {
    .uri       = "/goodbye",
    .method    = HTTP_GET,
    .handler   = goodbye_get_handler,
    .user_ctx  = NULL
};

void register_routes(httpd_handle_t server) {
    httpd_register_uri_handler(server, &hello);
    httpd_register_uri_handler(server, &goodbye);
}
