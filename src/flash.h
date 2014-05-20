/*
 * PIC18 Flash write/erase library
 *
 * Tested on PIC 18F26K22 and Microchip C18 v3.40
 *
 * Copyright (c) 2014 Tomas Barton, tommz9@gmail.com
 * Released under MIT license. See LICENSE.txt.
 *
 */

#ifndef FLASH_H
#define	FLASH_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdint.h"

    void flash_erase_page(void *address);
    void flash_write_page(void *address, uint8_t * data);

#ifdef	__cplusplus
}
#endif

#endif	/* FLASH_H */
