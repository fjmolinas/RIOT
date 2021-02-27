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
 * @brief       uwb-core DPL (Decawave Porting Layer) thread/task wrappers
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef OS_TASKS_H
#define OS_TASKS_H

#include "os/types.h"

#include "sched.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief dpl task wrapper
 */
struct os_task {
    kernel_pid_t pid;   /**< the process id */
};

/**
 * @brief dpl task function
 */
typedef thread_task_func_t os_task_func_t;

/**
 * @brief Initialize a task.
 *
 * This function initializes the task structure pointed to by t,
 * clearing and setting it's stack pointer, provides sane defaults
 * and sets the task as ready to run, and inserts it into the operating
 * system scheduler.
 *
 * @param[in]   t               the task to initialize
 * @param[in]   name            task name
 * @param[in]   func            task function to call
 * @param[in]   arg             argument to pass in task init function
 * @param[in]   prio            task priority
 * @param[in]   sanity_itvl     UNUSED
 * @param[in]   stack_bottom    pointer to bottom of the stack
 * @param[in]   stack_size      task stack size
 *
 * @return 0 on success, non-zero on failure.
 */
int os_task_init(struct os_task *t, const char *name, os_task_func_t func,
                  void *arg, uint8_t prio, os_time_t sanity_itvl,
                  os_stack_t *stack_bottom, uint16_t stack_size);

/**
 * @brief removes specified task
 *
 * NOTE: This interface is currently experimental and not ready for common use
 */
int os_task_remove(struct os_task *t);

/**
 * @brief Return the number of tasks initialized.
 *
 * @return number of tasks initialized
 */
uint8_t os_task_count(void);

/**
 * @brief   Lets current thread yield.
 */
void os_task_yield(void);

#ifdef __cplusplus
}
#endif

#endif /* OS_TASKS_H */
