/******************************** INCLUDE FILES *******************************/
#include <stdbool.h>
/********************************** DEFINES ***********************************/

/********************************* TYPEDEFS ***********************************/
typedef bool (*wifi_connect_t) (const char *, char *);
typedef void (*wifi_disconnect_t) (void);
typedef void (*wifi_status_t) (void);

typedef struct _wifi_iface_t {
    wifi_connect_t connect;
    wifi_disconnect_t disconnect;
    wifi_status_t status;
} wifi_iface_t;

/***************************** INTERFACE FUNCTIONS ****************************/
wifi_iface_t *wifi_get(void);
