/**
 * @file packages.c
 * @brief File meant to handle packages and their functions 
 * 
 */

#include "packages.h"
#include "crc.h"
void PUS1_initializeTMvalid(TMvalid * tm)
{
    memset(tm, 0, sizeof(TMvalid));
}

void PUS3_initializeTMHR(TMHR * tm)
{
    memset(tm, 0, sizeof(TMHR));
}
void PUS222_initializeTM22210(TM22210 * tm)
{
    memset(tm, 0, sizeof(TM22210));
}
void PUS1_initializeTMerror(TMerror * tm)
{
     memset(tm, 0, sizeof(TMerror));
}

void PUS1_setVariableValuesTMvalid(TMvalid * tm, PacketHeader packetHeader,uint8_t ServiceSubType, uint16_t FineTime, uint32_t CoarseTime)
{
    //Packet Header (48 bits)

    //Packet ID
    tm->packetHeader.packetID.VersionNumber=0b000;
    tm->packetHeader.packetID.PacketType=0b0;
    tm->packetHeader.packetID.DataFieldHeaderFlag=0b1;

    //APID
    tm->packetHeader.PID=0b0101101;
    tm->packetHeader.PCAT=0b0001;

    //Package Sequence Control
    tm->packetHeader.SequenceFlags=0b00;
    tm->packetHeader.SourceSequenceCount=0b00000000000000;

    tm->packetHeader.PacketLength=0b0000000000010000;

    //Packet Data Field

    //Data Field Header
    tm->dataFieldHeader.Spare1=0b0;
    tm->dataFieldHeader.PUSversion=0b001;
    tm->dataFieldHeader.Spare2=0b0000;
    tm->dataFieldHeader.ServiceType=0b00000001;
    tm->dataFieldHeader.ServiceSubtype=ServiceSubType;
    tm->dataFieldHeader.DestinationID=0b00000000;
    tm->dataFieldHeader.CoarseTime=CoarseTime;
    tm->dataFieldHeader.FineTime=FineTime;

    //Request ID
    tm->PacketVersionNumber=packetHeader.packetID.VersionNumber;

    //Packet ID
    tm->PacketTypeS=packetHeader.packetID.PacketType;
    bool SecondaryHeaderFlag=packetHeader.packetID.DataFieldHeaderFlag;
    tm->ApplicationProcessID=(packetHeader.PID<<4)+packetHeader.PCAT;

    //Packet Sequence Control
    tm->SequenceFlagsS=packetHeader.SequenceFlags;
    tm->PacketSequenceCount=packetHeader.SourceSequenceCount;

    tm->PacketErrorControl=0b0000000000000000;
}

void PUS1_setVariableValuesTMerror(TMerror * tm,PacketHeader packetHeader ,uint8_t ServiceSubType, uint16_t FineTime, uint32_t CoarseTime,uint16_t FailureID,uint32_t DataError)
{
    //Packet Header (48 bits)

    //Packet ID
    tm->packetHeader.packetID.VersionNumber=0b000;
    tm->packetHeader.packetID.PacketType=0b0;
    tm->packetHeader.packetID.DataFieldHeaderFlag=0b1;

    //APID
    tm->packetHeader.PID=0b0101101;
    tm->packetHeader.PCAT=0b0001;

    //Package Sequence Control
    tm->packetHeader.SequenceFlags=0b00;
    tm->packetHeader.SourceSequenceCount=0b00000000000000;

    tm->packetHeader.PacketLength=0b0000000000010001;

    //Packet Data Field

    //Data Field Header
    tm->dataFieldHeader.Spare1=0b0;
    tm->dataFieldHeader.PUSversion=0b001;
    tm->dataFieldHeader.Spare2=0b0000;
    tm->dataFieldHeader.ServiceType=0b00000001;
    tm->dataFieldHeader.ServiceSubtype=ServiceSubType;
    tm->dataFieldHeader.DestinationID=0b00000000;
    tm->dataFieldHeader.CoarseTime=CoarseTime;
    tm->dataFieldHeader.FineTime=FineTime;

    //Request ID
    tm->PacketVersionNumber=packetHeader.packetID.VersionNumber;

    //Packet ID
    tm->PacketTypeS=packetHeader.packetID.PacketType;
    bool SecondaryHeaderFlag=packetHeader.packetID.DataFieldHeaderFlag;
    tm->ApplicationProcessID=(packetHeader.PID<<4)+packetHeader.PCAT;

    //Packet Sequence Control
    tm->SequenceFlagsS=packetHeader.SequenceFlags;
    tm->PacketSequenceCount=packetHeader.SourceSequenceCount;
    tm->Code=FailureID;
    tm->Data=DataError;

    tm->PacketErrorControl=0b0000000000000000;
}

