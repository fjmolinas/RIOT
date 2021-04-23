/*
 * Copyright (C) 2021 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_senml_json SenML JSON
 * @ingroup     sys_senml
 * @brief       Functionality for encoding SenML values as JSON
 *
 * @{
 *
 * @file
 * @brief       Functionality for encoding SenML values as JSON
 *
 */

#ifndef SENML_JSON_H
#define SENML_JSON_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "senml.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SenML JSON Types
 * @{
 */
#define SENML_JSON_VERSION       "bver" /**< Key for the version attribute in JSON documents       */
#define SENML_JSON_BASE_NAME     "bn"   /**< Key for the base name attribute in JSON documents     */
#define SENML_JSON_BASE_TIME     "bt"   /**< Key for the base time attribute in JSON documents     */
#define SENML_JSON_BASE_UNIT     "bu"   /**< Key for the base unit attribute in JSON documents     */
#define SENML_JSON_BASE_VALUE    "bv"   /**< Key for the base value attribute in JSON documents    */
#define SENML_JSON_BASE_SUM      "bs"   /**< Key for the base sum attribute in JSON documents      */
#define SENML_JSON_NAME          "n"    /**< Key for the name attribute in JSON documents          */
#define SENML_JSON_UNIT          "u"    /**< Key for the unit attribute in JSON documents          */
#define SENML_JSON_VALUE         "v"    /**< Key for the value attribute in JSON documents         */
#define SENML_JSON_STRING_VALUE  "vs"   /**< Key for the string value attribute in JSON documents  */
#define SENML_JSON_BOOL_VALUE    "vb"   /**< Key for the boolean value attribute in JSON documents */
#define SENML_JSON_SUM           "s"    /**< Key for the value sum attribute in JSON documents     */
#define SENML_JSON_TIME          "t"    /**< Key for the time attribute in JSON documents          */
#define SENML_JSON_UPDATE_TIME   "ut"   /**< Key for the update time attribute in JSON documents   */
#define SENML_JSON_DATA_VALUE    "vd"   /**< Key for the data value attribute in JSON documents    */
#define SENML_JSON_LINK          "l"    /**< Key for the link attribute in JSON documents */
/** @} */

/**
 * @brief JSON SenML encoder context
 */
typedef struct json_senml_encoder {
    char *cur;  /**< Current position in the buffer */
    char *end;  /**< end of the buffer */
    size_t len; /**< Length in bytes of supplied JSON data. Incremented
                     separate from the buffer check  */
} json_senml_encoder_t;

/**
 * @brief Init the encoding of a JSON senml pack
 *
 * @note this opens the JSON array
 *
 * @param enc JSON SenML encoder.
 * @param buf buffer to write into
 * @param len length of the buffer
 */
void json_senml_encoder_init(json_senml_encoder_t *enc, char *buf, size_t len);

/**
 * @brief End the encoding of a JSON senml pack
 *
 * @note this closes the JSON array
 *
 * @param enc JSON SenML encoder.
 *
 * @return Size of the encoded data.
 */
size_t json_senml_encoder_end(json_senml_encoder_t *enc);

/**
 * @brief Return the length of the encoded JSON
 *
 * @param enc JSON SenML encoder.
 *
 * @return Size of the encoded data.
 */
size_t json_senml_encoder_len(json_senml_encoder_t *enc);

/**
 * @brief Encode @ref senml_attr_t containing `sum` as JSON.
 *
 * @param enc JSON SenML encoder.
 * @param attr Attributes (including `sum`) to encode.
 *
 * @return Size of the encoded data.
 */
int senml_encode_sum_json(json_senml_encoder_t *enc, const senml_attr_t *attr);

/**
 * @brief Encode @ref senml_bool_value_t as JSON.
 *
 * @param enc JSON SenML encoder.
 * @param val value to encode.
 *
 * @return Size of the encoded data.
 */
int senml_encode_bool_json(json_senml_encoder_t *enc, const senml_bool_value_t *val);

/**
 * @brief Encode @ref senml_value_t as JSON.
 *
 * @param enc JSON SenML encoder.
 * @param val value to encode.
 *
 * @return Size of the encoded data.
 */
int senml_encode_value_json(json_senml_encoder_t *enc, const senml_value_t *val);

/**
 * @brief Encode @ref senml_string_value_t as JSON.
 *
 * @param enc JSON SenML encoder.
 * @param val value to encode.
 *
 * @return Size of the encoded data.
 */
int senml_encode_string_json(json_senml_encoder_t *enc, const senml_string_value_t *val);

/**
 * @brief Encode @ref senml_data_value_t as JSON.
 *
 * @param enc JSON SenML encoder.
 * @param val value to encode.
 *
 * @return Size of the encoded data.
 */
int senml_encode_data_json(json_senml_encoder_t *enc, const senml_data_value_t *val);

#ifdef __cplusplus
}
#endif

#endif /* SENML_JSON_H */
/** @} */
