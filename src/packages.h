#ifndef _PACKAGES_
#define _PACKAGES_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**********************************************************************

* Structure:   _packetID

* Description: This structure represents the Packet ID section in a telecomand/telemetry

**********************************************************************/
typedef struct _packetID{

    unsigned int VersionNumber : 3;
    unsigned int PacketType : 1;
    unsigned int DataFieldHeaderFlag : 1;

} PacketID;

/**********************************************************************

* Structure:   _packetHeader

* Description: This structure represents the Packet Header section in a telecomand/telemetry

**********************************************************************/
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

/**********************************************************************

* Structure:   _dataFieldHeaderTM

* Description: This structure represents the Data Field Header section in a telecomand

**********************************************************************/
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

/**********************************************************************

* Structure:   _dataFieldHeaderTC

* Description: This structure represents the Data Field Header section in a telemetry

**********************************************************************/
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

/**********************************************************************

* Structure:   _TMvalid

* Description: This structure represents the telemetry package for success

**********************************************************************/
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

/**********************************************************************

* Function:    PUS1_initializeTMvalid

* Description: This function is used to initialize the telemetry package of service type 1 with 0 on all variables

* Params:      TM * tm

* Return:      void

**********************************************************************/
void PUS1_initializeTMvalid(TMvalid * tm);

/**********************************************************************

* Function:    PUS1_setVariableValuesTMvalid

* Description: This function is used to initialize the telemetry package of service type 1 with 0 on all variables

* Params:      TM * tm

* Return:      void

**********************************************************************/
void PUS1_setVariableValuesTMvalid(TMvalid * tm);

/**********************************************************************

* Function:    PUS1_transformIntoByteArrayTMvalid

* Description: This function is used to transform the package into an array of bytes in order to be sent through the serial

* Params:      TM *tm, uint8_t * octets

* Return:      void

**********************************************************************/
void PUS1_transformIntoByteArrayTMvalid(TMvalid *tm, uint8_t * octets);

 /**********************************************************************

* Structure:   _TMerror

* Description: This structure represents the telemetry package for failure

**********************************************************************/
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

/**********************************************************************

* Structure:   _TC

* Description: This structure represents the telecomand package

**********************************************************************/
typedef struct _TC 
{
    PacketHeader packetHeader;

    //Packet Data Field

    //Data Field Header

    DataFieldHeaderTC dataFieldHeader;

    unsigned int PacketErrorControl : 16;
}TC171;

/**********************************************************************

* Function:    PUS17_initializeFromByteArrayTC

* Description: This function is used to initialize the telecomand package using a byte array coming from the MAIN

* Params:      TC171 * TC,uint8_t * buffer

* Return:      unsigned int: 1 - bytes received successfully

**********************************************************************/
unsigned int PUS17_initializeFromByteArrayTC(TC171 * TC, uint8_t * buffer);

#endif
