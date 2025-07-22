/******************************** INCLUDE FILES *******************************/
#include "proto/cmds.pb.h"

/********************************** DEFINES ***********************************/

/********************************* TYPEDEFS ***********************************/
typedef int (*cmd_hndlr_t) (const request *req, response *res);

/***************************** INTERFACE FUNCTIONS ****************************/
cmd_hndlr_t *cmd_interpreter_get(void);
