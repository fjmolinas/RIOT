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

static int _norm_f(double *val)
{
    double value = *val;
    const double pos_exp_thresh = 1e7;
    const double neg_exp_thresh = 1e-5;
    int exp = 0;

    if (value >= pos_exp_thresh) {
        if (value >= 1e256) {
            value /= 1e256;
            exp += 256;
        }
        if (value >= 1e128) {
            value /= 1e128;
            exp += 128;
        }
        if (value >= 1e64) {
            value /= 1e64;
            exp += 64;
        }
        if (value >= 1e32) {
            value /= 1e32;
            exp += 32;
        }
        if (value >= 1e16) {
            value /= 1e16;
            exp += 16;
        }
        if (value >= 1e8) {
            value /= 1e8;
            exp += 8;
        }
        if (value >= 1e4) {
            value /= 1e4;
            exp += 4;
        }
        if (value >= 1e2) {
            value /= 1e2;
            exp += 2;
        }
        if (value >= 1e1) {
            value /= 1e1;
            exp += 1;
        }
    }

    if (value > 0 && value <= neg_exp_thresh) {
        if (value < 1e-255) {
            value *= 1e256;
            value *= 1e128;
            exp -= 128;
        }
        if (value < 1e-63) {
            value *= 1e64;
            exp -= 64;
        }
        if (value < 1e-31) {
            value *= 1e32;
            exp -= 32;
        }
        if (value < 1e-15) {
            value *= 1e16;
            exp -= 16;
        }
        if (value < 1e-7) {
            value *= 1e8;
            exp -= 8;
        }
        if (value < 1e-3) {
            value *= 1e4;
            exp -= 4;
        }
        if (value < 1e-1) {
            value *= 1e2;
            exp -= 2;
        }
        if (value < 1e0) {
            value *= 1e1;
            exp -= 1;
        }
    }

    return exp;
}


static void _split_float(double value, uint32_t *integer, uint32_t *fraction, int16_t *exponent)
{
    uint32_t int_part, dec_part;
    int16_t exp;

    exp = _norm_f(&value);
    int_part = (uint32_t)value;
    double rem = value - int_part;

    rem *= 1e9;
    dec_part = (uint32_t)rem;

    rem -= dec_part;
    if (rem >= 0.5) {
        dec_part++;
        if (dec_part >= 1000000000) {
            dec_part = 0;
            int_part++;
            if (exp != 0 && int_part >= 10) {
                exp++;
                int_part = 1;
            }
        }
    }

    int width = 9;
    while( dec_part % 10 == 0 && width > 0) {
        dec_part /= 10;
        width--;
    }

    *exponent = exp;
    *integer = int_part;
    *fraction = dec_part;
}

static size_t _fmt_float(char *out, double value){
    unsigned negative = (value < 0);

    if (isnan(value)) {
        return fmt_str(out, "nan");
    }

    if (isinf(value)) {
        return fmt_str(out, "inf");
    }

    if (negative) {
        value = -value;
        if (out) {
            *out++ = '-';
        }
    }

    uint32_t integer, fraction;
    int16_t exp;
    _split_float(value, &integer, &fraction, &exp);

    size_t res = fmt_u32_dec(out, integer);
    if (fraction) {
        if (out) {
            out += res;
            *out++ = '.';
        }
        res++;
        size_t tmp = fmt_u32_dec(out, fraction);
        res += tmp;
        if (out) {
            out += tmp;
        }
    }

    if (exp < 0) {
        size_t tmp = fmt_str(out, "e-");
        res += tmp;
        if (out) {
            out += tmp;
        }
        fmt_u16_dec(out, -exp);
    }

    if (exp > 0) {
        if (out) {
            *out++ = 'e';
        }
        res++;
        size_t tmp = fmt_u16_dec(out, exp);
        res += tmp;
        if (out) {
            out += tmp;
        }
    }

    return res;
}

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

void json_senml_encoder_init(json_senml_encoder_t *enc, char *buf, size_t len)
{
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
    size_t len = enc->end - enc->cur;

    base64_encode(data->value, data->len, enc->cur, &len);
    enc->cur += len;
    enc->len += len;
    return len;
}

static int _json_encode_unit(json_senml_encoder_t *enc, const senml_unit_t unit)
{
    return _json_encode_str(enc, senml_unit_to_str(unit));
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
        len += _fmt_float(enc->cur, val->value.f);
        break;
    case SENML_TYPE_NUMERIC_DOUBLE:
        len += _fmt_float(enc->cur, val->value.d);
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
