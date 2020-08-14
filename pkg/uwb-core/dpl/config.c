/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "config/config.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

int conf_value_from_str(char *val_str, enum conf_type type, void *vp, int maxlen)
{
    int32_t val;
    int64_t val64;
    char *eptr;

    if (!val_str) {
        goto err;
    }
    switch (type) {
    case CONF_INT8:
    case CONF_INT16:
    case CONF_INT32:
    case CONF_BOOL:
        val = strtol(val_str, &eptr, 0);
        if (*eptr != '\0') {
            goto err;
        }
        if (type == CONF_BOOL) {
            if (val < 0 || val > 1) {
                goto err;
            }
            *(bool *)vp = val;
        } else if (type == CONF_INT8) {
            if (val < INT8_MIN || val > UINT8_MAX) {
                goto err;
            }
            *(int8_t *)vp = val;
        } else if (type == CONF_INT16) {
            if (val < INT16_MIN || val > UINT16_MAX) {
                goto err;
            }
            *(int16_t *)vp = val;
        } else if (type == CONF_INT32) {
            *(int32_t *)vp = val;
        }
        break;
    case CONF_INT64:
        val64 = strtoll(val_str, &eptr, 0);
        if (*eptr != '\0') {
            goto err;
        }
        *(int64_t *)vp = val64;
        break;
    case CONF_STRING:
        val = strlen(val_str);
        if (val + 1 > maxlen) {
            goto err;
        }
        strcpy(vp, val_str);
        break;
    default:
        goto err;
    }
    return 0;
err:
    return EINVAL;
}
