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
 *
 * @}
 */

#include <stdatomic.h>

#include "board.h"
#include "uart_ow.h"
#include "periph/uart.h"
#include "periph/timer.h"

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

static uart_vars_t uart_vars;
static atomic_char uart_rx_byte;

static void _openwsn_uart_write(const uint8_t* data) {
    uart_write(OPENWSN_UART_DEV, data, 1);
    timer_set(OPENWSN_UART_TIMER_DEV, OPENWSN_UART_TIME_CHAN, \
            OPENWSN_UART_TX_CB_OFFSET);
}

static void _riot_rx_cb(void *arg, uint8_t data)
{
    (void)arg;
    if (IS_USED(MODULE_OPENWSN_SERIAL)) {
        uart_rx_byte = data;
        uart_vars.rxCb();
    }
    else {
        (void) data;
    }
}

static void _riot_tx_cb(void *arg, int count)
{
    (void)count;
    (void)arg;

    if (IS_USED(MODULE_OPENWSN_SERIAL)) {
        if (uart_vars.fXonXoffEscaping == 0x01) {
            uart_vars.fXonXoffEscaping = 0x00;
            uart_vars.xonXoffEscapedByte ^= XONXOFF_MASK;
            _openwsn_uart_write((uint8_t*) &(uart_vars.xonXoffEscapedByte));
        } else {
            uart_vars.txCb();
        }
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

void uart_init_openwsn(void)
{
    if (IS_USED(MODULE_OPENWSN_SERIAL)) {
        uart_init(OPENWSN_UART_DEV, OPENWSN_UART_BAUDRATE, \
                  (uart_rx_cb_t) _riot_rx_cb, NULL);
        /* OpenWSN uses uart tx interrupts to update openserial state
           machine as well as writing following bytes, since we dont
           have uart tx interrupts we set a timer interrupt on every
           uart_write */
        timer_init(OPENWSN_UART_TIMER_DEV, OPENWSN_UART_TIMER_FREQ, \
                   _riot_tx_cb, NULL);
    }
}

void uart_setCallbacks(uart_tx_cbt txCb, uart_rx_cbt rxCb)
{
    if (IS_USED(MODULE_OPENWSN_SERIAL)) {
        uart_vars.txCb = txCb;
        uart_vars.rxCb = rxCb;
    }
    else {
        (void) rxCb;
        (void) txCb;
    }
}

void uart_setCTS(bool state)
{
    if (IS_USED(MODULE_OPENWSN_SERIAL)) {
        uint8_t byte;
        if (state == true) {
            byte = XON;
        } else {
            byte = XOFF;
        }
        _openwsn_uart_write(&byte);
    }
    else {
        (void) state;
    }
}

void uart_writeByte(uint8_t byteToWrite)
{
    if (IS_USED(MODULE_OPENWSN_SERIAL)) {
        if (byteToWrite == XON || byteToWrite == XOFF || \
            byteToWrite == XONXOFF_ESCAPE) {
            uint8_t byte = XONXOFF_ESCAPE;
            uart_vars.fXonXoffEscaping = 0x01;
            uart_vars.xonXoffEscapedByte = byteToWrite;
            _openwsn_uart_write(&byte);
        }
        else {
            _openwsn_uart_write(&byteToWrite);

        }
    }
    else {
        (void) byteToWrite;
    }
}

inline uint8_t uart_readByte(void)
{
    if (IS_USED(MODULE_OPENWSN_SERIAL)) {
        return uart_rx_byte;
    }
    else {
        return 0x00;
    }
}
