/**
 * @file main.c
 * @brief File meant to handle the main logic of the SECONDARY MACHINE
 * 
 * 
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

#define UART_ID uart0 ///< GPIO used 

#define BAUD_RATE 115200 ///< Pi Pico baudrate

#define DATA_BITS 8 ///< How many bits are read at a time

#define STOP_BITS 1 ///< Number of stop bits

#define PARITY UART_PARITY_NONEindex ///< Parity option

#define UART_TX_PIN 0 ///< We are using pins 0 and 1, but see the GPIO function select table in the

#define UART_RX_PIN 1 ///< We are using pins 0 and 1, but see the GPIO function select table in the


/// @brief Queue used for sending data between on_uart_rx() and readFromBuffer()
QueueHandle_t xQueueISR=NULL; 

/// @brief Queue used for sending data bewteen readFromBuffer()/getParamHK() and answer()
QueueHandle_t xQueueAnswer=NULL; 

/// @brief Queue used for sending data bewteen readFromBuffer() and getParamHK()
QueueHandle_t xQueueHouseKeep=NULL; 

/// @brief Queue used for sending data bewteen readFromBuffer() and takePictureTC2221()
QueueHandle_t xQueueTakePicture=NULL; 


/// @brief Priority for readFromBuffer() task
UBaseType_t readPriority = 0x1;

/// @brief Priority for getParamHK() task
UBaseType_t getParamPriority = 0x1;

/// @brief Priority for answer() task
UBaseType_t answerPriority = 0x1;

/// @brief Priority for timeManagementFine() task
UBaseType_t fineTimePriority = 0x1;

/// @brief Priority for timeManagementCoarse() task
UBaseType_t coarseTimePriority = 0x1;

/// @brief Priority for takePictureTC2221() task
UBaseType_t takePicturePriority = 0x1;


/// @brief Task handler for readFromBuffer()
TaskHandle_t readHandle;

/// @brief Task handler for getParamHK()
TaskHandle_t getParamHandle;

/// @brief Task handler for answer()
TaskHandle_t answerHandle;

/// @brief Task handler for timeManagementFine()
TaskHandle_t fineHandle;

/// @brief Task handler for timeManagementCoarse()
TaskHandle_t coarseHandle;

/// @brief Task handler for timeManagementCoarse()
TaskHandle_t takePictureHandle;



/// @brief TC[17, 1] package
TC171 TC;

/// @brief TM[1, 1], TM[1, 7]  packages
TMvalid TM1;

/// @brief TM[1, 2], TM[1, 8]  packages
TMerror TM1E;

/// @brief TC[3, 130] package
TCHR TCHR130;

/// @brief TM[3, 25] packagebuffer
TMHR TMHR25;

/// @brief TC[9, 132] package
TCTiM TCTiM132;

/// @brief TC[222, 1] package
TC2221 TCTakePicture;

/// @brief TC[222, 2] package
TC2222 TCConfigSensor;

/// @brief TC[222, 3] package
TC2223 TCDownloadPicture;
TM22210 TMDownloadPicture;

char log_buffer[100]; 
char log_bufferTime[100];

/// @brief Declare the circullar buffer for use in tasks
CircularBuffer buffer;

/// @brief Vector that stores the byte stream coming from the serial
uint8_t inputStream[300];

/// @brief Vector that stores the bytes meant to be sent on the serial by the SECONDARY
uint8_t outputStream[4200];


/// @brief Total length of a package
uint32_t lengthTotal=0;

/// @brief index for input stream
uint indexStream = 0;


/// @brief vector that stores the parameters for Housekeeping
uint32_t arrayHK[3] = {0};


/// @brief matrix that stores the parameters for Picture
uint8_t takeImg[64][64];

/// @brief matrix size
uint16_t lengthMatrix=64;

/// @brief auxilliary matrix
uint8_t small_matrix[32][32];
uint8_t small_matrix2[16][16];


/// @brief variable that checks if a TC[9, 132] has been received
uint16_t TimeISR=0;

/// @brief variable that holds the global coarse time for use in tasks 
uint32_t coarseTime=0;

/// @brief variable that holds the global fine time for use in tasks
uint16_t fineTime=0;


/**
 * @brief This function is used insert data into the buffer while there is incoming data
 * 
 */
