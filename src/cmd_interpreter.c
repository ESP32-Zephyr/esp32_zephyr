/******************************** INCLUDE FILES *******************************/
#include <pb_encode.h>
#include <pb_decode.h>

#include <zephyr/logging/log.h>

#include "cmd_interpreter.h"
#include "adc.h"
#include "pwm.h"

#include "version.h"
/******************************** LOCAL DEFINES *******************************/
LOG_MODULE_REGISTER(cmd_interpreter, LOG_LEVEL_DBG);

/********************************* TYPEDEFS ***********************************/

/******************************** LOCAL DATA **********************************/
static cmd_hndlr_t cmd_handlers[_command_id_ARRAYSIZE];

/******************************** LOCAL FUNCTIONS *****************************/
static void response_set_hdr(response *res, command_id cmd_id, ret_code ret, const char *err_msg)
{
    res->hdr.id = cmd_id;
    res->hdr.has_ret = true;
    res->hdr.ret = ret;
    if (ret != ret_code_OK && err_msg != NULL) {
        strcpy(res->hdr.err_msg, err_msg);
        res->hdr.has_err_msg = true;
    }
    else
    {
        res->hdr.has_err_msg = false;
    }

}

static int request_version_get_handler(const request *req, response *res)
{
    LOG_DBG("%s", __func__);
    (void)req;

    res->pl.version_get.version = APP_VERSION;
    res->pl.version_get.has_version = true;
    strcpy(res->pl.version_get.branch, git_Branch());
    res->pl.version_get.has_branch = true;
    strcpy(res->pl.version_get.sha1, git_CommitSHA1());
    res->pl.version_get.has_sha1 = true;
    strcpy(res->pl.version_get.commit_date, git_CommitDate());
    res->pl.version_get.has_commit_date = true;
    res->which_pl = response_version_get_tag;
    response_set_hdr(res, command_id_VERSION_GET, ret_code_OK, NULL);

    return 0;
}

static int request_adc_chs_get_handler(const request *req, response *res)
{
    LOG_DBG("%s", __func__);
    (void)req;

    adc_t *adc = adc_get();
    res->pl.adc_chs_get.adc_chs = adc->channels_n_get();
    res->pl.adc_chs_get.has_adc_chs = true;
    res->which_pl = response_adc_chs_get_tag;
    response_set_hdr(res, command_id_ADC_CHS_GET, ret_code_OK, NULL);

    return 0;
}

static int request_adc_ch_read_handler(const request *req, response *res)
{
    LOG_DBG("%s", __func__);

    int32_t val = 0;
    int32_t ch = 0;
    adc_t *adc = adc_get();

    ch = req->pl.adc_ch_read.ch;
    if (ch < 0 || ch >= adc->channels_n_get()) {
        response_set_hdr(res, command_id_ADC_CH_READ, ret_code_INVALID_ARGUMENT,
            "Invalid channel!");
        return -1;
    }

    adc->ch_read(ch, &val);
    res->pl.adc_ch_read.val = val;
    res->pl.adc_ch_read.has_val = true;
    res->which_pl = response_adc_ch_read_tag;
    response_set_hdr(res, command_id_ADC_CH_READ, ret_code_OK, NULL);

    return 0;
}

static int request_pwm_chs_get_handler(const request *req, response *res)
{
    LOG_DBG("%s", __func__);
    (void)req;

    pwm_t *pwm = pwm_get();
    res->pl.pwm_chs_get.pwm_chs = pwm->channels_n_get();
    res->pl.pwm_chs_get.has_pwm_chs = true;
    res->which_pl = response_pwm_chs_get_tag;
    response_set_hdr(res, command_id_PWM_CHS_GET, ret_code_OK, NULL);

    return 0;
}

static int request_pwm_ch_set_handler(const request *req, response *res)
{
    LOG_DBG("%s", __func__);
    int ret = 0;
    int32_t ch = 0;
    pwm_t *pwm = pwm_get();

    if (ch < 0 || ch >= pwm->channels_n_get()) {
        response_set_hdr(res, command_id_PWM_CH_SET, ret_code_INVALID_ARGUMENT,
            "Invalid channel!");
        return -1;
    }

    res->which_pl = response_pwm_ch_set_tag;
    ret = pwm->duty_cycle_set(ch, req->pl.pwm_ch_set.period, req->pl.pwm_ch_set.pulse);
    if (0 != ret)
    {
        response_set_hdr(res, command_id_PWM_CH_SET, ret_code_ERROR, "Failed to configure PWM chanel!");
        return -1;
    }
    else
    {
        response_set_hdr(res, command_id_PWM_CH_SET, ret_code_OK, NULL);
    }

    return 0;
}

static int request_pwm_ch_get_handler(const request *req, response *res)
{
    LOG_DBG("%s", __func__);
    int ret = 0;
    int32_t ch = 0;
    uint32_t period = 0;
    uint32_t pulse = 0;
    pwm_t *pwm = pwm_get();

    if (ch < 0 || ch >= pwm->channels_n_get()) {
        response_set_hdr(res, command_id_PWM_CH_GET, ret_code_INVALID_ARGUMENT,
            "Invalid channel!");
        return -1;
    }

    res->which_pl = response_pwm_ch_get_tag;
    ret = pwm->duty_cycle_get(ch, &period, &pulse);
    if (0 != ret)
    {
        response_set_hdr(res, command_id_PWM_CH_GET, ret_code_ERROR, "Failed to get PWM chanel configuration!");
        return -1;
    }
    else
    {
        res->pl.pwm_ch_get.period = period;
        res->pl.pwm_ch_get.pulse = pulse;
        response_set_hdr(res, command_id_PWM_CH_GET, ret_code_OK, NULL);
    }

    return 0;
}

static int request_pwm_periods_get_handler(const request *req, response *res)
{
    LOG_DBG("%s", __func__);
    (void)req;
    pwm_t *pwm = pwm_get();
    uint32_t period_max;
    uint32_t period_min;

    res->which_pl = response_pwm_periods_get_tag;
    pwm->periods_interval_get(&period_min, &period_max);
    res->pl.pwm_periods_get.period_min = period_min;
    res->pl.pwm_periods_get.period_max = period_max;
    response_set_hdr(res, command_id_PWM_PERIOD_INTERVAL_GET, ret_code_OK, NULL);

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

int cmd_interpreter_encode(char *buffer, int buffer_len_max, int *buffer_len, response *res)
{
    int status;

    pb_ostream_t o_stream = pb_ostream_from_buffer(buffer, buffer_len_max);
    status = pb_encode(&o_stream, response_fields, res);
    if (!status)
    {
        LOG_ERR("Encoding failed: %s\n", PB_GET_ERROR(&o_stream));
        return -1;
    }
    *buffer_len = o_stream.bytes_written;

    return 0;
}

cmd_hndlr_t *cmd_interpreter_get(void)
{
    cmd_handlers[command_id_UNKNOWN] = NULL;
    cmd_handlers[command_id_VERSION_GET] = request_version_get_handler;
    cmd_handlers[command_id_ADC_CHS_GET] = request_adc_chs_get_handler;
    cmd_handlers[command_id_ADC_CH_READ] = request_adc_ch_read_handler;
    cmd_handlers[command_id_PWM_CHS_GET] = request_pwm_chs_get_handler;
    cmd_handlers[command_id_PWM_CH_SET] = request_pwm_ch_set_handler;
    cmd_handlers[command_id_PWM_CH_GET] = request_pwm_ch_get_handler;
    cmd_handlers[command_id_PWM_PERIOD_INTERVAL_GET] = request_pwm_periods_get_handler;

    return cmd_handlers;
}
