/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @defgroup    sys_suit_storage_mtd  riotboot mtd storage backend
 * @ingroup     sys_suit_storage
 * @brief       SUIT riotboot firmware storage backend
 *
 * @{
 *
 * @brief       riotboot mtd storage backend functions for SUIT manifests
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef SUIT_STORAGE_MTD_H
#define SUIT_STORAGE_MTD_H

#include "suit.h"
#include "../../drivers/include/mtd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Storage location string separators
 */
#ifndef CONFIG_SUIT_STORAGE_MTD_LOCATION_SEPARATOR
#define CONFIG_SUIT_STORAGE_MTD_LOCATION_SEPARATOR '.'
#endif

/**
 * @brief Storage location string prefix
 *
 * Must include the leading and trailing separators
 */
#ifndef CONFIG_SUIT_STORAGE_MTD_LOCATION_PREFIX
#define CONFIG_SUIT_STORAGE_MTD_LOCATION_PREFIX  ".mtd."
#endif

/**
 * @brief MTD regions
 */
typedef struct {
    mtd_dev_t *dev;                 /**< MTD device */
    size_t occupied;                /**< Region space filled */
} suit_storage_mtd_region_t;

/**
 * @brief riotboot mtd SUIT storage context
 */
typedef struct {
    suit_storage_t storage;             /**< parent struct */
    uint32_t sequence_no;               /**< Ephemeral sequence number */
    suit_storage_mtd_region_t *regions; /**< */
    size_t active_region;               /**< Active region to write to */
} suit_storage_mtd_t;

#ifdef __cplusplus
}
#endif

#endif /* SUIT_STORAGE_MTD_H */
/** @} */
