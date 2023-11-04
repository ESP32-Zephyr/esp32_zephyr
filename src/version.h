/******************************** INCLUDE FILES *******************************/
#include <zephyr/kernel.h>

#include "git.h"
/********************************** DEFINES ***********************************/
#define APP_VERSION_MAJOR 0
#define APP_VERSION_MINOR 1
#define APP_VERSION_PATCH 0

#define APP_MAKE_VERSION(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))
#define APP_VERSION \
    APP_MAKE_VERSION(APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_PATCH)

/********************************* TYPEDEFS ***********************************/

/***************************** INTERFACE FUNCTIONS ****************************/
static inline void version_print(void)
{
    printk("*** Application %s-v%d.%d.%d-%s-%s %s ***\n", __APP_NAME__, APP_VERSION_MAJOR,
     APP_VERSION_MINOR, APP_VERSION_PATCH, git_Branch(), git_CommitSHA1(), git_CommitDate());
}
