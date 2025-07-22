/******************************** INCLUDE FILES *******************************/
#include <pb_encode.h>
#include <pb_decode.h>

#include <zephyr/logging/log.h>

#include "cmd_interpreter.h"

#include "version.h"
/******************************** LOCAL DEFINES *******************************/
LOG_MODULE_REGISTER(cmd_interpreter, LOG_LEVEL_DBG);

/********************************* TYPEDEFS ***********************************/

/******************************** LOCAL DATA **********************************/
static cmd_hndlr_t cmd_handlers[_command_id_ARRAYSIZE];

/******************************** LOCAL FUNCTIONS *****************************/
static int request_version_get_handler(const request *req, response *res)
{
    LOG_INF("%s\n", __func__);
    (void)req;

    res->pl.version_get.version = APP_VERSION;
    strcpy(res->pl.version_get.branch, git_Branch());
    strcpy(res->pl.version_get.sha1, git_CommitSHA1());
    strcpy(res->pl.version_get.commit_date, git_CommitDate());
    res->which_pl = response_version_get_tag;
    res->hdr.ret = ret_code_OK;
    res->has_hdr = 1;

    //res->hdr.ret = ret_code_ERROR;
    //strcpy(res->hdr.err_msg, "Failed!");;

    return 0;
}

static int request_adc_chs_get_handler(const request *req, response *res)
{
    LOG_INF("%s\n", __func__);
    (void)req;

    res->pl.adc_chs_get.adc_chs = 2;
    res->which_pl = response_adc_chs_get_tag;
    res->hdr.ret = ret_code_OK;

    return 0;
}

/***************************** INTERFACE FUNCTIONS ****************************/
int cmd_interpreter_decode(char *buffer, int buffer_len, request *req)
{
    int status;

    pb_istream_t i_stream = pb_istream_from_buffer(buffer, buffer_len);
    status = pb_decode(&i_stream, request_fields, req);
    /* Check for errors... */
    if (!status)
    {
        LOG_ERR("Decoding failed: %s\n", PB_GET_ERROR(&i_stream));
        return -1;
    }

    return 0;
}

int cmd_interpreter_encode(char *buffer, int *buffer_len, response *res)
{
    int status;

    pb_ostream_t o_stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    status = pb_encode(&o_stream, response_fields, res);
    *buffer_len = o_stream.bytes_written;
    if (!status)
    {
        LOG_ERR("Encoding failed: %s\n", PB_GET_ERROR(&o_stream));
        return -1;
    }

    return 0;
}

cmd_hndlr_t *cmd_interpreter_get(void)
{
    cmd_handlers[command_id_UNKNOWN] = NULL;
    cmd_handlers[command_id_VERSION_GET] = request_version_get_handler;
    cmd_handlers[command_id_ADC_CHS_GET] = request_adc_chs_get_handler;

    return cmd_handlers;
}

