#include "packages.h"
#include "crc.h"
void PUS1_initializeTMvalid(TMvalid * tm)
{
    memset(tm, 0, sizeof(TMvalid));
}

void PUS1_setVariableValuesTMvalid(TMvalid * tm)
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
    tm->dataFieldHeader.ServiceSubtype=0b00000001;
    tm->dataFieldHeader.DestinationID=0b00000000;
    tm->dataFieldHeader.CoarseTime=0b00000000000000000000000000000000;
    tm->dataFieldHeader.FineTime=0b0000000000000000;

    //Request ID
    tm->PacketVersionNumber=0b000;

    //Packet ID
    tm->PacketTypeS=0b000;
    bool SecondaryHeaderFlag=0b0;
    tm->ApplicationProcessID=0b00000000000;

    //Packet Sequence Control
    tm->SequenceFlagsS=0b00;
    tm->PacketSequenceCount=0b00000000000000;

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



