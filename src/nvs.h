/****************************************************************************
 * **File:** nvs.h
 *
 * **Description:** Non-Volatile Storage (NVS) interface.
 *
 * This header defines the abstract NVS API used to store and retrieve WiFi
 * credentials.
 ***************************************************************************/
#ifndef NVS_H
#define NVS_H

/******************************* INCLUDE FILES *******************************/
#include <stdbool.h>
#include <stddef.h>

/******************************* TYPEDEFS ***********************************/

/**
 * Initialize NVS storage.
 *
 * Function pointer type used to initialize the underlying NVS/flash
 * implementation. Should return true on success, false on failure.
 */
typedef bool (*nvs_init_t)(void);

/**
 * Store credentials function type.
 *
 * @param ssid Pointer to the SSID buffer (NUL-terminated or length provided).
 * @param ssid_len Length in bytes of the SSID buffer (including NUL if used).
 * @param pass Pointer to the password buffer.
 * @param pass_len Length in bytes of the password buffer (including NUL if used).
 *
 * @return true on success, false on failure.
 */
typedef bool (*nvs_store_credentials_t)(const char *ssid, size_t ssid_len,
    const char *pass, size_t pass_len);

/**
 * Read stored credentials function type.
 *
 * Caller provides buffers for SSID and password. Implementations should copy
 * the stored values (including terminating NUL) into the provided buffers and
 * return true on success.
 *
 * @param[out] ssid Buffer to receive SSID.
 * @param ssid_len Size of the SSID buffer.
 * @param[out] pass Buffer to receive password.
 * @param pass_len Size of the password buffer.
 * @return true if credentials were read successfully, false otherwise.
 */
typedef bool (*nvs_read_credentials_t)(char *ssid, size_t ssid_len,
    char *pass, size_t pass_len);

/**
 * Erase stored credentials function type.
 *
 * Removes any stored SSID/password from NVS. Return true on success.
 */
typedef bool (*nvs_erase_credentials_t)(void);

/**
 * Abstract NVS interface.
 *
 * Holds function pointers for NVS operations. Call nvs_get() to obtain a
 * pointer to a populated instance for the platform-specific implementation.
 */
typedef struct _nvs_t {
    /** Initialize the NVS subsystem. */
    nvs_init_t init;

    /** Store credentials into NVS. */
    nvs_store_credentials_t store_credentials;

    /** Read credentials from NVS. */
    nvs_read_credentials_t read_credentials;

    /** Erase stored credentials from NVS. */
    nvs_erase_credentials_t erase_credentials;
} nvs_t;

/***************************** INTERFACE FUNCTIONS ****************************/

/**
 * Retrieve the NVS interface instance.
 *
 * The returned pointer references a structure with platform-specific
 * implementations populated. Do not free the returned pointer.
 *
 * @return Pointer to a `nvs_t` instance.
 */
nvs_t *nvs_get(void);

#endif /* NVS_H */
