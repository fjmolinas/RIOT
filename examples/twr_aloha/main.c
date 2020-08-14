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
 * @brief       Compile test for pkg_uwb_core
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "control.h"

int main(void)
{
    puts("pkg uwb-dw1000 + uwb-core test application");
    /* this should start ranging... */
    init_ranging(NULL);
    return 1;
}
