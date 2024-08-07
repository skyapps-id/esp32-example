#ifndef OTA_POST_HANDLER_H
#define OTA_POST_HANDLER_H

#include "esp_http_server.h"

esp_err_t ota_post_handler(httpd_req_t *req);

#endif // OTA_POST_HANDLER_H
