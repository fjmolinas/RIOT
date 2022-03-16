/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_thread_fn_bench Thread Function Benchmark
 * @ingroup     sys
 * @brief       Prints stack usage of function
 *
 * @{
 *
 * @file
 *
 * @author      Francisco Molina <femolina@uc.cl>
 */

#ifndef THREAD_FN_BENCH_H
#define THREAD_FN_BENCH_H

#include <inttypes.h>
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_THREAD_FN_BENCH_STACKSIZE
#define CONFIG_THREAD_FN_BENCH_STACKSIZE    THREAD_STACKSIZE_LARGE
#endif

void thread_fn_bench(void *(*fn)(void *), void *arg, uint8_t prio, char* name);

#ifdef __cplusplus
}
#endif

#endif /* THREAD_FN_BENCH_H */
/** @} */
