/**
 * @file packages.h
 * @brief File meant to define packages and their functions 
 * 
 * 
 */

#ifndef _PACKAGES_
#define _PACKAGES_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief This structure represents the Packet ID section in a telecomand/telemetry
 * 
 */
typedef struct _packetID{

    unsigned int VersionNumber : 3;
    unsigned int PacketType : 1;
    unsigned int DataFieldHeaderFlag : 1;

} PacketID;

/**
 * @brief This structure represents the Packet Header section in a telecomand/telemetry
 * 
 */
typedef struct _packetHeader
{
    PacketID packetID;

    //APID
    unsigned int PID : 7;
    unsigned int PCAT : 4;

    //Package Sequence Control
    unsigned int SequenceFlags : 2;
    unsigned int SourceSequenceCount : 14;

    unsigned int PacketLength : 16;

} PacketHeader;

/**
 * @brief This structure represents the Data Field Header section in a telecomand
 * 
 */
typedef struct _dataFieldHeaderTM
{
    //Data Field Header
    unsigned int Spare1 : 1;
    unsigned int PUSversion : 3;
    unsigned int Spare2 : 4;
    unsigned int ServiceType : 8;
    unsigned int ServiceSubtype : 8;
    unsigned int DestinationID : 8;
    unsigned int CoarseTime : 32;
    unsigned int FineTime : 16;

} DataFieldHeaderTM;

/**
 * @brief This structure represents the Data Field Header section in a telemetry
 * 
 */
typedef struct _dataFieldHeaderTC
{
    //Data Field Header
    unsigned int CCSDSsecondaryHeaderFlag : 1;
    unsigned int PUSversion : 3;
    unsigned int ACK : 4;
    unsigned int ServiceType : 8;
    unsigned int ServiceSubtype : 8;
    unsigned int SourceID : 8;

} DataFieldHeaderTC;

/**
 * @brief This structure represents the telemetry package for success
 * 
 */
typedef struct _TMvalid 
{
    PacketHeader packetHeader;

    //Packet Data Field

    DataFieldHeaderTM dataFieldHeader;

    //Request ID
    unsigned int PacketVersionNumber : 3;

    //Packet ID
    unsigned int PacketTypeS : 3;
    unsigned int SecondaryHeaderFlag : 1;
    unsigned int ApplicationProcessID : 11;

    //Packet Sequence Control
    unsigned int SequenceFlagsS: 2;
    unsigned int PacketSequenceCount : 14;

    unsigned int PacketErrorControl : 16;
}TMvalid;

/**
 * @brief This function is used to initialize the telemetry package of service type 1 with 0 on all variables
 * 
 * @param[out] tm 
 */
void PUS1_initializeTMvalid(TMvalid * tm);

/**
 * @brief This function is used to set the values of the variables in a telemetry package of service type 1
 * 
 * @param[out] tm 
 * @param[in] packetHeader 
 * @param[in] ServiceSubType 
 * @param[in] FineTime 
 * @param[in] CoarseTime 
 */
void PUS1_setVariableValuesTMvalid(TMvalid * tm,PacketHeader packetHeader ,uint8_t ServiceSubType, uint16_t FineTime, uint32_t CoarseTime);

/**
 * @brief This function is used to transform the package into an array of bytes in order to be sent through the serial
 * 
 * @param[out] tm 
 * @param[in] octets 
 */
void PUS1_transformIntoByteArrayTMvalid(TMvalid *tm, uint8_t * octets);

/**
 * @brief This structure represents the telemetry package for failure
 * 
 */
typedef struct _TMerror 
{
    PacketHeader packetHeader;

    //Packet Data Field

    DataFieldHeaderTM dataFieldHeader;

    //Request ID
    unsigned int PacketVersionNumber : 3;

    //Packet ID
    unsigned int PacketTypeS : 3;
    bool SecondaryHeaderFlag : 1;
    unsigned int ApplicationProcessID : 11;

    //Packet Sequence Control
    unsigned int SequenceFlagsS : 2;
    unsigned int PacketSequenceCount : 14;

    //Failure Notice
    unsigned int Code : 1;
    unsigned int Data : 1;

    unsigned int PacketErrorControl : 16;
}TMerror;

/**
 * @brief This function is used to initialize the telemetry package of service type 1 with 0 on all variables
 * 
 * @param[out] tm 
 */
void PUS1_initializeTMerror(TMerror * tm);

