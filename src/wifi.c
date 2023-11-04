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
    struct net_mgmt_event_callback event_cb;
    struct wifi_connect_req_params params;
    struct k_sem connected;
} wifi_ctx_t;

/********************************* LOCAL DATA *********************************/
static wifi_ctx_t wifi;
static wifi_iface_t wifi_iface;

/******************************* LOCAL FUNCTIONS ******************************/

/* WIFI Event handler */
static void wifi_connect_event_handler(struct net_mgmt_event_callback *cb) {
    const struct wifi_status *info = (const struct wifi_status *)cb->info;

    if (info->status) {
        LOG_ERR("Connection request failed (%d)", info->status);
    } else {
        LOG_INF("Connected");
        k_sem_give(&wifi.connected);
    }
}

static void wifi_disconnect_event_handler(struct net_mgmt_event_callback *cb) {
    const struct wifi_status *info = (const struct wifi_status *)cb->info;

    if (info->status) {
        LOG_ERR("Disconnection request (%d)", info->status);
    } else {
        LOG_INF("Disconnected");
        k_sem_take(&wifi.connected, K_NO_WAIT);
    }
}

static void wifi_mgmt_event_handler(struct net_mgmt_event_callback *cb,
    uint32_t wifi_mgmt_event, struct net_if *iface) {
    switch (wifi_mgmt_event) {
    case NET_EVENT_WIFI_CONNECT_RESULT:
        wifi_connect_event_handler(cb);
        break;
    case NET_EVENT_WIFI_DISCONNECT_RESULT:
        wifi_disconnect_event_handler(cb);
        break;
    default:
        break;
    }
}

static bool wifi_connect(const char *ssid, char *pass) {
    int retries = 3;
    bool net_iface_up = false;

    k_sem_init (&wifi.connected, 0, 1);
    wifi.iface = net_if_get_default();
    /* Check if the network interface is up */
    while (retries) {
        net_iface_up = net_if_is_up(wifi.iface);
        if (net_iface_up)
            break;

        k_sleep(K_MSEC(100));
        retries--;
    }

    if (!net_iface_up)
        return false;

    net_mgmt_init_event_callback(&wifi.event_cb, wifi_mgmt_event_handler,
        NET_EVENT_WIFI_CONNECT_RESULT | NET_EVENT_WIFI_DISCONNECT_RESULT);
    net_mgmt_add_event_callback(&wifi.event_cb);

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
    k_sem_take(&wifi.connected, K_FOREVER);

    return true;
}

static void wifi_disconnect(void) {
    if (net_mgmt(NET_REQUEST_WIFI_DISCONNECT, wifi.iface, NULL, 0))
    {
        LOG_ERR("WiFi Disconnection Request Failed");
    }
}

static void wifi_status(void) {
    struct wifi_iface_status status = {0};

    if (net_mgmt(NET_REQUEST_WIFI_IFACE_STATUS, wifi.iface, &status,
        sizeof(struct wifi_iface_status))) {
        LOG_ERR("WiFi Status Request Failed");
    }

    if (status.state >= WIFI_STATE_ASSOCIATED) {
        LOG_INF("Wifi network state:");
        LOG_INF("\tSSID: %-32s", status.ssid);
        LOG_INF("\tBand: %s", wifi_band_txt(status.band));
        LOG_INF("\tChannel: %d", status.channel);
        LOG_INF("\tSecurity: %s", wifi_security_txt(status.security));
        LOG_INF("\tRSSI: %d", status.rssi);
    }
}

/***************************** INTERFACE FUNCTIONS ****************************/
wifi_iface_t *wifi_get(void) {
    wifi_iface.connect = wifi_connect;
    wifi_iface.disconnect = wifi_disconnect;
    wifi_iface.status = wifi_status;

    return &wifi_iface;
}
