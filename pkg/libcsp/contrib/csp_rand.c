/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file
 * @brief       LibCSP random functions
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include <stdlib.h>

#include "random.h"
#include "csp/arch/csp_rand.h"

uint8_t csp_rand8(void)
{
    return (uint8_t)random_uint32();
}
uint16_t csp_rand16(void)
{
    return (uint16_t)random_uint32();
}
