#include "esp_ghota_client.h"
#include "esp_ghota_config.h"
#include "sdkconfig.h"
#include "interface/ghota_interface.h"
#include "esp_partition.h"

typedef struct ghota_client_handle_t
{
    ghota_config_t config;
    char *username;
    char *token;
    struct
    {
        char tag_name[CONFIG_MAX_FILENAME_LEN];
        char name[CONFIG_MAX_FILENAME_LEN];
        char url[CONFIG_MAX_URL_LEN];
        char storageurl[CONFIG_MAX_URL_LEN];
        uint8_t flags;
    } result;
    struct
    {
        char name[CONFIG_MAX_FILENAME_LEN];
        char url[CONFIG_MAX_URL_LEN];
    } scratch;
    semver_t current_version;
    semver_t latest_version;
    uint32_t countdown;
    TaskHandle_t task_handle;
    const esp_partition_t *storage_partition;
    ghota_interface_t *interface;
} ghota_client_handle_t;

char *ghota_client_get_username(
    ghota_client_handle_t *handle)
{
    return handle->username;
}

char *ghota_client_get_token(
    ghota_client_handle_t *handle)
{
    return handle->token;
}

void ghota_client_set_result_flag(
    ghota_client_handle_t *handle,
    uint8_t flag)
{
    handle->result.flags |= flag;
}

void ghota_client_set_result_flags(
    ghota_client_handle_t *handle,
    uint8_t value)
{
    handle->result.flags = value;
}

uint8_t ghota_client_get_result_flag(
    ghota_client_handle_t *handle,
    uint8_t flag)
{
    return handle->result.flags & flag;
}

void ghota_client_clear_result_flag(
    ghota_client_handle_t *handle,
    uint8_t flag)
{
    handle->result.flags &= ~flag;
}

size_t ghota_client_get_handle_size()
{
    return sizeof(ghota_client_handle_t);
}

void ghota_client_set_config(
    ghota_client_handle_t *handle,
    ghota_config_t *config)
{
    strncpy(
        handle->config.filenamematch,
        config->filenamematch,
        CONFIG_MAX_FILENAME_LEN);
    strncpy(
        handle->config.storagenamematch,
        config->storagenamematch,
        CONFIG_MAX_FILENAME_LEN);
    strncpy(
        handle->config.storagepartitionname,
        config->storagepartitionname,
        17);

    if (config->hostname == NULL)
        asprintf(
            &handle->config.hostname,
            CONFIG_GITHUB_HOSTNAME);
    else
        asprintf(
            &handle->config.hostname,
            config->hostname);

    if (config->orgname == NULL)
        asprintf(
            &handle->config.orgname,
            CONFIG_GITHUB_OWNER);
    else
        asprintf(
            &handle->config.orgname,
            config->orgname);

    if (config->reponame == NULL)
        asprintf(
            &handle->config.reponame,
            CONFIG_GITHUB_REPO);
    else
        asprintf(
            &handle->config.reponame,
            config->reponame);

    handle->config.updateInterval =
        config->updateInterval;
}

semver_t ghota_client_get_current_version(
    ghota_client_handle_t *handle)
{
    return handle->current_version;
}

void ghota_client_set_current_version(
    ghota_client_handle_t *handle,
    semver_t curr_ver)
{
    handle->current_version = curr_ver;
}

void ghota_client_set_task_handle(
    ghota_client_handle_t *client_handle,
    TaskHandle_t task_handle)
{
    client_handle->task_handle = task_handle;
}

ghota_config_t ghota_client_get_config(
    ghota_client_handle_t *handle)
{
    return handle->config;
}

semver_t ghota_client_get_latest_version(
    ghota_client_handle_t *handle)
{
    return handle->latest_version;
}
