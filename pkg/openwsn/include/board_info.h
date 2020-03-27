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
 * @author      Thomas Watteyne <watteyne@eecs.berkeley.edu>, February 2012
 * @author      Tengfei Chang <tengfei.chang@gmail.com>, July 2012
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>, July 2017
 * @author      Francisco Molina <francisco.molina@inria.cl>
 *
 * @}
 */

#ifndef __BOARD_INFO_H
#define __BOARD_INFO_H

#include <stdint.h>
#include <string.h>
#include "cpu.h"
#include "periph_conf.h"

/**
 * @name    OpenWSN interrupt wrappers
 * @{
 */
#define INTERRUPT_DECLARATION()             /* unused in RIOT */
#define DISABLE_INTERRUPTS()                irq_disable();
#define ENABLE_INTERRUPTS()                 irq_enable();
/** @} */

/**
 * @name    RTT (sctimer) derived parameters
 * @{
 */
#if RTT_MAX_VALUE == 0xffffffff
#define PORT_TIMER_WIDTH                    uint32_t
#define PORT_RADIOTIMER_WIDTH               uint32_t
#elif RTT_MAX_VALUE == 0x00ffffff
#define PORT_TIMER_WIDTH                    uint24_t
#define PORT_RADIOTIMER_WIDTH               uint24_t
#elif RTT_MAX_VALUE == 0x0000ffff
#define PORT_TIMER_WIDTH                    uint16_t
#define PORT_RADIOTIMER_WIDTH               uint16_t
#else
#error "RTT_MAX_VALUE not supported"
#endif

#if __SIZEOF_POINTER__ == 2
#define PORT_SIGNED_INT_WIDTH               int16_t
#else
#define PORT_SIGNED_INT_WIDTH               int32_t
#endif
/*  */
#ifndef PORT_TICS_PER_MS
#if defined(BOARD_IOTLAB_M3) || defined(BOARD_IOTLAB_A8_M3) || \
    defined(BOARD_NUCLEO_F103RB)
#define PORT_TICS_PER_MS                    32
#else
#define PORT_TICS_PER_MS                    33
#endif
#endif
/** @} */

/* number of us per 32kHz clock tick */
#define PORT_US_PER_TICK                    30

/* Used in OpeWSN for waking up the scheduler */
#define SCHEDULER_WAKEUP()                  /* unused by RIOT */
#define SCHEDULER_ENABLE_INTERRUPT()        /* unused by RIOT */

/**
 * @name    IEEE802154E timing
 * @{
 */
/* standard slot duration is 10ms but code execution time for most OpenWSN
   supported BOARDS takes longer than 10ms, so use the default 20ms upstream
   slot */
#ifndef SLOTDURATION
#define SLOTDURATION                        20        /* in miliseconds */
#endif

/* These parameters are BOARD and CPU specific.
   Values cant be taken directly from OpenWSN since they do not necessarily use
   the same BSP configuration (timers, clock sepeed, etc.)
   For precise synchronization these valued should be measured and tuned for
   every BOARD.
   To understand the relationship between these values and OpenWSN state machine
   look at:
   https://openwsn.atlassian.net/wiki/spaces/OW/pages/688251/State+Machine
*/
#if SLOTDURATION == 20
#ifndef PORT_TsSlotDuration
#define PORT_TsSlotDuration                 655   /* 20ms */
#endif

/* Execution speed related parameters */
#ifndef PORT_maxTxDataPrepare
#define PORT_maxTxDataPrepare               110   /* 3355us (not measured) */
#endif
#ifndef PORT_maxRxAckPrepare
#define PORT_maxRxAckPrepare                20    /*  610us (not measured) */
#endif
#ifndef PORT_maxRxDataPrepare
#define PORT_maxRxDataPrepare               33    /* 1000us (not measured) */
#endif
#ifndef PORT_maxTxAckPrepare
#define PORT_maxTxAckPrepare                50    /* 1525us (not measured) */
#endif

/* Radio speed related parameters */
#ifndef PORT_delayTx
#if defined(BOARD_IOTLAB_M3) || defined(BOARD_IOTLAB_A8_M3) || \
    defined(BOARD_NUCLEO_F103RB)
#define PORT_delayTx                         10   /*  549us (not measured) */
#else
#define PORT_delayTx                         18   /*  549us (not measured) */
#endif
#endif
#ifndef PORT_delayRx
#define PORT_delayRx                         0    /*    0us (can not measure) */
#endif
#else
#error "Only 20ms slot duration is currently supported"
#endif
/** @} */ /* SLOTDURATION==20 */

/**
 * @name    Adaptive sync accuracy
 *
 *          Used for synchronization in heterogenous networks (different BOARDs)
 * @{
 */
#if defined(BOARD_IOTLAB_M3) || defined(BOARD_IOTLAB_A8_M3) || \
    defined(BOARD_NUCLEO_F103RB)
#define SYNC_ACCURACY                        2    /* ticks */
#else
#define SYNC_ACCURACY                        1    /* ticks */
#endif
/** @} */

static const uint8_t rreg_uriquery[] = "h=ucb";
static const uint8_t infoBoardname[] = "riot-os";
static const uint8_t infouCName[]    = "various";
static const uint8_t infoRadioName[] = "riot-netdev";

#endif  /* __BOARD_INFO_H */
