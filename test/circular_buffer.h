/**
 * @file circular_buffer.h
 * @brief File meant to define the circullar buffer and its functions
 */

#ifndef _CIRCULAR_BUFFER_
#define _CIRCULAR_BUFFER_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define SIZE_OF_BUFFER 4200 ///< The size of the buffer in bytes
#define RESULT_SUCCES 0 ///< in case of success the value returned is 0
#define RESULT_FAIL -1 ///< in case of failure the value returned is -1

/**
 * @brief This structure simulates a circular buffer
 * 
 */
typedef struct _circular_buffer_t
{

uint8_t buffer[SIZE_OF_BUFFER];

uint16_t read_idx;

uint16_t write_idx;

uint16_t length; 

} CircularBuffer ;

/**
 * @brief This function is used to initialize the circular buffer with 0 on all variables
 * 
 * @param[out] buffer 
 */
void circularBufferInit(CircularBuffer * buffer);

/**
 * @brief This function is used to get the length of the packet
 * 
 * @param[out] buffer 
 */
uint16_t getPacketLength(CircularBuffer * buffer);

/**
 * @brief This function is used to check how many octets are in the buffer
 * 
 * @param[out] buffer 
 */
uint16_t getCircularBufferSize(CircularBuffer * buffer);

/**
 * @brief This function is used to check if the buffer contains octets
 * 
 * @param[out] buffer 
 */
uint16_t getIsReadable(CircularBuffer * buffer);

/**
 * @brief This function is used to get the write index
 * 
 * @param[out] buffer 
 */
uint16_t getWriteId(CircularBuffer * buffer);

/**
 * @brief This function is used to get the read index
 * 
 * @param[out] buffer 
 */
uint16_t getReadId(CircularBuffer * buffer);



/**
 * @brief This function is used to insert new elements into the circular buffer
 * 
 * @param[out] buffer 
 * @param[in] ch 
 * @return int: 0 - execution success; -1 - execution failure
 */
uint16_t circularBufferPut(CircularBuffer * buffer, const uint8_t ch);

/**
 * @brief This function is used to read elements from the circular buffer
 * 
 * @param[out] buffer 
 * @param[out] ch 
 * @return int: 0 - execution success; -1 - execution failure
 */
uint16_t circularBufferGet(CircularBuffer * buffer, uint8_t * ch);


#endif
