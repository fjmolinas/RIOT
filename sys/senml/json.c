/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_senml_json
 * @{
 *
 * @file
 * @brief       JSON enconding of SenML data
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "senml.h"
#include "senml/json.h"
#include "fmt.h"
#include "base64.h"


static int _json_array_open(json_senml_encoder_t *enc)
{
    enc->len += fmt_char(enc->cur++, '[');
    return 1;
}

static int _json_array_close(json_senml_encoder_t *enc)
{
    /* a ',' is always appended so remove from the last map */
    fmt_char(enc->cur - 1, ']');
    enc->len += fmt_char(enc->cur++, '\0');
    return 1;
}

void json_senml_encoder_init(json_senml_encoder_t *enc, char *buf, int len)
{
    assert(buf && len != 0);
    enc->len = 0;
    enc->cur = buf;
    enc->end = buf + len;
    _json_array_open(enc);
}

size_t json_senml_encoder_len(json_senml_encoder_t *enc)
{
    return enc->len;
}

size_t json_senml_encoder_end(json_senml_encoder_t *enc)
{
    _json_array_close(enc);
    return enc->len;
}

static int _json_container_open(json_senml_encoder_t *enc)
{
    enc->len += fmt_char(enc->cur++, '{');
    return 1;
}

static int _json_container_close(json_senml_encoder_t *enc)
{
    fmt_char(enc->cur - 1, '}');
    enc->len += fmt_char(enc->cur++, ',');
    return 1;
}

static int _json_encode_key(json_senml_encoder_t *enc, const char *key)
{
    int len = 0;

    len += fmt_char(enc->cur, '\"');
    len += fmt_str(enc->cur + len, key);
    len += fmt_char(enc->cur + len, '\"');
    len += fmt_char(enc->cur + len, ':');
    enc->len += len;
    enc->cur += len;
    return len;
}

static int _json_encode_str(json_senml_encoder_t *enc, const char *str)
{
    int len = 0;

    len += fmt_char(enc->cur, '\"');
    len += fmt_str(enc->cur + len, str);
    len += fmt_char(enc->cur + len, '\"');
    len += fmt_char(enc->cur + len, ',');
    enc->len += len;
    enc->cur += len;
    return len;
}

static int _json_encode_data(json_senml_encoder_t *enc, const senml_data_value_t *data)
{
    int len = enc->end - enc->cur;

    base64_encode(data->value, data->len, enc->cur, &len);
    enc->cur += len;
    enc->len += len;
    return len;
}

static int _json_encode_unit(json_senml_encoder_t *enc, const senml_unit_t unit)
{
    return _json_encode_str(enc, senml_unit_to_str(unit));
}

static uint8_t _required_precision(float f)
{
    uint8_t i;

    for (i = 0; f += f; i++) {
        if (f == rint(f)) {
            break;
        }
    }
    return i;
}

static int _json_encode_numeric(json_senml_encoder_t *enc, const senml_numeric_t *val)
{
    int len = 0;

    switch (val->type) {
    case SENML_TYPE_NUMERIC_UINT:
        if (val->value.u >= UINT32_MAX) {
            len += fmt_u64_dec(enc->cur, val->value.u);
        }
        len += fmt_u32_dec(enc->cur, val->value.u);
        break;
    case SENML_TYPE_NUMERIC_INT:
        if (val->value.i >= INT32_MAX || val->value.i <= INT32_MIN) {
            len += fmt_s64_dec(enc->cur, val->value.i);
        }
        len += fmt_s32_dec(enc->cur, val->value.i);
        break;
    case SENML_TYPE_NUMERIC_FLOAT:
        len += fmt_float(enc->cur, val->value.f, _required_precision(val->value.f));
        break;
    case SENML_TYPE_NUMERIC_DOUBLE:
        len += sprintf(enc->cur, "%lf", val->value.d);
        break;
    case SENML_TYPE_NUMERIC_DECFRAC:
        len += fmt_s32_dec(enc->cur, val->value.df.m);
        len += fmt_char(enc->cur + len, 'e');
        len += fmt_s32_dec(enc->cur + len, val->value.df.e);
        break;
    default:
        len += fmt_u16_dec(enc->cur, 0);
        break;
    }
    /* always append a ',' to split element */
    len += fmt_char(enc->cur + len, ',');
    enc->len += len;
    enc->cur += len;
    return len;
}

static int _json_encode_common(json_senml_encoder_t *enc, const senml_attr_t *attr)
{
    int len = 0;

    if (attr->base_name != NULL) {
        len += _json_encode_key(enc, SENML_JSON_BASE_NAME);
        len += _json_encode_str(enc, attr->base_name);
    }
    if (attr->base_time.value.u != 0) {
        len +=  _json_encode_key(enc, SENML_JSON_BASE_TIME);
        len +=  _json_encode_numeric(enc, &attr->base_time);
    }
    if (attr->base_value.value.u != 0) {
        len +=  _json_encode_key(enc, SENML_JSON_BASE_VALUE);
        len += _json_encode_numeric(enc, &attr->base_value);
    }
    if (attr->base_sum.value.u != 0) {
        len += _json_encode_key(enc, SENML_JSON_BASE_SUM);
        len +=  _json_encode_numeric(enc, &attr->base_sum);
    }
    if (attr->base_unit != SENML_UNIT_NONE) {
        len +=  _json_encode_key(enc, SENML_JSON_BASE_UNIT);
        len +=  _json_encode_unit(enc, attr->base_unit);
    }
    if (attr->base_version != 0 && attr->base_version != 10) {
        len +=  _json_encode_key(enc, SENML_JSON_VERSION);
        senml_numeric_t val = { .type = SENML_TYPE_NUMERIC_UINT,
                                .value.u = attr->base_version };
        len += _json_encode_numeric(enc, &val);
    }
    if (attr->name != NULL) {
        len += _json_encode_key(enc, SENML_JSON_NAME);
        len += _json_encode_str(enc, attr->name);
    }
    if (attr->time.value.u != 0) {
        len +=  _json_encode_key(enc, SENML_JSON_TIME);
        len +=  _json_encode_numeric(enc, &attr->time);
    }
    if (attr->sum.value.u != 0) {
        len += _json_encode_key(enc, SENML_JSON_SUM);
        len += _json_encode_numeric(enc, &attr->sum);
    }
    if (attr->update_time.value.u != 0) {
        len += _json_encode_key(enc, SENML_JSON_UPDATE_TIME);
        len += _json_encode_numeric(enc, &attr->update_time);
    }
    if (attr->unit != SENML_UNIT_NONE) {
        len +=  _json_encode_key(enc, SENML_JSON_UNIT);
        len += _json_encode_unit(enc, attr->unit);
    }
    return len;
}


static size_t _json_encode_bool(json_senml_encoder_t *enc, const bool val)
{
    int len = 0;

    if (val) {
        len += fmt_str(enc->cur, "true");
    }
    else {
        len += fmt_str(enc->cur, "false");
    }
    len += fmt_char(enc->cur + len, ',');
    enc->len += len;
    enc->cur += len;
    return len;
}

int senml_encode_bool_json(json_senml_encoder_t *enc, const senml_bool_value_t *val)
{
    int len = 0;

    len += _json_container_open(enc);
    len += _json_encode_common(enc, &val->attr);
    len += _json_encode_key(enc, SENML_JSON_BOOL_VALUE);
    len += _json_encode_bool(enc, val->value);
    len += _json_container_close(enc);
    return len;
}

int senml_encode_sum_json(json_senml_encoder_t *enc, const senml_attr_t *attr)
{
    int len = 0;

    len += _json_container_open(enc);
    len += _json_encode_common(enc, attr);
    len += _json_container_close(enc);
    return len;
}

int senml_encode_string_json(json_senml_encoder_t *enc, const senml_string_value_t *val)
{
    int len = 0;

    len += _json_container_open(enc);
    len += _json_encode_common(enc, &val->attr);
    len += _json_encode_key(enc, SENML_JSON_STRING_VALUE);
    len += _json_encode_str(enc, val->value);
    len += _json_container_close(enc);
    return len;
}

int senml_encode_data_json(json_senml_encoder_t *enc, const senml_data_value_t *val)
{
    int len = 0;

    len += _json_container_open(enc);
    len += _json_encode_common(enc, &val->attr);
    len += _json_encode_key(enc, SENML_JSON_DATA_VALUE);
    len += _json_encode_data(enc, val);
    len += _json_container_close(enc);
    return len;
}

int senml_encode_value_json(json_senml_encoder_t *enc, const senml_value_t *val)
{
    int len = 0;

    len += _json_container_open(enc);
    len += _json_encode_common(enc, &val->attr);
    len += _json_encode_key(enc, SENML_JSON_VALUE);
    len += _json_encode_numeric(enc, &val->value);
    len += _json_container_close(enc);
    return len;
}
