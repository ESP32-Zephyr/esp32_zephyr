/*
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/kernel.h>
#include <stddef.h>

#include "wifi.h"
#include "server.h"
/************************************ DEFINES **********************************/
#define UDP_SERVER_PORT 4242
#define UDP_SERVER_STACK_SIZE 4000

#if defined(CONFIG_NET_TC_THREAD_COOPERATIVE)
#define UDP_SERVER_THREAD_PRIO K_PRIO_COOP(CONFIG_NUM_COOP_PRIORITIES - 1)
#else
#define UDP_SERVER_THREAD_PRIO K_PRIO_PREEMPT(8)
#endif

/************************************* DATA ***********************************/
static k_tid_t udp_server_tid;
static K_THREAD_STACK_DEFINE(udp_server_stack, UDP_SERVER_STACK_SIZE);
static struct k_thread udp_server_td;

/******************************* MAIN ENTRY POINT *****************************/
int main(void)
{
    wifi_iface_t *wifi = NULL;
    server_t *udp_server = NULL;

    /* Connect to Wifi */
    wifi = wifi_get();
    wifi->connect(SSID, PSK);
    wifi->status();

    /* Create UDP server context */
    udp_server = server_new(SERVER_PROTO_UDP, UDP_SERVER_PORT);
    /* Spawn UDP server */
    udp_server_tid = k_thread_create(&udp_server_td, udp_server_stack,
                 K_THREAD_STACK_SIZEOF(udp_server_stack),
                 server_start, udp_server, NULL, NULL, UDP_SERVER_THREAD_PRIO, 0,
                 K_NO_WAIT);

    for (;;)
        k_sleep(K_MSEC(100));

    return 0;
}
