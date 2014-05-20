/* 
 * File:   crc16.h
 * Author: tommz
 *
 * Created on May 20, 2014, 1:40 PM
 */

#ifndef CRC16_H
#define	CRC16_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdint.h"

    uint16_t gen_crc16(const uint8_t *data, uint16_t size);

#ifdef	__cplusplus
}
#endif

#endif	/* CRC16_H */

