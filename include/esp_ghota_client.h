#ifndef GITHUB_OTA_CLIENT_H
#define GITHUB_OTA_CLIENT_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
        ghota_client_handle
            ghota_client_handle_t;

    char *ghota_client_get_username(
        ghota_client_handle_t *handle);

    char *ghota_client_get_token(
        ghota_client_handle_t *handle);

    void ghota_client_set_result_flag(
        ghota_client_handle_t *handle,
        uint8_t flag);

    uint8_t ghota_client_get_result_flag(
        ghota_client_handle_t *handle,
        uint8_t flag);

    void ghota_client_clear_result_flag(
        ghota_client_handle_t *handle,
        uint8_t flag);

    size_t ghota_client_get_handle_size();

    void ghota_client_set_config(
        ghota_client_handle_t *handle,
        ghota_config_t *config)

#ifdef __cplusplus
}
#endif

#endif // GITHUB_OTA_CLIENT_H
