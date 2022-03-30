/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mtd_writer
 * @{
 *
 * @file
 * @brief       Device driver implementation for the MTD writer helper
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <assert.h>
#include <string.h>
#include <inttypes.h>

#include "mtd_writer.h"
#include "log.h"

static inline size_t min(size_t a, size_t b)
{
    return a <= b ? a : b;
}

uint32_t _mtd_sector_size(mtd_dev_t *dev)
{
    return (dev->page_size * dev->pages_per_sector);
}

int mtd_writer_init(mtd_writer_t *state, mtd_dev_t *dev, size_t offset, uint8_t *buf, size_t buflen)
{
    assert(dev->page_size >= buflen);

    memset(state, 0, sizeof(mtd_writer_t));
    memset(buf, 0, buflen);

    state->buf = buf;
    state->buflen = buflen;
    state->dev = dev;
    state->offset = offset;
    state->sector = offset / _mtd_sector_size(dev);

    /* Erase the first sector */
    mtd_erase_sector(state->dev, state->sector, 1);

    return 0;
}

int mtd_writer_putbytes(mtd_writer_t *state, const uint8_t *bytes, size_t len, bool more)
{
    LOG_DEBUG("[mtd_writer]: bytes %u-%u\n", state->offset, state->offset + len - 1);


    while (len) {
        size_t sector_pos = state->offset - state->sector * _mtd_sector_size(state->dev);
        size_t buf_pos = state->offset % state->buflen;
        size_t avail = state->buflen - buf_pos;
        size_t to_copy = min(avail, len);

        if (sector_pos == _mtd_sector_size(state->dev)) {
            state->sector++;
            LOG_DEBUG("[mtd_writer]: erase sector %" PRIu32 "\n", state->sector);
            mtd_erase_sector(state->dev, state->sector, 1);
        }
        if (buf_pos == 0) {
            memset(state->buf, 0, state->buflen);
        }

        memcpy(state->buf + buf_pos, bytes, to_copy);
        avail -= to_copy;
        state->offset += to_copy;
        len -= to_copy;

        if (!avail || !more) {
            /* */
            uint32_t offset = ((state->offset - 1) / state->buflen) * state->buflen;
            LOG_DEBUG("[mtd_writer]: sector-offset %" PRIu32 "-%" PRIu32 "\n", state->sector,
                      offset);
            if (mtd_write_page_raw(state->dev, state->buf, 0, offset, state->buflen - avail)) {
                LOG_WARNING("[mtd_writer]: write failed\n");
            }
        }

    }
    return 0;
}

int mtd_writer_flush(mtd_writer_t *state)
{
    size_t buf_pos = state->offset % state->buflen;

    if (buf_pos) {

        uint32_t offset = ((state->offset - 1) / state->buflen) * state->buflen;
        mtd_write_page_raw(state->dev, state->buf, 0, offset, state->buflen);
    }
    return 0;
}
