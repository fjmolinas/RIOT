/*
 * Copyright (C) 2022 Koen Zandberg
 *               2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mtd_ram
 * @{
 *
 * @file
 * @brief       MTD Ram storage module implementation
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */
#include <stdint.h>
#include <errno.h>
#include <string.h>

#include "kernel_defines.h"
#include "mtd.h"
#include "mtd_ram.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

static int _init(mtd_dev_t *dev)
{
    (void)dev;
    return 0;
}

static int _read(mtd_dev_t *dev, void *buff, uint32_t addr, uint32_t size)
{
    mtd_ram_t *mtd_ram = container_of(dev, mtd_ram_t, base);

    if (addr + size > sizeof(mtd_ram->mem)) {
        return -EOVERFLOW;
    }
    memcpy(buff, mtd_ram->mem + addr, size);

    return 0;
}

static int _read_page(mtd_dev_t *dev, void *buff, uint32_t page, uint32_t offset, uint32_t size)
{
    mtd_ram_t *mtd_ram = container_of(dev, mtd_ram_t, base);
    uint32_t addr = page * dev->page_size + offset;

    if (page >= dev->sector_count * dev->pages_per_sector) {
        return -EOVERFLOW;
    }

    if (offset > dev->page_size) {
        return -EOVERFLOW;
    }

    size = MIN(dev->page_size - offset, size);

    memcpy(buff, mtd_ram->mem + addr, size);

    return size;
}

static int _write(mtd_dev_t *dev, const void *buff, uint32_t addr,
                  uint32_t size)
{
    mtd_ram_t *mtd_ram = container_of(dev, mtd_ram_t, base);

    if (addr + size > sizeof(mtd_ram->mem)) {
        return -EOVERFLOW;
    }
    if (size > CONFIG_MTD_RAM_PAGE_SIZE) {
        return -EOVERFLOW;
    }
    memcpy(mtd_ram->mem + addr, buff, size);

    return 0;
}

static int _write_page(mtd_dev_t *dev, const void *buff, uint32_t page, uint32_t offset,
                       uint32_t size)
{
    mtd_ram_t *mtd_ram = container_of(dev, mtd_ram_t, base);
    uint32_t addr = page * dev->page_size + offset;

    if (page >= dev->sector_count * dev->pages_per_sector) {
        return -EOVERFLOW;
    }

    if (offset > dev->page_size) {
        return -EOVERFLOW;
    }

    size = MIN(dev->page_size - offset, size);

    memcpy(mtd_ram->mem + addr, buff, size);

    return size;
}

static int _erase(mtd_dev_t *dev, uint32_t addr, uint32_t size)
{
    mtd_ram_t *mtd_ram = container_of(dev, mtd_ram_t, base);

    if (size % (CONFIG_MTD_RAM_PAGE_PER_SECTOR * CONFIG_MTD_RAM_PAGE_SIZE) != 0) {
        return -EOVERFLOW;
    }
    if (addr % (CONFIG_MTD_RAM_PAGE_PER_SECTOR * CONFIG_MTD_RAM_PAGE_SIZE) != 0) {
        return -EOVERFLOW;
    }
    if (addr + size > sizeof(mtd_ram->mem)) {
        return -EOVERFLOW;
    }
    memset(mtd_ram->mem + addr, 0xff, size);

    return 0;
}

static int _erase_sector(mtd_dev_t *dev, uint32_t sector, uint32_t count)
{
    mtd_ram_t *mtd_ram = container_of(dev, mtd_ram_t, base);
    uint32_t addr = sector * dev->page_size * dev->pages_per_sector;

    if (sector + count > dev->sector_count) {
        return -EOVERFLOW;
    }

    memset(mtd_ram->mem + addr, 0xff,
           count * dev->page_size * dev->pages_per_sector);

    return 0;
}

static int _power(mtd_dev_t *dev, enum mtd_power_state power)
{
    (void)dev;
    (void)power;
    return 0;
}

const mtd_desc_t mtd_ram_driver = {
    .init = _init,
    .read = _read,
    .write = _write,
    .erase = _erase,
    .power = _power,
    .read_page = _read_page,
    .write_page = _write_page,
    .erase_sector = _erase_sector,
    .flags = MTD_DRIVER_FLAG_DIRECT_WRITE,
};