void on_uart_rx() 
{
    uint8_t isReadable = -1; 
    
        uint8_t ch = uart_getc(UART_ID); // variable that stores an incoming byte from the serial
        
        if(circularBufferPut(&buffer,ch) == 0)
        {
            if (getCircularBufferSize(&buffer)==7)
            {
                lengthTotal=0;
                uint16_t packetLength = getPacketLength(&buffer); // variable that stores the Packet Data Field length
                if(packetLength != 0) 
                {
                    lengthTotal=7+packetLength;
                    
                }
            }
            if(getWriteId(&buffer)==lengthTotal)
            {
                isReadable=getIsReadable(&buffer);
                xQueueSendFromISR(xQueueISR, &isReadable, NULL);
            }

        }
    
}

/**
 * @brief This function is used to read from the buffer in a separate task and generate the TC and confirms the generation by sending a PUS1 acknowledgement TM
 * 
 */
void readFromBuffer()
{
    uint8_t isReadable = -1; 
    uint8_t isDone = 0;
    

    while(true)
    {
        xQueueReceiveFromISR(xQueueISR, &isReadable, NULL); 
        if (isReadable == 0)
        {
            indexStream =getCircularBufferSize(&buffer);
            for (int i=0;i<indexStream;i++)
            {
                circularBufferGet(&buffer,&inputStream[i]);
            }
            isDone=1;
            isReadable = -1;
        }
        if ((indexStream==lengthTotal)&&isDone==1)
        { 
            if((((inputStream[0] & 0b111) << 4 )+(inputStream[1] >> 4)) == 0 && (inputStream[1] & 0b1111) == 0)
            {
                TimeISR=1;
                if (PUS9_initializeFromByteArrayTCTiM(&TCTiM132, inputStream))
                {
                    PUS1_initializeTMvalid(&TM1);
                    PUS1_setVariableValuesTMvalid(&TM1,TCTiM132.packetHeader, 1, fineTime, coarseTime);
                    PUS1_transformIntoByteArrayTMvalid(&TM1,outputStream);
                    for (int i=0;i<23;i++)
                    {
                        uart_putc(UART_ID,outputStream[i]);
                    }
                }
                else
                {
                    PUS1_initializeTMerror(&TM1E);
                    PUS1_setVariableValuesTMerror(&TM1E,TCTiM132.packetHeader, 2, fineTime, coarseTime,1,1);
                    PUS1_transformIntoByteArrayTMerror(&TM1E,outputStream);
                    for (int i=0;i<24;i++)
                    {
                        uart_putc(UART_ID,outputStream[i]);
                    }
                }

            
                xQueueSend(xQueueAnswer, &isDone, 0U);
          
            }
            else
            {
                switch(inputStream[7])
                {
                case 17:
                    switch(inputStream[8])
                    {
                        case 1:
                            
                            if (PUS17_initializeFromByteArrayTC(&TC,inputStream))
                            {
                                PUS1_initializeTMvalid(&TM1);
                                PUS1_setVariableValuesTMvalid(&TM1,TC.packetHeader, 1, fineTime, coarseTime);
                                PUS1_transformIntoByteArrayTMvalid(&TM1,outputStream);
                                for (int i=0;i<23;i++)
                                {
                                    uart_putc(UART_ID,outputStream[i]);
                                }
                            }
                            else
                            {
                                PUS1_initializeTMerror(&TM1E);
                                PUS1_setVariableValuesTMerror(&TM1E,TC.packetHeader, 2, fineTime, coarseTime,1,1);
                                PUS1_transformIntoByteArrayTMerror(&TM1E,outputStream);
                                for (int i=0;i<24;i++)
                                {
                                    uart_putc(UART_ID,outputStream[i]);
                                }
                            }

                        
                            xQueueSend(xQueueAnswer, &isDone, 0U);
                          
                        break;
                    }
                break;
                case 3:
                    switch(inputStream[8])
                    {
                        case 130:
                           
                            if(PUS3_initializeFromByteArrayTCHR(&TCHR130,inputStream))
                            {
                                PUS1_initializeTMvalid(&TM1);
                                PUS1_setVariableValuesTMvalid(&TM1,TCHR130.packetHeader, 1, fineTime, coarseTime);
                                PUS1_transformIntoByteArrayTMvalid(&TM1,outputStream);
                                for (int i=0;i<23;i++)
                                {
                                    uart_putc(UART_ID,outputStream[i]);
                                }
                            }   
                            else
                            {
                                PUS1_initializeTMerror(&TM1E);
                                PUS1_setVariableValuesTMerror(&TM1E,TCHR130.packetHeader, 2, fineTime, coarseTime,1,1);
                                PUS1_transformIntoByteArrayTMerror(&TM1E,outputStream);
                                for (int i=0;i<24;i++)
                                {
                                    uart_putc(UART_ID,outputStream[i]);
                                }
                            } 

                                             
                            xQueueSend(xQueueHouseKeep, &isDone, 0U);
                         
                        break;
                    }
                
                break;
                case 222:
                    switch(inputStream[8])
                    {
                        case 1:
                            
                          
                            if(PUS222_initializeFromByteArrayTC2221(&TCTakePicture,inputStream))
                            {
                                PUS1_initializeTMvalid(&TM1);
                                PUS1_setVariableValuesTMvalid(&TM1,TCTakePicture.packetHeader, 1, fineTime, coarseTime);
                                PUS1_transformIntoByteArrayTMvalid(&TM1,outputStream);
                                for (int i=0;i<23;i++)
                                {
                                    uart_putc(UART_ID,outputStream[i]);
                                }
                            }     
                            else
                            {
                                PUS1_initializeTMerror(&TM1E);
                                PUS1_setVariableValuesTMerror(&TM1E,TCTakePicture.packetHeader, 2, fineTime, coarseTime,1,1);
                                PUS1_transformIntoByteArrayTMerror(&TM1E,outputStream);
                                for (int i=0;i<24;i++)
                                {
                                    uart_putc(UART_ID,outputStream[i]);
                                }
                            }    

                                     
                            xQueueSend(xQueueTakePicture, &isDone, 0U);
                           
                        break;
                        case 2:
                            if(PUS222_initializeFromByteArrayTC2222(&TCConfigSensor,inputStream))
                            {
                                PUS1_initializeTMvalid(&TM1);
                                PUS1_setVariableValuesTMvalid(&TM1,TCConfigSensor.packetHeader, 1, fineTime, coarseTime);
                                PUS1_transformIntoByteArrayTMvalid(&TM1,outputStream);
                                for (int i=0;i<23;i++)
                                {
                                    uart_putc(UART_ID,outputStream[i]);
                                }
                                // sprintf(log_buffer," crc = %u",TCConfigSensor.PacketErrorControl);
                                // uart_puts(UART_ID,log_buffer);
                            }
                            else
                            {
                                PUS1_initializeTMerror(&TM1E);
                                PUS1_setVariableValuesTMerror(&TM1E,TCConfigSensor.packetHeader, 2, fineTime, coarseTime,1,1);
                                PUS1_transformIntoByteArrayTMerror(&TM1E,outputStream);
                                for (int i=0;i<24;i++)
                                {
                                    uart_putc(UART_ID,outputStream[i]);
                                }
                            }   

                            PUS1_initializeTMvalid(&TM1);
                            PUS1_setVariableValuesTMvalid(&TM1,TCConfigSensor.packetHeader, 7, fineTime, coarseTime);
                            PUS1_transformIntoByteArrayTMvalid(&TM1,outputStream);
                            for (int i=0;i<23;i++)
                            {
                                uart_putc(UART_ID,outputStream[i]);
                            }           

                     
                            xQueueSend(xQueueAnswer, &isDone, 0U);                 
                         
                        break;
                        case 3:
                            
                            if(PUS222_initializeFromByteArrayTC2223(&TCDownloadPicture,inputStream))
                            {
                                PUS1_initializeTMvalid(&TM1);
                                PUS1_setVariableValuesTMvalid(&TM1,TCDownloadPicture.packetHeader, 1, fineTime, coarseTime);
                                PUS1_transformIntoByteArrayTMvalid(&TM1,outputStream);
                                for (int i=0;i<23;i++)
                                {
                                    uart_putc(UART_ID,outputStream[i]);
                                }
                            }  
                            else   
                            {
                                PUS1_initializeTMerror(&TM1E);
                                PUS1_setVariableValuesTMerror(&TM1E,TCDownloadPicture.packetHeader, 2, fineTime, coarseTime,1,1);
                                PUS1_transformIntoByteArrayTMerror(&TM1E,outputStream);
                                for (int i=0;i<24;i++)
                                {
                                    uart_putc(UART_ID,outputStream[i]);
                                }
                            }      

                                                   
                            xQueueSend(xQueueAnswer, &isDone, 0U);                 
                           
                        break;
                    }
                break;
                }
                

            }   

            isDone=0;
        }     
    }
}

