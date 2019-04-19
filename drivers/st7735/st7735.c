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
 * @file
 * @brief       Basic functionality of st7735 driver
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *              Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <string.h>

#include "st7735.h"
#include "st7735_registers.h"
#include "st7735_params.h"

#include "byteorder.h"
#include "xtimer.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

static void _st7735_cmd_start(st7735_t *dev, uint8_t cmd, bool cont)
{
    gpio_clear(dev->params.dcx_pin);
    spi_transfer_byte(dev->params.spi, dev->params.cs_pin, cont, cmd);
    gpio_set(dev->params.dcx_pin);
}

static void _st7735_write_cmd(st7735_t *dev, uint8_t cmd, const uint8_t *data,
                       size_t len)
{
    spi_acquire(dev->params.spi, dev->params.cs_pin, SPI_MODE_0,
                dev->params.spi_clk);
    _st7735_cmd_start(dev, cmd, len ? true : false);
    if (len) {
        spi_transfer_bytes(dev->params.spi, dev->params.cs_pin, false, data,
                           NULL, len);
    }
    spi_release(dev->params.spi);
}

static void _st7735_set_area(st7735_t *dev, uint16_t x1, uint16_t x2,
                             uint16_t y1, uint16_t y2)
{
    be_uint16_t params[2];

    x1 += ST7735_OFFSET_X;
    y1 += ST7735_OFFSET_Y;
    x2 += ST7735_OFFSET_X;
    y2 += ST7735_OFFSET_Y;

    params[0] = byteorder_htons(x1);
    params[1] = byteorder_htons(x2);
    _st7735_write_cmd(dev, ST7735_CASET, (uint8_t *)params,
                      sizeof(params));
    params[0] = byteorder_htons(y1);
    params[1] = byteorder_htons(y2);
    _st7735_write_cmd(dev, ST7735_RASET, (uint8_t *)params,
                      sizeof(params));
}

int st7735_init(st7735_t *dev, const st7735_params_t *prms)
{
    memcpy(&dev->params, prms, sizeof(st7735_params_t));
    gpio_init(dev->params.dcx_pin, GPIO_OUT);

    uint8_t params[16] = { 0 };

    int res = spi_init_cs(dev->params.spi, dev->params.cs_pin);
    if (res != SPI_OK) {
        DEBUG("[st7735] init: error initializing the CS pin [%i]\n", res);
        return -1;
    }

    /* Device must be resseted to be properly initialized*/
    if (dev->params.rst_pin != GPIO_UNDEF) {
        gpio_init(dev->params.rst_pin, GPIO_OUT);
        gpio_clear(dev->params.rst_pin);
        xtimer_usleep(120 * US_PER_MS);
        gpio_set(dev->params.rst_pin);
    }

    /* Soft Reset */
    _st7735_write_cmd(dev, ST7735_SWRESET, NULL, 0);
    xtimer_usleep(150 * US_PER_MS);

    /* Out of sleep mode*/
    _st7735_write_cmd(dev, ST7735_SLPOUT, NULL, 0);
    xtimer_usleep(500 * US_PER_MS);

    /* Set normal mode framerate rate = fosc/(1x2+40) * (LINE+2C+2D) */
    memcpy(params, (uint8_t [3]){ 0x01, 0x2C, 0x2D }, 3);
    _st7735_write_cmd(dev, ST7735_FRMCTR1, params, 3);
    /* Set idle mode framerate rate = fosc/(1x2+40) * (LINE+2C+2D) */
    memcpy(params, (uint8_t [3]){ 0x01, 0x2C, 0x2D }, 3);
    _st7735_write_cmd(dev, ST7735_FRMCTR2, params, 3);
    /* Set partial mode framerate */
    /* Dot and line inversion mode frame rate = fosc/(1x2+40) * (LINE+2C+2D) */
    memcpy(params, (uint8_t [6]){ 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D }, 6);
    _st7735_write_cmd(dev, ST7735_FRMCTR3, params, 6);

    /* No display Inversion , Line inversion */
    params[0] = 0x07;
    _st7735_write_cmd(dev, ST7735_INVCTR, params, 1);
    _st7735_write_cmd(dev, ST7735_INVOFF, NULL, 0);

    /* Power control, -4.6V Auto mode*/
    memcpy(params, (uint8_t [3]){ 0xA2, 0x02, 0x84 }, 3);
    _st7735_write_cmd(dev, ST7735_PWCTR1, params, 3);

    /* Power control, VGH25=2.4C VGSEL=-10 VGH=3 * AVDD*/
     params[0] = 0xC5;
    _st7735_write_cmd(dev, ST7735_PWCTR2, params, 1);

    /* Power control, VCOM = -0.775*/
     params[0] = 0x0E;
    _st7735_write_cmd(dev, ST7735_VMCTR1, params, 1);

    /* Mem accs ctl (directions)*/
    params[0] = ST7735_MADCTL_BGR;
    _st7735_write_cmd(dev, ST7735_MADCTL, params, 1);

    /* Set display color mode to 16 bit color */
    params[0] = 0x05;
    _st7735_write_cmd(dev, ST7735_COLMOD, params, 1);

    /* Set column and row address */
    _st7735_set_area(dev, 0x00, ST7735_WIDTH , 0x00, ST7735_HEIGHT );

    /* Gamma polarity correction */
    memcpy(params, (uint8_t [16]){ 0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d,
                 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10 }, 16);
    _st7735_write_cmd(dev, ST7735_GMCTRP1, params, 16);

    memcpy(params, (uint8_t [16]){ 0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D,
                0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10 }, 16);
    _st7735_write_cmd(dev, ST7735_GMCTRN1, params, 16);

    /* Normal display mode on */
    _st7735_write_cmd(dev, ST7735_NORON, NULL, 0);
    xtimer_usleep(10 * US_PER_MS);

    /* Turn On main Scren */
    _st7735_write_cmd(dev, ST7735_DISPON, NULL, 0);
    xtimer_usleep(100 * US_PER_MS);

    return 0;
}

