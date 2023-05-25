import struct
import ctypes


##This structure represents the Packet ID section in a telecomand/telemetry
class _packetID(ctypes.BigEndianStructure):
    _fields_ = [
        ("VersionNumber", ctypes.c_uint32, 3),
        ("PacketType", ctypes.c_uint32, 1),
        ("DataFieldHeaderFlag", ctypes.c_uint32, 1),
    ]

    def __init__(self,
                 VersionNumber=0b000,
                 PacketType=0b1,
                 DataFieldHeaderFlag=0b1):
        super(_packetID, self).__init__(VersionNumber,
                                        PacketType,
                                        DataFieldHeaderFlag)


## This structure represents the Packet Header section in a telecomand/telemetry
class _packetHeader(ctypes.BigEndianStructure):
    _fields_ = [
        ("packetID", _packetID),

        # APID

        ("PID", ctypes.c_uint32, 7),  # 4
        ("PCAT", ctypes.c_uint32, 4),

        # Package Sequence Control

        ("SequenceFlags", ctypes.c_uint32, 2),
        ("SourceSequenceCount", ctypes.c_uint32, 14),

        ("PacketLength", ctypes.c_uint32, 16),
    ]

    def __init__(self,
                 packetID=_packetID(),
                 PID=0b0101101,
                 PCAT=0b1100,
                 SequenceFlags=0b11,
                 SourceSequenceCount=0b00000000000000,
                 PacketLength=0b0000000000000101):
        super(_packetHeader, self).__init__(packetID,
                                            PID,
                                            PCAT,
                                            SequenceFlags,
                                            SourceSequenceCount,
                                            PacketLength)



##This structure represents the Data Field Header section in a telecomand
class _dataFieldHeaderTC(ctypes.BigEndianStructure):
    _fields_ = [
        ("CCSDSsecondaryHeaderFlag", ctypes.c_uint32, 1),
        ("PUSversion", ctypes.c_uint32, 3),
        ("ACK", ctypes.c_uint32, 4),
        ("ServiceType", ctypes.c_uint32, 8),
        ("ServiceSubtype", ctypes.c_uint32, 8),
        ("SourceID", ctypes.c_uint32, 8)
    ]

    def __init__(self,
                 CCSDSsecondaryHeaderFlag=0b0,
                 PUSVersion=0b1,
                 ACK=0b0001,
                 ServiceType=0b0000011,
                 ServiceSubType=0b10000010,
                 SourceID=0b00000000):
        super(_dataFieldHeaderTC, self).__init__(CCSDSsecondaryHeaderFlag,
                                                 PUSVersion,
                                                 ACK,
                                                 ServiceType,
                                                 ServiceSubType,
                                                 SourceID)


## This structure represents the Data Field Header section in a telemetry
class _dataFieldHeaderTM(ctypes.BigEndianStructure):
    _fields_ = [
        ("Spare1", ctypes.c_uint32, 1),
        ("PUSversion", ctypes.c_uint32, 3),
        ("Spare2", ctypes.c_uint32, 4),
        ("ServiceType", ctypes.c_uint32, 8),
        ("ServiceSubtype", ctypes.c_uint32, 8),
        ("DestinationID", ctypes.c_uint32, 8),
        ("CoarseTime", ctypes.c_uint32, 32),
        ("FineTime", ctypes.c_uint32, 16)
    ]

    def __init__(self,
                 Spare1=0b0,
                 PUSVersion=0b1,
                 Spare2=0b0000,
                 ServiceType=0b00000001,
                 ServiceSubType=0b00000001,
                 DestinationID=0b00000000,
                 CoarseTime=0b00000000000000000000000000000000,
                 FineTime=0b0000000000000000):
        super(_dataFieldHeaderTM, self).__init__(Spare1,
                                                 PUSVersion,
                                                 Spare2,
                                                 ServiceType,
                                                 ServiceSubType,
                                                 DestinationID,
                                                 CoarseTime,
                                                 FineTime)



