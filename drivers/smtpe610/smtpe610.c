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
 * @brief       Device driver implementation for the sensors
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <errno.h>
#include "smtpe610.h"
#include "smtpe610_constants.h"
#include "smtpe610_params.h"

#if IS_USED(MODULE_STMPE610_SPI)
#define BUS                 (dev->params.spi)
#define CS                  (dev->params.cs)
#define CLK                 (dev->params.clk)
#define MODE                SPI_MODE_0
#define WRITE_MASK          (0x7F)
#else
#define BUS                 (dev->params.i2c_dev)
#define ADDR                (dev->params.i2c_addr)
#endif


#if IS_USED(MODULE_STMPE610_SPI) /* using SPI mode */
static inline int _acquire(const stmpe610_t *dev)
{
    if (spi_acquire(BUS, CS, MODE, CLK) != SPI_OK) {
        return -EIO;
    }
    return 0;
}

static inline void _release(const stmpe610_t *dev)
{
    spi_release(BUS);
}

static int _read_reg(const stmpe610_t *dev, uint8_t reg, uint8_t *data)
{
    *data = spi_transfer_reg(BUS, CS, reg, 0);
    return 0;
}

static int _write_reg(const stmpe610_t *dev, uint8_t reg, uint8_t data)
{
    (void)spi_transfer_reg(BUS, CS, (reg & WRITE_MASK), data);
    return 0;
}

static int _read_burst(const stmpe610_t *dev, uint8_t reg, void *buf, size_t len)
{
    spi_transfer_regs(BUS, CS, reg, NULL, buf, len);
    return 0;
}

#else /* using I2C mode */

static inline int _acquire(const stmpe610_t *dev)
{
    if (i2c_acquire(BUS) != 0) {
        return -EIO;
    }
    return 0;
}

static inline void _release(const stmpe610_t *dev)
{
    i2c_release(BUS);
}

static int _read_reg(const stmpe610_t *dev, uint8_t reg, uint8_t *data)
{
    if (i2c_read_reg(BUS, ADDR, reg, data, 0) != 0) {
        return -EIO;
    }
    return 0;
}

static int _write_reg(const stmpe610_t *dev, uint8_t reg, uint8_t data)
{
    if (i2c_write_reg(BUS, ADDR, reg, data, 0) != 0) {
        return -EIO;
    }
    return 0;
}

static int _read_burst(const stmpe610_t *dev, uint8_t reg, void *buf, size_t len)
{
    if (i2c_read_regs(BUS, ADDR, reg, buf, len, 0) != 0) {
        return -EIO;
    }
    return 0;
}

#endif /* bus mode selection */

int smtpe610_init(smtpe610_t *dev, const smtpe610_params_t *params)
{
    assert(dev && params);

    dev->params = *params;
    uint8_t reg;

#if IS_USED(MODULE_STMPE610_SPI) /* using SPI mode */
    /* configure the chip-select pin */
    if (spi_init_cs(BUS, CS) != SPI_OK) {
        DEBUG("[stmpe680] error: unable to configure chip the select pin\n");
        return -EIO;
    }
#endif

    /* acquire bus bus, this also tests the bus parameters in SPI mode */
    if (_acquire(dev) != BMX280_OK) {
        DEBUG("[stmpe680] error: unable to acquire bus\n");
        return -EPROTO;
    }


           """Reset the controller"""
        self._write_register_byte(_STMPE_SYS_CTRL1, _STMPE_SYS_CTRL1_RESET)
        time.sleep(0.001)

        self._write_register_byte(_STMPE_SYS_CTRL2, 0x0)  # turn on clocks!
        self._write_register_byte(
            _STMPE_TSC_CTRL, _STMPE_TSC_CTRL_XYZ | _STMPE_TSC_CTRL_EN
        )  # XYZ and enable!
        self._write_register_byte(_STMPE_INT_EN, _STMPE_INT_EN_TOUCHDET)
        self._write_register_byte(
            _STMPE_ADC_CTRL1, _STMPE_ADC_CTRL1_10BIT | (0x6 << 4)
        )  # 96 clocks per conversion
        self._write_register_byte(_STMPE_ADC_CTRL2, _STMPE_ADC_CTRL2_6_5MHZ)
        self._write_register_byte(
            _STMPE_TSC_CFG,
            _STMPE_TSC_CFG_4SAMPLE
            | _STMPE_TSC_CFG_DELAY_1MS
            | _STMPE_TSC_CFG_SETTLE_5MS,
        )
        self._write_register_byte(_STMPE_TSC_FRACTION_Z, 0x6)
        self._write_register_byte(_STMPE_FIFO_TH, 1)
        self._write_register_byte(_STMPE_FIFO_STA, _STMPE_FIFO_STA_RESET)
        self._write_register_byte(_STMPE_FIFO_STA, 0)  # unreset
        self._write_register_byte(_STMPE_TSC_I_DRIVE, _STMPE_TSC_I_DRIVE_50MA)
        self._write_register_byte(_STMPE_INT_STA, 0xFF)  # reset all ints
        self._write_register_byte(
            _S
}
