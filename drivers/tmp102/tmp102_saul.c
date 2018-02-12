/**
 * @ingroup     drivers_tmp102
 * @{
 *
 * @file
 * @brief       TMP102 adaption to the RIOT actuator/sensor interface
 *
 * @author      Francisco Molina fFrancisco.molina@inria.cl>
 *
 * @}
 */

#include <string.h>

#include "saul.h"
#include "tmp102.h"

static int read_temp(const void *dev, phydat_t *res)
{
    if (tmp102_read_temperature((const tmp102_t *)dev, &res->val[0]) != TMP102_OK) {
        return -ECANCELED;
    }
    res->val[1] = 0;
    res->val[2] = 0;
#if TMP102_USE_RAW_VALUES
    res->unit = UNIT_NONE;
    res->scale = 0;
#else
    res->unit = UNIT_TEMP_C;
    res->scale = -2;
#endif
    return 1;
}

const saul_driver_t tmp102_saul_driver = {
    .read = read_temp,
    .write = saul_notsup,
    .type = SAUL_SENSE_OBJTEMP,
};
