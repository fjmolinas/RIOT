/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       RIOT adaption definition of the "eui64" bsp module
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include "eui64.h"

#define BSP_EUI64_ADDRESS_HI_H      ( 0x0028002F )
#define BSP_EUI64_ADDRESS_HI_L      ( 0x0028002C )
#define BSP_EUI64_ADDRESS_LO_H      ( 0x0028002B )
#define BSP_EUI64_ADDRESS_LO_L      ( 0x00280028 )

void eui64_get(uint8_t* addressToWrite) {
   uint8_t* eui64_flash;

   eui64_flash = (uint8_t*)BSP_EUI64_ADDRESS_LO_H;
   while(eui64_flash >= (uint8_t*)BSP_EUI64_ADDRESS_LO_L) {
      *addressToWrite++ = *eui64_flash--;
   }

   eui64_flash = (uint8_t*)BSP_EUI64_ADDRESS_HI_H;
   while(eui64_flash >= (uint8_t*)BSP_EUI64_ADDRESS_HI_L) {
      *addressToWrite++ = *eui64_flash--;
   }
}
