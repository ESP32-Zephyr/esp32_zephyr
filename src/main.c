/*
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/kernel.h>
#include <stddef.h>

#include "version.h"
#include "wifi.h"
#include "server.h"

/************************************ DEFINES **********************************/
#define SERVER_TL_TYPE SERVER_PROTO_UDP
#define SERVER_PORT 4242
#define REGISTER_SERVICE true
#define SERVER_STACK_SIZE 2000

#if defined(CONFIG_NET_TC_THREAD_COOPERATIVE)
#define SERVER_THREAD_PRIO K_PRIO_COOP(CONFIG_NUM_COOP_PRIORITIES - 1)
#else
#define SERVER_THREAD_PRIO K_PRIO_PREEMPT(8)
#endif

/************************************* DATA ***********************************/
static k_tid_t server_tid;
static K_THREAD_STACK_DEFINE(server_stack, SERVER_STACK_SIZE);
static struct k_thread server_td;

/******************************* MAIN ENTRY POINT *****************************/
int main(void)
{
    wifi_iface_t *wifi = NULL;
    server_t *server = NULL;

    version_print();

    /* Connect to Wifi */
    wifi = wifi_get();
    wifi->connect(SSID, PSK);
    wifi->status();

    /* Create server context */
    server = server_new(SERVER_TL_TYPE, SERVER_PORT, REGISTER_SERVICE);

    /* Spawn server */
    server_tid = k_thread_create(&server_td, server_stack,
                 K_THREAD_STACK_SIZEOF(server_stack),
                 server_start, server, NULL, NULL, SERVER_THREAD_PRIO, 0,
                 K_NO_WAIT);

    for (;;)
        k_sleep(K_MSEC(1000));

    return 0;
}