/**
 * @brief This function is used to increment Fine Time and to send a notification that increments Coarse Time
 * 
 */
void timeManagementFine(){
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    while (true)
    {
        fineTime=fineTime+1;

        if (fineTime >= 1000)
        {
            fineTime=0;
            vTaskNotifyGiveFromISR( coarseHandle, &xHigherPriorityTaskWoken);
        }

        vTaskDelay(10);
    }
}

/**
 * @brief This function is used to increment Coarse Time and to check if a new TC[9,132] has been received 
 *        and change the current coarse time with the new coarse time in that case
 * 
 */
void timeManagementCoarse()
{
    uint32_t ulNotifiedValue;
    while(1){
        ulNotifiedValue = ulTaskNotifyTake( pdTRUE,portMAX_DELAY );
        if (ulNotifiedValue>0)
        {
            if(TimeISR == 1)
            {
                coarseTime=TCTiM132.CoarseTime;
                TimeISR=0;
                
                if (coarseTime == TCTiM132.CoarseTime)
                {
                    PUS1_initializeTMvalid(&TM1);
                    PUS1_setVariableValuesTMvalid(&TM1,TCTiM132.packetHeader, 7, fineTime, coarseTime);
                    PUS1_transformIntoByteArrayTMvalid(&TM1,outputStream);

                    for (int i=0;i<23;i++)
                    {
                        uart_putc(UART_ID,outputStream[i]);
                        
                    }
                    // uart_putc(UART_ID,255);
                }
                else
                {
                    PUS1_initializeTMerror(&TM1E);
                    PUS1_setVariableValuesTMerror(&TM1E,TCTiM132.packetHeader, 8, fineTime, coarseTime,1,1);
                    PUS1_transformIntoByteArrayTMerror(&TM1E,outputStream);

                    for (int i=0;i<24;i++)
                    {
                        uart_putc(UART_ID,outputStream[i]);
                    } 
                }
            }
            coarseTime=coarseTime+1;
            sprintf(log_buffer,"coarse= %u",coarseTime);
        }
        else
        {
            uart_putc(UART_ID, 4);
        }
        
    }
}

