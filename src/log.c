/*
 * PIC18 Logging
 *
 * Tested on PIC 18F26K22 and Microchip C18 v3.40
 *
 * Copyright (c) 2014 Tomas Barton, tommz9@gmail.com
 * Released under MIT license. See LICENSE.txt.
 *
 */

#include "log.h"
#include "flash.h"
#include "crc16.h"

#include <string.h>
#include <stdio.h>

/* Declarations */

/*Used for quick check if there is a message in storage position. */
#define LOG_MAGIC {'M', 'S', 'G'}
#define LOG_MAGIC_SIZE 3

/* message_t must be LOG_POSITION_SIZE bytes long. */
typedef struct {
    uint8_t magic[LOG_MAGIC_SIZE];
    char data[LOG_POSITION_SIZE - LOG_MAGIC_SIZE - 2];
    uint16_t checksum;
} message_t;

uint8_t log_get_free_index(void);
log_return_t log_save_buffer(void);
void log_create_crc(void);
log_return_t log_check_crc(void);


/* Definitions */

static const rom far char log_magic[LOG_MAGIC_SIZE] = LOG_MAGIC;

/* Local buffer for operations in RAM - we can write to flash only whole pages */
static message_t log_buffer;

/* Reserved space in flash for messages. */
#pragma romdata const_table=0x1000
static const rom far message_t log_storage[LOG_POSITIONS];
#pragma romdata

/* Writes log_buffer to the first free position. */
log_return_t log_save_buffer(){
    uint8_t index;

    index = log_get_free_index();

    if (index >= LOG_POSITIONS) {
        return LOG_FULL;
    }

    log_create_crc();

    flash_erase_page((void*) &log_storage[index]);
    flash_write_page((void*) &log_storage[index], (uint8_t*) & log_buffer);

    memcpypgm2ram(&log_buffer, (const rom far char *) &log_storage[index], LOG_POSITION_SIZE);

    return log_check_crc();
}

/* Computes crc of log_buffer and stores it to log_buffer.checksum */
void log_create_crc(){
    uint16_t crc;
    crc = gen_crc16((uint8_t*)&log_buffer, sizeof(message_t)-2);
    log_buffer.checksum = crc;
}

/* Computes crc of log_buffer and compares it to log_buffer.checksum */
log_return_t log_check_crc(){
    uint16_t crc;
    crc = gen_crc16((uint8_t*)&log_buffer, sizeof(message_t)-2);
    if(crc == log_buffer.checksum){
        return LOG_OK;
    } else {
        return LOG_CRC_ERROR;
    }
}

/* Save message to the first log position. */
log_return_t log_log_string(char * message) {

    memcpypgm2ram(log_buffer.magic, log_magic, sizeof (log_magic));
    strncpy(log_buffer.data, message, sizeof (log_buffer.data));
    /* TODO: Checksum */

    return log_save_buffer();
}

/* Copy next message to buffer. Search is started from *index and after function
 * ends *index is set to 1 position after the found message.
 * Usage
    uint8_t index = 0;
    while((ret = log_get_next_messge(&index, buffer)) == LOG_OK){
        // process buffer
    }
 */
log_return_t  log_get_next_messge(uint8_t *index, char * buffer){
    uint8_t i;
    
    for (i = *index; i < LOG_POSITIONS; ++i) {
        if (strncmppgm((const rom far char*) log_storage[i].magic, log_magic, sizeof (log_magic)) == 0) {
            strncpypgm2ram(buffer, (const rom far char *) log_storage[i].data, LOG_POSITION_SIZE);
            *index = i+1;
            return log_check_crc();
        }
    }

    return LOG_EMPTY;
    
}

/*
 * Returns the count of messages in flash.
 */
uint8_t log_messages_count() {
    uint8_t i, count;

    count = 0;

    /* TODO Checksum*/
    for (i = 0; i < LOG_POSITIONS; ++i) {
        if (strncmppgm((const rom far char*) log_storage[i].magic, log_magic, sizeof (log_magic)) == 0) {
            ++count;
        }
    }

    return count;
}

/*
 * Erase all messages.
 */
log_return_t log_erase_messages() {
    uint8_t i;

    for (i = 0; i < LOG_POSITIONS; ++i) {
        if (strncmppgm((const rom far char*) log_storage[i].magic, log_magic, sizeof (log_magic)) == 0) {
            flash_erase_page((void*) &log_storage[i]);
        }
    }
}

/*
 * Find the first free index in storage.
 */
uint8_t log_get_free_index() {
    uint8_t i;

    for (i = 0; i < LOG_POSITIONS; ++i) {
        if (strncmppgm((const rom far char*) log_storage[i].magic, log_magic, sizeof (log_magic))) {
            break;
        }
    }

    return i;
}

/*
 * Assert helper function.
 */
void log_assert__(const far rom char * fn, unsigned int line, const far rom char *fmt, ...){

    va_list va;
    int index;

    va_start (va, fmt);

    /* TODO sNprintf() */
    memcpypgm2ram(log_buffer.magic, log_magic, sizeof (log_magic));
    index = sprintf(log_buffer.data , (far rom char*)"%HS:%d:", fn, line);

    if(fmt)
        vsprintf(log_buffer.data+index, fmt, va);

    log_save_buffer();
}
