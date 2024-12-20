#include <esp_http_client.h>
#include <esp_tls.h>
#include <esp_crt_bundle.h>
#include <esp_https_ota.h>
#include <esp_log.h>
#include <esp_ota_ops.h>

#include "interface/ghota_wifi_interface.h"
#include "esp_ghota_client.h"
#include "esp_ghota_event.h"

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#define PRICONTENT_LENGTH PRId64
#else
#define PRICONTENT_LENGTH PRId32
#endif

static char *WIFI_INTERFACE_TAG =
    "Ghota Wi-Fi Interface";

static esp_err_t _http_event_handler(
    esp_http_client_event_t *evt)
{
    lwjsonr_t res;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_HEADER:
        if (strncasecmp(
                evt->header_key,
                "x-ratelimit-remaining",
                strlen("x-ratelimit-remaining")) == 0)
        {
            int limit = atoi(evt->header_value);
            ESP_LOGD(
                WIFI_INTERFACE_TAG,
                "Github API Rate Limit Remaining: %d",
                limit);
            if (limit < 10)
            {
                ESP_LOGW(
                    WIFI_INTERFACE_TAG,
                    "Github API Rate Limit Remaining is low: %d",
                    limit);
            }
        }
        break;
    case HTTP_EVENT_ON_DATA:
        if (!esp_http_client_is_chunked_response(evt->client))
        {
            char *buf = evt->data;
            for (int i = 0; i < evt->data_len; i++)
            {
                res = lwjson_stream_parse(
                    (lwjson_stream_parser_t *)evt->user_data,
                    *buf);
                if (!(res == lwjsonOK ||
                      res == lwjsonSTREAMDONE ||
                      res == lwjsonSTREAMINPROG))
                {
                    ESP_LOGE(
                        WIFI_INTERFACE_TAG,
                        "Lwjson Error: %d",
                        res);
                }
                buf++;
            }
        }
        break;
    case HTTP_EVENT_DISCONNECTED:
    {
        int mbedtls_err = 0;
        esp_err_t err = esp_tls_get_and_clear_last_error(
            evt->data,
            &mbedtls_err,
            NULL);
        if (err != 0)
        {
            ESP_LOGE(
                WIFI_INTERFACE_TAG,
                "Last esp error code: 0x%x",
                err);
            ESP_LOGE(
                WIFI_INTERFACE_TAG,
                "Last mbedtls failure: 0x%x",
                mbedtls_err);
        }
        break;
    }
    }
#pragma GCC diagnostic pop
    return ESP_OK;
}

static esp_err_t wifi_get_release_info(
    ghota_client_handle_t *handle,
    char *url,
    lwjson_stream_parser_t *parser)
{
    esp_http_client_config_t httpconfig = {
        .url = url,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .event_handler = _http_event_handler,
        .user_data = parser,
    };
    char *username =
        ghota_client_get_username(handle);

    if (username)
    {
        ESP_LOGD(
            WIFI_INTERFACE_TAG,
            "Using Authenticated Request to %s",
            url);
        httpconfig.username = username;
        httpconfig.password =
            ghota_client_get_token(handle);
        httpconfig.auth_type = HTTP_AUTH_TYPE_BASIC;
    }
    ESP_LOGI(
        WIFI_INTERFACE_TAG,
        "Searching for Firmware from %s",
        url);

    esp_http_client_handle_t client =
        esp_http_client_init(&httpconfig);

    esp_err_t err = esp_http_client_perform(client);

    int status_code =
        esp_http_client_get_status_code(client);

    if (err == ESP_OK)
    {
        ESP_LOGD(
            WIFI_INTERFACE_TAG,
            "HTTP GET Status = %d, "
            "content_length = %" PRICONTENT_LENGTH,
            status_code,
            esp_http_client_get_content_length(client));
    }
    else
    {
        ESP_LOGE(
            WIFI_INTERFACE_TAG,
            "HTTP GET request failed: %s",
            esp_err_to_name(err));
    }

    if (status_code != 200)
        err = ESP_FAIL;

    if (esp_http_client_cleanup(client) != ESP_OK)
        ESP_LOGE(
            WIFI_INTERFACE_TAG,
            "HTTP client cleanup failed");

    return err;
}

static esp_err_t http_client_set_header_cb(
    esp_http_client_handle_t http_client)
{
    return esp_http_client_set_header(
        http_client,
        "Accept",
        "application/octet-stream");
}

