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
 * @brief       Abstraction layer for RIOT adaption
 *
 */
#ifndef OS_DEV_H
#define OS_DEV_H

#include <assert.h>
#include <stdint.h>
#include "dpl/dpl.h"

#include "os/queue.h"

#include "net/ieee802154.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Device structure.
 */
struct os_dev {
    netdev_ieee802154_t netdev;        /**< Netdev parent struct */
};

#define OS_DEV_SETHANDLERS(__dev, __open, __close)          \
    (void) __dev;                \
    (void) __open;                \
    (void) __close;

#ifdef __cplusplus
}
#endif

#endif /* OS_DEV_H */
