/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sm_pwm_01c
 * @{
 * @file
 * @brief       Implementation of SM_PWM_01C dust sensor
 *
 * @author      Francisco Molina <francisco.molina@inria.cl>
 * @}
 */


#include <assert.h>
#include <string.h>

#include "log.h"
#include "xtimer.h"

#include "periph/gpio.h"

#include "sm_pwm_01c.h"
#include "sm_pwm_01c_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Scaling value to get 1/100 of a % resolution for lpo values */
#define LPO_SCALING         (100)

/* Sampling Timer */
static xtimer_t timer;

/* Last time tsp/tlp pin went low */
static volatile uint32_t tlp_start_time;
static volatile uint32_t tsp_start_time;

#ifdef SM_PWM_01C_MOVING_AVERAGE
static void _circ_buf_push(circ_buf_t *buf, uint16_t data)
{
    int next;

    next = buf->head + 1;
    if (next >= buf->len) {
        next = 0;
    }

    buf->buf[buf->head] = data;
    buf->head = next;
}

static uint16_t _circ_buf_avg(circ_buf_t *buf) {
    uint64_t sum = 0;
    for (int i = 0; i < buf->len; i++) {
        sum += *(buf->buf + i);
    }
    return (uint16_t) (sum / buf->len);
}
#endif

/* Interval approximation of theoretical Dust Concentration / LPO % curve
   https://www.sgbotic.com/products/datasheets/sensors/app-SM-PWM-01C.pdf */
static uint16_t _lpo_to_dust_cons(uint16_t lpo){
    if(lpo <= (2 * LPO_SCALING)) {
        return (143 * lpo) / (2 * LPO_SCALING);
    }
    else if(lpo <= (4 * LPO_SCALING)){
        return (208 * lpo + 130) / (3 * LPO_SCALING);
    }
    else if(lpo <= (15 * LPO_SCALING)){
        return (1155 * lpo - 1572) / (10 * LPO_SCALING);
    }
    else {
        return (2354 * lpo - 19560) / (10 * LPO_SCALING);
    }
}

static void _sample_timer_cb(void *arg)
{
    sm_pwm_01c_t* dev = (sm_pwm_01c_t*) arg;
    /* schedule next sample */
    xtimer_set(&timer, SM_PWM_01C_SAMPLE_TIME);

    /* calculate low Pulse Output Occupancy in (% * LPO_SCALING),
       e.g. 1% -> 100 */
    uint16_t tsp_ratio = (uint16_t) ((uint64_t) (100 * LPO_SCALING * dev->values.tsp_lpo) / SM_PWM_01C_SAMPLE_TIME);
    uint16_t tlp_ratio = (uint16_t) ((uint64_t) (100 * LPO_SCALING * dev->values.tlp_lpo) / SM_PWM_01C_SAMPLE_TIME);
    DEBUG("[sm_pwm_01c] tsp_ratio %"PRIu16"/%d %%\n", tsp_ratio, LPO_SCALING);
    DEBUG("[sm_pwm_01c] tlp_ratio %"PRIu16"/%d %%\n", tlp_ratio, LPO_SCALING);

    /* convert lpo to particle concentration */
    uint16_t tsp = _lpo_to_dust_cons(tsp_ratio);
    uint16_t tlp = _lpo_to_dust_cons(tlp_ratio);
    DEBUG("[sm_pwm_01c] new sample tsp conc: %"PRIu16" ug/m3\n", tsp);
    DEBUG("[sm_pwm_01c] new sample tlp conc: %"PRIu16" ug/m3\n", tlp);

    /* update concentration values*/
#ifdef SM_PWM_01C_MOVING_AVERAGE
    _circ_buf_push(&dev->tsp_circ_buf, tsp);
    _circ_buf_push(&dev->tlp_circ_buf, tlp);
#else
    dev->values.tlp_conc = (uint16_t) ((tlp + (uint32_t) (SM_PWM_01C_WEIGHT - 1) * dev->values.tlp_conc) / SM_PWM_01C_WEIGHT);
    dev->values.tsp_conc = (uint16_t) ((tsp + (uint32_t) (SM_PWM_01C_WEIGHT - 1) * dev->values.tsp_conc) / SM_PWM_01C_WEIGHT);
#endif
    /* reset lpo */
    dev->values.tlp_lpo = 0;
    dev->values.tsp_lpo = 0;
}

