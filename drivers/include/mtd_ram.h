/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mtd_ram   ram MTD
 * @ingroup     drivers_storage
 * @brief       RAM-based storage backend
 *
 * This module implements a RAM-backed MTD device. This module can be used
 * for testing, however the interface could also be used for to target backup
 * ram storage by changing @ref CONFIG_MTD_RAM_RAM_ATTR to store it in backup ram.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the ram memory driver
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef MTD_RAM_H
#define MTD_RAM_H

#include "mtd.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief MTD ram sector count
 */
#ifndef CONFIG_MTD_RAM_SECTOR_COUNT
#define CONFIG_MTD_RAM_SECTOR_COUNT     (16)
#endif
/**
 * @brief MTD ram page per sector
 */
#ifndef CONFIG_MTD_RAM_PAGE_PER_SECTOR
#define CONFIG_MTD_RAM_PAGE_PER_SECTOR  (4)
#endif
/**
 * @brief MTD ram page size
 */
#ifndef CONFIG_MTD_RAM_PAGE_SIZE
#define CONFIG_MTD_RAM_PAGE_SIZE        (64)
#endif
/**
 * @brief Extra attributes for allocating the RAM struct
 */
#ifndef CONFIG_MTD_RAM_ATTR
#define CONFIG_MTD_RAM_ATTR
#endif

/**
 * @brief MTD ram size
 */
#define CONFIG_MTD_RAM_SIZE             (CONFIG_MTD_RAM_SECTOR_COUNT * \
                                         CONFIG_MTD_RAM_PAGE_PER_SECTOR * \
                                         CONFIG_MTD_RAM_PAGE_SIZE)

/**
 * @brief   ram MTD device operations table
 */
extern const mtd_desc_t mtd_ram_driver;

/**
 * @brief    MTD ram descriptor
 */
typedef struct {
    mtd_dev_t base;                                         /**< MTD generic device */
    uint8_t mem[CONFIG_MTD_RAM_SIZE] CONFIG_MTD_RAM_ATTR;   /**< MTD ram area */
} mtd_ram_t;

#ifdef __cplusplus
}
#endif

#endif /* MTD_RAM_H */
/** @} */
