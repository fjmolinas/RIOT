#ifndef SUIT_LOG_H
#define SUIT_LOG_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

#include "log.h"

#define LOG_SUITREG(level, reg_type, type, content, ...) do { log_write_suitreg(level, reg_type, type, content, __VA_ARGS__); } while (0U)

#define LOG_ERROR_SUITREG(reg_type, type, content, fmt, ...)   LOG_SUITREG(LOG_ERROR  , reg_type, type, content, fmt, ##__VA_ARGS__)
#define LOG_WARNING_SUITREG(reg_type, type, content, fmt, ...) LOG_SUITREG(LOG_WARNING, reg_type, type, content, fmt, ##__VA_ARGS__)
#define LOG_INFO_SUITREG(reg_type, type, content, fmt, ...)    LOG_SUITREG(LOG_INFO   , reg_type, type, content, fmt, ##__VA_ARGS__)
#define LOG_DEBUG_SUITREG(reg_type, type, content, fmt, ...)   LOG_SUITREG(LOG_DEBUG  , reg_type, type, content, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */

#endif /* SUIT_LOG_H */
