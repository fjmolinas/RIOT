#ifndef TMP102_REGS_H
#define TMP102_REGS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Register Map
 * @{
 */
#define TMP102_REGS_T                  (0x00)
#define TMP102_REGS_CONFIG             (0x01)
#define TMP102_REGS_T_LOW              (0x02)
#define TMP102_REGS_T_HIGH             (0x03)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* TMP102_REGS_H */