static esp_err_t validate_image_header(
    esp_app_desc_t *new_app_info)
{
    if (new_app_info == NULL)
        return ESP_ERR_INVALID_ARG;

    ESP_LOGI(
        WIFI_INTERFACE_TAG,
        "New Firmware Details:");
    ESP_LOGI(
        WIFI_INTERFACE_TAG,
        "Project name: %s",
        new_app_info->project_name);
    ESP_LOGI(
        WIFI_INTERFACE_TAG,
        "Firmware version: %s",
        new_app_info->version);
    ESP_LOGI(
        WIFI_INTERFACE_TAG,
        "Compiled time: %s %s",
        new_app_info->date,
        new_app_info->time);
    ESP_LOGI(
        WIFI_INTERFACE_TAG,
        "ESP-IDF: %s",
        new_app_info->idf_ver);
    ESP_LOGI(
        WIFI_INTERFACE_TAG,
        "SHA256:");
    ESP_LOG_BUFFER_HEX(
        WIFI_INTERFACE_TAG,
        new_app_info->app_elf_sha256,
        sizeof(new_app_info->app_elf_sha256));

    const esp_partition_t *running =
        esp_ota_get_running_partition();
    ESP_LOGD(
        WIFI_INTERFACE_TAG,
        "Current partition %s type %d "
        "subtype %d (offset 0x%08" PRIx32 ")",
        running->label,
        running->type,
        running->subtype,
        running->address);
    const esp_partition_t *update =
        esp_ota_get_next_update_partition(NULL);
    ESP_LOGD(
        WIFI_INTERFACE_TAG,
        "Update partition %s type %d "
        "subtype %d (offset 0x%08" PRIx32 ")",
        update->label,
        update->type,
        update->subtype,
        update->address);

#ifdef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
    /**
     * Secure version check from firmware image header prevents subsequent download and flash write of
     * entire firmware image. However this is optional because it is also taken care in API
     * esp_https_ota_finish at the end of OTA update procedure.
     */
    const uint32_t hw_sec_version =
        esp_efuse_read_secure_version();
    if (new_app_info->secure_version < hw_sec_version)
    {
        ESP_LOGW(
            TAG,
            "New firmware security version is less than eFuse programmed, %d < %d",
            new_app_info->secure_version,
            hw_sec_version);
        return ESP_FAIL;
    }
#endif

    return ESP_OK;
}

static esp_err_t wifi_install_firmware(
    ghota_client_handle_t *handle)
{
    esp_http_client_config_t httpconfig = {
        .url = ghota_client_get_result_url(handle),
        .crt_bundle_attach = esp_crt_bundle_attach,
        .keep_alive_enable = true,
        .buffer_size_tx = 4096};

    char *username = ghota_client_get_username(handle);
    if (username)
    {
        ESP_LOGD(
            WIFI_INTERFACE_TAG,
            "Using Authenticated Request to %s",
            httpconfig.url);
        httpconfig.username = username;
        httpconfig.password =
            ghota_client_get_token(handle);
        httpconfig.auth_type = HTTP_AUTH_TYPE_BASIC;
    }

    esp_https_ota_config_t ota_config = {
        .http_config = &httpconfig,
        .http_client_init_cb = http_client_set_header_cb,
    };

    esp_https_ota_handle_t https_ota_handle = NULL;
    esp_err_t err = esp_https_ota_begin(
        &ota_config,
        &https_ota_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(
            WIFI_INTERFACE_TAG,
            "ESP HTTPS OTA Begin failed: %s",
            esp_err_to_name(err));

        esp_https_ota_abort(https_ota_handle);
        return err;
    }

    esp_app_desc_t app_desc;
    err = esp_https_ota_get_img_desc(
        https_ota_handle,
        &app_desc);
    if (err != ESP_OK)
    {
        ESP_LOGE(
            WIFI_INTERFACE_TAG,
            "esp_https_ota_read_img_desc failed: %d",
            err);

        esp_https_ota_abort(https_ota_handle);
        return err;
    }

    err = validate_image_header(&app_desc);
    if (err != ESP_OK)
    {
        ESP_LOGE(
            WIFI_INTERFACE_TAG,
            "image header verification failed: %s",
            esp_err_to_name(err));

        esp_https_ota_abort(https_ota_handle);
        return err;
    }

    int last_progress = -1;
    while (1)
    {
        err = esp_https_ota_perform(https_ota_handle);
        if (err != ESP_ERR_HTTPS_OTA_IN_PROGRESS)
        {
            break;
        }
        int32_t dl = esp_https_ota_get_image_len_read(
            https_ota_handle);
        int32_t size = esp_https_ota_get_image_size(
            https_ota_handle);
        int progress = 100 * ((float)dl / (float)size);
        if ((progress % 5 == 0) &&
            (progress != last_progress))
        {
            err = esp_event_post(
                GHOTA_EVENTS,
                GHOTA_EVENT_FIRMWARE_UPDATE_PROGRESS,
                &progress,
                sizeof(progress),
                portMAX_DELAY);
            if (err != ESP_OK)
            {
                ESP_LOGE(
                    WIFI_INTERFACE_TAG,
                    "event %s post failed: %s",
                    ghota_get_event_str(
                        GHOTA_EVENT_FIRMWARE_UPDATE_PROGRESS),
                    esp_err_to_name(err));

                esp_https_ota_abort(https_ota_handle);
                return err;
            }
            ESP_LOGV(
                WIFI_INTERFACE_TAG,
                "Firmware Update Progress: %d%%",
                progress);
            last_progress = progress;
        }
    }

    if (esp_https_ota_is_complete_data_received(
            https_ota_handle) != true)
    {
        // the OTA image was not completely received and
        // user can customise the response to this situation.
        ESP_LOGE(
            WIFI_INTERFACE_TAG,
            "Complete data was not received.");

        esp_https_ota_abort(https_ota_handle);
        return ESP_FAIL;
    }

    err = esp_https_ota_finish(
        https_ota_handle);
    if (err ==
        ESP_ERR_OTA_VALIDATE_FAILED)
    {
        ESP_LOGE(
            WIFI_INTERFACE_TAG,
            "Image validation failed, image is corrupted");
    }

    return err;
}

static esp_err_t wifi_install_storage(
    ghota_client_handle_t *handle)
{
    return ESP_OK;
}

static ghota_interface_t ghota_wifi_interface = {
    .get_release_info = &wifi_get_release_info,
    .install_firmware = &wifi_install_firmware,
    .install_storage = &wifi_install_storage};

ghota_interface_t *get_ghota_wifi_interface()
{
    return &ghota_wifi_interface;
}