void PUS3_setVariableValuesTMHR(TMHR * tm,uint32_t  paramHR[], uint16_t FineTime, uint32_t CoarseTime)
{
    //Packet Header (48 bits)

    //Packet ID
    tm->packetHeader.packetID.VersionNumber=0b000;
    tm->packetHeader.packetID.PacketType=0b0;
    tm->packetHeader.packetID.DataFieldHeaderFlag=0b1;

    //APID
    tm->packetHeader.PID=0b0101101;
    tm->packetHeader.PCAT=0b0001;

    //Package Sequence Control
    tm->packetHeader.SequenceFlags=0b00;
    tm->packetHeader.SourceSequenceCount=0b00000000000000;

    tm->packetHeader.PacketLength=0b0000000000010001;

    //Packet Data Field

    //Data Field Header
    tm->dataFieldHeader.Spare1=0b0;
    tm->dataFieldHeader.PUSversion=0b001;
    tm->dataFieldHeader.Spare2=0b0000;
    tm->dataFieldHeader.ServiceType=0b00000011;
    tm->dataFieldHeader.ServiceSubtype=0b00011001;
    tm->dataFieldHeader.DestinationID=0b00000000;
    tm->dataFieldHeader.CoarseTime=CoarseTime;
    tm->dataFieldHeader.FineTime=FineTime;

    tm->structureID=paramHR[0] & 0b1111111111111111;
    tm->paramHR=paramHR[1];

    tm->PacketErrorControl=0b0000000000000000;
}

void PUS1_transformIntoByteArrayTMvalid(TMvalid *tm, uint8_t * octets) 
{
    octets[0] = (((((tm->packetHeader.packetID.VersionNumber << 1) + tm->packetHeader.packetID.PacketType) << 1) + tm->packetHeader.packetID.DataFieldHeaderFlag) << 3) + (tm->packetHeader.PID >> 4);
    octets[1] = ((tm->packetHeader.PID & 0b1111) << 4) + tm->packetHeader.PCAT;
    octets[2] = (tm->packetHeader.SequenceFlags << 6) + (tm->packetHeader.SourceSequenceCount >> 8);
    octets[3] = (tm->packetHeader.SourceSequenceCount & 0b11111111);
    octets[4] = tm->packetHeader.PacketLength >> 8;
    octets[5] = tm->packetHeader.PacketLength & 0b11111111;
    octets[6] = (((tm->dataFieldHeader.Spare1 << 3) + tm->dataFieldHeader.PUSversion) << 4) + tm->dataFieldHeader.Spare2;
    octets[7] = tm->dataFieldHeader.ServiceType;
    octets[8] = tm->dataFieldHeader.ServiceSubtype;
    octets[9] = tm->dataFieldHeader.DestinationID;
    octets[10] = tm->dataFieldHeader.CoarseTime >> 24;
    octets[11] = (tm->dataFieldHeader.CoarseTime >> 16) & 0b11111111;
    octets[12] = (tm->dataFieldHeader.CoarseTime >> 8) & 0b11111111;
    octets[13] = tm->dataFieldHeader.CoarseTime & 0b11111111;
    octets[14] = tm->dataFieldHeader.FineTime >> 8;
    octets[15] = tm->dataFieldHeader.FineTime & 0b11111111;
    octets[16] = (((((tm->PacketVersionNumber << 3) + tm->PacketTypeS) << 1) + tm->SecondaryHeaderFlag) << 1) + (tm->ApplicationProcessID >> 10);
    octets[17] = (tm->ApplicationProcessID >> 2) & 0b11111111;
    octets[18] = ((((tm->ApplicationProcessID & 0b11) << 2) + tm->SequenceFlagsS) << 4) + (tm->PacketSequenceCount >> 10);
    octets[19] = (tm->PacketSequenceCount >> 2) & 0b11111111;
    octets[20] = tm->PacketSequenceCount & 0b11;

    uint16_t PacketErrorControl = crc16_ccitt(octets, 21);

    tm->PacketErrorControl = PacketErrorControl;

    octets[21] = tm->PacketErrorControl >> 8;
    octets[22] = tm->PacketErrorControl & 0b11111111;
}

