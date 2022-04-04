/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_suit_storage
 * @{
 *
 * @file
 * @brief       SUIT mtd storage module implementation
 *
 *
 * @}
 */
#include <string.h>
#include <inttypes.h>

#include "board.h"

#include "xfa.h"
#include "fmt.h"
#include "kernel_defines.h"
#include "log.h"

#include "mtd.h"
#include "suit.h"
#include "suit/storage.h"
#include "suit/storage/mtd.h"

XFA_USE(suit_storage_t, suit_storage_reg);
XFA_INIT(suit_storage_mtd_region_t, suit_storage_mtd_reg);

static inline size_t _get_mtd_sector_size(suit_storage_mtd_region_t *region)
{
    return region->dev->page_size * region->dev->pages_per_sector;
}

static inline size_t _get_mtd_size(suit_storage_mtd_region_t *region)
{
    return _get_mtd_sector_size(region) * region->dev->sector_count;
}

static inline suit_storage_mtd_t *_get_mtd(suit_storage_t *storage)
{
    return container_of(storage, suit_storage_mtd_t, storage);
}

static inline const suit_storage_mtd_t *_get_mtd_const(const suit_storage_t *storage)
{
    return container_of(storage, suit_storage_mtd_t, storage);
}

static inline suit_storage_mtd_region_t *_get_active_region(suit_storage_mtd_t *mtd)
{
    return &mtd->regions[mtd->active_region];
}

static bool _get_region_by_string(const char *location, uint32_t *val)
{
    /* Matching on .mtd.### */
    static const char prefix[] = CONFIG_SUIT_STORAGE_MTD_LOCATION_PREFIX;
    static const size_t prefix_len = sizeof(prefix) - 1;

    /* Check for prefix */
    if (strncmp(prefix, location, prefix_len) == 0 &&
        location[prefix_len] != '\n') {
        /* Advance to the number */
        location += prefix_len;
        /* Check if the rest of the string is a number */
        if (fmt_is_number(location)) {
            /* grab the number */
            *val = scn_u32_dec(location, 5);
            /* Number must be smaller than the number of regions */
            if (*val < XFA_LEN(suit_storage_mtd_region_t, suit_storage_mtd_reg)) {
                return true;
            }
        }
    }

    return false;
}

static int _mtd_init(suit_storage_t *storage)
{
    suit_storage_mtd_t *mtd = _get_mtd(storage);

    mtd->regions = &suit_storage_mtd_reg[0];
    mtd->sequence_no = 0;

    for (size_t i = 0; i < XFA_LEN(suit_storage_mtd_region_t, suit_storage_mtd_reg); i++) {
        suit_storage_mtd_region_t *region = &suit_storage_mtd_reg[i];
        if (!region->dev) {
            return SUIT_ERR_STORAGE;
        }
        if (mtd_init(region->dev)) {
            return SUIT_ERR_STORAGE;
        }
    }

    return SUIT_OK;
}

static int _mtd_start(suit_storage_t *storage, const suit_manifest_t *manifest,
                      size_t len)
{
    (void)manifest;
    suit_storage_mtd_t *mtd = _get_mtd(storage);
    suit_storage_mtd_region_t *region = _get_active_region(mtd);

    if (len > _get_mtd_size(region)) {
        return SUIT_ERR_STORAGE_EXCEEDED;
    }
    region->occupied = 0;

    return SUIT_OK;
}

static int _mtd_write(suit_storage_t *storage, const suit_manifest_t *manifest,
                      const uint8_t *buf, size_t offset, size_t len)
{
    (void)manifest;
    suit_storage_mtd_t *mtd = _get_mtd(storage);
    suit_storage_mtd_region_t *region = _get_active_region(mtd);

    if (offset + len > _get_mtd_size(region)) {
        return SUIT_ERR_STORAGE_EXCEEDED;
    }
    mtd_write_page_raw(region->dev, buf, 0, offset, len);
    region->occupied += len;
    return SUIT_OK;
}

static int _mtd_finish(suit_storage_t *storage, const suit_manifest_t *manifest)
{
    (void)storage;
    (void)manifest;
    return SUIT_OK;
}

static int _mtd_install(suit_storage_t *storage, const suit_manifest_t *manifest)
{
    (void)manifest;
    (void)storage;
    return SUIT_OK;
}

static int _mtd_erase(suit_storage_t *storage)
{
    suit_storage_mtd_t *mtd = _get_mtd(storage);
    suit_storage_mtd_region_t *region = _get_active_region(mtd);

    if (region->occupied == 0) {
        return SUIT_OK;
    }

    /* round up sectors */
    uint32_t sectors = ((region->occupied - 1) / _get_mtd_sector_size(region)) + 1;

    if (mtd_erase_sector(region->dev, 0, sectors)) {
        return SUIT_ERR_STORAGE;
    }
    return SUIT_OK;
}

static int _mtd_read(suit_storage_t *storage, uint8_t *buf, size_t offset,
                     size_t len)
{
    suit_storage_mtd_t *mtd = _get_mtd(storage);
    suit_storage_mtd_region_t *region = _get_active_region(mtd);

    if (offset + len > _get_mtd_size(region)) {
        return SUIT_ERR_STORAGE_EXCEEDED;
    }

    if (mtd_read_page(region->dev, buf, 0, offset, len)) {
        return SUIT_ERR_STORAGE;
    }

    return SUIT_OK;
}

static bool _mtd_has_location(const suit_storage_t *storage, const char *location)
{
    (void)storage;
    uint32_t val;

    return _get_region_by_string(location, &val);
}

static int _mtd_set_active_location(suit_storage_t *storage,
                                    const char *location)
{
    suit_storage_mtd_t *mtd = _get_mtd(storage);
    uint32_t region = 0;

    if (!_get_region_by_string(location, &region)) {
        return -1;
    }

    mtd->active_region = region;
    return SUIT_OK;
}

static int _mtd_get_seq_no(const suit_storage_t *storage, uint32_t *seq_no)
{
    const suit_storage_mtd_t *mtd = _get_mtd_const(storage);

    *seq_no = mtd->sequence_no;
    LOG_DEBUG("Retrieved sequence number: %" PRIu32 "\n", *seq_no);
    return SUIT_OK;
}

static int _mtd_set_seq_no(suit_storage_t *storage, uint32_t seq_no)
{
    suit_storage_mtd_t *mtd = _get_mtd(storage);

    if (mtd->sequence_no < seq_no) {
        LOG_DEBUG("Stored sequence number: %" PRIu32 "\n", seq_no);
        mtd->sequence_no = seq_no;
        return SUIT_OK;
    }

    return SUIT_ERR_SEQUENCE_NUMBER;
}

const suit_storage_driver_t suit_storage_mtd_driver = {
    .init = _mtd_init,
    .start = _mtd_start,
    .write = _mtd_write,
    .finish = _mtd_finish,
    .read = _mtd_read,
    .install = _mtd_install,
    .erase = _mtd_erase,
    .has_location = _mtd_has_location,
    .set_active_location = _mtd_set_active_location,
    .get_seq_no = _mtd_get_seq_no,
    .set_seq_no = _mtd_set_seq_no,
    .separator = CONFIG_SUIT_STORAGE_MTD_LOCATION_SEPARATOR,
};

suit_storage_mtd_t suit_storage_mtd = {
    .storage = {
        .driver = &suit_storage_mtd_driver,
    }
};
XFA(suit_storage_reg, 0) suit_storage_t* suit_storage_mtd_ptr = &suit_storage_mtd.storage;
