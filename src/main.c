/**

* Copyright (c) 2020 Raspberry Pi (Trading) Ltd.

*

* SPDX-License-Identifier: BSD-3-Clause

*/


#include <FreeRTOS.h>

#include <task.h>

#include <queue.h>

#include <stdio.h>

#include <stdbool.h>

#include <stdint.h>

#include <stdlib.h>

#include "pico/stdlib.h"

#include "hardware/uart.h"

#include "hardware/irq.h"

#include "circular_buffer.h"

#include "packages.h"

#define UART_ID uart0

#define BAUD_RATE 115200

#define DATA_BITS 8

#define STOP_BITS 1

#define PARITY UART_PARITY_NONEindex


// We are using pins 0 and 1, but see the GPIO function select table in the

// datasheet for information on which other pins can be used.

#define UART_TX_PIN 0

#define UART_RX_PIN 1

static QueueHandle_t xQueueISR=NULL; 
static QueueHandle_t xQueueAnswer=NULL; 

TC171 TC;
TMvalid TM1;

char log_buffer[10000]; // buffer used for cataloging errors
CircularBuffer buffer;
uint8_t inputStream[128];
uint8_t outputStream[128];
uint32_t lengthTotal=0;
uint indexStream = 0;

/**********************************************************************

* Function:    on_uart_rx

* Description: This function is used insert data into the buffer while there is incoming data

* Params:      -

* Return:      void

**********************************************************************/
void on_uart_rx() 
{
    uint8_t isPut = 0;

    while(1) 
    {
        uint8_t ch = uart_getc(UART_ID);
        
        if(circularBufferPut(&buffer,ch) == 0)
        {
            isPut = 1;
            xQueueSendFromISR(xQueueISR, &isPut, NULL);

            taskYIELD();
        }
    } 
    
}

/**********************************************************************

* Function:    readFromBuffer

* Description: This function is used to read from the buffer in a separate task and generate the TC 

* Params:      -

* Return:      void

**********************************************************************/
void readFromBuffer()
{
    uint8_t isPut = 0; 
    uint8_t isDone = 0;

    while(true)
    {
        xQueueReceiveFromISR(xQueueISR, &isPut, NULL); 

        if(isPut == 1) 
        {
            isPut=0;
            circularBufferGet(&buffer,&inputStream[indexStream++]);
        }  

        if (indexStream==7 && isDone == 0)
        {
            uint16_t packetLength = (inputStream[4] << 8) + inputStream[5];
            if(packetLength != 0) {
                lengthTotal=7+packetLength;
                isDone=1;
            }
        }
        
        if ((indexStream==lengthTotal)&&isDone==1)
        {
            PUS17_initializeFromByteArrayTC(&TC,inputStream);
            xQueueSend(xQueueAnswer, &isDone, 0U);

            taskYIELD();
        } 

    }
}

/**********************************************************************

* Function:    answer

* Description: This function is used to send the response-package on the serial

* Params:      -

* Return:      void

**********************************************************************/
void answer()
{
    uint8_t receiveValue=0;

    while(true)
    {
        xQueueReceive(xQueueAnswer , &receiveValue, portMAX_DELAY);  

        if(receiveValue == 1)
        {
            switch(TC.dataFieldHeader.ServiceType)
            {
            case 17:
                switch(TC.dataFieldHeader.ServiceSubtype)
                {
                    case 1:
                        PUS1_initializeTMvalid(&TM1);
                        PUS1_setVariableValuesTMvalid(&TM1);
                        PUS1_transformIntoByteArrayTMvalid(&TM1,outputStream);
                        for (int i=0;i<23;i++)
                        {
                            uart_putc(UART_ID,outputStream[i]);
                        }
                    break;
                }
            break;
            } 

            indexStream = 0;
            memset(inputStream, 0, sizeof(uint8_t)*128);
            memset(outputStream, 0, sizeof(uint8_t)*128);

            taskYIELD();    
        }         
    }
}

/**********************************************************************

* Function:    main

* Description: This function is used to setup the serial conection, the ISR and the FreeRTOS tasks

* Params:      -

* Return:      int 

**********************************************************************/
int main()
{

// Set up our UART with a basic baud rate.

uart_init(UART_ID, 2400);

xQueueISR=xQueueCreate(256, sizeof(uint8_t));
xQueueAnswer=xQueueCreate(256, sizeof(uint8_t));

// Set the TX and RX pins by using the function select on the GPIO

// Set datasheet for more information on function select

gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);

gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);



// Actually, we want a different speed

// The call will return the actual baud rate selected, which will be as close as

// possible to that requested

uart_set_baudrate(UART_ID, BAUD_RATE);



// Set UART flow control CTS/RTS, we don't want these, so turn them off

uart_set_hw_flow(UART_ID, false, false);



// Set our data format

uart_set_format(UART_ID, DATA_BITS, STOP_BITS, UART_PARITY_NONE);



// Turn off FIFO's - we want to do this character by character

uart_set_fifo_enabled(UART_ID, false);



// Set up a RX interrupt

// We need to set up the handler first

// Select correct interrupt for the UART we are using

int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;



// Initialize buffer

circularBufferInit(&buffer);



// And set up and enable the interrupt handlers

irq_set_exclusive_handler(UART_IRQ, on_uart_rx);

irq_set_enabled(UART_IRQ, true);



// Now enable the UART to send interrupts - RX only

uart_set_irq_enables(UART_ID, true, false);



// Create task Queue

// Create task

// Start the task scheduler

xTaskCreate(readFromBuffer, "Read BF", 256, NULL, 1, NULL);
xTaskCreate(answer, "Send TM", 256, NULL, 2, NULL);
vTaskStartScheduler();



// OK, all set up.

// Lets send a basic inputStreaming out, and then run a loop and wait for RX interrupts

// The handler will count them, but also reflect the incoming data back with a slight change!

while (1)
{
tight_loop_contents();
}

}