void PUS1_transformIntoByteArrayTMerror(TMerror *tm, uint8_t * octets)
{
    octets[0] = (((((tm->packetHeader.packetID.VersionNumber << 1) + tm->packetHeader.packetID.PacketType) << 1) + tm->packetHeader.packetID.DataFieldHeaderFlag) << 3) + (tm->packetHeader.PID >> 4);
    octets[1] = ((tm->packetHeader.PID & 0b1111) << 4) + tm->packetHeader.PCAT;
    octets[2] = (tm->packetHeader.SequenceFlags << 6) + (tm->packetHeader.SourceSequenceCount >> 8);
    octets[3] = (tm->packetHeader.SourceSequenceCount & 0b11111111);
    octets[4] = tm->packetHeader.PacketLength >> 8;
    octets[5] = tm->packetHeader.PacketLength & 0b11111111;
    octets[6] = (((tm->dataFieldHeader.Spare1 << 3) + tm->dataFieldHeader.PUSversion) << 4) + tm->dataFieldHeader.Spare2;
    octets[7] = tm->dataFieldHeader.ServiceType;
    octets[8] = tm->dataFieldHeader.ServiceSubtype;
    octets[9] = tm->dataFieldHeader.DestinationID;
    octets[10] = tm->dataFieldHeader.CoarseTime >> 24;
    octets[11] = (tm->dataFieldHeader.CoarseTime >> 16) & 0b11111111;
    octets[12] = (tm->dataFieldHeader.CoarseTime >> 8) & 0b11111111;
    octets[13] = tm->dataFieldHeader.CoarseTime & 0b11111111;
    octets[14] = tm->dataFieldHeader.FineTime >> 8;
    octets[15] = tm->dataFieldHeader.FineTime & 0b11111111;
    octets[16] = (((((tm->PacketVersionNumber << 3) + tm->PacketTypeS) << 1) + tm->SecondaryHeaderFlag) << 1) + (tm->ApplicationProcessID >> 10);
    octets[17] = (tm->ApplicationProcessID >> 2) & 0b11111111;
    octets[18] = ((((tm->ApplicationProcessID & 0b11) << 2) + tm->SequenceFlagsS) << 4) + (tm->PacketSequenceCount >> 10);
    octets[19] = (tm->PacketSequenceCount >> 2) & 0b11111111;
    octets[20] = tm->PacketSequenceCount & 0b11;
    octets[21] = ((tm->Code & 0b11)<<4 ) + tm->Data;

    uint16_t PacketErrorControl = crc16_ccitt(octets, 22);

    tm->PacketErrorControl = PacketErrorControl;

    octets[21] = tm->PacketErrorControl >> 8;
    octets[22] = tm->PacketErrorControl & 0b11111111;
}

unsigned int PUS17_initializeFromByteArrayTC(TC171 * TC, uint8_t * buffer)
{
    TC->packetHeader.packetID.VersionNumber=buffer[0]>>5;
    TC->packetHeader.packetID.PacketType=(buffer[0]>>4) & 0b1;
    TC->packetHeader.packetID.DataFieldHeaderFlag=(buffer[0]>>3) & 0b1;
    TC->packetHeader.PID=((buffer[0] & 0b111) <<4 )+(buffer[1]>>4);
    TC->packetHeader.PCAT=buffer[1] & 0b1111;
    TC->packetHeader.SequenceFlags=buffer[2]>>6;
    TC->packetHeader.SourceSequenceCount=((buffer[2] &0b111111)<< 8 ) +buffer[3];
    TC->packetHeader.PacketLength=(buffer[4]<<8) + buffer[5];
    TC->dataFieldHeader.CCSDSsecondaryHeaderFlag=buffer[6]>>7;
    TC->dataFieldHeader.PUSversion=(buffer[6]>>4) & 0b111;
    TC->dataFieldHeader.ACK=buffer[6] & 0b1111;
    TC->dataFieldHeader.ServiceType=buffer[7];
    TC->dataFieldHeader.ServiceSubtype=buffer[8];
    TC->dataFieldHeader.SourceID=buffer[9];
    TC->PacketErrorControl=(buffer[10]<<8)+buffer[11];
    
    size_t n=10;
    uint16_t PacketErrorControl = crc16_ccitt(buffer,n);
    if (PacketErrorControl != TC->PacketErrorControl) 
    {
        return 0;
    }
        
    return 1;
}

