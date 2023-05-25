/**
 * @file circular_buffer.c
 * @brief File that handles the circullar buffer
 */

#include "circular_buffer.h"

void circularBufferInit(CircularBuffer * buffer)
{
    memset(buffer,0,sizeof(CircularBuffer));
}
uint16_t getIsReadable(CircularBuffer * buffer)
{
    if ((buffer->write_idx - buffer->read_idx) > 0)
    {
        return RESULT_SUCCES ;
    }
    return  RESULT_FAIL;
}
uint16_t getCircularBufferSize(CircularBuffer * buffer)
{
    int16_t size = buffer->write_idx - buffer->read_idx;
    if (size<0)
    {
        return size + SIZE_OF_BUFFER;
    }
    return  size;
}
uint16_t getPacketLength(CircularBuffer * buffer)
{
    uint16_t size = (buffer->buffer[buffer->read_idx+4] << 8) + buffer->buffer[buffer->read_idx+5];
    
    return  size;
}
uint16_t getWriteId(CircularBuffer * buffer)
{
    return buffer->write_idx;
}
uint16_t getReadId(CircularBuffer * buffer)
{
    return buffer->read_idx;
}
uint16_t circularBufferPut(CircularBuffer * buffer, const uint8_t ch)
{
    if (buffer==NULL )
    {
        return RESULT_FAIL;
    }
    if (buffer->write_idx >= SIZE_OF_BUFFER)
    {
        buffer->write_idx=0;
    }
    buffer->buffer[buffer->write_idx]=ch;
    buffer->write_idx++;
    buffer->length++;
    return RESULT_SUCCES;
}

uint16_t circularBufferGet(CircularBuffer * buffer, uint8_t * ch)
{
    if (buffer==NULL || ch==NULL)
    {
        return RESULT_FAIL;
    }
    if (buffer->read_idx > buffer->write_idx)
    {
       return RESULT_FAIL; 
    }
    if (buffer->read_idx >= SIZE_OF_BUFFER)
    {
        buffer->read_idx = 0;
    }
    *ch=buffer->buffer[buffer->read_idx];
    buffer->read_idx++;
    return RESULT_SUCCES;
}