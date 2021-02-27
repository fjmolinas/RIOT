/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       uwb-core DPL (Decawave Porting Layer) types
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef OS_TYPES_H
#define OS_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Macro to wait forever on events and mutexes
 * @{
 */
#define OS_TIMEOUT_NEVER   (UINT32_MAX)
#define OS_WAIT_FOREVER    (OS_TIMEOUT_NEVER)
/** @} */

/**
 * @brief dpl time type
 */
typedef uint32_t os_time_t;

/**
 * @brief dpl stack buffer type
 */
typedef char os_stack_t;

#ifdef __cplusplus
}
#endif

#endif /* OS_TYPES_H */