## This structure represents the telecomand package 
class _TC(ctypes.BigEndianStructure):
    _fields_ = [
        ("packetHeader", _packetHeader),

        # Packet Data Field

        ("dataFieldHeader", _dataFieldHeaderTC),

        ("PacketErrorControl", ctypes.c_uint32, 16),
    ]

    def __init__(self,
                 packetHeader=_packetHeader(),
                 dataFieldHeader=_dataFieldHeaderTC(),
                 PacketErrorControl=0b0000000000000000):
        super(_TC, self).__init__(packetHeader,
                                  dataFieldHeader,
                                  PacketErrorControl
                                  )



## This structure represents the telemetry package for execution success
class _TMvalid(ctypes.BigEndianStructure):
    _fields_ = [
        ("packetHeader", _packetHeader),

        # Packet Data Field

        ("dataFieldHeader", _dataFieldHeaderTM),

        # Request ID
        ("PacketVersionNumber", ctypes.c_uint32, 3),

        # Packet ID
        ("PacketType", ctypes.c_uint32, 3),
        ("SecondaryHeaderFlag", ctypes.c_uint32, 1),
        ("ApplicationProcessID", ctypes.c_uint32, 11),

        # Packet Sequence Control
        ("SequenceFlags", ctypes.c_uint32, 2),
        ("PacketSequenceCount", ctypes.c_uint32, 14),

        ("PacketErrorControl", ctypes.c_uint32, 16),
    ]

    def __init__(self,
                 packetHeader=_packetHeader(),
                 dataFieldHeader=_dataFieldHeaderTM(),
                 PacketVersionNumberS=0b000,
                 PacketTypeS=0b000,
                 SecondaryHeaderFlag=0b0,
                 ApplicationProcessID=0b00000000000,
                 SequenceFlagsS=0b00,
                 PacketSequenceCount=0b00000000000000,
                 PacketErrorControl=0b0000000000000000):
        super(_TMvalid, self).__init__(packetHeader,
                                       dataFieldHeader,
                                       PacketVersionNumberS,
                                       PacketTypeS,
                                       SecondaryHeaderFlag,
                                       ApplicationProcessID,
                                       SequenceFlagsS,
                                       PacketSequenceCount,
                                       PacketErrorControl
                                       )




##This structure represents the telemetry package for execution failure
class _TMerror(ctypes.BigEndianStructure):
    _fields_ = [
        ("packetHeader", _packetHeader),

        # Packet Data Field

        ("dataFieldHeader", _dataFieldHeaderTM),

        # Request ID
        ("PacketVersionNumberS", ctypes.c_uint32, 3),

        # Packet ID
        ("PacketTypeS", ctypes.c_uint32, 3),
        ("SecondaryHeaderFlag", ctypes.c_uint32, 1),
        ("ApplicationProcessID", ctypes.c_uint32, 11),

        # Packet Sequence Control
        ("SequenceFlagsS", ctypes.c_uint32, 2),
        ("PacketSequenceCount", ctypes.c_uint32, 14),

        # Failure Notice
        ("Code", ctypes.c_uint32),
        ("Data", ctypes.c_uint32),

        ("PacketErrorControl", ctypes.c_uint32, 16),
    ]

    def __init__(self,
                 packetHeader=_packetHeader(),
                 dataFieldHeader=_dataFieldHeaderTM(),
                 PacketVersionNumberS=0b000,
                 PacketTypeS=0b000,
                 SecondaryHeaderFlag=0b0,
                 ApplicationProcessID=0b00000000000,
                 SequenceFlagsS=0b00,
                 PacketSequenceCount=0b00000000000000,
                 Code=0b0,
                 Data=0b0,
                 PacketErrorControl=0b0000000000000000):
        super(_TMerror, self).__init__(packetHeader,
                                       dataFieldHeader,
                                       PacketVersionNumberS,
                                       PacketTypeS,
                                       SecondaryHeaderFlag,
                                       ApplicationProcessID,
                                       SequenceFlagsS,
                                       PacketSequenceCount,
                                       Code,
                                       Data,
                                       PacketErrorControl
                                       )