static void _tsp_pin_cb(void *arg)
{
    sm_pwm_01c_t* dev = (sm_pwm_01c_t*) arg;
    uint32_t now = xtimer_now_usec();
    if(gpio_read(dev->params.tsp_pin) == 0) {
        tsp_start_time = now;
    }
    else {
        dev->values.tsp_lpo += (now - tsp_start_time);
    }
}

static void _tlp_pin_cb(void *arg)
{
    sm_pwm_01c_t* dev = (sm_pwm_01c_t*) arg;
    uint32_t now = xtimer_now_usec();
    if(gpio_read(dev->params.tlp_pin) == 0) {
        tlp_start_time = now;
    }
    else {
        dev->values.tlp_lpo += (now - tlp_start_time);
    }
}

int sm_pwm_01c_init(sm_pwm_01c_t* dev, const sm_pwm_01c_params_t* params)
{
    dev->params = *params;

    /* set up irq */
    if (gpio_init_int(dev->params.tsp_pin, GPIO_IN_PU, GPIO_BOTH, _tsp_pin_cb, dev) < 0) {
        DEBUG("[sm_pwm_01c] init_int of tsp_pin failed [ERROR]\n");
        return SM_PWM_01C_ERR_GPIO;
    }
    if (gpio_init_int(dev->params.tlp_pin, GPIO_IN_PU, GPIO_BOTH, _tlp_pin_cb, dev) < 0) {
        DEBUG("[sm_pwm_01c] init_int of tlp_pin failed [ERROR]\n");
        return SM_PWM_01C_ERR_GPIO;
    }

    /* setup timer */
    timer.callback = _sample_timer_cb;
    timer.arg = dev;

#ifdef SM_PWM_01C_MOVING_AVERAGE
    dev->tsp_circ_buf.buf = dev->values.tsp_conc_buf;
    dev->tsp_circ_buf.len = SM_PWM_01C_BUFFER_LEN;
    dev->tlp_circ_buf.buf = dev->values.tlp_conc_buf;
    dev->tlp_circ_buf.len = SM_PWM_01C_BUFFER_LEN;
#endif

    return SM_PWM_01C_OK;
}

void sm_pwm_01c_start(sm_pwm_01c_t* dev)
{
    /* reset old values */
    memset((void*) &dev->values, 0, sizeof(sm_pwm_01c_values_t));
    /* enable irq and set timer */
    xtimer_set(&timer, SM_PWM_01C_SAMPLE_TIME);
    gpio_irq_enable(dev->params.tsp_pin);
    gpio_irq_enable(dev->params.tlp_pin);
    DEBUG("[sm_pwm_01c] started weighted average measurements\n");
}

void sm_pwm_01c_stop(sm_pwm_01c_t* dev)
{
    /* disable irq and remove timer */
    xtimer_remove(&timer);
    gpio_irq_disable(dev->params.tsp_pin);
    gpio_irq_disable(dev->params.tlp_pin);
    DEBUG("[sm_pwm_01c] stopped weighted average measurements\n");
}

int16_t sm_pwm_01c_read_tsp(sm_pwm_01c_t *dev)
{
#ifdef SM_PWM_01C_MOVING_AVERAGE
    return _circ_buf_avg(&dev->tsp_circ_buf);
#else
    return dev->values.tsp_conc;
#endif
}

int16_t sm_pwm_01c_read_tlp(sm_pwm_01c_t *dev)
{
#ifdef SM_PWM_01C_MOVING_AVERAGE
    return _circ_buf_avg(&dev->tlp_circ_buf);
#else
    return dev->values.tlp_conc;
#endif
}
