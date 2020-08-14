#ifndef CONTROL_H
#define CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <float.h>

/**
 * @brief Starts ranging
 */
void *init_ranging(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* CONTROL_H */
