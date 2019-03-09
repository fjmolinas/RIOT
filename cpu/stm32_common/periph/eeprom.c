/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @ingroup     drivers_periph_eeprom
 * @{
 *
 * @file
 * @brief       Low-level eeprom driver implementation
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

// https://c0de.pw/bg/stlink/blob/c0e962c3787102df1a14bc09596534bdf05f7e70/example/libstm32l_discovery/src/stm32l1xx_flash.c

#include <assert.h>

#include "cpu.h"
#include "periph/eeprom.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

extern void _lock(void);
extern void _unlock(void);
extern void _wait_for_pending_operations(void);

#ifndef EEPROM_START_ADDR
#error "periph/eeprom: EEPROM_START_ADDR is not defined"
#endif

static void erase_word(uint32_t pos)
{
    assert(pos + sizeof(uint32_t) <= EEPROM_SIZE);

    /* Wait for last operation to be completed */
    _wait_for_pending_operations();

    /* Write "00000000h" to valid address in the data memory" */
    *(__IO uint32_t *) pos = 0x00000000;
}

static void write_word(uint32_t pos, uint32_t data)
{
    assert(pos + sizeof(uint32_t) <= EEPROM_SIZE);

    /* Wait for last operation to be completed */
    _wait_for_pending_operations();

    *(__IO uint32_t *) pos = data;

    /* Wait for last operation to be completed */
    _wait_for_pending_operations();
}

static void write_byte(uint32_t pos, uint8_t data)
{
    uint32_t tmp = 0, tmpaddr = 0;

    /* Check the parameters */
    assert(pos + sizeof(uint8_t) <= EEPROM_SIZE);

    /* Wait for last operation to be completed */
    _wait_for_pending_operations();

    if(data != (uint8_t) 0x00)
    {
        *(__IO uint8_t *)pos = data;

        /* Wait for last operation to be completed */
        _wait_for_pending_operations();
    }
    else
    {
        tmpaddr = pos & 0xFFFFFFFC;
        tmp = * (__IO uint32_t *) tmpaddr;
        tmpaddr = 0xFF << ((uint32_t) (0x8 * (pos & 0x3)));
        tmp &= ~tmpaddr;
        erase_word(pos & 0xFFFFFFFC);
        write_word((pos & 0xFFFFFFFC), tmp);
    }
}

size_t eeprom_read(uint32_t pos, uint8_t *data, size_t len)
{
    assert(pos + len <= EEPROM_SIZE);

    uint8_t *p = data;

    _unlock();

    DEBUG("Reading data from EEPROM at pos %" PRIu32 ": ", pos);
    for (size_t i = 0; i < len; i++) {
        _wait_for_pending_operations();
        *p++ = *(__IO uint8_t *)(EEPROM_START_ADDR + pos++);
        DEBUG("0x%02X ", *p);
    }
    DEBUG("\n");

    _wait_for_pending_operations();

    _lock();

    return len;
}

size_t eeprom_write(uint32_t pos, const uint8_t *data, size_t len)
{
    assert(pos + len <= EEPROM_SIZE);

    uint8_t *p = (uint8_t *)data;

    _unlock();

    for (size_t i = 0; i < len; i++) {
        _wait_for_pending_operations();
        write_byte((EEPROM_START_ADDR + pos++), *p++);
    }

    _wait_for_pending_operations();

    _lock();

    return len;
}
