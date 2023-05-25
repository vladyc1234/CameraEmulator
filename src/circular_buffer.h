#ifndef _CIRCULAR_BUFFER_
#define _CIRCULAR_BUFFER_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define SIZE_OF_BUFFER 8
#define RESULT_SUCCES 0
#define RESULT_FAIL -1

/**********************************************************************

* Structure:   _circular_buffer_t

* Description: This structure simulates a circular buffer

* Variables:   uint8_t buffer[SIZE_OF_BUFFER], uint8_t read_idx, uint8_t write_idx, uint8_t length,

**********************************************************************/
typedef struct _circular_buffer_t
{

uint8_t buffer[SIZE_OF_BUFFER];

uint8_t read_idx;

uint8_t write_idx;

uint8_t length; 

} CircularBuffer ;

/**********************************************************************

* Function:    circularBufferInit

* Description: This function is used to initialize the circular buffer with 0 on all variables

* Params:      CircularBuffer * buffer

* Return:      void

**********************************************************************/
void circularBufferInit(CircularBuffer * buffer);

/**********************************************************************

* Function:    circularBufferPut

* Description: This function is used to insert new elements into the circular buffer

* Params:      CircularBuffer * buffer, uint8_t ch

* Return:      int: 0 - execution success; -1 - execution failure

**********************************************************************/
int circularBufferPut(CircularBuffer * buffer, const uint8_t ch);

/**********************************************************************

* Function:    circularBufferGet

* Description: This function is used to read elements from the circular buffer

* Params:      CircularBuffer * buffer, uint8_t * ch

* Return:      int: 0 - execution success; -1 - execution failure

**********************************************************************/
int circularBufferGet(CircularBuffer * buffer, uint8_t * ch);


#endif