void st7735_read_cmd(st7735_t *dev, uint8_t cmd, uint8_t *data, size_t len)
{
    assert(len);
    spi_acquire(dev->params.spi, dev->params.cs_pin, SPI_MODE_0,
                dev->params.spi_clk);
    _st7735_cmd_start(dev, cmd, true);
    /* Dummy read */
    spi_transfer_byte(dev->params.spi, dev->params.cs_pin, true, 0x00);
    spi_transfer_bytes(dev->params.spi, dev->params.cs_pin, false, NULL,
                       data, len);
    spi_release(dev->params.spi);
}

void st7735_fill(st7735_t *dev, uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, uint16_t color)
{
    /* Send fill area to the display */
    _st7735_set_area(dev, x1, x2, y1, y2);

    /* Calculate number of pixels */
    int32_t pix = (x2 - x1 + 1) * (y2 - y1 + 1);
    DEBUG("[st7735]: Write x1: %d, x2: %d, y1: %d, y2: %d. Num pixels: %ld\n",
          x1, x2, y1, y2, pix);

    /* Memory access command */
    spi_acquire(dev->params.spi, dev->params.cs_pin, SPI_MODE_0,
                dev->params.spi_clk);

    _st7735_cmd_start(dev, ST7735_RAMWR, true);

    color = htons(color);

    for (int i = 0; i < (pix - 1); i++) {
        spi_transfer_bytes(dev->params.spi, dev->params.cs_pin, true,
                           (uint8_t *)&color, NULL, sizeof(color));
    }
    spi_transfer_bytes(dev->params.spi, dev->params.cs_pin, false,
                       (uint8_t *)&color, NULL, sizeof(color));
    spi_release(dev->params.spi);
}

void st7735_map(st7735_t *dev, uint16_t x1, uint16_t x2, uint16_t y1,
                 uint16_t y2, const uint16_t *color)
{
    size_t num_pix = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* Send fill area to the display */
    _st7735_set_area(dev, x1, x2, y1, y2);

    /* Memory access command */
    spi_acquire(dev->params.spi, dev->params.cs_pin, SPI_MODE_0,
                dev->params.spi_clk);
    _st7735_cmd_start(dev, ST7735_RAMWR, true);

    for (size_t i = 0; i < num_pix - 1; i++) {
        uint16_t ncolor = htons(*(color+i));
        spi_transfer_bytes(dev->params.spi, dev->params.cs_pin, true,
                           &ncolor, NULL, 2);
    }
    uint16_t ncolor = htons(*(color+num_pix));
    spi_transfer_bytes(dev->params.spi, dev->params.cs_pin, false,
                       &ncolor, NULL, 2);

    spi_release(dev->params.spi);
}