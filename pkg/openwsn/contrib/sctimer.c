/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 *
 * @author      Tengfei Chang <tengfei.chang@gmail.com>, July 2012
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>, July 2017
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>, April 2019
 *
 * @}
 */
#include <stdatomic.h>

#include "sctimer.h"
#include "debugpins.h"

#include "periph/rtt.h"

#define LOG_LEVEL LOG_NONE
#include "log.h"

/**
 * @brief   Maximum counter difference to not consider an ISR late, this should
 *          account for the largest timer interval OpenWSN scheduler might work
 *          with. When running only the stack this should not be more than
 *          SLOT_DURATION, but when using cjoin it is 65535ms
 */
#ifndef SCTIMER_LOOP_THRESHOLD
#define SCTIMER_LOOP_THRESHOLD       (2*PORT_TICS_PER_MS*65535)
#endif

/**
 * @brief   Minimum value difference between current counter value for the
 *          hardware to correctly set the ISR.
 */
#ifndef SCTIMER_MIN_COMP_ADVANCE
#define SCTIMER_MIN_COMP_ADVANCE     (5)
#endif

/**
 * @brief   Offset to schedule already late sctimer interrupts
 */
#ifndef SCTIMER_ISR_NOW_OFFSET
#ifdef RTT_IRQ
/* irrelevant value some time in the future, pending ISR is set manually */
#define SCTIMER_ISR_NOW_OFFSET       (1000)
#else
/* execute RTT callback as early as possible */
#define SCTIMER_ISR_NOW_OFFSET         (10)
#endif
#endif

#ifdef SCTIMER_TIME_DIVISION
#define SCTIMER_PRESCALER            __builtin_ctz( \
        SCTIMER_FREQUENCY / RTT_FREQUENCY)
#define SCTIMER_TIME_DIVISION_MASK   (RTT_MAX_VALUE >> SCTIMER_PRESCALER)
#define SCTIMER_PRESCALER_MASK       (~SCTIMER_TIME_DIVISION_MASK)
#define SCTIMER_PRESCALER_SHIFT      __builtin_ctz(SCTIMER_TIME_DIVISION_MASK)

static PORT_TIMER_WIDTH prescaler;
static atomic_bool enable;
#endif

static sctimer_cbt sctimer_cb;

static void sctimer_isr_internal(void *arg)
{
    (void)arg;

    if (sctimer_cb != NULL) {
        debugpins_isr_set();
        sctimer_cb();
        debugpins_isr_clr();
    }
}

void sctimer_init(void)
{
    LOG_DEBUG("%s\n", __FUNCTION__);
    sctimer_cb = NULL;
    rtt_init();
#ifdef SCTIMER_TIME_DIVISION
    prescaler = 0;
    enable = false;
#endif
}

void sctimer_set_callback(sctimer_cbt cb)
{
    LOG_DEBUG("%s\n", __FUNCTION__);
    sctimer_cb = cb;
}

#ifdef SCTIMER_TIME_DIVISION
PORT_TIMER_WIDTH _update_val(PORT_TIMER_WIDTH val, uint32_t now)
{
    now = now & SCTIMER_PRESCALER_MASK;
    val = val >> SCTIMER_PRESCALER;
    /* Check if next value would cause an overflow */
    if ((now - val) > SCTIMER_LOOP_THRESHOLD && enable && now > val) {
        prescaler += (1 << SCTIMER_PRESCALER_SHIFT);
        enable = false;
    }
    /* Make sure it only updates the prescaler once per overflow cycle */
    if (val > SCTIMER_LOOP_THRESHOLD && val < 2*SCTIMER_LOOP_THRESHOLD) {
        enable = true;
    }
    val |= prescaler;

    return val;
}
#endif

void sctimer_setCompare(uint32_t val)
{
    unsigned state = irq_disable();

    uint32_t now = rtt_get_counter();

#ifdef SCTIMER_TIME_DIVISION
    val = _update_val(val, now);
#endif

    now = rtt_get_counter();

    /* If the next compare value (isr) to schedule is already later than the
       required value, but close enough to think we have been slow in scheduling
       it, trigger the ISR right away */
    if (now - val <  SCTIMER_LOOP_THRESHOLD && now >= val) {
#ifdef RTT_IRQ
        rtt_set_alarm(now + SCTIMER_ISR_NOW_OFFSET, sctimer_isr_internal, NULL);
        NVIC_SetPendingIRQ(RTT_IRQ);
#else
        rtt_set_alarm(now + SCTIMER_ISR_NOW_OFFSET, sctimer_isr_internal, NULL);
#endif
    }
    /* set callback early when too close to current counter value, hardware
    limitations might have the callback scheduled 1 full cycle late */
    else if (val - now < SCTIMER_MIN_COMP_ADVANCE && val >= now) {
#ifdef RTT_IRQ
        rtt_set_alarm(now + SCTIMER_ISR_NOW_OFFSET, sctimer_isr_internal, NULL);
        NVIC_SetPendingIRQ(RTT_IRQ);
#else
        rtt_set_alarm(now + SCTIMER_ISR_NOW_OFFSET, sctimer_isr_internal, NULL);
#endif
    }
    else {
        rtt_set_alarm(val, sctimer_isr_internal, NULL);
    }

    irq_restore(state);
}

uint32_t sctimer_readCounter(void)
{
    uint32_t now = rtt_get_counter();
#ifdef SCTIMER_TIME_DIVISION
    now &= SCTIMER_TIME_DIVISION_MASK;
    now = (now << SCTIMER_PRESCALER);
#endif
    return now;
}

void sctimer_enable(void)
{
    LOG_DEBUG("%s\n", __FUNCTION__);
    rtt_poweron();
}

void sctimer_disable(void)
{
    LOG_DEBUG("%s\n", __FUNCTION__);
    rtt_poweroff();
}
