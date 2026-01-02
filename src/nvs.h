/****************************************************************************
 * **File:** nvs.h
 *
 * **Description:** Non-Volatile Storage (NVS) interface.
 *
 */

#ifndef NVS_H
#define NVS_H

/******************************** INCLUDE FILES *******************************/
#include <stdbool.h>

/********************************** DEFINES ***********************************/

/********************************* TYPEDEFS ***********************************/
typedef bool (*nvs_init_t)(void);

typedef bool (*nvs_store_credentials_t)(const char *ssid, size_t ssid_len,
    const char *pass, size_t pass_len);
typedef bool (*nvs_read_credentials_t)(char *ssid, size_t ssid_len,
    char *pass, size_t pass_len);
typedef bool (*nvs_erase_credentials_t)(void);
        
/**
 * @struct nvs_t
 * NVS interface containing function pointers for NVS operations.
 *
 * This struct abstracts NVS operations with function pointers to initialize.
 */
typedef struct _nvs_t {
    nvs_init_t init;            /**< Initialize NVS */

    nvs_store_credentials_t store_credentials;  /**< Store WiFi credentials */
    nvs_read_credentials_t read_credentials;    /**< Read WiFi credentials */
    nvs_erase_credentials_t erase_credentials;  /**< Erase WiFi credentials */
} nvs_t;

/***************************** INTERFACE FUNCTIONS ****************************/

/**
 * Get the NVS interface instance.
 *
 * Returns a pointer to the NVS interface struct containing NVS operation functions.
 *
 * @return Pointer to a `nvs_t` instance.
 */
nvs_t *nvs_get(void);

#endif /* WIFI_H */
