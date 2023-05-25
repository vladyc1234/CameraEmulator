#include "circular_buffer.h"

void circularBufferInit(CircularBuffer * buffer)
{
    memset(buffer,0,sizeof(CircularBuffer));
}

int circularBufferPut(CircularBuffer * buffer, const uint8_t ch)
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

int circularBufferGet(CircularBuffer * buffer, uint8_t * ch)
{
    if (buffer==NULL || ch==NULL)
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