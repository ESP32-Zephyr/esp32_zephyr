/******************************** INCLUDE FILES *******************************/
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>

#include "pwm.h"
/******************************** LOCAL DEFINES *******************************/
LOG_MODULE_REGISTER(pwm, LOG_LEVEL_DBG);

#define NUM_PWM_CHANNELS 3
#define MIN_PERIOD 1
#define MAX_PERIOD 1000000000

/********************************* TYPEDEFS ***********************************/

/********************************* LOCAL DATA *********************************/
// PWM channels from device tree aliases
static const struct pwm_dt_spec pwm_channels[NUM_PWM_CHANNELS] = {
    PWM_DT_SPEC_GET(DT_ALIAS(pwm_mc0)),
    PWM_DT_SPEC_GET(DT_ALIAS(pwm_mc1)),
    PWM_DT_SPEC_GET(DT_ALIAS(pwm_mc2)),
};

// Store current period and pulse width for each channel
static uint32_t current_periods[NUM_PWM_CHANNELS];
static uint32_t current_pulses[NUM_PWM_CHANNELS];

static pwm_t pwm = {NULL, false, NULL, NULL};
/******************************* LOCAL FUNCTIONS ******************************/
/**
 * Initialize all PWM channels:
 *  - Check if device is ready
 *  - Calibrate max_period for each channel
 *  - Set initial period and pulse width
 *
 * Returns 0 on success, negative errno on failure
 */
static int pwm_init(void)
{
    LOG_INF("Initializing PWM channels...\n");

    for (int i = 0; i < NUM_PWM_CHANNELS; i++) {
        // Check device readiness
        if (!pwm_is_ready_dt(&pwm_channels[i])) {
            LOG_ERR("PWM device %s is not ready\n",
                   pwm_channels[i].dev->name);
            return -ENODEV;
        }
    }
    pwm.init_done = true;

    return 0;
}

static int pwm_channels_n_get(void) {
    return NUM_PWM_CHANNELS;
}

/**
 * Set the PWM period and pulse for a single channel by index.
 * Validates channel index.
 *
 * Returns 0 on success or negative errno on failure.
 */
static int duty_cycle_set(int channel_idx, uint32_t period, uint32_t pulse)
{
    if (!pwm.init_done) {
        LOG_ERR("PWM not initialized!");
        return -1;
    }

    if (channel_idx < 0 || channel_idx >= NUM_PWM_CHANNELS) {
        return -EINVAL;
    }

    if (period > MAX_PERIOD || period < MIN_PERIOD) {
        return -EINVAL;
    }

    if (pulse > period) {
        return -EINVAL;
    }

    int ret = pwm_set_dt(&pwm_channels[channel_idx], period, pulse);
    if (ret == 0) {
        current_periods[channel_idx] = period;
        current_pulses[channel_idx] = pulse;
    } else {
        LOG_ERR("%d: failed to set PWM for channel %d\n", ret, channel_idx);
    }

    return ret;
}

/**
 * Get the current PWM period and pulse width for a channel by index.
 * Returns 0 on success, -EINVAL for invalid channel index.
 */
static int duty_cycle_get(int channel_idx, uint32_t *period, uint32_t *pulse)
{
    if (!pwm.init_done) {
        LOG_ERR("PWM not initialized!");
        return -1;
    }

    if (channel_idx < 0 || channel_idx >= NUM_PWM_CHANNELS || !period || !pulse) {
        return -EINVAL;
    }

    *period = current_periods[channel_idx];
    *pulse = current_pulses[channel_idx];

    return 0;
}

static void periods_interval_get(uint32_t *period_min, uint32_t *period_max)
{
    *period_max = MAX_PERIOD;
    *period_min = MIN_PERIOD;
}

/***************************** INTERFACE FUNCTIONS ****************************/
pwm_t *pwm_get(void) {
    pwm.init = pwm_init;
    pwm.channels_n_get = pwm_channels_n_get;
    pwm.duty_cycle_set = duty_cycle_set;
    pwm.duty_cycle_get = duty_cycle_get;
    pwm.periods_interval_get = periods_interval_get;

    return &pwm;
}
