/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test for Decawave DW1000 UWB radio driver
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "dw1000.h"
#include "dw1000_params.h"

static dw1000_t dev;

int main(void)
{
    puts("Generated RIOT application: 'driver_dw1000'");

    dw1000_init(&dev, &dw1000_params[0]);
    
    return 0;
}
