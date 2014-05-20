/*
 * PIC18 Test file for log.c
 *
 * Tested on PIC 18F26K22 and Microchip C18 v3.40
 *
 * Copyright (c) 2014 Tomas Barton, tommz9@gmail.com
 * Released under MIT license. See LICENSE.txt.
 *
 */


#include <p18f26k22.h>
#include <string.h>
#include <stdio.h>

#include "log.h"

#pragma config FOSC = INTIO67
#pragma config PLLCFG = OFF
#pragma config WDTEN = OFF
#pragma config PBADEN = OFF
#pragma config DEBUG = ON
#pragma config LVP = OFF

void my_putc(char c) {
    while (!PIR1bits.TX1IF) {
        Nop();
    }
    TX1REG = c;
}

void my_puts(const char *s) {
    while (*s) {
        my_putc(*s);
        s++;
    }
}

void my_putsr(const rom char *s) {
    while (*s) {
        my_putc(*s);
        s++;
    }
}

void main(void) {

    char buffer[64] = "Test";
    uint8_t index, ret;

    /* Enable UART,  9600 8N1*/
    ANSELC = 0x00;
    TX1STAbits.TXEN1 = 1;
    TX1STAbits.BRGH1 = 1;
    RCSTA1bits.SPEN1 = 1;
    BAUDCON1bits.BRG16 = 1;

    /* For 1 MHz clock */
    SPBRGH1 = 0;
    SPBRG1 = 25;

    my_putsr("\r\n\rLOG Test\r\n");

    /* Test: Empty after programming. */
    if (log_messages_count() != 0) {
        my_putsr("Error 1\r\n");
        goto end;
    }

    /* Test: Can hold LOG_POSITIONS values. */
    for (index = 0; index < LOG_POSITIONS; ++index) {
        ret = log_log_string(buffer);
        if (ret != LOG_OK) {
            my_putsr("Error 2\r\n");
            goto end;
        }
    }

    /* Test: Cannot hold more messages then LOG_POSITIONS. */
    ret = log_log_string(buffer);
    if (ret != LOG_FULL) {
        my_putsr("Error 3\r\n");
        goto end;
    }

    if (log_messages_count() != LOG_POSITIONS) {
        my_putsr("Error 4\r\n");
        goto end;
    }

    /* After erase is empty. */
    log_erase_messages();
    if (log_messages_count() != 0) {
        my_putsr("Error 5\r\n");
        goto end;
    }

    /* Test: Can hold LOG_POSITIONS values after Erase. */
    for (index = 0; index < LOG_POSITIONS; ++index) {
        ret = log_log_string(buffer);
        if (ret != LOG_OK) {
            my_putsr("Error 6\r\n");
            goto end;
        }
    }

    /* Check if we can read all messages. */
    index = 0;
    while((ret = log_get_next_messge(&index, buffer)) == LOG_OK){

    }

    if(index != LOG_POSITIONS){
        if(ret == LOG_CRC_ERROR){
            my_putsr("CRC Error\r\n");
        }
        my_putsr("Error 7\r\n");
        goto end;
    }

    my_putsr("All tests OK.\r\n\r\n");


end:
    while (1) {
        Nop();
    }

}

