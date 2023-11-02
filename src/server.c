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
#define SERVER_IO_BUFFER_SIZE 1000

#define SERVER_MAX_CLIENT_QUEUE CONFIG_NET_SAMPLE_NUM_HANDLERS

/********************************* TYPEDEFS ***********************************/
typedef int (*server_bind_t)(server_t *);
typedef int (*server_process_t)(server_t *);

typedef struct _server_iface_t {
    server_bind_t bind;
    server_process_t process;
} server_iface_t;
typedef server_iface_t *(*server_iface_create_t)(void);

struct _server_t
{
    server_proto_t type;
    int port;
    int sock;
    server_iface_t *iface;
};

/********************************* LOCAL DATA *********************************/


/******************************* LOCAL FUNCTIONS ******************************/
static int server_udp_bind(server_t *self)
{
    int ret = 0;
    struct sockaddr_in s_addr;
    socklen_t s_addr_len = 0;

    if (self->type != SERVER_PROTO_UDP)
    {
        LOG_ERR("Invalid server context!");
        return -1;
    }

    (void)memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(self->port);

    self->sock = socket(s_addr.sin_family, SOCK_DGRAM, IPPROTO_UDP);
    if (self->sock < 0) {
        LOG_ERR("Failed to create UDP socket: %d, %s", errno, strerror(errno));
        ret = -errno;
    }

    if (0 == ret) {
        s_addr_len = sizeof(s_addr);
        ret = bind(self->sock, (struct sockaddr *)&s_addr, s_addr_len);
        if (ret < 0) {
            LOG_ERR("Failed to bind UDP socket: %d, %s", errno, strerror(errno));
            ret = -errno;
        }
    }

    return ret;
}

static int server_udp_process(server_t *self)
{
    int ret = 0;
    int received = 0;
    struct sockaddr client_addr;
    socklen_t client_addr_len;
    char recv_buffer[SERVER_IO_BUFFER_SIZE];

    if (self->type != SERVER_PROTO_UDP)
    {
        LOG_ERR("Invalid server context!");
        return -1;
    }

    LOG_INF("Listeing on UDP port %d...", self->port);
    for(;;)
    {
        client_addr_len = sizeof(client_addr);
        received = recvfrom(self->sock, recv_buffer,
                    SERVER_IO_BUFFER_SIZE, 0, &client_addr, &client_addr_len);
        if (received < 0) {
            /* Socket error */
            NET_ERR("UDP: Connection error: %d, %s", errno, strerror(errno));
            continue;
        }
        LOG_DBG("Received: %d bytes", received);

        ret = sendto(self->sock, recv_buffer, received, 0,
                 &client_addr, client_addr_len);
        if (ret < 0) {
            NET_ERR("UDP: Failed to send: %d, %s", errno, strerror(errno));
            continue;
        }
    }
}

static server_iface_t *server_udp_create(void)
{
    server_iface_t *udp = (server_iface_t *) k_malloc (sizeof (server_iface_t));
    udp->bind = server_udp_bind;
    udp->process = server_udp_process;

    return udp;
}

static int server_tcp_bind(server_t *self)
{
    int ret = 0;
    struct sockaddr_in s_addr;
    socklen_t s_addr_len = 0;

    if (self->type != SERVER_PROTO_TCP)
    {
        LOG_ERR("Invalid server context!");
        return -1;
    }

    (void)memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(self->port);

    self->sock = socket(s_addr.sin_family, SOCK_STREAM, 0);
    if (self->sock < 0) {
        LOG_ERR("Failed to create TCP socket: %d, %s", errno, strerror(errno));
        return -errno;
    }

    s_addr_len = sizeof(s_addr);
    ret = bind(self->sock, (struct sockaddr *)&s_addr, s_addr_len);
    if (ret < 0) {
        LOG_ERR("Failed to bind TCP socket: %d, %s", errno, strerror(errno));
        return -errno;
    }

    ret = listen(self->sock, SERVER_MAX_CLIENT_QUEUE);
    if (ret < 0) {
        LOG_ERR("Failed to listen on TCP socket: %d, %s", errno, strerror(errno));
        return -errno;
    }

    return 0;
}

static int server_tcp_conn_handler(server_t *self, int conn)
{
    int ret = 0;
    int received = 0;
    char recv_buffer[SERVER_IO_BUFFER_SIZE];

    for(;;)
    {
        received = recv(conn, recv_buffer, SERVER_IO_BUFFER_SIZE, 0);
        if (received == 0) {
            /* Connection closed */
            LOG_INF("TCP: Connection closed");
            break;
        } else if (received < 0) {
            /* Socket error */
            LOG_ERR("TCP: Connection error %d", errno);
            ret = errno;
            break;
        }
        send(conn, recv_buffer, received, 0);
    }

    (void)close(conn);
    return ret;
}

static int server_tcp_process(server_t *self)
{
    int conn = -1;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = 0;

    LOG_INF("Waiting for TCP connections on port %d...", self->port);
    client_addr_len = sizeof(client_addr);
    for (;;) {
        conn = accept(self->sock, (struct sockaddr *)&client_addr,
                &client_addr_len);
        if (conn < 0) {
            LOG_ERR("TCP: Accept error: %d, %s", errno, strerror(errno));
            continue;
        }

        server_tcp_conn_handler(self, conn);
    }

    return 0;
}

static server_iface_t *server_tcp_create(void)
{
    server_iface_t *tcp = (server_iface_t *) k_malloc (sizeof (server_iface_t));
    tcp->bind = server_tcp_bind;
    tcp->process = server_tcp_process;

    return tcp;
}

static server_iface_create_t server_iface_create[SERVER_PROTO_N] = {
    server_udp_create,
    server_tcp_create,
};

/***************************** INTERFACE FUNCTIONS ****************************/
server_t *server_new (server_proto_t type, int port)
{
    LOG_INF("Creating new server object");

    server_t *self = (server_t *) k_malloc (sizeof (server_t));
    assert (self);
    self->type = type;
    self->port = port;
    self->iface = server_iface_create[type]();

    return self;
}

void server_destroy (server_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        server_t *self = *self_p;
        //  Free class properties here
        //  Free object itself
        k_free(self->iface);
        k_free (self);
        *self_p = NULL;
    }
}

void server_start (void *p1, void *p2, void *p3)
{
    int ret = 0;
    server_t *self = (server_t *)p1;

    ret = self->iface->bind(self);
    if (0 != ret)
    {
        LOG_ERR("Bind Failed!");
        return;
    }

    self->iface->process(self);
}