/**
 * @brief This function is used to get parameters for housekeeping
 * 
 */
void getParamHK()
{
    uint8_t receiveValue = 0;
    uint32_t timer = 0; // variable that holds time in micro-seconds from the task start
    uint32_t dif; // variable that calculates difference between start time and current time
    while(true) 
    {
        xQueueReceive(xQueueHouseKeep, &receiveValue, portMAX_DELAY);
        
        if (timer==0)
        {
            timer=time_us_32();
        }

        dif=time_us_32()-timer;

        while(dif / 1000 <= 2000)
        {
            dif=time_us_32()-timer;
            arrayHK[0] = 1;
            arrayHK[1] = 12;  
        }

       
        xQueueSend(xQueueAnswer, &receiveValue, 0U);
   
    }
}

/**
 * @brief This function is used to get picture for PUS222
 * 
 */
void takePictureTC2221()
{
    uint8_t receiveValue = 0;
    while(true) 
    {
        xQueueReceive(xQueueTakePicture, &receiveValue, portMAX_DELAY);
                
        for(int i=0;i<64;i++)
        {
            for(int j=0;j<64;j++)
        {
            takeImg[i][j]=i+j;
        }
        }
        PUS1_initializeTMvalid(&TM1);
        PUS1_setVariableValuesTMvalid(&TM1,TCTakePicture.packetHeader, 7, fineTime, coarseTime);
        PUS1_transformIntoByteArrayTMvalid(&TM1,outputStream);
        for (int i=0;i<23;i++)
        {
            uart_putc(UART_ID,outputStream[i]);
        }   

        if (takeImg[1][0]==0) 
        {
            PUS1_initializeTMerror(&TM1E);
            PUS1_setVariableValuesTMerror(&TM1E,TCTakePicture.packetHeader, 8, fineTime, coarseTime,1,1);
            PUS1_transformIntoByteArrayTMerror(&TM1E,outputStream);
            for (int i=0;i<24;i++)
            {
                uart_putc(UART_ID,outputStream[i]);
            }   
        }                      

        
        xQueueSend(xQueueAnswer, &receiveValue, 0U);
   
    }
}

