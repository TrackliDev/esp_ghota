#ifndef GITHUB_OTA_INTERFACE_H
#define GITHUB_OTA_INTERFACE_H

#include "esp_err.h"
#include "lwjson.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct ghota_interface
    {
        esp_err_t (*get_release_info)(
            ghota_client_handle_t *,
            char *,
            lwjson_stream_parser_t *);
    } ghota_interface_t;

#ifdef __cplusplus
}
#endif

#endif // GITHUB_OTA_INTERFACE_H
