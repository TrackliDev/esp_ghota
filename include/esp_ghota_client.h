#ifndef GITHUB_OTA_CLIENT_H
#define GITHUB_OTA_CLIENT_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
        ghota_client_handle_t
            ghota_client_handle_t;

    char *get_ghota_client_username(
        ghota_client_handle_t *handle);
    char *get_ghota_client_token(
        ghota_client_handle_t *handle);

#ifdef __cplusplus
}
#endif

#endif // GITHUB_OTA_CLIENT_H