/**
 * @brief The under-sampling function skips one line every two lines and on each selected lines one cell every
 *          two-cells is skipped, reducing the matrix size by a ratio of 4
 * 
 * @param n matrix size
 * @return uint8_t : m - the new usable size of the matrix
 */
uint8_t under_seapling( uint8_t n,uint8_t (*takeImg)[n],uint8_t (*small_matrix)[(int)(n/2)]) {
    
uint8_t m = (uint8_t)(n / 2);

for(uint8_t i = 0; i < m; i++)
    {
        for(uint8_t j = 0; j < m; j++)
        {
            small_matrix[i][j] = 0;
        }
    }

uint8_t x = 0;
for (uint8_t i = 0; i < n; i += 2) {
uint8_t y = 0;
    for (uint8_t j = 0; j < n; j += 2) {
        small_matrix[x][y] = takeImg[i][j];
        y++;
}
    x++;
}
memset(takeImg, 0, 64 * 64 * sizeof(uint8_t));
memcpy(takeImg,small_matrix, m*m*sizeof(uint8_t));

return m;
}

/**
 * @brief The binning function computes the average of a cluster of 2x2 cells into a single "super-cell"
 *          reducing the matrix size by a ratio of 4
 * 
 * @param n matrix size
 * @return uint8_t : m - the new usable size of the matrix
 */
uint8_t binning( uint8_t n, uint8_t (*takeImg)[n],uint8_t (*small_matrix)[(int)(n/2)]) {
    
    uint8_t m = (uint8_t)(n / 2);

    for(uint8_t i = 0; i < m; i++)
    {
        for(uint8_t j = 0; j < m; j++)
        {
            small_matrix[i][j] = 0;
        }
    }

    uint8_t x = 0;
    uint8_t avg=0;
    for (uint8_t i = 0; i < n; i += 2) {
    uint8_t y = 0;
    for (uint8_t j = 0; j < n; j += 2) {
        avg=(takeImg[i][j]+takeImg[i+1][j]+takeImg[i][j+1]+takeImg[i+1][j+1])/4;
        small_matrix[x][y] = avg;
    y++;
    }
    x++;
    }
    memset(takeImg, 0, 64 * 64 * sizeof(uint8_t));
    memcpy(takeImg,small_matrix, m*m*sizeof(uint8_t));

 return m;
}

/**
 * @brief The windowing function reduces the picture size by a ratio of 4 except for the configuration 'F'
 *          that does not change the ratio. All the other configurations select different parts of the matrix
 * 
 * @param[in] wi configuration
 * @param[in] n matrix size
 * @return uint8_t : m - the new size of the usable space in the matrix
 */
