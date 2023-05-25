#ifndef _CIRCULAR_BUFFER_
#define _CIRCULAR_BUFFER_

#include <stdint.h>

uint16_t crc16_ccitt_init(void);
uint16_t crc_calculate_table(uint8_t *data, uint8_t len);
uint16_t crc16_ccitt_update(uint8_t byte, uint16_t crc);
uint16_t crc16_ccitt(uint8_t *data, uint8_t len);

#endif
