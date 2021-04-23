/*
 * Copyright (C) 2021 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#include "saul.h"
#include "saul_reg.h"
#include "senml.h"
#include "senml/phydat.h"
#include "senml/json.h"
#include "senml/cbor.h"
#include "senml/saul.h"

/**
 * @brief SenML encoder type
 */
typedef enum senml_enc_type {
    SENML_ENC_TYPE_CBOR,        /**< CBOR encoder */
    SENML_ENC_TYPE_JSON,        /**< JSON encoder */
} senml_enc_type_t;

static void senml_encode_phydat_bool(void *enc, senml_enc_type_t type,
                                     const saul_reg_t *dev,
                                     const phydat_t *data,
                                     const uint8_t dim)
{
    senml_bool_value_t val = { .attr = { .name = dev->name } };

    phydat_to_senml_bool(&val, data, dim);
    if (type == SENML_ENC_TYPE_JSON && IS_USED(MODULE_SENML_JSON)) {
        senml_encode_bool_json(enc, &val);
    }
    else if (type == SENML_ENC_TYPE_CBOR && IS_USED(MODULE_SENML_CBOR)) {
        senml_encode_bool_cbor(enc, &val);
    }
}

static void senml_encode_phydat_time(void *enc, senml_enc_type_t type,
                                     const saul_reg_t *dev,
                                     const phydat_t *data)
{
    senml_value_t val = { .attr = { .name = dev->name } };

    phydat_time_to_senml(&val, data);
    if (type == SENML_ENC_TYPE_JSON && IS_USED(MODULE_SENML_JSON)) {
        senml_encode_value_json(enc, &val);
    }
    else if (type == SENML_ENC_TYPE_CBOR && IS_USED(MODULE_SENML_CBOR)) {
        senml_encode_value_cbor(enc, &val);
    }
}

static void senml_encode_phydat_date(void *enc, senml_enc_type_t type,
                                     const saul_reg_t *dev,
                                     const phydat_t *data)
{
    senml_value_t val = { .attr = { .name = dev->name } };

    phydat_date_to_senml(&val, data);
    if (type == SENML_ENC_TYPE_JSON && IS_USED(MODULE_SENML_JSON)) {
        senml_encode_value_json(enc, &val);
    }
    else if (type == SENML_ENC_TYPE_CBOR && IS_USED(MODULE_SENML_CBOR)) {
        senml_encode_value_cbor(enc, &val);
    }
}

static uint8_t senml_fix_unit(const saul_reg_t *dev, const uint8_t unit)
{
    /* Fix the unit for relative humidity. */
    if (dev->driver->type == SAUL_SENSE_HUM &&
        unit == SENML_UNIT_PERCENT) {
        return SENML_UNIT_RELATIVE_HUMIDITY_PERCENT;
    }
    return unit;
}

static void senml_encode_phydat_float(void *enc, senml_enc_type_t type,
                                      const saul_reg_t *dev,
                                      const phydat_t *data, const uint8_t dim)
{
    senml_value_t val = { .attr = { .name = dev->name } };

    phydat_to_senml_float(&val, data, dim);
    val.attr.unit = senml_fix_unit(dev, val.attr.unit);
    if (type == SENML_ENC_TYPE_JSON && IS_USED(MODULE_SENML_JSON)) {
        senml_encode_value_json(enc, &val);
    }
    else if (type == SENML_ENC_TYPE_CBOR && IS_USED(MODULE_SENML_CBOR)) {
        senml_encode_value_cbor(enc, &val);
    }
}

static void senml_encode_phydat_decimal(void *enc, senml_enc_type_t type,
                                        const saul_reg_t *dev,
                                        const phydat_t *data, const uint8_t dim)
{
    senml_value_t val = { .attr = { .name = dev->name } };

    phydat_to_senml_decimal(&val, data, dim);
    val.attr.unit = senml_fix_unit(dev, val.attr.unit);
    if (type == SENML_ENC_TYPE_JSON && IS_USED(MODULE_SENML_JSON)) {
        senml_encode_value_json(enc, &val);
    }
    else if (type == SENML_ENC_TYPE_CBOR && IS_USED(MODULE_SENML_CBOR)) {
        senml_encode_value_cbor(enc, &val);
    }
}

