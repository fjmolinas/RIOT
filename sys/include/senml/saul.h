/*
 * Copyright (C) 2021 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_senml_saul SenML SAUL
 * @ingroup     sys_senml
 * @brief       Functionality for reading @ref drivers_saul sensors as @ref sys_senml
 *
 * The `senml_saul` module contains functions for reading sensors using
 * @ref drivers_saul and converting them to @ref sys_senml_cbor.
 *
 * @{
 *
 * @file
 * @brief       Functionality for reading @ref drivers_saul sensors as @ref sys_senml
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 */

#ifndef SENML_SAUL_H
#define SENML_SAUL_H

#include <stdint.h>
#include "saul.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Use floats instead of decimal types when encoding SAUL measurements.
 *
 * If this is set to `1` the @ref phydat_t values from SAUL are converted to
 * @ref senml_numeric_t using @ref phydat_to_senml_float.
 * Values are converted using @ref phydat_to_senml_decimal otherwise.
 */
#ifndef CONFIG_SENML_SAUL_USE_FLOATS
#define CONFIG_SENML_SAUL_USE_FLOATS    0
#endif

/**
 * @brief   Encode sensor data from a single @ref saul_reg_t as JSON SenML.
 *
 * @param buf   Buffer to store the JSON in.
 * @param len   Length of the buffer.
 * @param dev   The saul_reg_t to read from
 *
 * @return  size of the encoded data.
 */
size_t senml_saulreg_encode_json(uint8_t *buf, size_t len, saul_reg_t *dev);

/**
 * @brief   Encode sensor data from an array of @ref saul_reg_t as JSON SenML.
 *
 * @param buf   Buffer to store the JSON in.
 * @param len   Length of the buffer.
 * @param dev   The first element in the saul_reg_t array
 * @param n     The number of saul_reg_t to encode
 *
 * @return  size of the encoded data.
 */
size_t senml_saulreg_encode_n_json(uint8_t *buf, size_t len, saul_reg_t **dev,
                                   uint8_t n);
/**
 * @brief   Encode all sensors from @ref drivers_saul as JSON SenML.
 *
 * @param buf Buffer to store the JSON in.
 * @param len Length of the buffer.
 *
 * @return  size of the encoded data.
 */
size_t senml_saulreg_encode_all_json(uint8_t *buf, size_t len);

/**
 * @brief   Encode sensor data from an array of @ref saul_reg_t as CBOR SenML.
 *
 * @param buf   Buffer to store the CBOR in.
 * @param len   Length of the buffer.
 * @param dev   The first element in the saul_reg_t array
 * @param n     The number of saul_reg_t to encode
 *
 * @return  size of the encoded data.
 */
size_t senml_saulreg_encode_n_cbor(uint8_t *buf, size_t len, saul_reg_t **dev,
                                   uint8_t n);

/**
 * @brief   Encode sensor data from a single @ref saul_reg_t as JSON SenML.
 *
 * @param buf   Buffer to store the JSON in.
 * @param len   Length of the buffer.
 * @param dev   the saul_reg_t to read from
 *
 * @return  size of the encoded data.
 */
size_t senml_saulreg_encode_cbor(uint8_t *buf, size_t len, saul_reg_t *dev);

/**
 * @brief   Encode all sensors from @ref drivers_saul as CBOR SenML.
 *
 * @param buf Buffer to store the CBOR in.
 * @param len Length of the buffer.
 *
 * @return  size of the encoded data.
 */
size_t senml_saulreg_encode_all_cbor(uint8_t *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* SENML_SAUL_H */
/** @} */
