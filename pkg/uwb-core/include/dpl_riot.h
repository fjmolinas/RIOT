/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


#ifndef DPL_RIOT_H
#define DPL_RIOT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Priority used for
 */
#ifndef UWB_CORE__PRIO
#define UWB_CORE__PRIO            (THREAD_PRIORITY_MAIN - 2)
#endif

/**
 * @brief   Stacksize used for
 */
#ifndef UWB_CORE__STACKSIZE
#define UWB_CORE__STACKSIZE       (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   Setup and run uwb-core thread
 */
void uwb_core_riot_init(void);

#ifdef __cplusplus
}
#endif

#endif /* DPL_RIOT_H */
/** @} */