uint8_t windowing( uint8_t wi,uint8_t n, uint8_t (*takeImg)[n],uint8_t (*small_matrix)[(int)(n/2)]) {
    uint8_t k,l,m;
    m=(uint8_t)(n/2);
    

for(uint8_t i = 0; i < m; i++)
    {
        for(uint8_t j = 0; j < m; j++)
        {
            small_matrix[i][j] = 0;
        }
    }

 switch (wi)
 {
 case 1:
    k=0;
    l=0;
    break;
 case 2:
    k=0;
    l=n/2;
    break;
case 3:
    k=n/2;
    l=n/2;
    break;
case 4:
    k=n/2;
    l=0;
    break;
case 5:
    k=0;
    l=n/4;
    break;
case 6:
    k=n/2;
    l=n/4;
    break;
case 7:
    k=n/4;
    l=0;
    break;
case 8:
    k=n/4;
    l=n/2;
    break;
case 9:
    k=(n/4);
    l=(n/4);
    break;
 default:
    k=0;
    l=0;
    return n;
 }
 uint8_t x=0;
 for (uint8_t i = k; i < k+m; i ++) {
 uint8_t y = 0;
 for (uint8_t j = l; j < l+m; j ++) {
    
    small_matrix[x][y] = takeImg[i][j];
 y++;
}
 x++;
}
 memset(takeImg, 0, 64 * 64 * sizeof(uint8_t));
 memcpy(takeImg,small_matrix, m*m*sizeof(uint8_t));
 

 return m;
}

/**
 * @brief This function is used to send the response-package on the serial and confirms the execution of the TC package by sending a PUS1 execution TM
 * 
 */
void answer()
{
    uint8_t receiveValue=0;

    while(true)
    {
        xQueueReceive(xQueueAnswer , &receiveValue, portMAX_DELAY);  
        if(receiveValue == 1)
        {
            switch(inputStream[7])
            {
            case 3:
                switch(inputStream[8])
                {
                    case 130:
                        PUS3_initializeTMHR(&TMHR25);
                        PUS3_setVariableValuesTMHR(&TMHR25, arrayHK, fineTime, coarseTime);
                        PUS3_initializeFromByteArrayTMHR(&TMHR25,outputStream);
                        for (int i=0;i<24;i++)
                        {
                            uart_putc(UART_ID,outputStream[i]);
                        }  

                        if(TMHR25.packetHeader.PacketLength !=0)
                        {
                            PUS1_initializeTMvalid(&TM1);
                            PUS1_setVariableValuesTMvalid(&TM1,TCHR130.packetHeader, 7, fineTime, coarseTime);
                            PUS1_transformIntoByteArrayTMvalid(&TM1,outputStream);
                            for (int i=0;i<23;i++)
                            {
                                uart_putc(UART_ID,outputStream[i]);
                            }  
                        }
                        else
                        {
                            PUS1_initializeTMerror(&TM1E);
                            PUS1_setVariableValuesTMerror(&TM1E,TCHR130.packetHeader, 8, fineTime, coarseTime,1,1);
                            PUS1_transformIntoByteArrayTMerror(&TM1E,outputStream);
                            for (int i=0;i<24;i++)
                            {
                                uart_putc(UART_ID,outputStream[i]);
                            }  
                        }                 
                    break;
                }
            break;
            case 222:
                switch (inputStream[8])
                {
                    case 3:
                    
                    if (TCDownloadPicture.UnderSampling==1)
                    {
                        lengthMatrix=under_seapling(lengthMatrix, takeImg, small_matrix);
                    }
                    if (TCDownloadPicture.Binning==1)
                    {
                        lengthMatrix=binning(lengthMatrix, takeImg, small_matrix);
                    }
                    if (TCDownloadPicture.Windowing!=0)
                    {
                        lengthMatrix=windowing(TCDownloadPicture.Windowing,lengthMatrix, takeImg, small_matrix);
                    }

                    PUS222_setVariableValuesTM22210(&TMDownloadPicture,TCTakePicture,TCConfigSensor,lengthMatrix,takeImg,fineTime,coarseTime);

                    uint16_t length1=PUS222_initializeFromByteArrayTM22210(&TMDownloadPicture,outputStream,lengthMatrix);
                    
                    for (uint16_t i=0;i<length1+1;i++){
                        uart_putc(UART_ID,outputStream[i]);
                    }
                    
                    if(TMDownloadPicture.packetHeader.PacketLength !=0)
                    {
                        PUS1_initializeTMvalid(&TM1);
                        PUS1_setVariableValuesTMvalid(&TM1,TCDownloadPicture.packetHeader, 7, fineTime, coarseTime);
                        PUS1_transformIntoByteArrayTMvalid(&TM1,outputStream);

                        for (int i=0;i<23;i++)
                        {
                            uart_putc(UART_ID,outputStream[i]);
                        }  
                    }
                    else
                    {
                        PUS1_initializeTMerror(&TM1E);
                        PUS1_setVariableValuesTMerror(&TM1E,TCDownloadPicture.packetHeader, 8, fineTime, coarseTime,1,1);
                        PUS1_transformIntoByteArrayTMerror(&TM1E,outputStream);
                        for (int i=0;i<24;i++)
                        {
                            uart_putc(UART_ID,outputStream[i]);
                        }  
                    } 

                    lengthMatrix = 64;
                    memset(takeImg, 0, 64 * 64 * sizeof(uint8_t));
                    memset(small_matrix, 0, 32 * 32 * sizeof(uint8_t));
                    break;
                }
            break;
            } 
            
            indexStream = 0;
            receiveValue=0;           
            circularBufferInit(&buffer);
            memset(inputStream, 0, sizeof(uint8_t)*300);
            memset(outputStream, 0, sizeof(uint8_t)*4200);           
           
        }  
             
    }
}

