/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_stmpe811
 * @{
 *
 * @file
 * @brief       Device driver implementation for the STMPE811 touchscreen controller.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "xtimer.h"
#include "periph/i2c.h"

#include "stmpe811.h"
#include "stmpe811_constants.h"
#include "stmpe811_params.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#define DEV_I2C      (dev->params.i2c)
#define DEV_ADDR     (dev->params.addr)

int stmpe811_init(stmpe811_t *dev, const stmpe811_params_t * params)
{
    dev->params = *params;

    xtimer_usleep(50 * US_PER_MS);

    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    stmpe811_reset(dev);

    /* start of the documented initialization sequence */
    stmpe811_disable_ts(dev);
    stmpe811_disable_gpio(dev);

    stmpe811_enable_fifo_of(dev);
    stmpe811_enable_fifo_th(dev);
    stmpe811_enable_fifo_touch_det(dev);

    stmpe811_set_adc_sample(dev, 0x40);     /* set to 80 cycles */
    stmpe811_set_adc_resolution(dev, 0x08); /* set adc resolution to 12 bit */

    stmpe811_set_adc_freq(dev, 0x01);       /* set adc clock speed to 3.25 MHz */

    stmpe811_set_gpio_af(dev, 0x00);        /* set GPIO AF to function as ts/adc */

    /*
     * tsc cfg set to avg control = 4 samples
     * touch detection delay = 500 microseconds
     * panel driver settling time = 500 microcseconds
     */
    stmpe811_set_tsc(dev, 0x9A);

    stmpe811_set_fifo_th(dev, 0x01);        /* set fifo threshold */
    stmpe811_reset_fifo(dev);             /* reset fifo */

    /* set fractional part to 7, whole part to 1 */
    stmpe811_set_tsc_fraction_z(dev, 0x07);

    /* set current limit value to 50 mA */
    stmpe811_set_tsc_i_drive(dev, 0x01);

    stmpe811_enable_tsc(dev);             /* enable tsc (touchscreen clock) */

    stmpe811_set_int_sta(dev, 0xFF);        /* clear interrupt status */

    stmpe811_enable_interrupts(dev);
    /* end of the documented initialization sequence */

    /* Release I2C device */
    i2c_release(DEV_I2C);

    xtimer_usleep(2 * US_PER_MS);

    return STMPE811_OK;
}

void stmpe811_reset(const stmpe811_t *dev)
{
    uint8_t data;
    i2c_read_reg(DEV_I2C, DEV_ADDR, STMPE811_SYS_CTRL1, &data, 0);
    data |= 0x02;
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_SYS_CTRL1, data, 0);
    data &= ~(0x02);
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_SYS_CTRL1, data, 0);

    xtimer_usleep(2 * US_PER_MS);
}

void stmpe811_enable_ts(const stmpe811_t *dev)
{
    uint8_t mode;
    i2c_read_reg(DEV_I2C, DEV_ADDR, STMPE811_SYS_CTRL2, &mode, 0);
    mode &= ~STMPE811_SYS_CTRL2_TS_OFF;
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_SYS_CTRL2, mode, 0);
}

void stmpe811_disable_ts(const stmpe811_t *dev)
{
    i2c_write_reg(DEV_I2C, DEV_ADDR,
                  STMPE811_SYS_CTRL2, STMPE811_SYS_CTRL2_TS_OFF, 0);
}

void stmpe811_enable_gpio(const stmpe811_t *dev)
{
    uint8_t mode;
    i2c_read_reg(DEV_I2C, DEV_ADDR, STMPE811_SYS_CTRL2, &mode, 0);
    mode &= ~STMPE811_SYS_CTRL2_GPIO_OFF;
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_SYS_CTRL2, mode, 0);
}

void stmpe811_disable_gpio(const stmpe811_t *dev)
{
    i2c_write_reg(DEV_I2C, DEV_ADDR,
                  STMPE811_SYS_CTRL2, STMPE811_SYS_CTRL2_GPIO_OFF, 0);
}

void stmpe811_enable_tsc(const stmpe811_t *dev)
{
    uint8_t mode;
    i2c_read_reg(DEV_I2C, DEV_ADDR, STMPE811_SYS_CTRL2, &mode, 0);
    mode &= ~STMPE811_SYS_CTRL2_TSC_OFF;
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_SYS_CTRL2, mode, 0);

    i2c_read_reg(DEV_I2C, DEV_ADDR, STMPE811_TSC_CTRL, &mode, 0);
    mode |= STMPE811_TSC_CTRL_EN;
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_TSC_CTRL, mode, 0);
}

void stmpe811_disable_tsc(const stmpe811_t *dev)
{
    i2c_write_reg(DEV_I2C, DEV_ADDR,
                  STMPE811_SYS_CTRL2, STMPE811_SYS_CTRL2_TSC_OFF, 0);
}

