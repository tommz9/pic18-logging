/*
 * PIC18 Logging
 *
 * Tested on PIC 18F26K22 and Microchip C18 v3.40
 *
 * Copyright (c) 2014 Tomas Barton, tommz9@gmail.com
 * Released under MIT license. See LICENSE.txt.
 *
 */


#ifndef LOG_H
#define	LOG_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdint.h"

    typedef enum {
        LOG_OK = 0,
        LOG_FULL,
        LOG_EMPTY,
        LOG_CRC_ERROR
    } log_return_t;

#define LOG_POSITIONS 16u
#define LOG_POSITION_SIZE 64u

    log_return_t log_log_string(char * message);

    log_return_t log_get_next_messge(uint8_t *index, char * buffer);

    uint8_t log_messages_count(void);

    log_return_t log_erase_messages(void);

    void log_assert__(const far rom char * fn, unsigned int line, const far rom char *fmt, ...);

#ifdef NDEBUG
#define log_assert_message(cond, fmt, ...)((void) 0)
#else
#define log_assert_message(cond, fmt, ...) if(!(cond)) {log_assert__(__FILE__, __LINE__, fmt, __VA_ARGS__);}
#endif

#ifdef NDEBUG
#define log_assert(cond)((void) 0)
#else
#define log_assert(cond) if(!(cond)) {log_assert__(__FILE__, __LINE__, "Assertion '%S' failed.", #cond);}
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* LOG_H */

