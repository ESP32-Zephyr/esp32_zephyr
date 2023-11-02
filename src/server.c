/******************************** INCLUDE FILES *******************************/
#include <zephyr/net/socket.h>
#include <zephyr/kernel.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <zephyr/logging/log.h>

#include "server.h"

/******************************** LOCAL DEFINES *******************************/
LOG_MODULE_REGISTER(server, LOG_LEVEL_DBG);

/********************************* TYPEDEFS ***********************************/
struct _server_t
{
    int sock;
};
/********************************* LOCAL DATA *********************************/

/******************************* LOCAL FUNCTIONS ******************************/

/***************************** INTERFACE FUNCTIONS ****************************/
server_t *server_new (void)
{
    LOG_INF("Creating new server object");

    server_t *self = (server_t *) k_malloc (sizeof (server_t));
    assert (self);

    return self;
}

void server_destroy (server_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        server_t *self = *self_p;
        //  Free class properties here
        //  Free object itself
        k_free (self);
        *self_p = NULL;
    }
}