unsigned int PUS3_initializeFromByteArrayTCHR(TCHR * TC, uint8_t * buffer) 
{
    TC->packetHeader.packetID.VersionNumber=buffer[0]>>5;
    TC->packetHeader.packetID.PacketType=(buffer[0]>>4) & 0b1;
    TC->packetHeader.packetID.DataFieldHeaderFlag=(buffer[0]>>3) & 0b1;
    TC->packetHeader.PID=((buffer[0] & 0b111) <<4 )+(buffer[1]>>4);
    TC->packetHeader.PCAT=buffer[1] & 0b1111;
    TC->packetHeader.SequenceFlags=buffer[2]>>6;
    TC->packetHeader.SourceSequenceCount=((buffer[2] &0b111111)<< 8 ) +buffer[3];
    TC->packetHeader.PacketLength=(buffer[4]<<8) + buffer[5];
    TC->dataFieldHeader.CCSDSsecondaryHeaderFlag=buffer[6]>>7;
    TC->dataFieldHeader.PUSversion=(buffer[6]>>4) & 0b111;
    TC->dataFieldHeader.ACK=buffer[6] & 0b1111;
    TC->dataFieldHeader.ServiceType=buffer[7];
    TC->dataFieldHeader.ServiceSubtype=buffer[8];
    TC->dataFieldHeader.SourceID=buffer[9];
    TC->SID=buffer[10];
    TC->DataCollectionInterval=(buffer[11]<<8) + buffer[12];
    TC->PacketErrorControl=(buffer[13]<<8)+buffer[14];
    
    size_t n=13;
    uint16_t PacketErrorControl = crc16_ccitt(buffer,n);
    if (PacketErrorControl != TC->PacketErrorControl) 
    {
        return 0;
    }
        
    return 1;
}

unsigned int PUS3_initializeFromByteArrayTMHR(TMHR * tm, uint8_t * octets)
{
    octets[0] = (((((tm->packetHeader.packetID.VersionNumber << 1) + tm->packetHeader.packetID.PacketType) << 1) + tm->packetHeader.packetID.DataFieldHeaderFlag) << 3) + (tm->packetHeader.PID >> 4);
    octets[1] = ((tm->packetHeader.PID & 0b1111) << 4) + tm->packetHeader.PCAT;
    octets[2] = (tm->packetHeader.SequenceFlags << 6) + (tm->packetHeader.SourceSequenceCount >> 8);
    octets[3] = (tm->packetHeader.SourceSequenceCount & 0b11111111);
    octets[4] = tm->packetHeader.PacketLength >> 8;
    octets[5] = tm->packetHeader.PacketLength & 0b11111111;
    octets[6] = (((tm->dataFieldHeader.Spare1 << 3) + tm->dataFieldHeader.PUSversion) << 4) + tm->dataFieldHeader.Spare2;
    octets[7] = tm->dataFieldHeader.ServiceType;
    octets[8] = tm->dataFieldHeader.ServiceSubtype;
    octets[9] = tm->dataFieldHeader.DestinationID;
    octets[10] = tm->dataFieldHeader.CoarseTime >> 24;
    octets[11] = (tm->dataFieldHeader.CoarseTime >> 16) & 0b11111111;
    octets[12] = (tm->dataFieldHeader.CoarseTime >> 8) & 0b11111111;
    octets[13] = tm->dataFieldHeader.CoarseTime & 0b11111111;
    octets[14] = tm->dataFieldHeader.FineTime >> 8;
    octets[15] = tm->dataFieldHeader.FineTime & 0b11111111;
    octets[16] = tm->structureID >> 8;
    octets[17] = tm->structureID & 0b11111111;
    octets[18] = tm->paramHR >> 24;
    octets[19] = (tm->paramHR >> 16) & 0b11111111;
    octets[20] = (tm->paramHR >> 8) & 0b11111111;
    octets[21] = tm->paramHR & 0b11111111;

    uint16_t PacketErrorControl = crc16_ccitt(octets, 22);

    tm->PacketErrorControl = PacketErrorControl;

    octets[22] = tm->PacketErrorControl >> 8;
    octets[23] = tm->PacketErrorControl & 0b11111111;
}

