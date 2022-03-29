/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       mtd_mapper module test
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdint.h>
#include <errno.h>
#include <string.h>

#include "embUnit.h"

#include "mtd.h"
#include "mtd_mapper.h"
#include "mtd_ram.h"

#define MEMORY_PAGE_COUNT    (CONFIG_MTD_RAM_PAGE_PER_SECTOR * CONFIG_MTD_RAM_SECTOR_COUNT)

#define MEMORY_SIZE          (CONFIG_MTD_RAM_PAGE_SIZE * MEMORY_PAGE_COUNT)

#define REGION_FLASH_SIZE    (MEMORY_SIZE / 2)

#define REGION_PAGE_COUNT    (MEMORY_PAGE_COUNT / 2)


static uint8_t _buffer[CONFIG_MTD_RAM_PAGE_SIZE];

extern const mtd_desc_t mtd_ram_driver;
static mtd_ram_t dev = {
    .base = {
        .driver = &mtd_ram_driver,
        .sector_count = CONFIG_MTD_RAM_SECTOR_COUNT,
        .pages_per_sector = CONFIG_MTD_RAM_PAGE_PER_SECTOR,
        .page_size = CONFIG_MTD_RAM_PAGE_SIZE
    }
};

static mtd_mapper_parent_t _parent = MTD_PARENT_INIT(&dev.base);

static mtd_mapper_region_t _region_a = {
    .mtd = {
        .driver = &mtd_mapper_driver,
        .sector_count = CONFIG_MTD_RAM_SECTOR_COUNT / 2,
        .pages_per_sector = CONFIG_MTD_RAM_PAGE_PER_SECTOR,
        .page_size = CONFIG_MTD_RAM_PAGE_SIZE,
    },
    .parent = &_parent,
    .sector = 0,
};

static mtd_mapper_region_t _region_b = {
    .mtd = {
        .driver = &mtd_mapper_driver,
        .sector_count = CONFIG_MTD_RAM_SECTOR_COUNT / 2,
        .pages_per_sector = CONFIG_MTD_RAM_PAGE_PER_SECTOR,
        .page_size = CONFIG_MTD_RAM_PAGE_SIZE,
    },
    .parent = &_parent,
    .sector = CONFIG_MTD_RAM_SECTOR_COUNT / 2,
};

static mtd_dev_t *_dev_a = &_region_a.mtd;
static mtd_dev_t *_dev_b = &_region_b.mtd;

static void _test_mem(uint8_t *buffer, size_t len, uint8_t expected)
{
    for (size_t i = 0; i < len; i++) {
        TEST_ASSERT_EQUAL_INT(expected, buffer[i]);
    }
}

static void test_mtd_init(void)
{
    int ret = mtd_init(_dev_a);

    TEST_ASSERT_EQUAL_INT(0, ret);
    ret = mtd_init(_dev_b);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

static void test_mtd_erase(void)
{
    /* Erase first region */
    int ret = mtd_erase(_dev_a, 0, REGION_FLASH_SIZE);

    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Erase second region */
    ret = mtd_erase(_dev_b, 0, REGION_FLASH_SIZE);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = mtd_erase(_dev_a, REGION_FLASH_SIZE, 1);
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);

    ret = mtd_erase(_dev_b, REGION_FLASH_SIZE, 1);
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);
}

static void test_mtd_read(void)
{
    for (uint32_t i = 0; i < REGION_FLASH_SIZE; i += CONFIG_MTD_RAM_PAGE_SIZE) {
        mtd_read(_dev_a, _buffer, i, CONFIG_MTD_RAM_PAGE_SIZE);
        _test_mem(_buffer, CONFIG_MTD_RAM_PAGE_SIZE, 0xff);
    }

    for (uint32_t i = 0; i < REGION_FLASH_SIZE; i += CONFIG_MTD_RAM_PAGE_SIZE) {
        mtd_read(_dev_b, _buffer, i, CONFIG_MTD_RAM_PAGE_SIZE);
        _test_mem(_buffer, CONFIG_MTD_RAM_PAGE_SIZE, 0xff);
    }
}

static void test_mtd_read_page(void)
{
    int ret;

    for (uint32_t i = 0; i < REGION_PAGE_COUNT; i += 1) {
        ret = mtd_read_page(_dev_a, _buffer, i, 0, CONFIG_MTD_RAM_PAGE_SIZE);
        TEST_ASSERT_EQUAL_INT(0, ret);
        _test_mem(_buffer, CONFIG_MTD_RAM_PAGE_SIZE, 0xff);
    }

    for (uint32_t i = 0; i < REGION_PAGE_COUNT; i += 1) {
        ret = mtd_read_page(_dev_b, _buffer, i, 0, CONFIG_MTD_RAM_PAGE_SIZE);
        TEST_ASSERT_EQUAL_INT(0, ret);
        _test_mem(_buffer, CONFIG_MTD_RAM_PAGE_SIZE, 0xff);
    }

    ret = mtd_read_page(_dev_b, _buffer, REGION_PAGE_COUNT, 0, CONFIG_MTD_RAM_PAGE_SIZE);
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);
}

