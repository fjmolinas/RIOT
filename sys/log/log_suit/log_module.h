#ifndef LOG_MODULE_H
#define LOG_MODULE_H

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "log.h"

#include "suitreg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Default log_write function, just maps to printf
 */
#define log_write(level, ...) printf(__VA_ARGS__)

/**
 * @brief log_write_suitreg writes the log message as well as calling `suitreg_notify`
 *
 * @param[in] level     Level of the message
 * @param[in] reg_type   The @ref suitreg_type_t
 * @param[in] type       The message type.
 * @param[in] content    Content of the message, can also be a pointer to a structure
 * @param[in] format     String that the function will print
 */
static inline void log_write_suitreg(unsigned level, uint8_t reg_type, uint16_t type, uint32_t content, const char *format, ...)
{
#ifdef MODULE_SUITREG
    suitreg_notify(reg_type, type, content);
#else
    (void) reg_type;
    (void) content;
    (void) type;
#endif
    assert(level > 0);

    if(level < LOG_LEVEL) {
        va_list args;
        va_start(args, format);

        vprintf(format, args);

        va_end(args);
    }
}
#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* LOG_MODULE_H */