unsigned int PUS9_initializeFromByteArrayTCTiM(TCTiM * TC, uint8_t * buffer)
{
    TC->packetHeader.packetID.VersionNumber=buffer[0] >> 5;
    TC->packetHeader.packetID.PacketType=(buffer[0] >> 4) & 0b1;
    TC->packetHeader.packetID.DataFieldHeaderFlag=(buffer[0] >> 3) & 0b1;
    TC->packetHeader.PID=((buffer[0] & 0b111) << 4 )+(buffer[1] >> 4);
    TC->packetHeader.PCAT=buffer[1] & 0b1111;
    TC->packetHeader.SequenceFlags=buffer[2]>>6;
    TC->packetHeader.SourceSequenceCount=((buffer[2] &0b111111)<< 8 ) +buffer[3];
    TC->packetHeader.PacketLength=(buffer[4]<<8) + buffer[5];
    TC->CoarseTime=(buffer[6] << 24) + (buffer[7] << 16) + (buffer[8] << 8) + buffer[9];
    TC->FineTime=(buffer[10] << 8) + buffer[11];
    TC->PacketErrorControl=(buffer[12]<<8)+buffer[13];
    
    size_t n=12;
    uint16_t PacketErrorControl = crc16_ccitt(buffer,n);
    if (PacketErrorControl != TC->PacketErrorControl) 
    {
        return 0;
    }
        
    return 1;
}


unsigned int PUS222_initializeFromByteArrayTC2221(TC2221 * TC, uint8_t * buffer)
{
    TC->packetHeader.packetID.VersionNumber=buffer[0]>>5;
    TC->packetHeader.packetID.PacketType=(buffer[0]>>4) & 0b1;
    TC->packetHeader.packetID.DataFieldHeaderFlag=(buffer[0]>>3) & 0b1;
    TC->packetHeader.PID=((buffer[0] & 0b111) <<4 )+(buffer[1]>>4);
    TC->packetHeader.PCAT=buffer[1] & 0b1111;
    TC->packetHeader.SequenceFlags=buffer[2]>>6;
    TC->packetHeader.SourceSequenceCount=((buffer[2] &0b111111)<< 8 ) +buffer[3];
    TC->packetHeader.PacketLength=(buffer[4]<<8) + buffer[5];
    TC->dataFieldHeader.CCSDSsecondaryHeaderFlag=buffer[6]>>7;
    TC->dataFieldHeader.PUSversion=(buffer[6]>>4) & 0b111;
    TC->dataFieldHeader.ACK=buffer[6] & 0b1111;
    TC->dataFieldHeader.ServiceType=buffer[7];
    TC->dataFieldHeader.ServiceSubtype=buffer[8];
    TC->dataFieldHeader.SourceID=buffer[9];
    TC->Exposure=(buffer[10]<<8)+(buffer[11] & 0b1111111);
    TC->ExposureStep=buffer[11] & 0b1;
    TC->ImageID=buffer[12]>>6;
    TC->Reserved=buffer[12] & 0b111111;
    TC->PacketErrorControl=(buffer[13]<<8)+buffer[14];
    
    size_t n=15;
    uint16_t PacketErrorControl = crc16_ccitt(buffer,n);
    if (PacketErrorControl != TC->PacketErrorControl) 
    {
        return 0;
    }
        
    return 1;
}
unsigned int PUS222_initializeFromByteArrayTC2222(TC2222 * TC, uint8_t * buffer)
{
    TC->packetHeader.packetID.VersionNumber=buffer[0]>>5;
    TC->packetHeader.packetID.PacketType=(buffer[0]>>4) & 0b1;
    TC->packetHeader.packetID.DataFieldHeaderFlag=(buffer[0]>>3) & 0b1;
    TC->packetHeader.PID=((buffer[0] & 0b111) <<4 )+(buffer[1]>>4);
    TC->packetHeader.PCAT=buffer[1] & 0b1111;
    TC->packetHeader.SequenceFlags=buffer[2]>>6;
    TC->packetHeader.SourceSequenceCount=((buffer[2] &0b111111)<< 8 ) +buffer[3];
    TC->packetHeader.PacketLength=(buffer[4]<<8) + buffer[5];
    TC->dataFieldHeader.CCSDSsecondaryHeaderFlag=buffer[6]>>7;
    TC->dataFieldHeader.PUSversion=(buffer[6]>>4) & 0b111;
    TC->dataFieldHeader.ACK=buffer[6] & 0b1111;
    TC->dataFieldHeader.ServiceType=buffer[7];
    TC->dataFieldHeader.ServiceSubtype=buffer[8];
    TC->dataFieldHeader.SourceID=buffer[9];
    TC->Offsest=(buffer[10]<<6) + (buffer[11] & 0b111111);
    TC->Reserved=buffer[11] & 0b11;
    TC->ADC=buffer[12];
    TC->PGA=buffer[13] >> 5;
    TC->Reserved_=buffer[13] & 0b11111;
    TC->PacketErrorControl=(buffer[14]<<8)+buffer[15];
    
    size_t n=16;
    uint16_t PacketErrorControl = crc16_ccitt(buffer,n);
    TC->PacketErrorControl=PacketErrorControl;
    if (PacketErrorControl != TC->PacketErrorControl) 
    {
        return 0;
    }
        
    return 1;

}

