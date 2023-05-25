/**
 * @file crc.h
 * @brief File meant to define the crc verification functions
 */

#ifndef _CIRCULAR_BUFFER_
#define _CIRCULAR_BUFFER_

#include <stdint.h>

/**
 * @brief initialize crc 
 * 
 * @return uint16_t : 0xFFFF
 */
uint16_t crc16_ccitt_init(void);

/**
 * @brief calculates the crc 
 * 
 * @param[in] data 
 * @param[in] len 
 * @return uint16_t : crc
 */
uint16_t crc_calculate_table(uint8_t *data, uint8_t len);

/**
 * @brief update crc 
 * 
 * @param[in] byte 
 * @param[in] crc 
 * @return uint16_t : crc
 */
uint16_t crc16_ccitt_update(uint8_t byte, uint16_t crc);

/**
 * @brief it calls the crc_calculate_table()
 * 
 * @param[in] data 
 * @param[in] len 
 * @return uint16_t : crc
 */
uint16_t crc16_ccitt(uint8_t *data, uint8_t len);

#endif
