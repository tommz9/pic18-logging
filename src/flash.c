/*
 * PIC18 Flash write/erase library
 *
 * Tested on PIC 18F26K22 and Microchip C18 v3.40
 *
 * Copyright (c) 2014 Tomas Barton, tommz9@gmail.com
 * Released under MIT license. See LICENSE.txt.
 *
 */

#include "flash.h"

#include <p18f26k22.h>

#define FLASH_PAGE_MASK 0x3F
#define FLASH_PAGE_SIZE 64u

/*
 * Erases (sets memory to 0xFF) one 64 B page in memory.
 * Address must be 64 B aligned.
 */
void
flash_erase_page(void *address) {
    uint8_t i_enabled;
    uint24_t _a = (uint24_t) address;

    i_enabled = INTCONbits.GIE; // Probably not safe
    if (i_enabled)
        INTCONbits.GIE = 0;

    _a &= ~((uint24_t) FLASH_PAGE_MASK);

    /* Setup page erase operation. */
    TBLPTR = _a;
    EECON1bits.EEPGD = 1;
    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1;
    EECON1bits.FREE = 1;

    /* Start operation. */
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;

    if (i_enabled)
        INTCONbits.GIE = 1;
}

/*
 * Writes 64 B from data in ram to address in flash.
 * Address must be 64B aligned. Page erase should be performed prior to write
 * because page_write can write only zeros. (erase on the other side writes ones)
 */
void
flash_write_page(void *address, uint8_t * data) {
    uint8_t i_enabled, i;
    uint24_t _a = (uint24_t) address;

    i_enabled = INTCONbits.GIE; // Probably not safe
    if (i_enabled)
        INTCONbits.GIE = 0;

    _a &= ~((uint24_t) FLASH_PAGE_MASK);

    TBLPTR = 0;

    /* Copy data to holding registers. */
    for (i = 0u; i < FLASH_PAGE_SIZE; i++) {
        TABLAT = data[i];
        _asm
        TBLWTPOSTINC
                _endasm
    }

    TBLPTR = _a;

    /* Setup memory write operation. */
    EECON1 = 0x00;
    EECON1bits.EEPGD = 1;
    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1;

    /* Start operation. */
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;

    if (i_enabled)
        INTCONbits.GIE = 1;
}
