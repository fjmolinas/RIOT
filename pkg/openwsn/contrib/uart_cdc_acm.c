/*
 * Copyright (C) 2018 Hamburg University of Applied Sciences
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       RIOT adaption of the OpenWSN "uart" bsp module.
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdatomic.h>

#include "usb/usbus.h"
#include "usb/usbus/cdc/acm.h"
#include "isrpipe.h"
#include "ztimer.h"

#include "openwsn_uart.h"

#define XOFF            0x13
#define XON             0x11
#define XONXOFF_ESCAPE  0x12
#define XONXOFF_MASK    0x10

typedef struct {
    uart_tx_cbt txCb;
    uart_rx_cbt rxCb;
    atomic_bool fXonXoffEscaping;
    atomic_char xonXoffEscapedByte;
} uart_vars_t;

static atomic_char _uart_rx_byte;
static uart_vars_t _uart_vars;
static uint8_t _cdc_tx_buf_mem[CONFIG_USBUS_CDC_ACM_STDIO_BUF_SIZE];
static uint8_t _cdc_rx_buf_mem[CONFIG_USBUS_CDC_ACM_STDIO_BUF_SIZE];
static isrpipe_t _cdc_stdio_isrpipe = ISRPIPE_INIT(_cdc_rx_buf_mem);
static usbus_cdcacm_device_t cdcacm;

static ztimer_t _ztimer_tx_uart;
static ztimer_t _ztimer_rx_uart;

static void _openwsn_uart_write(const uint8_t *data)
{
    if (IS_USED(MODULE_OPENWSN_SERIAL)) {
        usbus_cdc_acm_submit(&cdcacm, data, 1);
        usbus_cdc_acm_flush(&cdcacm);
        ztimer_set(ZTIMER_USEC, &_ztimer_tx_uart, 0);
    }
}

static void _cdc_acm_rx_pipe(usbus_cdcacm_device_t *cdcacm,
                             uint8_t *data, size_t len)
{
    (void)cdcacm;
    for (size_t i = 0; i < len; i++) {
        isrpipe_write_one(&_cdc_stdio_isrpipe, data[i]);
    }
    ztimer_set(ZTIMER_USEC, &_ztimer_rx_uart, 0);
}

static void _riot_tx_cb(void *arg)
{
    (void)arg;

    if (IS_USED(MODULE_OPENWSN_SERIAL)) {
        if (_uart_vars.fXonXoffEscaping == 0x01) {
            _uart_vars.fXonXoffEscaping = 0x00;
            _uart_vars.xonXoffEscapedByte ^= XONXOFF_MASK;
            _openwsn_uart_write((uint8_t *)&(_uart_vars.xonXoffEscapedByte));
        }
        else {
            if (_uart_vars.txCb) {
                _uart_vars.txCb();
            }
        }
    }
}

static void _riot_rx_cb(void *arg)
{
    (void)arg;
    if (_uart_vars.rxCb) {
        if
        _uart_vars.rxCb();
    }
}

void uart_enableInterrupts(void)
{
    /* unused in RIOT */
}

void uart_disableInterrupts(void)
{
    /* unused in RIOT */
}

void uart_clearRxInterrupts(void)
{
    /* unused in RIOT */
}

void uart_clearTxInterrupts(void)
{
    /* unused in RIOT */
}

void uart_init_openwsn(usbus_t *usbus)
{
    usbus_cdc_acm_init(usbus, &cdcacm, _cdc_acm_rx_pipe, NULL,
                       _cdc_tx_buf_mem, sizeof(_cdc_tx_buf_mem));
    _ztimer_tx_uart.callback = &_riot_tx_cb;
    _ztimer_rx_uart.callback = &_riot_rx_cb;
}

void uart_setCallbacks(uart_tx_cbt txCb, uart_rx_cbt rxCb)
{
    _uart_vars.txCb = txCb;
    _uart_vars.rxCb = rxCb;
}

void uart_setCTS(bool state)
{
    uint8_t byte;
    if (state == true) {
        byte = XON;
    }
    else {
        byte = XOFF;
    }
    _openwsn_uart_write(&byte);
}

void uart_writeByte(uint8_t byteToWrite)
{
    if (byteToWrite == XON || byteToWrite == XOFF || \
        byteToWrite == XONXOFF_ESCAPE) {
        uint8_t byte = XONXOFF_ESCAPE;
        _uart_vars.fXonXoffEscaping = 0x01;
        _uart_vars.xonXoffEscapedByte = byteToWrite;
        _openwsn_uart_write(&byte);
    }
    else {
        _openwsn_uart_write(&byteToWrite);
    }
}

inline uint8_t uart_readByte(void)
{
    uint8_t byte;
    isrpipe_read(&_cdc_stdio_isrpipe, &byte, 1);
    return _uart_rx_byte;
}
