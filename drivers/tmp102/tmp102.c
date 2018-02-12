#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "log.h"
#include "periph/i2c.h"
#include "tmp102.h"
#include "tmp102_regs.h"
#if TMP102_USE_LOW_POWER
#include "xtimer.h"
#endif

#define ENABLE_DEBUG                (0)
#include "debug.h"

#define I2C_SPEED                   I2C_SPEED_FAST
#define BUS                         (dev->params.i2c)
#define ADDR                        (dev->params.addr)

#define TMP102_CONFIG_CR_MASK       (0x3F)
#define TMP102_CONFIG_CR_SHIFT      (6u)

int tmp102_set_conversion_rate(tmp102_t *dev, uint8_t cr){
    uint8_t reg_byte[2]; // Store the data from the register here
    uint16_t tmp;

    cr = cr&0x03; // Make sure cr is not set higher than 3.

    /* Read from configuration register*/
    i2c_acquire(BUS);
    i2c_read_regs(BUS, ADDR, TMP102_REGS_CONFIG, &tmp, 2);

    /* Read current configuration register value */
    reg_byte[0] = (tmp >> 8);
    reg_byte[1] = tmp & TMP102_CONFIG_CR_MASK ;

    /* Load new conversion conversion rate */
    reg_byte[0] &= TMP102_CONFIG_CR_MASK;  // Clear CR0/1 (bit 6 and 7 of second byte)
    reg_byte[0] |= cr<<6;	// Shift in new conversion rate

    /* Set configuration registers */
    if (i2c_write_regs(BUS, ADDR, TMP102_REGS_CONFIG, reg_byte, 2) != 2) {
        i2c_release(BUS);
        LOG_ERROR("tmp102_init: error setting conversion rate!\n");
        return -TMP102_ERROR_BUS;
    }
    i2c_release(BUS);


    return TMP102_OK;
}

int tmp102_init(tmp102_t *dev, const tmp102_params_t *params)
{
    /* check parameters */
    assert(dev && params);

    /* initialize the device descriptor */
    memcpy(&dev->params, params, sizeof(tmp102_params_t));

    if (dev->params.rate > TMP102_CONFIG_CR3) {
        LOG_ERROR("tmp102_init: invalid conversion rate!\n");
        return -TMP102_ERROR_CONF;
    }

    /* setup the I2C bus */
    i2c_acquire(BUS);
    if (i2c_init_master(BUS, I2C_SPEED) < 0) {
        i2c_release(BUS);
        LOG_ERROR("tmp102_init: error initializing I2C bus\n");
        return -TMP102_ERROR_BUS;
    }
    i2c_release(BUS);

    /* set conversion rate */
    tmp102_set_conversion_rate(dev, dev->params.rate);

    return TMP102_OK;
}

int tmp102_set_extended_mode(tmp102_t *dev, uint8_t mode)
{
    uint16_t tmp;
    uint8_t reg_byte[2];

    /* Read from configuration register*/
    i2c_acquire(BUS);
    i2c_read_regs(BUS, ADDR, TMP102_REGS_CONFIG, &tmp, 2);
    reg_byte[0] = (tmp >> 8);
    reg_byte[1] = tmp;

    /* Load new value for extention mode */
    reg_byte[1] &= 0xEF;    // Clear EM (bit 4 of second byte)
    reg_byte[1] |= mode<<4; // Shift in new exentended mode bit

    /* Set configuration registers */
    if (i2c_write_regs(BUS, ADDR, TMP102_REGS_CONFIG, reg_byte, 2) != 2) {
        i2c_release(BUS);
        LOG_ERROR("tmp102: error setting extended mode!\n");
        return -TMP102_ERROR_BUS;
    }
    i2c_release(BUS);

    return TMP102_OK;
}

