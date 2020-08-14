/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       System logging header for uwb-core
 *
 */

#ifndef DPL_LOG_H
#define DPL_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "log.h"

#define LOG_WARN(...)       LOG(LOG_WARNING, __VA_ARGS__)
#define LOG_CRITICAL(...)   LOG(LOG_ERROR, __VA_ARGS__)

struct log {
};

#define log_register(__X, __Y, __Z, __A, __B) {}

#ifdef __cplusplus
}
#endif

#endif /* DPL_LOG_H */
