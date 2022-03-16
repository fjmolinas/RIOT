/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     module_thread_fn_bench
 * @{
 *
 * @file
 * @brief       Thread Function Benchmark implementation
 *
 * @author      Francisco Molina <femolina@uc.cl>
 *
 * @}
 */
#include <stdio.h>
#include <string.h>

#include "thread_fn_bench.h"
#include "thread.h"
#include "ztimer.h"

static char _stack[CONFIG_THREAD_FN_BENCH_STACKSIZE];

void thread_fn_bench(void *(*fn)(void *), void *arg, uint8_t prio, char* name)
{
    kernel_pid_t pid = thread_create(_stack, sizeof(_stack), prio,
                                     THREAD_CREATE_STACKTEST | THREAD_CREATE_WOUT_YIELD,
                                     fn, arg, "thread-fn-bench");
    thread_t *thread = thread_get(pid);

    uint32_t start = ztimer_now(ZTIMER_USEC);
    thread_yield();
    uint32_t stop = ztimer_now(ZTIMER_USEC);
    if (name) {
        printf("%s: ",name);
    }
    printf("stack usage %d/%d, %"PRIu32"us (%"PRIu32"ms)\n",
           sizeof(_stack) - thread_measure_stack_free(thread_get_stackstart(thread)),
           sizeof(_stack), stop - start, (stop - start) / 1000);
    memset(_stack, '\0', sizeof(_stack));
}