/**
 * @brief This function is used to setup the serial conection, the ISR and the FreeRTOS tasks
 * 
 */
int main()
{

/// Set up our UART with a basic baud rate.
uart_init(UART_ID, 2400);

xQueueISR=xQueueCreate(256, sizeof(uint8_t));
xQueueHouseKeep=xQueueCreate(256, sizeof(uint8_t));
xQueueAnswer=xQueueCreate(256, sizeof(uint8_t));
xQueueTakePicture=xQueueCreate(256, sizeof(uint8_t));

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
UBaseType_t uxCoreAffinityMask0=0x1;
UBaseType_t uxCoreAffinityMask1=0x2;

xTaskCreate(readFromBuffer, "Read BF", 256, NULL, readPriority, &readHandle);
xTaskCreate(getParamHK, "Set Parameters for HouseKeep", 256, NULL, getParamPriority, &getParamHandle);
xTaskCreate(answer, "Send TM", 256, NULL, answerPriority, &answerHandle);
xTaskCreate(timeManagementFine, "fine Time", 256, NULL, fineTimePriority, &fineHandle);
xTaskCreate(timeManagementCoarse, "coarse Time", 256, NULL, coarseTimePriority, &coarseHandle);
xTaskCreate(takePictureTC2221, "get picture from CAM", 256, NULL, takePicturePriority, &takePictureHandle);

vTaskCoreAffinitySet(readHandle,uxCoreAffinityMask0);
vTaskCoreAffinitySet(getParamHandle,uxCoreAffinityMask0);
vTaskCoreAffinitySet(answerHandle,uxCoreAffinityMask0);
vTaskCoreAffinitySet(coarseHandle,uxCoreAffinityMask0);
vTaskCoreAffinitySet(takePictureHandle,uxCoreAffinityMask0);
vTaskCoreAffinitySet(fineHandle,uxCoreAffinityMask1);
vTaskStartScheduler();

// OK, all set up.

// Lets send a basic inputStreaming out, and then run a loop and wait for RX interrupts

// The handler will count them, but also reflect the incoming data back with a slight change!

while (1)
{
tight_loop_contents();
}

}
