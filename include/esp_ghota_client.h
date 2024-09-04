#ifndef GITHUB_OTA_CLIENT_H
#define GITHUB_OTA_CLIENT_H

#include "semver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_partition.h"
#include "esp_ghota_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
        ghota_client_handle
            ghota_client_handle_t;

    char *ghota_client_get_username(
        ghota_client_handle_t *handle);

    int ghota_client_set_username(
        ghota_client_handle_t *handle,
        const char *username);

    char *ghota_client_get_token(
        ghota_client_handle_t *handle);

    int ghota_client_set_token(
        ghota_client_handle_t *handle,
        const char *token);

    void ghota_client_set_result_flag(
        ghota_client_handle_t *handle,
        uint8_t flag);

    void ghota_client_set_result_flags(
        ghota_client_handle_t *handle,
        uint8_t value);

    uint8_t ghota_client_get_result_flag(
        ghota_client_handle_t *handle,
        uint8_t flag);

    void ghota_client_clear_result_flag(
        ghota_client_handle_t *handle,
        uint8_t flag);

    char *ghota_client_get_result_tag_name(
        ghota_client_handle_t *handle);

    void ghota_client_set_result_tag_name(
        ghota_client_handle_t *handle,
        char *name);

    char *ghota_client_get_result_name(
        ghota_client_handle_t *handle);

    void ghota_client_set_result_name(
        ghota_client_handle_t *handle,
        char *name);

    char *ghota_client_get_result_url(
        ghota_client_handle_t *handle);

    void ghota_client_set_result_url(
        ghota_client_handle_t *handle,
        char *url);

    char *ghota_client_get_result_storage_url(
        ghota_client_handle_t *handle);

    void ghota_client_set_result_storage_url(
        ghota_client_handle_t *handle,
        char *url);

    size_t ghota_client_get_handle_size();

    ghota_config_t *ghota_client_get_config(
        ghota_client_handle_t *handle);

    void ghota_client_set_config(
        ghota_client_handle_t *handle,
        ghota_config_t *config);

    semver_t *ghota_client_get_current_version(
        ghota_client_handle_t *handle);

    void ghota_client_set_current_version(
        ghota_client_handle_t *handle,
        semver_t curr_ver);
        
    TaskHandle_t ghota_client_get_task_handle(
        ghota_client_handle_t *client_handle);

    void ghota_client_set_task_handle(
        ghota_client_handle_t *client_handle,
        TaskHandle_t task_handle);

    semver_t *ghota_client_get_latest_version(
        ghota_client_handle_t *handle);

    void ghota_client_set_latest_version(
        ghota_client_handle_t *handle,
        semver_t version);

    char *ghota_client_get_scratch_name(
        ghota_client_handle_t *handle);

    void ghota_client_set_scratch_name(
        ghota_client_handle_t *handle,
        char *name);

    char *ghota_client_get_scratch_url(
        ghota_client_handle_t *handle);

    void ghota_client_set_scratch_url(
        ghota_client_handle_t *handle,
        char *url);

    const esp_partition_t *ghota_client_get_storage_partition(
        ghota_client_handle_t *handle);

    const esp_partition_t *ghota_client_get_storage_partition(
        ghota_client_handle_t *handle);

    void ghota_client_set_partition(
        ghota_client_handle_t *handle,
        const esp_partition_t *storage_partition);

    uint32_t ghota_client_get_countdown(
        ghota_client_handle_t *handle);

    uint32_t ghota_client_set_countdown(
        ghota_client_handle_t *handle,
        uint32_t countdown);

#ifdef __cplusplus
}
#endif

#endif // GITHUB_OTA_CLIENT_H
