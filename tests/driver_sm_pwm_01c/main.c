/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 * @file
 * @brief       Test application for SM_PWM_01C driver
 *
 * @author      Francisco Molina <francisco.molina@inria.cl>
 * @}
 */

#include <stdio.h>

#include "xtimer.h"

#include "sm_pwm_01c.h"
#include "sm_pwm_01c_params.h"

int main(void)
{
    sm_pwm_01c_t dev;

    puts("sm_pwm_01c driver test application\n");
    if (sm_pwm_01c_init(&dev,  &sm_pwm_01c_params[0]) != 0) {
        puts("init device [ERROR]");
        return -1;
    }

    puts("starting weighted average measurement of particle concentration ");
    sm_pwm_01c_start(&dev);

    while (1) {
        xtimer_usleep(200*US_PER_MS);
        printf("tsp conc %"PRIu16" ug/m3\n", sm_pwm_01c_read_tsp(&dev));
        printf("tlp conc %"PRIu16" ug/m3\n", sm_pwm_01c_read_tlp(&dev));
    }

    return 0;
}
