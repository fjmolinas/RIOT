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

#ifndef MYNEWT_TYPES_H
#define MYNEWT_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Macro to wait forever on events and mutexes
 * @{
 */
#define MYNEWT_TIMEOUT_NEVER   (UINT32_MAX)
#define MYNEWT_WAIT_FOREVER    (MYNEWT_TIMEOUT_NEVER)
/** @} */

/**
 * @name    Decawave porting layer (DPL) stack alignment requirement
 * @{
 */
#define MYNEWT_STACK_ALIGNMENT (4)
/** @} */

/**
 * @brief dpl time type
 */
typedef uint32_t mynewt_time_t;

/**
 * @brief dpl stack buffer type
 */
typedef char mynewt_stack_t;

#ifdef __cplusplus
}
#endif

#endif /* MYNEWT_TYPES_H */
