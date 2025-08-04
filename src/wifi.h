/****************************************************************************
 * **File:** wifi.h
 *
 * **Description:** WiFi interface abstraction.
 *
 * Defines function pointer types for WiFi operations and the WiFi interface
 * struct containing those function pointers. Provides an interface function to
 * get the WiFi interface instance.
 */

#ifndef WIFI_H
#define WIFI_H

/******************************** INCLUDE FILES *******************************/
#include <stdbool.h>

/********************************** DEFINES ***********************************/

/********************************* TYPEDEFS ***********************************/

/**
 * Function pointer type for connecting to a WiFi network.
 *
 * Attempts to connect to a WiFi network given an SSID and a password.
 *
 * @param[in] ssid Pointer to a null-terminated string containing the SSID.
 * @param[in] password Pointer to a null-terminated string containing the password.
 *                        The password may be modified during the connection attempt.
 *
 * @return true if the connection was successful, false otherwise.
 */
typedef bool (*wifi_connect_t)(const char *ssid, char *password);

/**
 * Function pointer type for disconnecting from the WiFi network.
 */
typedef void (*wifi_disconnect_t)(void);

/**
 * Function pointer type for reporting the WiFi status.
 */
typedef void (*wifi_status_t)(void);

/**
 * @struct wifi_iface_t
 * WiFi interface containing function pointers for WiFi operations.
 *
 * This struct abstracts WiFi operations with function pointers to connect,
 * disconnect, and report status.
 */
typedef struct _wifi_iface_t {
    wifi_connect_t connect;        /**< Connect to WiFi network */
    wifi_disconnect_t disconnect;  /**< Disconnect from WiFi network */
    wifi_status_t status;          /**< Report WiFi status */
} wifi_iface_t;

/***************************** INTERFACE FUNCTIONS ****************************/

/**
 * Get the WiFi interface instance.
 *
 * Returns a pointer to the WiFi interface struct containing WiFi operation functions.
 *
 * @return Pointer to a `wifi_iface_t` instance.
 */
wifi_iface_t *wifi_get(void);

#endif /* WIFI_H */
