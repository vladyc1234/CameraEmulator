import struct
import ctypes

"""

* Structure:   _packetID

* Description: This structure represents the Packet ID section in a telecomand/telemetry

"""


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


"""

* Structure:   _packetHeader

* Description: This structure represents the Packet Header section in a telecomand/telemetry

"""


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


"""

* Structure:   _dataFieldHeaderTC

* Description: This structure represents the Data Field Header section in a telecomand

"""


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
                 ServiceType=0b00010001,
                 ServiceSubType=0b00000001,
                 SourceID=0b00000000):
        super(_dataFieldHeaderTC, self).__init__(CCSDSsecondaryHeaderFlag,
                                                 PUSVersion,
                                                 ACK,
                                                 ServiceType,
                                                 ServiceSubType,
                                                 SourceID)


"""

* Structure:   _dataFieldHeaderTM

* Description: This structure represents the Data Field Header section in a telemetry

"""


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


"""

* Structure:   _TC

* Description: This structure represents the telecomand package 

"""


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


"""

* Structure:   _TMvalid

* Description: This structure represents the telemetry package for success

"""


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


"""

* Structure:   _TMerror

* Description: This structure represents the telemetry package for failure

"""


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
