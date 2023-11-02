/*
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>

#include "wifi.h"
#include "server.h"

int main(void)
{
    wifi_iface_t *wifi = NULL;
    server_t *server = NULL;

    wifi = wifi_get();
    wifi->connect(SSID, PSK);
    wifi->status();

    server = server_new();



    server_destroy(&server);

    wifi->disconnect();

    return 0;
}
