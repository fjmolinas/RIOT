#ifdef MODULE_TMP102

#include "log.h"
#include "saul_reg.h"

#include "tmp102.h"
#include "tmp102_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define TMP102_NUM     (sizeof(tmp102_params) / sizeof(tmp102_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static tmp102_t tmp102_devs[TMP102_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[TMP102_NUM];

/**
 * @brief   Reference the driver struct
 */
extern const saul_driver_t tmp102_saul_driver;

void auto_init_tmp102(void)
{
    for (unsigned i = 0; i < TMP102_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing tmp102 #%u\n", i);

        if (tmp102_init(&tmp102_devs[i], &tmp102_params[i]) != TMP102_OK) {
            LOG_ERROR("[auto_init_saul] error initializing tmp102 #%u\n", i);
            continue;
        }
#if TMP102_USE_LOW_POWER
        if (tmp102_sleep(&tmp102_devs[i]) != TMP102_OK) {
            LOG_ERROR("[auto_init_saul] error set standby tmp102 #%u\n", i);
            continue;
        }
#endif
        saul_entries[i].dev = &(tmp102_devs[i]);
        saul_entries[i].name = tmp102_saul_info[i].name;
        saul_entries[i].driver = &tmp102_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_TMP102 */
