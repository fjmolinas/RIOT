#ifndef TMP102_PARAMS_H
#define TMP102_PARAMS_H

#include "saul_reg.h"
#include "board.h"
#include "tmp102.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the TMP102 driver
 * @{
 */
#ifndef TMP102_PARAM_I2C
#define TMP102_PARAM_I2C           I2C_DEV(0)
#endif
#ifndef TMP102_PARAM_ADDR
#define TMP102_PARAM_ADDR          (TMP102_I2C_ADDRESS)
#endif
#ifndef TMP102_PARAM_RATE
#define TMP102_PARAM_RATE           (TMP102_CONFIG_CR_DEF)
#endif

#define TMP102_PARAMS_DEFAULT    {  .i2c  = TMP102_PARAM_I2C, \
                                    .addr = TMP102_PARAM_ADDR, \
                                    .rate = TMP102_PARAM_RATE}
/**@}*/

/**
 * @brief   TMP102 configuration
 */
static const tmp102_params_t tmp102_params[] =
{
#ifdef TMP102_PARAMS_BOARD
    TMP102_PARAMS_BOARD,
#else
    TMP102_PARAMS_DEFAULT,
#endif
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
const saul_reg_info_t tmp102_saul_info[] =
{
    { .name = "tmp102" }
};

#ifdef __cplusplus
}
#endif

#endif /* TMP102_PARAMS_H */
/** @} */
