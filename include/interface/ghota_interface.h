#ifndef GITHUB_OTA_INTERFACE_H
#define GITHUB_OTA_INTERFACE_H

#include "esp_err.h"
#include "lwjson.h"
#include "esp_ghota_client.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct ghota_interface
    {
        esp_err_t (*get_release_info)(
            ghota_client_handle_t *,  // handle
            char *,                   // url
            lwjson_stream_parser_t *  // JSON stream parser
        );
        esp_err_t (*install_firmware)(
            ghota_client_handle_t *   // handle
        );
        esp_err_t (*install_storage)(
            ghota_client_handle_t *   // handle
        );
    } ghota_interface_t;

#ifdef __cplusplus
}
#endif

#endif // GITHUB_OTA_INTERFACE_H
