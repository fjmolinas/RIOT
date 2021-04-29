/*
 * Copyright (C) 2021 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Short SenML CBOR test
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "embUnit.h"
#include "senml/json.h"
#include "fmt.h"

#define BUF_SIZE (256)

char json_buf[BUF_SIZE];
char expect[] = "[{\"bn\":\"JSON test\",\"bt\":1619264720,\"ut\":120,\"u\":\"m\",\"v\":61},"
                "{\"t\":1,\"v\":61.500000},{\"t\":2,\"v\":61},{\"t\":3,\"v\":61},"
                "{\"t\":4,\"v\":615e-1},{\"t\":5,\"vb\":true},"
                "{\"t\":6,\"vs\":\"RIOT OS\"},{\"t\":7,\"vd\":AAECAw=},"
                "{\"s\":61,\"u\":\"kg\"}]";

void test_senml_encode(void)
{
    json_senml_encoder_t enc;

    /* Some common attributes to set on the first element */
    senml_attr_t attr = {
        .base_name = "JSON test",
        .base_time = senml_duration_s(1619264720),
        .update_time = senml_duration_s(120),
        .unit = SENML_UNIT_METER,
    };

    /* A numeric (float) value */
    senml_value_t vf = { .attr = attr, .value = senml_float(61.5) };

    /* A numeric (double) value */
    senml_value_t vd = { .attr = { .time = senml_duration_s(1) },
                         .value = senml_double(61.5) };

    /* A numeric (int) value */
    senml_value_t vi = { .attr = { .time = senml_duration_s(2) },
                         .value = senml_int(61) };

    /* A numeric (uint) value */
    senml_value_t vu = { .attr = { .time = senml_duration_s(3) },
                         .value = senml_uint(61) };

    /* A numeric (decimal fraction) value */
    senml_value_t vdf = { .attr = { .time = senml_duration_s(4) },
                          .value = senml_decfrac(615, -1) };

    /* A boolean value */
    senml_bool_value_t vb = { .attr = { .time = senml_duration_s(5) },
                              .value = true };

    /* A string value */
    char string[] = "RIOT OS";
    senml_string_value_t vs = { .attr = { .time = senml_duration_s(6) },
                                .value = string, .len = sizeof string - 1 };

    /* A data value */
    uint8_t data[] = { 0x00, 0x01, 0x02, 0x03 };
    senml_data_value_t vdat = { .attr = { .time = senml_duration_s(7) },
                                .value = data, .len = sizeof data };

    /* A numeric (float) sum value */
    senml_attr_t sum = {
        .sum = senml_int(61),
        .unit = SENML_UNIT_KILOGRAM,
    };

    /* Initialize encoder, and start array */
    json_senml_encoder_init(&enc, json_buf, sizeof(json_buf));

    /* Encode the values */
    senml_encode_value_json(&enc, &vf);
    senml_encode_value_json(&enc, &vd);
    senml_encode_value_json(&enc, &vi);
    senml_encode_value_json(&enc, &vu);
    senml_encode_value_json(&enc, &vdf);
    senml_encode_bool_json(&enc, &vb);
    senml_encode_string_json(&enc, &vs);
    senml_encode_data_json(&enc, &vdat);
    senml_encode_sum_json(&enc, &sum);

    size_t len = json_senml_encoder_end(&enc);
    print_str((char*) json_buf);
    TEST_ASSERT_EQUAL_INT(strlen(json_buf), strlen(expect));
    TEST_ASSERT_EQUAL_INT(0, strncmp(expect, json_buf, len));
}

Test *tests_senml(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_senml_encode),
    };
    EMB_UNIT_TESTCALLER(senml_tests, NULL, NULL, fixtures);
    return (Test *)&senml_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_senml());
    TESTS_END();
    return 0;
}
