/******************************** INCLUDE FILES *******************************/
#include <zephyr/net/net_if.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_event.h>
#include <zephyr/logging/log.h>

#include "wifi.h"
/******************************** LOCAL DEFINES *******************************/
LOG_MODULE_REGISTER(wifi, LOG_LEVEL_DBG);

/********************************* TYPEDEFS ***********************************/
typedef struct _wifi_ctx_t {
    /* Private */
    struct net_if *iface;
    struct wifi_connect_req_params params;
} wifi_ctx_t;

/********************************* LOCAL DATA *********************************/
static wifi_ctx_t wifi;
static wifi_iface_t wifi_iface;

/******************************* LOCAL FUNCTIONS ******************************/
static bool wifi_connect(const char *ssid, char *pass) {
    wifi.iface = net_if_get_wifi_sta();
    if (wifi.iface == NULL) {
        LOG_ERR("Failed to get WiFi interface");
        return false;
    }
    net_if_up(wifi.iface);

    memset(&wifi.params, 0x00, sizeof(struct wifi_connect_req_params));
    /* Connect to WIFI */
    wifi.params.ssid = ssid;
    wifi.params.psk = pass;
    wifi.params.ssid_length = strlen(ssid);
    wifi.params.psk_length = strlen(pass);
    wifi.params.channel = WIFI_CHANNEL_ANY;
    wifi.params.security = WIFI_SECURITY_TYPE_PSK;
    wifi.params.band = WIFI_FREQ_BAND_2_4_GHZ;
    wifi.params.mfp = WIFI_MFP_OPTIONAL;

    LOG_INF("Connecting to: %s", wifi.params.ssid);
    if (net_mgmt(NET_REQUEST_WIFI_CONNECT, wifi.iface,
        &wifi.params, sizeof(struct wifi_connect_req_params))) {
        LOG_ERR("WiFi Connection Request Failed");
        return false;
    }
    LOG_INF("Waiting for authorization...");
    k_sleep(K_SECONDS(15));

    return true;
}

static void wifi_disconnect(void) {
    if (net_mgmt(NET_REQUEST_WIFI_DISCONNECT, wifi.iface, NULL, 0))
    {
        LOG_ERR("WiFi Disconnection Request Failed");
    }

    net_if_down(wifi.iface);
}

static bool wifi_status(void) {
    struct wifi_iface_status status = {0};
    bool connected = false;

    net_mgmt(NET_REQUEST_WIFI_IFACE_STATUS, wifi.iface, &status,
        sizeof(struct wifi_iface_status));

    if (status.state == WIFI_STATE_COMPLETED) {
        // Module is connected to WiFi
        LOG_INF("Connected");
        LOG_INF("Wifi network state:");
        LOG_INF("\tSSID: %-32s", status.ssid);
        LOG_INF("\tBand: %s", wifi_band_txt(status.band));
        LOG_INF("\tChannel: %d", status.channel);
        LOG_INF("\tSecurity: %s", wifi_security_txt(status.security));
        LOG_INF("\tRSSI: %d", status.rssi);
        connected = true;
    } else {
        // Not connected
        LOG_INF("Not connected, state: %d", status.state);
    }

    return connected;
}

/***************************** INTERFACE FUNCTIONS ****************************/
wifi_iface_t *wifi_get(void) {
    wifi_iface.connect = wifi_connect;
    wifi_iface.disconnect = wifi_disconnect;
    wifi_iface.status = wifi_status;

    return &wifi_iface;
}
