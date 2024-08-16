#include "esp_ghota_client.h"
#include "esp_ghota_config.h"
#include "sdkconfig.h"
#include "semver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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