int tmp102_sleep(tmp102_t *dev)
{
    uint16_t tmp;
    uint8_t reg_byte[2];

    /* Read from configuration register */
    i2c_acquire(BUS);
    i2c_read_regs(BUS, ADDR, TMP102_REGS_CONFIG, &tmp, 2);
    reg_byte[0] = (tmp >> 8);
    reg_byte[1] = tmp;

    /* Set SD (bit 0 of first byte) */
    reg_byte[0] |= 0x01;

    /* Set configuration registers */
    if (i2c_write_regs(BUS, ADDR, TMP102_REGS_CONFIG, &reg_byte, 2) != 2) {
        i2c_release(BUS);
        LOG_ERROR("tmp102_sleep: error putting to sleep!\n");
        return -TMP102_ERROR_BUS;
    }
    i2c_release(BUS);

    return TMP102_OK;
}

int tmp102_wakeup(tmp102_t *dev)
{
    uint16_t tmp;
    uint8_t reg_byte[2];

    /* Read from configuration register*/
    i2c_acquire(BUS);
    i2c_read_regs(BUS, ADDR, TMP102_REGS_CONFIG, &tmp, 2);
    reg_byte[0] = (tmp >> 8);
    reg_byte[1] = tmp;

    /* Clear SD (bit 0 of first byte) */
    reg_byte[0] &= 0xFE;

    /* Write to register registers */
    if (i2c_write_regs(BUS, ADDR, TMP102_REGS_CONFIG, &reg_byte, 2) != 2) {
        i2c_release(BUS);
        LOG_ERROR("tmp102_sleep: error putting to sleep!\n");
        return -TMP102_ERROR_BUS;
    }
    i2c_release(BUS);

    return TMP102_OK;
}

int tmp102_read(const tmp102_t *dev, int16_t *rawt)
{
    uint8_t reg_byte[2];

    i2c_acquire(BUS);
    /* Register bytes are sent MSB first. */
    if (i2c_read_regs(BUS, ADDR, TMP102_REGS_T, &reg_byte, 2) != 2) {
        i2c_release(BUS);
        return -TMP102_ERROR_BUS;
    }
    i2c_release(BUS);

    /* in 13-bit mode 8th bit of tmp:reg would be 1 */
    if(reg_byte[1]&0x01)
    {
        /* reorder bits to create signed int*/
        *rawt = ((reg_byte[0]) << 5 ) | ((reg_byte[1]) >> 3);
        /* If negative convert 13 bt to 16 bit and use 2s compliment */
        if(*rawt > 0xFFF)
        {
            *rawt |= 0xE000;
        }
    }
    /* 12 bit mode */
    else
    {
        /* reorder bits to create signed int*/
        *rawt = ((reg_byte[0]) << 4) | ((reg_byte[1]) >> 4);
        /* If negative convert 13 bt to 16 bit and use 2s compliment */
        if(*rawt > 0x7FF)
        {
            *rawt |= 0xF000;
        }
    }
    return TMP102_OK;
}

void tmp102_convert(int16_t rawt, float *tamb)
{
    /* calculate ambect temperature in Celsius */
    *tamb = (rawt * 0.0625);
}

int tmp102_read_temperature(const tmp102_t *dev, int16_t *ta)
{
#if (!TMP102_USE_RAW_VALUES)
    int16_t rawtemp;
    float tamb;
#endif

#ifdef TMP102_LOW_POWER
    if (tmp102_wakeup(dev)) {
        return TMP102_ERROR;
    }
    xtimer_usleep(TMP102_CONVERSION_TIME);
#endif

#if TMP102_USE_RAW_VALUES
    tmp102_read(dev, ta);
#else
    tmp102_read(dev, &rawtemp);
    tmp102_convert(rawtemp, &tamb);
    *ta = (int16_t)(tamb*100);
#endif
#ifdef TMP102_LOW_POWER
    if (tmp102_sleep(dev)) {
        return TMP102_ERROR;
    }
#endif

    return TMP102_OK;
}
