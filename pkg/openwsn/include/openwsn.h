/*
 * Copyright (C) 2018 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_openwsn    OpenWSN
 * @ingroup     pkg
 * @brief       An IoT Network Stack Implementing 6TiSCH
 * @see         https://github.com/openwsn-berkeley/openwsn-fw
 *
 *
 * @{
 *
 * @file
 *
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */
#ifndef OPENWSN_H
#define OPENWSN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "thread.h"

/**
 * @brief   Default PANID for OpenWSN network
 */
#ifndef  OPENWSN_PANID
#define  OPENWSN_PANID  (0xCAFE)
#endif

/**
 * @brief   Initializes OpenWSN thread
 *
 * @return  PID of OpenWSN thread
 * @return  -1 on initialization error
 */
int openwsn_bootstrap(void);

/**
 * @brief   get PID of OpenWsn thread.
 *
 * @return  PID of OpenWsn thread
 */
kernel_pid_t openwsn_get_pid(void);

#ifdef __cplusplus
}
#endif

#endif /* OPENWSN_H */
/** @} */
