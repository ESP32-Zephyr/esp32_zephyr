/******************************** INCLUDE FILES *******************************/
#include <stdbool.h>
/******************************** LOCAL DEFINES *******************************/

/********************************* TYPEDEFS ***********************************/
typedef struct _server_t server_t;
/***************************** INTERFACE FUNCTIONS ****************************/
server_t *server_new (void);
void server_destroy (server_t **self_p);
