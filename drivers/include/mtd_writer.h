/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mtd_writer MTD writer helper
 * @ingroup     drivers_storage
 * @brief       A helper for writing with no read-modify-write cycles
 *
 * @{
 *
 * @file
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef MTD_WRITER_H
#define MTD_WRITER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "mtd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    mtd_dev_t *dev;
    size_t offset;
    uint32_t sector;
    uint8_t * buf;
    size_t buflen;
} mtd_writer_t;

/**
 * @brief   Initialize the given device
 *
 * @param[inout] dev        Device descriptor of the driver
 * @param[in]    params     Initialization parameters
 *
 * @return                  0 on success
 */
int mtd_writer_init(mtd_writer_t *state, mtd_dev_t *dev, size_t offset, uint8_t *buf, size_t buflen);
int mtd_writer_putbytes(mtd_writer_t *state, const uint8_t *bytes, size_t len, bool more);
int mtd_writer_flush(mtd_writer_t *state);

#ifdef __cplusplus
}
#endif

#endif /* MTD_WRITER_H */
/** @} */
