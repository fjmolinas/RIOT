/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mics6814
 * @{
 *
 * @file
 * @brief       Device driver implementation for the driver_mics6814
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <math.h>

#include "mics6814.h"
#include "mics6814_constants.h"
#include "mics6814_params.h"
#include "periph/adc.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int mics6814_init(mics6814_t *dev, const mics6814_params_t *params)
{
    assert(dev && params);
    dev->params = *params;

    assert(dev->params.adc_res == ADC_RES_6BIT ||
           dev->params.adc_res == ADC_RES_8BIT ||
           dev->params.adc_res == ADC_RES_10BIT ||
           dev->params.adc_res == ADC_RES_12BIT ||
           dev->params.adc_res == ADC_RES_14BIT ||
           dev->params.adc_res == ADC_RES_16BIT);

    if (adc_init(dev->params.aox) < 0) {
        DEBUG_PUTS("Initialization of OX line failed\n");
        return -1;
    }
    if (adc_init(dev->params.ared) < 0) {
        DEBUG_PUTS("Initialization of RED line failed\n");
        return -1;
    }
    if (adc_init(dev->params.anh3) < 0) {
        DEBUG_PUTS("Initialization of NH3 line failed\n");
        return -1;
    }
}

int32_t _mics6814_read(adc_t adc, adc_res_t res)
{
    int32_t value = adc_sample(adc, res);
    int32_t adc_max;
    if (res == ADC_RES_6BIT) {
        adc_max = 64 - 1;
    }
    else if (res == ADC_RES_8BIT) {
        adc_max = 256 - 1;
    }
    else if (res == ADC_RES_10BIT) {
        adc_max = 1024 - 1;
    }
    else if (res == ADC_RES_12BIT) {
        adc_max = 4096 - 1;
    }
    else if (res == ADC_RES_14BIT) {
        adc_max = 16384 - 1;
    }
    else {
        adc_max = 65536 - 1;
    }
    if (value) {
        return (MICS6814_RLOAD / (adc_max / value) - 1);
    }
    else {
        return 0;
    }
}

int32_t mics6814_read_raw_ox(mics6814_t *dev)
{
    return _mics6814_read(dev->params.aox, dev->params.adc_res);
}

int32_t mics6814_read_raw_red(mics6814_t *dev)
{
    return _mics6814_read(dev->params.ared, dev->params.adc_res);
}

int32_t mics6814_read_raw_nh3(mics6814_t *dev)
{
    return _mics6814_read(dev->params.anh3, dev->params.adc_res);
}

#if IS_USED(MICS6814_FULL)
float mics6814_read_co(mics6814_t *dev)
{
    int32_t value = _mics6814_read(dev->params.aox, dev->params.adc_res);
    return pow(value, -1.179) * 4.385;
}

float mics6814_read_no2(mics6814_t *dev)
{
    int32_t value = _mics6814_read(dev->params.ared, dev->params.adc_res);
    return pow(value, 1.007) / 6.855;
}

float mics6814_read_nh3(mics6814_t *dev)
{
    int32_t value = _mics6814_read(dev->params.anh3, dev->params.adc_res);
    return pow(value, -1.67) / 1.47;
}

float mics6814_read_c3h8(mics6814_t *dev)
{
    int32_t value = _mics6814_read(dev->params.anh3, dev->params.adc_res);
    return pow(value, -2.518) * 570.164;
}

float mics6814_read_c4h10(mics6814_t *dev)
{
    int32_t value = _mics6814_read(dev->params.anh3, dev->params.adc_res);
    return pow(value, -2.138) * 398.107;
}

float mics6814_read_ch4(mics6814_t *dev)
{
    int32_t value = _mics6814_read(dev->params.aox, dev->params.adc_res);
    return pow(value, -4.363) * 630.957;
}

float mics6814_read_h2(mics6814_t *dev)
{
    int32_t value = _mics6814_read(dev->params.aox, dev->params.adc_res);
    return pow(value, -1.8) * 0.73;
}

float mics6814_read_ch50h(mics6814_t *dev)
{
    int32_t value = _mics6814_read(dev->params.aox, dev->params.adc_res);
    return pow(value, -1.552) * 1.622;
}
#endif
