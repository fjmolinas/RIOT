/*
 * Copyright (C) 2018 Hamburg University of Applied Sciences
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 *
 * @{
 *
 * @file
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */
#ifndef OPENWSN_H
#define OPENWSN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "thread.h"
#include "event.h"

/**
 * @brief   Default PANID for OpenWSN network
 */
#ifndef  OPENWSN_PANID
#define  OPENWSN_PANID  (0xCAFE)
#endif

/**
 * @brief   Initializes OpenWSN thread
 *
 * @return  0 if initialized
 * @return  -1 on initialization error
 */
int openwsn_bootstrap(void);

/**
 * @brief   get PID of OpenWsn scheduler thread.
 *
 * @return  PID of OpenWsn scheduler thread
 */
kernel_pid_t openwsn_sched_pid(void);

/**
 * @brief   get PID of OpenWsn TSCH thread.
 *
 * @return  PID of OpenWsn TSCH thread
 */
kernel_pid_t openwsn_tsch_pid(void);

event_queue_t* openwsn_tsch_radio_evq(void);

event_queue_t* openwsn_tsch_timer_evq(void);

#ifdef __cplusplus
}
#endif

#endif /* OPENWSN_H */
/** @} */
