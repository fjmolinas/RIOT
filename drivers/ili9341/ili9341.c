/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ili9341
 * @{
 *
 * @file
 * @brief       Device driver implementation for the ili9341 display controller
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <string.h>
#include "byteorder.h"
#include "periph/spi.h"
#include "xtimer.h"
#include "ili9341.h"
#include "ili9341_internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static void ili9341_cmd_start(ili9341_t *dev, uint8_t cmd, bool cont)
{
    gpio_clear(dev->params.dcx_pin);
    spi_transfer_byte(dev->params.spi, dev->params.cs_pin, cont, cmd);
    gpio_set(dev->params.dcx_pin);
}

/* datasheet page 178, table converted to equation.
 * gvdd in 1mv increments: 4850 = 4.85V */
static uint8_t ili9341_calc_pwrctl1(uint16_t gvdd)
{
    return (gvdd - 2850) / 50;
}

static uint8_t ili9341_calc_vmh(uint16_t vcomh)
{
    return (vcomh - 2700) / 25;
}

static uint8_t ili9341_calc_vml(int16_t vcoml)
{
    return (vcoml + 2500) / 25;
}

void ili9341_write_cmd(ili9341_t *dev, uint8_t cmd, const uint8_t *data,
                       size_t len)
{
    spi_acquire(dev->params.spi, dev->params.cs_pin, SPI_MODE_0,
                dev->params.spi_clk);
    ili9341_cmd_start(dev, cmd, len ? true : false);
    if (len) {
        spi_transfer_bytes(dev->params.spi, dev->params.cs_pin, false, data,
                           NULL, len);
    }
    spi_release(dev->params.spi);
}

void ili9341_read_cmd(ili9341_t *dev, uint8_t cmd, uint8_t *data, size_t len)
{
    assert(len);
    spi_acquire(dev->params.spi, dev->params.cs_pin, SPI_MODE_0,
                dev->params.spi_clk);
    ili9341_cmd_start(dev, cmd, true);
    /* Dummy transfer */
    spi_transfer_byte(dev->params.spi, dev->params.cs_pin, true, 0x00);
    spi_transfer_bytes(dev->params.spi, dev->params.cs_pin, false, NULL,
                       data, len);
    spi_release(dev->params.spi);
}

static void ili9341_set_area(ili9341_t *dev, uint16_t x1, uint16_t x2,
                             uint16_t y1, uint16_t y2)
{
    be_uint16_t params[2];

    params[0] = byteorder_htons(x1);
    params[1] = byteorder_htons(x2);
    ili9341_write_cmd(dev, ILI9341_CMD_CASET, (uint8_t *)params,
                      sizeof(params));
    params[0] = byteorder_htons(y1);
    params[1] = byteorder_htons(y2);
    ili9341_write_cmd(dev, ILI9341_CMD_PASET, (uint8_t *)params,
                      sizeof(params));
}

int ili9341_init(ili9341_t *dev, const ili9341_params_t *prms)
{
    memcpy(&dev->params, prms, sizeof(ili9341_params_t));
    uint8_t params[4] = { 0 };
    gpio_init(dev->params.dcx_pin, GPIO_OUT);
    int res = spi_init_cs(dev->params.spi, dev->params.cs_pin);
    if (res != SPI_OK) {
        DEBUG("[ili9341] init: error initializing the CS pin [%i]\n", res);
        return -1;
    }

    if (dev->params.rst_pin != GPIO_UNDEF) {
        gpio_init(dev->params.rst_pin, GPIO_OUT);
        gpio_clear(dev->params.rst_pin);
        xtimer_usleep(120 * US_PER_MS);
        gpio_set(dev->params.rst_pin);
    }
    xtimer_usleep(120 * US_PER_MS);

    /* Soft Reset */
    ili9341_write_cmd(dev, ILI9341_CMD_SWRESET, NULL, 0);
    xtimer_usleep(120 * US_PER_MS);

    /* Display off */
    ili9341_write_cmd(dev, ILI9341_CMD_DISPOFF, NULL, 0);

    /* PWRCTL1/2 */
    params[0] = ili9341_calc_pwrctl1(ILI9341_GVDD);
    ili9341_write_cmd(dev, ILI9341_CMD_PWCTRL1, params, 1);

    params[0] = 0x10; /* PWRCTL 0 0 0 */
    ili9341_write_cmd(dev, ILI9341_CMD_PWCTRL2, params, 1);

    /* VCOMCTL */
    params[0] = ili9341_calc_vmh(ILI9341_VCOMH);
    params[1] = ili9341_calc_vml(ILI9341_VCOML);
    ili9341_write_cmd(dev, ILI9341_CMD_VMCTRL1, params, 2);

    params[0] = 0x86;
    ili9341_write_cmd(dev, ILI9341_CMD_VMCTRL2, params, 1);

    /* Memory access CTL */
    params[0] = ILI9341_MADCTL_HORZ_FLIP | ILI9341_MADCTL_BGR;
    ili9341_write_cmd(dev, ILI9341_CMD_MADCTL, params, 1);

    /* Frame control */
    params[0] = 0x00;
    params[1] = 0x18;
    ili9341_write_cmd(dev, ILI9341_CMD_FRAMECTL1, params, 2);

    /* Display function control */
    params[0] = 0x08;
    params[1] = 0x82;
    params[2] = 0x27; /* 320 lines */
    ili9341_write_cmd(dev, ILI9341_CMD_DFUNC, params, 3);


    /* Pixel format */
    params[0] = 0x55; /* 16 bit mode */
    ili9341_write_cmd(dev, ILI9341_CMD_PIXSET, params, 1);


    params[0] = 0x01;
    ili9341_write_cmd(dev, ILI9341_CMD_GAMSET, params, 1);

    /* Gamma correction */
    {
        static const uint8_t gamma_pos[] = {
            0x0F,
            0x31,
            0x2B,
            0x0C,
            0x0E,
            0x08,
            0x4E,
            0xF1,
            0x37,
            0x07,
            0x10,
            0x03,
            0x0E,
            0x09,
            0x00 };
        ili9341_write_cmd(dev, ILI9341_CMD_PGAMCTRL, gamma_pos,
                          sizeof(gamma_pos));
    }
    {
        static const uint8_t gamma_neg[] = { 
            0x00,
            0x0E,
            0x14,
            0x03,
            0x11,
            0x07,
            0x31,
            0xC1,
            0x48,
            0x08,
            0x0F,
            0x0C,
            0x31,
            0x36,
            0x0F };
        ili9341_write_cmd(dev, ILI9341_CMD_NGAMCTRL, gamma_neg,
                          sizeof(gamma_neg));

    }
    /* Sleep out (turn off sleep mode) */
    ili9341_write_cmd(dev, ILI9341_CMD_SLPOUT, NULL, 0);
    /* Display on */
    ili9341_write_cmd(dev, ILI9341_CMD_DISPON, NULL, 0);
    return 0;
}

void ili9341_fill(ili9341_t *dev, uint16_t x1, uint16_t x2, uint16_t y1,
                  uint16_t y2, uint16_t color)
{
    /* Send fill area to the display */
    ili9341_set_area(dev, x1, x2, y1, y2);

    /* Calculate number of pixels */
    int32_t pix = (x2 - x1 + 1) * (y2 - y1 + 1);
    DEBUG("[ili9341]: Write x1: %d, x2: %d, y1: %d, y2: %d. Num pixels: %ld\n",
          x1, x2, y1, y2, pix);

    /* Memory access command */
    spi_acquire(dev->params.spi, dev->params.cs_pin, SPI_MODE_0,
                dev->params.spi_clk);

    ili9341_cmd_start(dev, ILI9341_CMD_RAMWR, true);
#if ILI9341_LE_MODE
    color = htons(color);
#endif
    for (int i = 0; i < (pix - 1); i++) {
        spi_transfer_bytes(dev->params.spi, dev->params.cs_pin, true,
                           (uint8_t *)&color, NULL, sizeof(color));
    }
    spi_transfer_bytes(dev->params.spi, dev->params.cs_pin, false,
                       (uint8_t *)&color, NULL, sizeof(color));
    spi_release(dev->params.spi);
}

void ili9341_map(ili9341_t *dev, uint16_t x1, uint16_t x2,
                 uint16_t y1, uint16_t y2, const uint16_t *color)
{
    size_t num_pix = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* Send fill area to the display */
    ili9341_set_area(dev, x1, x2, y1, y2);

    /* Memory access command */
    spi_acquire(dev->params.spi, dev->params.cs_pin, SPI_MODE_0,
                dev->params.spi_clk);
    ili9341_cmd_start(dev, ILI9341_CMD_RAMWR, true);
#if ILI9341_LE_MODE
    for (size_t i = 0; i < num_pix - 1; i++) {
        uint16_t ncolor = htons(*(color+i));
        spi_transfer_bytes(dev->params.spi, dev->params.cs_pin, true,
                           &ncolor, NULL, 2);
    }
    uint16_t ncolor = htons(*(color+num_pix));
    spi_transfer_bytes(dev->params.spi, dev->params.cs_pin, false,
                       &ncolor, NULL, 2);
#else
    spi_transfer_bytes(dev->params.spi, dev->params.cs_pin, false,
                       (const uint8_t*)color, NULL, num_pix * 2);

#endif

    spi_release(dev->params.spi);
}

void ili9341_invert_on(ili9341_t *dev)
{
    ili9341_write_cmd(dev, ILI9341_CMD_DINVON, NULL, 0);
}

void ili9341_invert_off(ili9341_t *dev)
{
    ili9341_write_cmd(dev, ILI9341_CMD_DINVOFF, NULL, 0);
}

void ili9341_set_brightness(ili9341_t *dev, uint8_t brightness)
{
    ili9341_write_cmd(dev, ILI9341_CMD_WRDISBV, &brightness, 1);
    uint8_t param = 0x26;
    ili9341_write_cmd(dev, ILI9341_CMD_WRCTRLD, &param, 1);
}
