/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup
 * @ingroup
 * @brief
 *
 * @{
 *
 * @file
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>
#include "event.h"
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

#define     CONTROLLER_STATUS_BRIGHTNESS         (1 << 0)
#define     CONTROLLER_STATUS_BLE_STDIO          (1 << 1)
#define     CONTROLLER_STATUS_BLE_TIME_ADV       (1 << 2)
#define     CONTROLLER_STATUS_BLE_AUTO_CONN      (1 << 3)

typedef struct controller {
    mutex_t lock;
    uint8_t brightness;
    uint8_t status;
} controller_t;

void controller_init(event_queue_t *queue);

void controller_status_changed(void);

controller_t* controller_adquire(void);

int controller_release(void);

#ifdef __cplusplus
}
#endif

#endif /* CONTROLLER_H */
/** @} */
