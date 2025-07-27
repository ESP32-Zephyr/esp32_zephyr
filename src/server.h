/**
 * @file server.h
 * @brief Server abstraction interface.
 *
 * Defines the server type, server protocol enumeration, and interface functions
 * for creating, destroying, and starting a server.
 */

#ifndef SERVER_H
#define SERVER_H

/******************************** INCLUDE FILES *******************************/
#include <stdbool.h>

/******************************** LOCAL DEFINES *******************************/

/********************************* TYPEDEFS ***********************************/

/**
 * @typedef server_t
 * @brief Opaque server instance type.
 */
typedef struct _server_t server_t;

/**
 * @enum server_proto_t
 * @brief Enum for selecting the server protocol type.
 */
typedef enum _server_proto_t
{
    SERVER_PROTO_UDP = 0, /**< UDP protocol */
    SERVER_PROTO_TCP,     /**< TCP protocol */
    SERVER_PROTO_N        /**< Count of server protocol types */
} server_proto_t;

/***************************** INTERFACE FUNCTIONS ****************************/

/**
 * @brief Create a new server instance.
 *
 * Allocates and initializes a new server of the specified protocol type,
 * listening on the given port. Optionally registers the service if requested.
 *
 * @param type The server protocol type (UDP or TCP).
 * @param port The port number on which the server will listen.
 * @param register_service Set to true to register the service, false otherwise.
 *
 * @return Pointer to the newly created server instance, or NULL on failure.
 */
server_t *server_new(server_proto_t type, int port, bool register_service);

/**
 * @brief Destroy a server instance.
 *
 * Deallocates and cleans up the server instance pointed to by `self_p`.
 * After this call, the pointer will be set to NULL.
 *
 * @param self_p Pointer to a pointer to the server instance to destroy.
 */
void server_destroy(server_t **self_p);

/**
 * @brief Start the server.
 *
 * Starts the server. The parameters p1, p2, p3 are generic pointers whose
 * meaning depends on the implementation and server type.
 *
 * @param p1 Pointer to context or first argument.
 * @param p2 Pointer to context or second argument.
 * @param p3 Pointer to context or third argument.
 */
void server_start(void *p1, void *p2, void *p3);

#endif /* SERVER_H */