##This structure represents the telecommand package for housekeeping
class _TCHR(ctypes.BigEndianStructure):
    _fields_ = [
        ("packetHeader", _packetHeader),

        # Packet Data Field

        ("dataFieldHeader", _dataFieldHeaderTC),

        ("SID", ctypes.c_uint32, 8),
        ("DataCollectionInterval", ctypes.c_uint32, 16),

        ("PacketErrorControl", ctypes.c_uint32, 16),
    ]

    def __init__(self,
                 packetHeader=_packetHeader(),
                 dataFieldHeader=_dataFieldHeaderTC(),
                 SID=0b00000001,
                 DataCollectionInterval=0b0000000000000001,
                 PacketErrorControl=0b0000000000000000):
        super(_TCHR, self).__init__(packetHeader,
                                    dataFieldHeader,
                                    SID,
                                    DataCollectionInterval,
                                    PacketErrorControl
                                    )



## This structure represents the telemetry package for housekeeping
class _TMHR(ctypes.BigEndianStructure):
    _fields_ = [
        ("packetHeader", _packetHeader),

        # Packet Data Field

        ("dataFieldHeader", _dataFieldHeaderTM),

        ("StructureID", ctypes.c_uint32, 16),
        ("ParamHR", ctypes.c_uint32, 32),

        ("PacketErrorControl", ctypes.c_uint32, 16),
    ]

    def __init__(self,
                 packetHeader=_packetHeader(),
                 dataFieldHeader=_dataFieldHeaderTM(),
                 StructureID=0b0000000000000000,
                 ParamHR=0b00000000000000000000000000000000,
                 PacketErrorControl=0b0000000000000000):
        super(_TMHR, self).__init__(packetHeader,
                                    dataFieldHeader,
                                    StructureID,
                                    ParamHR,
                                    PacketErrorControl
                                    )





##CTiM is a structure that represents the telecommand package for Time Management
class _TCTiM(ctypes.BigEndianStructure):
    _fields_ = [
        ("packetHeader", _packetHeader),

        ("CoarseTime", ctypes.c_uint32, 32),
        ("FineTime", ctypes.c_uint32, 16),

        ("PacketErrorControl", ctypes.c_uint32, 16),
    ]

    def __init__(self,
                 packetHeader=_packetHeader(),
                 CoarseTime=0b00000000000000000000000001100100,
                 FineTime=0b0000000000000000,
                 PacketErrorControl=0b0000000000000000):
        super(_TCTiM, self).__init__(packetHeader,
                                     CoarseTime,
                                     FineTime,
                                     PacketErrorControl
                                     )

##This structure represents the telecommand package for take picture
class _TC2221(ctypes.BigEndianStructure):
    _fields_ = [
        ("packetHeader", _packetHeader),

        # Packet Data Field

        ("dataFieldHeader", _dataFieldHeaderTC),

        ("Exposure", ctypes.c_uint32, 15),
        ("ExposureStep", ctypes.c_uint32, 1),
        ("ImageID", ctypes.c_uint32, 2),
        ("Reserved", ctypes.c_uint32, 6),
        ("PacketErrorControl", ctypes.c_uint32, 16)
    ]

    def __init__(self,
                 packetHeader=_packetHeader(),
                 dataFieldHeader=_dataFieldHeaderTC(),
                 Exposure=1,
                 ExposureStep=0,
                 ImageID=2,
                 Reserved=0,
                 PacketErrorControl=0
                 ):
        super(_TC2221, self).__init__(packetHeader,
                                    dataFieldHeader,
                                    Exposure,
                                    ExposureStep,
                                    ImageID,
                                    Reserved,
                                    PacketErrorControl
                                    )
        
##This structure represents the telecommand package for configure camera
class _TC2222(ctypes.BigEndianStructure):
    _fields_ = [
        ("packetHeader", _packetHeader),

        # Packet Data Field

        ("dataFieldHeader", _dataFieldHeaderTC),

        ("Offset", ctypes.c_uint32, 14),
        ("Reserved", ctypes.c_uint32, 2),
        ("ADC", ctypes.c_uint32, 8),
        ("PGA", ctypes.c_uint32, 3),
        ("Reserved_", ctypes.c_uint32, 5),
        ("PacketErrorControl", ctypes.c_uint32, 16)
    ]

    def __init__(self,
                 packetHeader=_packetHeader(),
                 dataFieldHeader=_dataFieldHeaderTC(),
                 Offset=16323,
                 Reserved=0,
                 ADC=63,
                 PGA=1,
                 Reserved_=0,
                 PacketErrorControl=0
                 ):
        super(_TC2222, self).__init__(packetHeader,
                                    dataFieldHeader,
                                    Offset,
                                    Reserved,
                                    ADC,
                                    PGA,
                                    Reserved_,
                                    PacketErrorControl
                                    )

