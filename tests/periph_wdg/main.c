/*
 * Copyright (C) 2019 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test for wdg Drivers
 *
 *              This test initializes the wdg counter to a preset value
 *              and spins until a wdg reset is triggered
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "periph/wdg.h"
#include "timex.h"

#ifndef WDG_RESET_TIME
#define WDG_RESET_TIME      (2U*US_PER_SEC)
#endif

int main(void)
{
    printf("RIOT edg test application\n");
    printf("Application should reset every ~ %d seconds\n", WDG_RESET_TIME);

    wdg_init(WDG_RESET_TIME);
    wdg_enable();

    while(1){};

    return 0;
}
