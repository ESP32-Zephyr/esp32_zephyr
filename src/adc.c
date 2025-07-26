/******************************** INCLUDE FILES *******************************/
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#include <zephyr/logging/log.h>

#include "adc.h"
/******************************** LOCAL DEFINES *******************************/
LOG_MODULE_REGISTER(adc, LOG_LEVEL_DBG);

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
    !DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
    ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

/********************************* TYPEDEFS ***********************************/

/********************************* LOCAL DATA *********************************/
/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
    DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
                 DT_SPEC_AND_COMMA)
};

static adc_t adc = {NULL, 0, NULL, NULL};

/******************************* LOCAL FUNCTIONS ******************************/
static int adc_init(void) {
    int err;

    for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
        if (!adc_is_ready_dt(&adc_channels[i])) {
            LOG_ERR("ADC controller device %s not ready\n", adc_channels[i].dev->name);
            return -1;
        }

        err = adc_channel_setup_dt(&adc_channels[i]);
        if (err < 0) {
            LOG_ERR("Could not setup channel #%d (%d)\n", i, err);
            return -1;
        }
    }

    adc.init_done = true;

    return 0;
}

static int adc_channels_n_get(void) {
    return ARRAY_SIZE(adc_channels);
}

static int adc_ch_read(int adc_ch, int32_t *val_mv) {
    if (!adc.init_done) {
        LOG_ERR("ADC not initialized!");
        return -1;
    }

    int err;
    uint16_t buf;
    struct adc_sequence sequence = {
        .buffer = &buf,
        /* buffer size in bytes, not number of samples */
        .buffer_size = sizeof(buf),
    };

    printk("- %s, channel %d: ", adc_channels[adc_ch].dev->name,
        adc_channels[adc_ch].channel_id);
    (void)adc_sequence_init_dt(&adc_channels[adc_ch], &sequence);
    err = adc_read_dt(&adc_channels[adc_ch], &sequence);
    if (err < 0) {
        LOG_ERR("Could not read (%d)\n", err);
        return -1;
    }

    /*
     * If using differential mode, the 16 bit value
     * in the ADC sample buffer should be a signed 2's
     * complement value.
     */
     if (adc_channels[adc_ch].channel_cfg.differential) {
        *val_mv = (int32_t)((int16_t)buf);
    } else {
        *val_mv = (int32_t)buf;
    }

    err = adc_raw_to_millivolts_dt(&adc_channels[adc_ch], val_mv);

    return err;
}

/***************************** INTERFACE FUNCTIONS ****************************/
adc_t *adc_get(void) {
    adc.init = adc_init;
    adc.channels_n_get = adc_channels_n_get;
    adc.ch_read = adc_ch_read;

    return &adc;
}
