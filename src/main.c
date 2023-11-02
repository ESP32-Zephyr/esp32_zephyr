/*
 * SPDX-License-Identifier: Apache-2.0
 */
#include "wifi.h"

int main(void)
{
    wifi_iface_t *wifi = wifi_get();

    wifi->connect(SSID, PSK);
    wifi->status();
    wifi->disconnect();

    return 0;
}
