#ifndef GITHUB_OTA_EVENT_H
#define GITHUB_OTA_EVENT_H

#include <esp_event.h>

#ifdef __cplusplus
extern "C"
{
#endif

    ESP_EVENT_DECLARE_BASE(GHOTA_EVENTS);

    /**
     * @brief Github OTA events
     * These events are posted to the event loop to track progress of the OTA process
     */
    typedef enum
    {
        GHOTA_EVENT_START_CHECK = 0x01,               /*!< Github OTA check started */
        GHOTA_EVENT_UPDATE_AVAILABLE = 0x02,          /*!< Github OTA update available */
        GHOTA_EVENT_NOUPDATE_AVAILABLE = 0x04,        /*!< Github OTA no update available */
        GHOTA_EVENT_START_UPDATE = 0x08,              /*!< Github OTA update started */
        GHOTA_EVENT_FINISH_UPDATE = 0x10,             /*!< Github OTA update finished */
        GHOTA_EVENT_UPDATE_FAILED = 0x20,             /*!< Github OTA update failed */
        GHOTA_EVENT_START_STORAGE_UPDATE = 0x40,      /*!< Github OTA storage update started. If the storage is mounted, you should unmount it when getting this call */
        GHOTA_EVENT_FINISH_STORAGE_UPDATE = 0x80,     /*!< Github OTA storage update finished. You can mount the new storage after getting this call if needed */
        GHOTA_EVENT_STORAGE_UPDATE_FAILED = 0x100,    /*!< Github OTA storage update failed */
        GHOTA_EVENT_FIRMWARE_UPDATE_PROGRESS = 0x200, /*!< Github OTA firmware update progress */
        GHOTA_EVENT_STORAGE_UPDATE_PROGRESS = 0x400,  /*!< Github OTA storage update progress */
        GHOTA_EVENT_PENDING_REBOOT = 0x800,           /*!< Github OTA pending reboot */
    } ghota_event_e;

    /**
     * @brief convience function to return a string representation of events emited by this library
     *
     * @param event the eventid passed to the event handler
     * @return char* a string representing the event
     */
    char *ghota_get_event_str(ghota_event_e event);

#ifdef __cplusplus
}
#endif

#endif // GITHUB_OTA_EVENT_H