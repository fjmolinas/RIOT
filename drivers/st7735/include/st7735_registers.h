/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_st7735
 * @{
 *
 * @file
 * @brief       Device driver definitions for the st7735 display controller
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.cl>
 *
 * @}
 */

#ifndef ST7735_REGISTER_H
#define ST7735_REGISTER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   ST7735R Internal register addresses
 * @{
 */
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MH  0x04

#define ST7735_FRMCTR1    0xB1
#define ST7735_FRMCTR2    0xB2
#define ST7735_FRMCTR3    0xB3
#define ST7735_INVCTR     0xB4
#define ST7735_DISSET5    0xB6

#define ST7735_PWCTR1     0xC0
#define ST7735_PWCTR2     0xC1
#define ST7735_PWCTR3     0xC2
#define ST7735_PWCTR4     0xC3
#define ST7735_PWCTR5     0xC4
#define ST7735_VMCTR1     0xC5

#define ST7735_PWCTR6     0xFC

#define ST7735_GMCTRP1    0xE0
#define ST7735_GMCTRN1    0xE1
/** @} */

/**
 * @name ST7735 commands
 * @{
 */
#define ST7735_NOP        0x00
#define ST7735_SWRESET    0x01
#define ST7735_RDDID      0x04
#define ST7735_RDDST      0x09
#define ST7735_RDDPW      0x0A
#define ST7735_RDD        0x0B
#define ST7735_RDDCOLMOD  0x0C
#define ST7735_RDDIM      0x0D
#define ST7735_RDDSM      0x0E

#define ST7735_SLPIN      0x10
#define ST7735_SLPOUT     0x11
#define ST7735_PTLON      0x12
#define ST7735_NORON      0x13

#define ST7735_INVOFF     0x20
#define ST7735_INVON      0x21
#define ST7735_GAMSET     0x26
#define ST7735_DISPOFF    0x28
#define ST7735_DISPON     0x29
#define ST7735_CASET      0x2A
#define ST7735_RASET      0x2B
#define ST7735_RAMWR      0x2C
#define ST7735_RGBSET     0x2D
#define ST7735_RAMRD      0x2E

#define ST7735_PTLAR      0x30
#define ST7735_TEOFF      0x34
#define ST7735_TEON       0x35
#define ST7735_MADCTL     0x36
#define ST7735_IDMOFF     0x38
#define ST7735_IDMON      0x39
#define ST7735_COLMOD     0x3A

#define ST7735_RDID1      0xDA
#define ST7735_RDID2      0xDB
#define ST7735_RDID3      0xDC
/** @} */

/**
 * @name   ST7735R settings
 * @{
 */
/* MADCTL settings */
#define ST7735_MADCTL_MY  0x80
#define ST7735_MADCTL_MX  0x40
#define ST7735_MADCTL_MV  0x20
#define ST7735_MADCTL_ML  0x10
#define ST7735_MADCTL_RGB 0x00
/** @} */

/**
 * @name Display rotation modes
 * @{
 */
#define ST7735_MADCTL_VERT         ST7735_MADCTL_MX       /**< Vertical mode */
#define ST7735_MADCTL_VERT_FLIP    ST7735_MADCTL_MY       /**< Flipped vertical */
#define ST7735_MADCTL_HORZ         ST7735_MADCTL_MV       /**< Horizontal mode */
#define ST7735_MADCTL_HORZ_FLIP    ST7735_MADCTL_MV | \
    ST7735_MADCTL_MY | \
    ST7735_MADCTL_MX                                   /**< Horizontal flipped */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ST7735_REGISTER_H */