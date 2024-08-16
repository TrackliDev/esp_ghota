#ifndef GITHUB_OTA_CONFIG_H
#define GITHUB_OTA_CONFIG_H

#include <stdint.h>
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Github OTA Configuration
     */
    typedef struct ghota_config_t
    {
        char filenamematch[CONFIG_MAX_FILENAME_LEN];    /*!< Filename to match against on Github indicating this is a firmware file */
        char storagenamematch[CONFIG_MAX_FILENAME_LEN]; /*!< Filename to match against on Github indicating this is a storage file */
        char storagepartitionname[17];                  /*!< Name of the storage partition to update */
        char *hostname;                                 /*!< Hostname of the Github server. Defaults to api.github.com*/
        char *orgname;                                  /*!< Name of the Github organization */
        char *reponame;                                 /*!< Name of the Github repository */
        uint32_t updateInterval;                        /*!< Interval in Minutes to check for updates if using the ghota_start_update_timer function */
    } ghota_config_t;

#ifdef __cplusplus
}
#endif

#endif // GITHUB_OTA_CONFIG_H