static void senml_saulreg_encode_record(void *enc, senml_enc_type_t type, saul_reg_t *dev)
{
    phydat_t data;
    int dim = saul_reg_read(dev, &data);

    if (dim <= 0) {
        return;
    }

    if (data.unit == UNIT_DATE) {
        senml_encode_phydat_date(enc, type, dev, &data);
    }

    if (data.unit == UNIT_TIME) {
        senml_encode_phydat_time(enc, type, dev, &data);
    }

    for (uint8_t i = 0; i < dim; i++) {
        if (data.unit == UNIT_BOOL) {
            senml_encode_phydat_bool(enc, type, dev, &data, i);
        }
        else if (CONFIG_SENML_SAUL_USE_FLOATS) {
            senml_encode_phydat_float(enc, type, dev, &data, i);
        }
        else {
            senml_encode_phydat_decimal(enc, type, dev, &data, i);
        }
    }
}

size_t senml_saulreg_encode_json(uint8_t *buf, size_t len, saul_reg_t *dev)
{
    json_senml_encoder_t enc;

    json_senml_encoder_init(&enc, (char *)buf, len);
    senml_saulreg_encode_record(&enc, SENML_ENC_TYPE_JSON, dev);
    return json_senml_encoder_end(&enc);
}

size_t senml_saulreg_encode_n_json(uint8_t *buf, size_t len, saul_reg_t **dev,
                                   uint8_t n)
{
    json_senml_encoder_t enc;

    json_senml_encoder_init(&enc, (char *)buf, len);
    for (uint8_t i = 0; i < n; i++) {
        senml_saulreg_encode_record(&enc, SENML_ENC_TYPE_JSON, *dev++);
    }
    return json_senml_encoder_end(&enc);
}

size_t senml_saulreg_encode_all_json(uint8_t *buf, size_t len)
{
    json_senml_encoder_t enc;
    saul_reg_t *dev = saul_reg;

    json_senml_encoder_init(&enc, (char *)buf, len);
    while (dev) {
        senml_saulreg_encode_record(&enc, SENML_ENC_TYPE_JSON, dev);
        dev = dev->next;
    }
    return json_senml_encoder_end(&enc);
}

size_t senml_saulreg_encode_n_cbor(uint8_t *buf, size_t len, saul_reg_t **dev,
                                    uint8_t n)
{
#if IS_ACTIVE(MODULE_NANOCBOR)
    nanocbor_encoder_t enc;

    nanocbor_encoder_init(&enc, buf, len);
    nanocbor_fmt_array(&enc, n);
    for (uint8_t i = 0; i < n; i++) {
        senml_saulreg_encode_record(&enc, SENML_ENC_TYPE_CBOR, *dev++);
    }
    return nanocbor_encoded_len(&enc);
#else
    (void) buf;
    (void) len;
    (void) dev;
    (void) n;
    return -1;
#endif
}

size_t senml_saulreg_encode_cbor(uint8_t *buf, size_t len, saul_reg_t *dev)
{
#if IS_ACTIVE(MODULE_NANOCBOR)
    nanocbor_encoder_t enc;

    nanocbor_encoder_init(&enc, buf, len);
    nanocbor_fmt_array(&enc, 1);
    senml_saulreg_encode_record(&enc, SENML_ENC_TYPE_CBOR, dev);
    return nanocbor_encoded_len(&enc);
#else
    (void) buf;
    (void) len;
    (void) dev;
    return -1;
#endif
}

size_t senml_saulreg_encode_all_cbor(uint8_t *buf, size_t len)
{
#if IS_ACTIVE(MODULE_NANOCBOR)
    nanocbor_encoder_t enc;

    saul_reg_t *dev = saul_reg;
    nanocbor_encoder_init(&enc, buf, len);
    nanocbor_fmt_array_indefinite(&enc);
    while (dev) {
        senml_saulreg_encode_record(&enc, SENML_ENC_TYPE_CBOR, dev);
        dev = dev->next;
    }
    nanocbor_fmt_end_indefinite(&enc);
    return nanocbor_encoded_len(&enc);
#else
    (void) buf;
    (void) len;
    return -1;
#endif
}