/**
 * @brief This function is used to set the values of the variables in a telemetry package of service type 1
 * 
 * @param[out] tm 
 * @param[in] packetHeader 
 * @param[in] ServiceSubType 
 * @param[in] FineTime 
 * @param[in] CoarseTime 
 * @param[in] FailureID 
 * @param[in] DataError  
 * 
 */
void PUS1_setVariableValuesTMerror(TMerror * tm,PacketHeader packetHeader ,uint8_t ServiceSubType, uint16_t FineTime, uint32_t CoarseTime,uint16_t FailureID,uint32_t DataError);

/**
 * @brief This function is used to transform the package into an array of bytes in order to be sent through the serial
 * 
 * @param[out] tm 
 * @param[in] octets 
 */
void PUS1_transformIntoByteArrayTMerror(TMerror *tm, uint8_t * octets);

/**
 * @brief This structure represents the telecomand package
 * 
 */
typedef struct _TC 
{
    PacketHeader packetHeader;

    //Packet Data Field

    //Data Field Header

    DataFieldHeaderTC dataFieldHeader;

    unsigned int PacketErrorControl : 16;
}TC171;

/**
 * @brief This function is used to initialize the telecomand package using a byte array coming from the MAIN
 * 
 * @param[out] TC 
 * @param[in] buffer 
 * @return unsigned int: 1 - bytes received successfully / 0 - otherwise
 */
unsigned int PUS17_initializeFromByteArrayTC(TC171 * TC, uint8_t * buffer);

/**
 * @brief This structure represents the telemetry package for Housekeeping Report
 * 
 */
typedef struct _TM_HR 
{
    PacketHeader packetHeader;

    //Packet Data Field

    DataFieldHeaderTM dataFieldHeader;

    unsigned int structureID : 16;
    unsigned int paramHR : 32;

    unsigned int PacketErrorControl : 16;
}TMHR;

/**
 * @brief This function is used to initialize the telemetry package of service type 3 with 0 on all variables
 * 
 * @param[out] tm 
 */
void PUS3_initializeTMHR(TMHR * tm);

/**
 * @brief This function is used to set the values of the variables in a telemetry package of service type 3
 * 
 * @param[out] tm 
 * @param[in] paramHR 
 * @param[in] FineTime 
 * @param[in] CoarseTime 
 */
void PUS3_setVariableValuesTMHR(TMHR * tm, uint32_t  paramHR[], uint16_t FineTime, uint32_t CoarseTime);

/**
 * @brief This function is used to initialize the telemetry package using a byte in order to be sent through the serial
 * 
 * @param[in] tm 
 * @param[out] octets 
 * @return unsigned int: 1 - bytes received successfully / 0 - otherwise 
 */
unsigned int PUS3_initializeFromByteArrayTMHR(TMHR * tm, uint8_t * octets);

/**
 * @brief This structure represents the telecomand package for Housekeeping Report
 * 
 */
typedef struct _TC_HR
{
    PacketHeader packetHeader;

    //Packet Data Field

    //Data Field Header

    DataFieldHeaderTC dataFieldHeader;

    unsigned int SID : 8;
    unsigned int DataCollectionInterval : 16;

    unsigned int PacketErrorControl : 16;
}TCHR;

/**
 * @brief This function is used to initialize the telecomand package using a byte array coming from the MAIN
 * 
 * @param[out] TC 
 * @param[in] buffer 
 * @return unsigned int: 1 - bytes received successfully / 0 - otherwise
 */
unsigned int PUS3_initializeFromByteArrayTCHR(TCHR * TC, uint8_t * buffer);

/**
 * @brief This structure represents the telecomand package for Time Managaement
 * 
 */
typedef struct _TC_TiM
{
    PacketHeader packetHeader;

    unsigned int CoarseTime : 32;
    unsigned int FineTime : 16;

    unsigned int PacketErrorControl : 16;
}TCTiM;

/**
 * @brief This function is used to initialize the telecomand package using a byte array coming from the MAIN
 * 
 * @param[out] TC 
 * @param[in] buffer 
 * @return unsigned int: 1 - bytes received successfully / 0 - otherwise
 */
unsigned int PUS9_initializeFromByteArrayTCTiM(TCTiM * TC, uint8_t * buffer);


/**
 * @brief This structure represents the telecomand package for Take picture
 * 
 */
