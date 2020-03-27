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

#include "sctimer.h"
#include "debugpins.h"

#include "periph/rtt.h"

#define LOG_LEVEL LOG_NONE
#include "log.h"

#if RTT_FREQUENCY != 32768U
    #error "RTT_FREQUENCY not supported"
#endif

/**
 * @brief   Maximum counter difference to not consider an ISR late
 */
#ifndef SCTIMER_MIN_COMP_ADVANCE
#define SCTIMER_MIN_COMP_ADVANCE     (10)
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
}

void sctimer_set_callback(sctimer_cbt cb)
{
    LOG_DEBUG("%s\n", __FUNCTION__);
    sctimer_cb = cb;
}

void sctimer_setCompare(uint32_t val)
{
    unsigned state = irq_disable();;

    uint32_t cnt = rtt_get_counter();

    /* If the next compare value (isr) to schedule is already late then trigger
       the ISR right away */
    /* ATTENTION! This needs to be an unsigned type */
    if ((int32_t)(val - cnt) < SCTIMER_MIN_COMP_ADVANCE) {
#ifdef RTT_IRQ
        rtt_set_alarm(cnt + SCTIMER_ISR_NOW_OFFSET, sctimer_isr_internal, NULL);
        NVIC_SetPendingIRQ(RTT_IRQ);
#else
        rtt_set_alarm(cnt + SCTIMER_ISR_NOW_OFFSET, sctimer_isr_internal, NULL);
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