void stmpe811_enable_adc(const stmpe811_t *dev)
{
	uint8_t mode;
	i2c_read_reg(DEV_I2C, DEV_ADDR, STMPE811_SYS_CTRL2, &mode, 0);
	mode &= ~STMPE811_SYS_CTRL2_ADC_OFF;
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_SYS_CTRL2, mode, 0);
}

void stmpe811_disable_adc(const stmpe811_t *dev)
{
    i2c_write_reg(DEV_I2C, DEV_ADDR,
                  STMPE811_SYS_CTRL2, STMPE811_SYS_CTRL2_ADC_OFF, 0);
}

void stmpe811_enable_fifo_of(const stmpe811_t *dev)
{
    uint8_t mode;
    i2c_read_reg(DEV_I2C, DEV_ADDR, STMPE811_INT_EN, &mode, 0);
    mode |= 0x04;
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_INT_EN, mode, 0);
}

void stmpe811_enable_fifo_th(const stmpe811_t *dev)
{
    uint8_t mode;
    i2c_read_reg(DEV_I2C, DEV_ADDR, STMPE811_INT_EN, &mode, 0);
    mode |= 0x02;
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_INT_EN, mode, 0);
}

void stmpe811_enable_fifo_touch_det(const stmpe811_t *dev)
{
    uint8_t mode;
    i2c_read_reg(DEV_I2C, DEV_ADDR, STMPE811_INT_EN, &mode, 0);
    mode |= 0x01;
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_INT_EN, mode, 0);
}

void stmpe811_set_adc_sample(const stmpe811_t *dev, uint8_t sample)
{
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_ADC_CTRL1, sample, 0);
}

void stmpe811_set_adc_resolution(const stmpe811_t *dev, uint8_t res)
{
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_ADC_CTRL1, res, 0);
}

void stmpe811_set_adc_freq(const stmpe811_t *dev, uint8_t freq)
{
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_ADC_CTRL2, freq, 0);
}

void stmpe811_set_gpio_af(const stmpe811_t *dev, uint8_t af)
{
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_GPIO_AF, af, 0);
}

void stmpe811_set_tsc(const stmpe811_t *dev, uint8_t tsc)
{
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_TSC_CFG, tsc, 0);
}

void stmpe811_set_fifo_th(const stmpe811_t *dev, uint8_t th)
{
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_FIFO_TH, th, 0);
}

void stmpe811_reset_fifo(const stmpe811_t *dev)
{
    uint8_t sta;
    i2c_read_reg(DEV_I2C, DEV_ADDR, STMPE811_FIFO_STA, &sta, 0);
    sta |= 0x01;
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_FIFO_STA, sta, 0);
    sta &= ~(0x01);
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_FIFO_STA, sta, 0);
}

void stmpe811_set_tsc_fraction_z(const stmpe811_t *dev, uint8_t z)
{
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_TSC_FRACTION_Z, z, 0);
}

void stmpe811_set_tsc_i_drive(const stmpe811_t *dev, uint8_t limit)
{
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_TSC_I_DRIVE, limit, 0);
}

void stmpe811_set_int_sta(const stmpe811_t *dev, uint8_t status)
{
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_INT_STA, status, 0);
}

void stmpe811_enable_interrupts(const stmpe811_t *dev)
{
    uint8_t mode;
    i2c_read_reg(DEV_I2C, DEV_ADDR, STMPE811_INT_CTRL, &mode, 0);
    mode |= 0x01;
    i2c_write_reg(DEV_I2C, DEV_ADDR, STMPE811_INT_CTRL, mode, 0);
}

void stmpe811_read_xy(const stmpe811_t *dev, uint16_t *x, uint16_t *y)
{
    /* Acquire I2C device */
    i2c_acquire(DEV_I2C);

    uint8_t tmp[2];
    i2c_read_regs(DEV_I2C, DEV_ADDR, STMPE811_TSC_DATA_Y, &tmp, 2, 0);
    *y = (tmp[0] << 8) | (tmp[1] & 0xff);
    i2c_read_regs(DEV_I2C, DEV_ADDR, STMPE811_TSC_DATA_X, &tmp, 2, 0);
    *x = (tmp[0] << 8) | (tmp[1] & 0xff);
    stmpe811_reset_fifo(dev);

    /* Release I2C device */
    i2c_release(DEV_I2C);
}

stmpe811_state_t stmpe811_read_touch(const stmpe811_t *dev)
{
    uint8_t val;

    /* Acquire I2C device */
    i2c_acquire(DEV_I2C);

    i2c_read_reg(DEV_I2C, DEV_ADDR, STMPE811_TSC_CTRL, &val, 0);

    if ((val & 0x80) == 0) {
        stmpe811_reset_fifo(dev);
        /* Release I2C device */
        i2c_release(DEV_I2C);

        return STMPE811_STATE_RELEASED;
    }

    /* Release I2C device */
    i2c_release(DEV_I2C);

    return STMPE811_STATE_PRESSED;
}