static void test_mtd_write(void)
{
    static const uint8_t test_val_a = 0xAA;

    memset(_buffer, test_val_a, CONFIG_MTD_RAM_PAGE_SIZE);

    /* Write first region */
    for (uint32_t i = 0; i < REGION_FLASH_SIZE; i += CONFIG_MTD_RAM_PAGE_SIZE) {
        mtd_write(_dev_a, _buffer, i, CONFIG_MTD_RAM_PAGE_SIZE);
    }

    /* Check second region, should still be 0xFF */
    for (uint32_t i = 0; i < REGION_FLASH_SIZE; i += CONFIG_MTD_RAM_PAGE_SIZE) {
        mtd_read(_dev_b, _buffer, i, CONFIG_MTD_RAM_PAGE_SIZE);
        _test_mem(_buffer, CONFIG_MTD_RAM_PAGE_SIZE, 0xFF);
    }

    static const uint8_t test_val_b = 0xBB;

    memset(_buffer, test_val_b, CONFIG_MTD_RAM_PAGE_SIZE);

    /* Write second region */
    for (uint32_t i = 0; i < REGION_FLASH_SIZE; i += CONFIG_MTD_RAM_PAGE_SIZE) {
        mtd_write(_dev_b, _buffer, i, CONFIG_MTD_RAM_PAGE_SIZE);
    }

    /* Check second region after write, should now be 0xBB */
    for (uint32_t i = 0; i < REGION_FLASH_SIZE; i += CONFIG_MTD_RAM_PAGE_SIZE) {
        mtd_read(_dev_b, _buffer, i, CONFIG_MTD_RAM_PAGE_SIZE);
        _test_mem(_buffer, CONFIG_MTD_RAM_PAGE_SIZE, 0xBB);
    }

    /* Check first region, should still be 0xAA */
    for (uint32_t i = 0; i < REGION_FLASH_SIZE; i += CONFIG_MTD_RAM_PAGE_SIZE) {
        mtd_read(_dev_a, _buffer, i, CONFIG_MTD_RAM_PAGE_SIZE);
        _test_mem(_buffer, CONFIG_MTD_RAM_PAGE_SIZE, 0xAA);
    }
}

static void test_mtd_write_page(void)
{
    static const uint8_t test_val_a = 0xAA;
    int ret;

    memset(_buffer, test_val_a, CONFIG_MTD_RAM_PAGE_SIZE);

    /* Write first region */
    for (uint32_t i = 0; i < REGION_PAGE_COUNT; i += 1) {
        ret = mtd_write_page(_dev_a, _buffer, i, 0, CONFIG_MTD_RAM_PAGE_SIZE);
        TEST_ASSERT_EQUAL_INT(0, ret);
    }

    /* Check second region, should still be 0xFF */
    for (uint32_t i = 0; i < REGION_PAGE_COUNT; i += 1) {
        ret = mtd_read_page(_dev_b, _buffer, i, 0, CONFIG_MTD_RAM_PAGE_SIZE);
        TEST_ASSERT_EQUAL_INT(0, ret);
        _test_mem(_buffer, CONFIG_MTD_RAM_PAGE_SIZE, 0xFF);
    }

    static const uint8_t test_val_b = 0xBB;

    memset(_buffer, test_val_b, CONFIG_MTD_RAM_PAGE_SIZE);

    /* Write second region */
    for (uint32_t i = 0; i < REGION_PAGE_COUNT; i += 1) {
        ret = mtd_write_page(_dev_b, _buffer, i, 0, CONFIG_MTD_RAM_PAGE_SIZE);
        TEST_ASSERT_EQUAL_INT(0, ret);
    }

    /* Check second region after write, should now be 0xBB */
    for (uint32_t i = 0; i < REGION_PAGE_COUNT; i += 1) {
        ret = mtd_read_page(_dev_b, _buffer, i, 0, CONFIG_MTD_RAM_PAGE_SIZE);
        TEST_ASSERT_EQUAL_INT(0, ret);
        _test_mem(_buffer, CONFIG_MTD_RAM_PAGE_SIZE, 0xBB);
    }

    /* Check first region, should still be 0xAA */
    for (uint32_t i = 0; i < REGION_PAGE_COUNT; i += 1) {
        ret = mtd_read_page(_dev_a, _buffer, i, 0, CONFIG_MTD_RAM_PAGE_SIZE);
        TEST_ASSERT_EQUAL_INT(0, ret);
        _test_mem(_buffer, CONFIG_MTD_RAM_PAGE_SIZE, 0xAA);
    }

    ret = mtd_write_page(_dev_b, _buffer, REGION_PAGE_COUNT, 0, CONFIG_MTD_RAM_PAGE_SIZE);
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);
}

static void set_up(void)
{
    memset(dev.mem, 0xff, sizeof(dev.mem));
}

Test *tests_mtd_mapper_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_mtd_init),
        new_TestFixture(test_mtd_erase),
        new_TestFixture(test_mtd_read),
        new_TestFixture(test_mtd_read_page),
        new_TestFixture(test_mtd_write),
        new_TestFixture(test_mtd_write_page),
    };

    EMB_UNIT_TESTCALLER(mtd_flashpage_tests, set_up, NULL, fixtures);

    return (Test *)&mtd_flashpage_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_mtd_mapper_tests());
    TESTS_END();
    return 0;
}