typedef struct _TC_2221 
{
    PacketHeader packetHeader;

    //Packet Data Field

    DataFieldHeaderTC dataFieldHeader;

    unsigned int Exposure : 15;
    unsigned int ExposureStep : 1;
    unsigned int ImageID : 2;
    unsigned int Reserved : 6;

    unsigned int PacketErrorControl : 16;
}TC2221;
/**
 * @brief This function is used to initialize the telecomand package using a byte array coming from the MAIN
 * 
 * @param[out] TC 
 * @param[in] buffer 
 * @return unsigned int: 1 - bytes received successfully / 0 - otherwise
 */
unsigned int PUS222_initializeFromByteArrayTC2221(TC2221 * TC, uint8_t * buffer);


/**
 * @brief This structure represents the telecomand package for configure camera
 * 
 */
typedef struct _TC_2222 
{
    PacketHeader packetHeader;

    //Packet Data Field

    DataFieldHeaderTC dataFieldHeader;

    unsigned int Offsest: 14;
    unsigned int Reserved : 2;
    unsigned int ADC : 8;
    unsigned int PGA : 3;
    unsigned int Reserved_ : 5;

    unsigned int PacketErrorControl : 16;
}TC2222;
/**
 * @brief This function is used to initialize the telecomand package using a byte array coming from the MAIN
 * 
 * @param[out] TC 
 * @param[in] buffer 
 * @return unsigned int: 1 - bytes received successfully / 0 - otherwise
 */
unsigned int PUS222_initializeFromByteArrayTC2222(TC2222 * TC, uint8_t * buffer);


/**
 * @brief This structure represents the telecomand package for downland picture
 * 
 */
typedef struct _TC_2223
{
    PacketHeader packetHeader;

    //Packet Data Field

    DataFieldHeaderTC dataFieldHeader;

    unsigned int ImageID: 2;
    unsigned int UnderSampling : 1;
    unsigned int Binning : 1;
    unsigned int Windowing : 4;
    unsigned int Reserved : 8;

    unsigned int PacketErrorControl : 16;
    
}TC2223;
/**
 * @brief This function is used to initialize the telecomand package using a byte array coming from the MAIN
 * 
 * @param[out] TC 
 * @param[in] buffer 
 * @return unsigned int: 1 - bytes received successfully / 0 - otherwise
 */
unsigned int PUS222_initializeFromByteArrayTC2223(TC2223 * TC, uint8_t * buffer);


typedef struct _ImageHeader
{
    unsigned int Reserved :11;
    unsigned int ImageNumber:2;
    unsigned int PGA :3 ;
    unsigned int ExposureStep :1;
    unsigned int ExposureTime : 15;
    unsigned int Reserved_:10;
    unsigned int Offset :14;
    unsigned int ADC :8;
    unsigned int Temperature :8;
    unsigned int TimeTag :32;
    unsigned int TotalPacketNumber :11;
    unsigned int PacketNumber:11;
    unsigned int ImageNumber_:2;

}ImageHeader;

/**
 * @brief This structure represents the telemetry package for send picture
 * 
 */
typedef struct _TM_22210
{
    PacketHeader packetHeader;

    //Packet Data Field

    DataFieldHeaderTM dataFieldHeader;
    ImageHeader imageHeader;

    uint8_t Pixels[64][64];


    unsigned int PacketErrorControl : 16;
}TM22210;

/**
 * @brief This function is used to initialize the telemetry package of service type 1 with 0 on all variables
 * 
 * @param[out] tm 
 */
void PUS222_initializeTM22210(TM22210 * tm);

/**
 * @brief This function is used to set the values of the variables in a telemetry package of service type 222
 * 
 * @param[out] tm 
 * @param[in] TC1 
 * @param[in] TC2 
 * @param[in] length 
 * @param[in] FineTime 
 * @param[in] CoarseTime 
 */
void PUS222_setVariableValuesTM22210(TM22210 * tm, TC2221 TC1, TC2222 TC2,uint16_t length, uint8_t buffer[length][length] , uint16_t FineTime, uint32_t CoarseTime);

/**
 * @brief This function is used to initialize the telemetry package using a byte in order to be sent through the serial
 * 
 * @param[in] tm 
 * @param[out] octets 
 * @param[in] length 
 * @return unsigned int: 1 - bytes received successfully / 0 - otherwise
 */
unsigned int PUS222_initializeFromByteArrayTM22210(TM22210 * tm, uint8_t * octets,uint16_t length);
#endif
