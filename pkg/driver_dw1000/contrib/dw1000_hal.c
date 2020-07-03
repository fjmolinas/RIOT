/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_driver_dw1000
 * @ingroup     drivers_dw1000
 * @{
 *
 * @file
 * @brief       Abstraction layer for RIOT adaption
 *
 */

#include <stdio.h>

#include "dw1000.h"
#include "libdw1000Types.h"

#include "periph/gpio.h"
#include "periph/spi.h"
#include "xtimer.h"

void _spiRead(dwDevice_t* dev, const void *header, size_t headerLength,
             void* data, size_t dataLength)
{
    dw1000_t *dw1000 = container_of(dev, dw1000_t, dev);

    spi_acquire(dw1000->params.spi,
                dw1000->params.cs_pin,
                dw1000->params.mode,
                dw1000->params.spi_clk);

    spi_transfer_bytes(dw1000->params.spi,
                       dw1000->params.cs_pin,
                       true,
                       header,
                       NULL,
                       headerLength);

    spi_transfer_bytes(dw1000->params.spi,
                       dw1000->params.cs_pin,
                       false,
                       NULL,
                       data,
                       dataLength);

    spi_release(dw1000->params.spi);
}

void _spiWrite(dwDevice_t* dev, const void *header, size_t headerLength,
              const void* data, size_t dataLength)
{
    dw1000_t *dw1000 = container_of(dev, dw1000_t, dev);

    spi_acquire(dw1000->params.spi,
                dw1000->params.cs_pin,
                dw1000->params.mode,
                dw1000->params.spi_clk);

    spi_transfer_bytes(dw1000->params.spi,
                       dw1000->params.cs_pin,
                       true,
                       header,
                       NULL,
                       headerLength);

    spi_transfer_bytes(dw1000->params.spi,
                       dw1000->params.cs_pin,
                       false,
                       data,
                       NULL,
                       dataLength);

    spi_release(dw1000->params.spi);
}

void _spiSetSpeed(dwDevice_t* dev, dwSpiSpeed_t speed)
{
    dw1000_t *dw1000 = container_of(dev, dw1000_t, dev);
    if (speed == dwSpiSpeedLow){
        dw1000->params.spi_clk = SPI_CLK_1MHZ;
    }
    else if (speed == dwSpiSpeedHigh){
        dw1000->params.spi_clk = SPI_CLK_5MHZ;
    }
}

void _delayms(dwDevice_t* dev, unsigned int delay)
{
    (void) dev;
    xtimer_usleep(US_PER_MS * delay);
}

void _reset(dwDevice_t *dev)
{
    dw1000_t *dw1000 = container_of(dev, dw1000_t, dev);
    dw1000_reset(dw1000);
}

const dwOps_t dw1000_ops = {
    .spiRead = _spiRead,
    .spiWrite = _spiWrite,
    .spiSetSpeed = _spiSetSpeed,
    .delayms = _delayms,
    .reset = _reset,
};
