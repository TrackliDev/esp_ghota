#include <esp_http_client.h>
#include <esp_tls.h>
#include <esp_crt_bundle.h>
#include <esp_https_ota.h>

#include "interface/ghota_wifi_interface.h"
#include "esp_ghota_client.h"

static char *WIFI_INTERFACE_TAG =
    "Ghota Wi-Fi Interface";

static esp_err_t wifi_get_release_info(
    ghota_client_handle_t *handle,
    char *url,
    lwjson_stream_parser_t *parser);

static ghota_interface_t wifi_ghota_interface = {
    .get_release_info = &wifi_get_release_info};

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
        if (!esp_http_client_is_chunked_response(
                evt->client))
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
        esp_err_t err = 
            esp_tls_get_and_clear_last_error(
            evt->data, &mbedtls_err, NULL);
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
        .user_data = &parser,
    };

    char *username = get_ghota_client_username(
        handle);
    if (username)
    {
        ESP_LOGD(
            WIFI_INTERFACE_TAG,
            "Using Authenticated Request to %s",
            url);
        httpconfig.username = username;
        httpconfig.password = get_ghota_client_token(
            handle);
        httpconfig.auth_type = HTTP_AUTH_TYPE_BASIC;
    }

    ESP_LOGI(
        WIFI_INTERFACE_TAG,
        "Searching for Firmware from %s",
        url);

    esp_http_client_handle_t client =
        esp_http_client_init(&httpconfig);

    esp_err_t err = esp_http_client_perform(client);
}

ghota_interface_t *get_wifi_ghota_interface()
{
    return &wifi_ghota_interface;
}
