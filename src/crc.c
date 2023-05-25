#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "packages.h"

uint16_t crc16_ccitt_table[16];

uint16_t crc16_ccitt_init(void) 
{
	return 0xFFFF;
}

uint16_t crc16_ccitt_update(uint8_t byte, uint16_t crc) 
{
	/* The data in the table constructed by running the algorithmic CRC
	 * with inital CRC (crc & 0xFF00), and the input data 0x00. The
	 * resulting CRC can be found in the table at (crc >> 8).
	 *
	 * Since the operation is performed on a zero rightmost half of the
	 * CRC, and a zero input byte, we can construct the new CRC by usin
	 * the rightmost half of the current CRC as the new leftmost half, and
	 * the byte as the leftmost half. We then XOR this construction with
	 * the contents of the table at position (crc >> 8). */
    // printf("crc table %u \n ",crc16_ccitt_table[crc >> 8]);
	crc = ((crc << 8) | byte) ^ crc16_ccitt_table[crc >> 8];

	return crc;
}

uint16_t crc_calculate_table(uint8_t * data, uint8_t len) 
{
	uint8_t *b = (uint8_t *)data;
	uint8_t i;
	uint16_t crc;

	crc = crc16_ccitt_init();

	/* Update the CRC using the data */
	for (i = 0; i < len; i++) 
	{
		crc = crc16_ccitt_update(b[i], crc);
	}

	return crc;
}

uint16_t crc16_ccitt(uint8_t * data, uint8_t len)
{
	uint16_t crc;
	crc = crc_calculate_table(data, len);
    return crc;
}
    
