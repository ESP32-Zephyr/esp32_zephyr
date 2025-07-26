/******************************** INCLUDE FILES *******************************/
#include "proto/cmds.pb.h"

/********************************** DEFINES ***********************************/

/********************************* TYPEDEFS ***********************************/
typedef int (*cmd_hndlr_t) (const request *req, response *res);

/***************************** INTERFACE FUNCTIONS ****************************/
cmd_hndlr_t *cmd_interpreter_get(void);
int cmd_interpreter_decode(char *buffer, int buffer_len, request *req);
int cmd_interpreter_encode(char *buffer, int buffer_len_max, int *buffer_len, response *res);
