/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_smtpe610
 * @{
 *
 * @file
 * @brief       Internal addresses, registers and constants
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef SMTPE610_CONSTANTS_H
#define SMTPE610_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

_STMPE_ADDR = const(0x41)
_STMPE_VERSION = const(0x0811)

_STMPE_SYS_CTRL1 = const(0x03)
_STMPE_SYS_CTRL1_RESET = const(0x02)
_STMPE_SYS_CTRL2 = const(0x04)

_STMPE_TSC_CTRL = const(0x40)
_STMPE_TSC_CTRL_EN = const(0x01)
_STMPE_TSC_CTRL_XYZ = const(0x00)
_STMPE_TSC_CTRL_XY = const(0x02)

_STMPE_INT_CTRL = const(0x09)
_STMPE_INT_CTRL_POL_HIGH = const(0x04)
_STMPE_INT_CTRL_POL_LOW = const(0x00)
_STMPE_INT_CTRL_EDGE = const(0x02)
_STMPE_INT_CTRL_LEVEL = const(0x00)
_STMPE_INT_CTRL_ENABLE = const(0x01)
_STMPE_INT_CTRL_DISABLE = const(0x00)


_STMPE_INT_EN = const(0x0A)
_STMPE_INT_EN_TOUCHDET = const(0x01)
_STMPE_INT_EN_FIFOTH = const(0x02)
_STMPE_INT_EN_FIFOOF = const(0x04)
_STMPE_INT_EN_FIFOFULL = const(0x08)
_STMPE_INT_EN_FIFOEMPTY = const(0x10)
_STMPE_INT_EN_ADC = const(0x40)
_STMPE_INT_EN_GPIO = const(0x80)

_STMPE_INT_STA = const(0x0B)
_STMPE_INT_STA_TOUCHDET = const(0x01)

_STMPE_ADC_CTRL1 = const(0x20)
_STMPE_ADC_CTRL1_12BIT = const(0x08)
_STMPE_ADC_CTRL1_10BIT = const(0x00)

_STMPE_ADC_CTRL2 = const(0x21)
_STMPE_ADC_CTRL2_1_625MHZ = const(0x00)
_STMPE_ADC_CTRL2_3_25MHZ = const(0x01)
_STMPE_ADC_CTRL2_6_5MHZ = const(0x02)

_STMPE_TSC_CFG = const(0x41)
_STMPE_TSC_CFG_1SAMPLE = const(0x00)
_STMPE_TSC_CFG_2SAMPLE = const(0x40)
_STMPE_TSC_CFG_4SAMPLE = const(0x80)
_STMPE_TSC_CFG_8SAMPLE = const(0xC0)
_STMPE_TSC_CFG_DELAY_10US = const(0x00)
_STMPE_TSC_CFG_DELAY_50US = const(0x08)
_STMPE_TSC_CFG_DELAY_100US = const(0x10)
_STMPE_TSC_CFG_DELAY_500US = const(0x18)
_STMPE_TSC_CFG_DELAY_1MS = const(0x20)
_STMPE_TSC_CFG_DELAY_5MS = const(0x28)
_STMPE_TSC_CFG_DELAY_10MS = const(0x30)
_STMPE_TSC_CFG_DELAY_50MS = const(0x38)
_STMPE_TSC_CFG_SETTLE_10US = const(0x00)
_STMPE_TSC_CFG_SETTLE_100US = const(0x01)
_STMPE_TSC_CFG_SETTLE_500US = const(0x02)
_STMPE_TSC_CFG_SETTLE_1MS = const(0x03)
_STMPE_TSC_CFG_SETTLE_5MS = const(0x04)
_STMPE_TSC_CFG_SETTLE_10MS = const(0x05)
_STMPE_TSC_CFG_SETTLE_50MS = const(0x06)
_STMPE_TSC_CFG_SETTLE_100MS = const(0x07)

_STMPE_FIFO_TH = const(0x4A)

_STMPE_FIFO_SIZE = const(0x4C)

_STMPE_FIFO_STA = const(0x4B)
_STMPE_FIFO_STA_RESET = const(0x01)
_STMPE_FIFO_STA_OFLOW = const(0x80)
_STMPE_FIFO_STA_FULL = const(0x40)
_STMPE_FIFO_STA_EMPTY = const(0x20)
_STMPE_FIFO_STA_THTRIG = const(0x10)

_STMPE_TSC_I_DRIVE = const(0x58)
_STMPE_TSC_I_DRIVE_20MA = const(0x00)
_STMPE_TSC_I_DRIVE_50MA = const(0x01)

_STMPE_TSC_DATA_X = const(0x4D)
_STMPE_TSC_DATA_Y = const(0x4F)
_STMPE_TSC_FRACTION_Z = const(0x56)

_STMPE_GPIO_SET_PIN = const(0x10)
_STMPE_GPIO_CLR_PIN = const(0x11)
_STMPE_GPIO_DIR = const(0x13)
_STMPE_GPIO_ALT_FUNCT = const(0x17)
#ifdef __cplusplus
}
#endif

#endif /* SMTPE610_CONSTANTS_H */
/** @} */