unsigned int PUS222_initializeFromByteArrayTC2223(TC2223 * TC, uint8_t * buffer)
{
    TC->packetHeader.packetID.VersionNumber=buffer[0]>>5;
    TC->packetHeader.packetID.PacketType=(buffer[0]>>4) & 0b1;
    TC->packetHeader.packetID.DataFieldHeaderFlag=(buffer[0]>>3) & 0b1;
    TC->packetHeader.PID=((buffer[0] & 0b111) <<4 )+(buffer[1]>>4);
    TC->packetHeader.PCAT=buffer[1] & 0b1111;
    TC->packetHeader.SequenceFlags=buffer[2]>>6;
    TC->packetHeader.SourceSequenceCount=((buffer[2] &0b111111)<< 8 ) +buffer[3];
    TC->packetHeader.PacketLength=(buffer[4]<<8) + buffer[5];
    TC->dataFieldHeader.CCSDSsecondaryHeaderFlag=buffer[6]>>7;
    TC->dataFieldHeader.PUSversion=(buffer[6]>>4) & 0b111;
    TC->dataFieldHeader.ACK=buffer[6] & 0b1111;
    TC->dataFieldHeader.ServiceType=buffer[7];
    TC->dataFieldHeader.ServiceSubtype=buffer[8];
    TC->dataFieldHeader.SourceID=buffer[9];
    TC->ImageID=buffer[10]>>6;
    TC->UnderSampling=(buffer[10]>>5) & 0b1;
    TC->Binning=(buffer[10]>>4) & 0b1;
    TC->Windowing=buffer[10] & 0b1111;
    TC->Reserved=buffer[11];
    TC->PacketErrorControl=(buffer[12]<<8)+buffer[13];
    
    size_t n=14;
    uint16_t PacketErrorControl = crc16_ccitt(buffer,n);
    if (PacketErrorControl != TC->PacketErrorControl) 
    {
        return 0;
    }
        
    return 1;
}

void PUS222_setVariableValuesTM22210(TM22210 * tm, TC2221 TC1, TC2222 TC2,uint16_t length, uint8_t  buffer[length][length]  , uint16_t FineTime, uint32_t CoarseTime)
{
    //Packet Header (48 bits)

    //Packet ID
    tm->packetHeader.packetID.VersionNumber=0b000;
    tm->packetHeader.packetID.PacketType=0b0;
    tm->packetHeader.packetID.DataFieldHeaderFlag=0b1;

    //APID
    tm->packetHeader.PID=0b0101101;
    tm->packetHeader.PCAT=0b1001;

    //Package Sequence Control
    tm->packetHeader.SequenceFlags=0b00;
    tm->packetHeader.SourceSequenceCount=0b00000000000000;
    tm->packetHeader.PacketLength=length*length+27;

    //Packet Data Field

    //Data Field Header
    tm->dataFieldHeader.Spare1=0b0;
    tm->dataFieldHeader.PUSversion=0b001;
    tm->dataFieldHeader.Spare2=0b0000;
    tm->dataFieldHeader.ServiceType=222;
    tm->dataFieldHeader.ServiceSubtype=10;
    tm->dataFieldHeader.DestinationID=0b00000000;
    tm->dataFieldHeader.CoarseTime=CoarseTime;
    tm->dataFieldHeader.FineTime=FineTime;
    tm->imageHeader.Reserved=0;
    tm->imageHeader.ImageNumber=2;
    tm->imageHeader.PGA=TC2.PGA;
    tm->imageHeader.ExposureStep=TC1.ExposureStep;
    tm->imageHeader.ExposureTime=TC1.Exposure;
    tm->imageHeader.Reserved_=0;
    tm->imageHeader.Offset=TC2.Offsest;
    tm->imageHeader.ADC=TC2.ADC;
    tm->imageHeader.Temperature=1;
    tm->imageHeader.TimeTag=CoarseTime;
    tm->imageHeader.TotalPacketNumber=1;
    tm->imageHeader.PacketNumber=1;
    tm->imageHeader.ImageNumber_=2;

    for (int i =0;i<length;i++)
    {
        for(int j=0;j<length;j++)
        {
            tm->Pixels[i][j]= buffer[i][j];
        }
    }


    tm->PacketErrorControl=0b0000000000000000;
}

