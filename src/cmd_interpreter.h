/**
 * @file cmds.h
 * @brief Command interpreter interface for handling protobuf commands.
 *
 * Defines the command handler function type and interface functions to get the
 * command interpreter, decode requests from a buffer, and encode responses into a buffer.
 */

#ifndef CMDS_H
#define CMDS_H

/******************************** INCLUDE FILES *******************************/
#include "proto/cmds.pb.h"

/********************************** DEFINES ***********************************/

/********************************* TYPEDEFS ***********************************/

/**
 * @typedef cmd_hndlr_t
 * @brief Command handler function pointer type.
 *
 * This function processes a command request and populates the command response.
 *
 * @param[in] req Pointer to the decoded request message.
 * @param[out] res Pointer to the response message to populate.
 *
 * @return Integer status code (e.g. 0 on success, non-zero on error).
 */
typedef int (*cmd_hndlr_t)(const request *req, response *res);

/***************************** INTERFACE FUNCTIONS ****************************/

/**
 * @brief Get a pointer to the command interpreter handler array.
 *
 * Returns a pointer to an array of command handler function pointers,
 * which the interpreter uses to dispatch commands.
 *
 * @return Pointer to an array of command handler function pointers.
 */
cmd_hndlr_t *cmd_interpreter_get(void);

/**
 * @brief Decode a command request from a buffer.
 *
 * Parses the serialized request data from the buffer into the protobuf
 * request struct.
 *
 * @param[in] buffer Pointer to the input buffer containing serialized data.
 * @param[in] buffer_len Length of the input buffer in bytes.
 * @param[out] req Pointer to the request struct to populate.
 *
 * @return Integer status code (0 on success, non-zero on failure).
 */
int cmd_interpreter_decode(char *buffer, int buffer_len, request *req);

/**
 * @brief Encode a command response into a buffer.
 *
 * Serializes the protobuf response struct into a buffer.
 *
 * @param[out] buffer Pointer to the output buffer to hold serialized data.
 * @param[in] buffer_len_max Maximum size of the output buffer in bytes.
 * @param[out] buffer_len Pointer to store the actual length of serialized data.
 * @param[in] res Pointer to the response struct to encode.
 *
 * @return Integer status code (0 on success, non-zero on failure).
 */
int cmd_interpreter_encode(char *buffer, int buffer_len_max, int *buffer_len, response *res);

#endif /* CMDS_H */
