/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_dw1000
 * @{
 *
 * @file
 * @brief       Netdev interface to Decawave DW1000 UWB radio driver
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef DW1000_NETDEV_H
#define DW1000_NETDEV_H

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev_driver_t dw1000_driver;

#ifdef __cplusplus
}
#endif

#endif /* DW1000_NETDEV_H */
/** @} */