unsigned int PUS222_initializeFromByteArrayTM22210(TM22210 * tm, uint8_t * octets,uint16_t length)
{
    octets[0] = (((((tm->packetHeader.packetID.VersionNumber << 1) + tm->packetHeader.packetID.PacketType) << 1) + tm->packetHeader.packetID.DataFieldHeaderFlag) << 3) + (tm->packetHeader.PID >> 4);
    octets[1] = ((tm->packetHeader.PID & 0b1111) << 4) + tm->packetHeader.PCAT;
    octets[2] = (tm->packetHeader.SequenceFlags << 6) + (tm->packetHeader.SourceSequenceCount >> 8);
    octets[3] = (tm->packetHeader.SourceSequenceCount & 0b11111111);
    octets[4] = tm->packetHeader.PacketLength >> 8;
    octets[5] = tm->packetHeader.PacketLength & 0b11111111;
    octets[6] = (((tm->dataFieldHeader.Spare1 << 3) + tm->dataFieldHeader.PUSversion) << 4) + tm->dataFieldHeader.Spare2;
    octets[7] = tm->dataFieldHeader.ServiceType;
    octets[8] = tm->dataFieldHeader.ServiceSubtype;
    octets[9] = tm->dataFieldHeader.DestinationID;
    octets[10] = tm->dataFieldHeader.CoarseTime >> 24;
    octets[11] = (tm->dataFieldHeader.CoarseTime >> 16) & 0b11111111;
    octets[12] = (tm->dataFieldHeader.CoarseTime >> 8) & 0b11111111;
    octets[13] = tm->dataFieldHeader.CoarseTime & 0b11111111;
    octets[14] = tm->dataFieldHeader.FineTime >> 8;
    octets[15] = tm->dataFieldHeader.FineTime & 0b11111111;
    octets[16] = tm->imageHeader.Reserved >>3;
    octets[17] = ((((tm->imageHeader.Reserved & 0b111) <<2) + tm->imageHeader.ImageNumber)<<3)+ tm->imageHeader.PGA;
    octets[18] =  (tm->imageHeader.ExposureStep << 7) +((tm->imageHeader.ExposureTime >>8) & 0b1111111);
    octets[19] = tm->imageHeader.ExposureTime & 0b11111111;
    octets[20] = (tm->imageHeader.Reserved_ >> 2) & 0b11111111;
    octets[21] =  ((tm->imageHeader.Reserved_ & 0b11)<<6) +(tm->imageHeader.Offset>>8);
    octets[22] = tm->imageHeader.Offset & 0b11111111;
    octets[23] = tm->imageHeader.ADC;
    octets[24] = tm->imageHeader.Temperature;
    octets[25] = tm->imageHeader.TimeTag >> 24;
    octets[26] = (tm->imageHeader.TimeTag >> 16) & 0b11111111;
    octets[27] = (tm->imageHeader.TimeTag >> 8) & 0b11111111;
    octets[28] = tm->imageHeader.TimeTag  & 0b11111111;
    octets[29] = (tm->imageHeader.TotalPacketNumber >> 3) & 0b11111111;
    octets[30] = ((tm->imageHeader.TotalPacketNumber & 0b111) << 5) +((tm->imageHeader.PacketNumber >>6)& 0b11111);
    octets[31] = ((tm->imageHeader.PacketNumber & 0b111111) << 2) + tm->imageHeader.ImageNumber_;
    uint32_t k=32;
    for (int i =0;i<length;i++)
    {
        for(int j=0;j<length;j++)
        {
            octets[k++]=tm->Pixels[i][j];
        }
    }
    uint16_t PacketErrorControl = crc16_ccitt(octets, k);

    tm->PacketErrorControl = PacketErrorControl;

    octets[k++] = tm->PacketErrorControl >> 8;
    octets[k] = tm->PacketErrorControl & 0b11111111;
    return k;
}