##This structure represents the telecommand package for download picture
class _TC2223(ctypes.BigEndianStructure):
    _fields_ = [
        ("packetHeader", _packetHeader),

        # Packet Data Field

        ("dataFieldHeader", _dataFieldHeaderTC),

        ("ImageID", ctypes.c_uint32, 2),
        ("UnderSampling", ctypes.c_uint32, 1),
        ("Binning", ctypes.c_uint32, 1),
        ("Windowing", ctypes.c_uint32, 4),
        ("Reserved", ctypes.c_uint32, 8),
        ("PacketErrorControl", ctypes.c_uint32, 16)
    ]

    def __init__(self,
                 packetHeader=_packetHeader(),
                 dataFieldHeader=_dataFieldHeaderTC(),
                 ImageID=2,
                 UnderSampling=1,
                 Binning=1,
                 Windowing=10,
                 Reserved=0,
                 PacketErrorControl=0
                 ):
        super(_TC2223, self).__init__(packetHeader,
                                    dataFieldHeader,
                                    ImageID,
                                    UnderSampling,
                                    Binning,
                                    Windowing,
                                    Reserved,
                                    PacketErrorControl
                                    )


## This structure represents the telemetry package for housekeeping
class _ImageHeader(ctypes.BigEndianStructure):
    _fields_ = [
        ("Reserved", ctypes.c_uint32, 11),
        ("ImageNumber", ctypes.c_uint32, 2),
        ("PGA", ctypes.c_uint32, 3),
        ("ExposureStep", ctypes.c_uint32, 1),
        ("ExposureTime", ctypes.c_uint32, 15),
        ("Reserved_", ctypes.c_uint32, 10),
        ("Offset", ctypes.c_uint32, 14),
        ("ADC", ctypes.c_uint32, 8),
        ("Temperature", ctypes.c_uint32, 8),
        ("TimeTag", ctypes.c_uint32, 32),
        ("TotalPacketNumber", ctypes.c_uint32, 11),
        ("PacketNumber", ctypes.c_uint32, 11),
        ("ImageNumber_", ctypes.c_uint32, 2)
    ]

    def __init__(self,
                 Reserved=0,
                 ImageNumber=0,
                 PGA=0,
                 ExposureStep=0,
                 ExposureTime=0,
                 Reserved_=0,
                 Offset=0,
                 ADC=0,
                 Temperature=0,
                 TimeTag=0,
                 TotalPacketNumber=0,
                 PacketNumber=0,
                 ImageNumber_=0
    ):
        super(_ImageHeader, self).__init__(
                                    Reserved ,
                                    ImageNumber ,
                                    PGA ,
                                    ExposureStep ,
                                    ExposureTime ,
                                    Reserved_ ,
                                    Offset ,
                                    ADC ,
                                    Temperature ,
                                    TimeTag ,
                                    TotalPacketNumber ,
                                    PacketNumber ,
                                    ImageNumber_ 
                                    )

ctype64=ctypes.c_uint8*64
ctype64ctype64=ctype64*64

## This structure represents the telemetry package for recive img
class _TM22210(ctypes.BigEndianStructure):
    _fields_ = [
        ("packetHeader", _packetHeader),

        # Packet Data Field

        ("dataFieldHeader", _dataFieldHeaderTM),
        
        ("imageHeader", _ImageHeader),
        
        ("Pixels",ctypes.c_uint8),
        
        ("PacketErrorControl", ctypes.c_uint32, 16)
    ]

    def __init__(self,
                 packetHeader=_packetHeader(),
                 dataFieldHeader=_dataFieldHeaderTM(),
                 imageHeader=_ImageHeader(),
                 Pixels=0,
                 PacketErrorControl=0b0000000000000000):
        super(_TM22210, self).__init__(packetHeader,
                                    dataFieldHeader,
                                    imageHeader,
                                    Pixels,
                                    PacketErrorControl
                                    )
