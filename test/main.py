import serial
import packets
import time
import crc

## declare telecommand
TC = packets._TC()

## declare telemetry valid
TM1 = packets._TMvalid()

## declare telemtry error
TM1E = packets._TMerror()

## declare telecommand for housekeeping
TCHR = packets._TCHR()

## declare telemetry for housekeeping
TMHR = packets._TMHR()

## declare telecommand for Time Management
TCTiM = packets._TCTiM()

## declare telecommand for Take picture
TC2221 = packets._TC2221()

## declare telecommand for configure camera
TC2222 = packets._TC2222()

## declare telecommand for downloading picture
TC2223 = packets._TC2223()

## declare telecommand for recive picture
TM22210 = packets._TM22210()

pictureLength = 64

if (TC2223.UnderSampling == 1):
    pictureLength = pictureLength / 2
if (TC2223.Binning == 1):
    pictureLength = pictureLength / 2
if (TC2223.Windowing > 0 and TC2223.Windowing < 10):
    pictureLength = pictureLength / 2
pictureLength = int(pictureLength)
Matrix22210 = [[0 for i in range(pictureLength)] for j in range(pictureLength)]


## It takes a TC2221 package and transforms it into a byte array
# @param packageTC2221 the package that I want to transform into a byte array
# @return a bytearray.
def PUS222_transformIntoByteArrayTC2221(packageTC2221):
    octets = bytearray([])
    packageTC2221.dataFieldHeader.ServiceType = 222
    packageTC2221.dataFieldHeader.ServiceSubtype = 1
    packageTC2221.packetHeader.PacketLength = 0b0000000000001000;
    octets.append((((((packageTC2221.packetHeader.packetID.VersionNumber << 1) +
                      packageTC2221.packetHeader.packetID.PacketType) << 1) +
                    packageTC2221.packetHeader.packetID.DataFieldHeaderFlag) << 3) +
                  (packageTC2221.packetHeader.PID >> 4)
                  )
    octets.append((((packageTC2221.packetHeader.PID & 0b1111) << 4) +
                   packageTC2221.packetHeader.PCAT)
                  )
    octets.append(((packageTC2221.packetHeader.SequenceFlags << 6) +
                   (packageTC2221.packetHeader.SourceSequenceCount >> 8))
                  )

    octets.append(packageTC2221.packetHeader.SourceSequenceCount & 0b11111111)

    octets.append(packageTC2221.packetHeader.PacketLength >> 8)
    octets.append(packageTC2221.packetHeader.PacketLength & 0b11111111)
    octets.append((((packageTC2221.dataFieldHeader.CCSDSsecondaryHeaderFlag << 3) +
                    packageTC2221.dataFieldHeader.PUSversion) << 4) +
                  packageTC2221.dataFieldHeader.ACK)

    octets.append(packageTC2221.dataFieldHeader.ServiceType)
    octets.append(packageTC2221.dataFieldHeader.ServiceSubtype)
    octets.append(packageTC2221.dataFieldHeader.SourceID)
    octets.append(packageTC2221.Exposure >> 7)
    octets.append(((packageTC2221.Exposure & 0b1111111) << 1) + packageTC2221.ExposureStep)
    octets.append((packageTC2221.ImageID << 6) + packageTC2221.Reserved)

    PacketErrorControl = crc.crc16_ccitt(octets, len(octets))

    packageTC2221.PacketErrorControl = PacketErrorControl

    octets.append(PacketErrorControl >> 8)
    octets.append(PacketErrorControl & 0b11111111)

    return octets


## It takes a TC2222 package and transforms it into a byte array
# @param packageTC2223 the package that I want to transform into a byte array
# @return a bytearray.
def PUS222_transformIntoByteArrayTC2222(packageTC2222):
    octets = bytearray([])
    packageTC2222.dataFieldHeader.ServiceType = 222
    packageTC2222.dataFieldHeader.ServiceSubtype = 2
    packageTC2222.packetHeader.PacketLength = 0b0000000000001001;
    octets.append((((((packageTC2222.packetHeader.packetID.VersionNumber << 1) +
                      packageTC2222.packetHeader.packetID.PacketType) << 1) +
                    packageTC2222.packetHeader.packetID.DataFieldHeaderFlag) << 3) +
                  (packageTC2222.packetHeader.PID >> 4)
                  )
    octets.append((((packageTC2222.packetHeader.PID & 0b1111) << 4) +
                   packageTC2222.packetHeader.PCAT)
                  )
    octets.append(((packageTC2222.packetHeader.SequenceFlags << 6) +
                   (packageTC2222.packetHeader.SourceSequenceCount >> 8))
                  )

    octets.append(packageTC2222.packetHeader.SourceSequenceCount & 0b11111111)

    octets.append(packageTC2222.packetHeader.PacketLength >> 8)
    octets.append(packageTC2222.packetHeader.PacketLength & 0b11111111)
    octets.append((((packageTC2222.dataFieldHeader.CCSDSsecondaryHeaderFlag << 3) +
                    packageTC2222.dataFieldHeader.PUSversion) << 4) +
                  packageTC2222.dataFieldHeader.ACK)

    octets.append(packageTC2222.dataFieldHeader.ServiceType)
    octets.append(packageTC2222.dataFieldHeader.ServiceSubtype)
    octets.append(packageTC2222.dataFieldHeader.SourceID)
    octets.append((packageTC2222.Offset >> 6) & 0b11111111)
    octets.append(((packageTC2222.Offset & 0b111111) << 2) + packageTC2222.Reserved)
    octets.append(packageTC2222.ADC)
    octets.append((packageTC2222.PGA << 5) + packageTC2222.Reserved_)

    PacketErrorControl = crc.crc16_ccitt(octets, len(octets))

    packageTC2222.PacketErrorControl = PacketErrorControl

    octets.append(PacketErrorControl >> 8)
    octets.append(PacketErrorControl & 0b11111111)

    return octets


## It takes a TC2223 package and transforms it into a byte array
# @param packageTC2223 the package that I want to transform into a byte array
# @return a bytearray.
def PUS222_transformIntoByteArrayTC2223(packageTC2223):
    octets = bytearray([])
    packageTC2223.dataFieldHeader.ServiceType = 222
    packageTC2223.dataFieldHeader.ServiceSubtype = 3
    packageTC2223.packetHeader.PacketLength = 0b0000000000000111;
    octets.append((((((packageTC2223.packetHeader.packetID.VersionNumber << 1) +
                      packageTC2223.packetHeader.packetID.PacketType) << 1) +
                    packageTC2223.packetHeader.packetID.DataFieldHeaderFlag) << 3) +
                  (packageTC2223.packetHeader.PID >> 4)
                  )
    octets.append((((packageTC2223.packetHeader.PID & 0b1111) << 4) +
                   packageTC2223.packetHeader.PCAT)
                  )
    octets.append(((packageTC2223.packetHeader.SequenceFlags << 6) +
                   (packageTC2223.packetHeader.SourceSequenceCount >> 8))
                  )

    octets.append(packageTC2223.packetHeader.SourceSequenceCount & 0b11111111)

    octets.append(packageTC2223.packetHeader.PacketLength >> 8)
    octets.append(packageTC2223.packetHeader.PacketLength & 0b11111111)
    octets.append((((packageTC2223.dataFieldHeader.CCSDSsecondaryHeaderFlag << 3) +
                    packageTC2223.dataFieldHeader.PUSversion) << 4) +
                  packageTC2223.dataFieldHeader.ACK)

    octets.append(packageTC2223.dataFieldHeader.ServiceType)
    octets.append(packageTC2223.dataFieldHeader.ServiceSubtype)
    octets.append(packageTC2223.dataFieldHeader.SourceID)
    octets.append(((((
                             packageTC2223.ImageID << 1) + packageTC2223.UnderSampling) << 1) + packageTC2223.Binning << 4) + packageTC2223.Windowing)
    octets.append(packageTC2223.Reserved)

    PacketErrorControl = crc.crc16_ccitt(octets, len(octets))

    packageTC2223.PacketErrorControl = PacketErrorControl

    octets.append(PacketErrorControl >> 8)
    octets.append(PacketErrorControl & 0b11111111)

    return octets


## This function is used to transform the input array into a telecommand package for recive picture
#  @param inputStream a byte array of the TM packet
def PUS222_initializeFromByteArrayTM22210(inputStream):
    TM22210.packetHeader.packetID.VersionNumber = inputStream[0] >> 5
    TM22210.packetHeader.packetID.PacketType = (inputStream[0] >> 4) & 0b1
    TM22210.packetHeader.packetID.DataFieldHeaderFlag = (inputStream[0] >> 3) & 0b1
    TM22210.packetHeader.PID = ((inputStream[0] & 0b111) << 4) + (inputStream[1] >> 4)
    TM22210.packetHeader.PCAT = inputStream[1] & 0b1111
    TM22210.packetHeader.SequenceFlags = inputStream[1] >> 6
    TM22210.packetHeader.SourceSequenceCount = ((inputStream[2] & 0b111111) << 8) + inputStream[3]
    TM22210.packetHeader.PacketLength = (inputStream[4] << 8) + inputStream[5]
    TM22210.dataFieldHeader.Spare1 = inputStream[6] >> 7
    TM22210.dataFieldHeader.PUSversion = inputStream[6] >> 4 & 0b111
    TM22210.dataFieldHeader.Spare2 = inputStream[6] & 0b1111
    TM22210.dataFieldHeader.ServiceType = inputStream[7]
    TM22210.dataFieldHeader.ServiceSubtype = inputStream[8]
    TM22210.dataFieldHeader.DestinationID = inputStream[9]
    TM22210.dataFieldHeader.CoarseTime = (inputStream[10] << 24) + (inputStream[11] << 16) + (inputStream[12] << 8) + \
                                         inputStream[13]
    TM22210.dataFieldHeader.FineTime = (inputStream[14] << 8) + inputStream[15]
    TM22210.imageHeader.Reserved = (inputStream[16] << 3) + inputStream[17] >> 5
    TM22210.imageHeader.ImageNumber = (inputStream[17] >> 3) & 0b11
    TM22210.imageHeader.PGA = inputStream[17] & 0b111
    TM22210.imageHeader.ExposureStep = inputStream[18] >> 7
    TM22210.imageHeader.ExposureTime = ((inputStream[18] & 0b1111111) << 8) + inputStream[19]
    TM22210.imageHeader.Reserved_ = (inputStream[20] << 2) + inputStream[21] >> 6
    TM22210.imageHeader.Offset = ((inputStream[21] & 0b111111) << 8) + inputStream[22]
    TM22210.imageHeader.ADC = inputStream[23]
    TM22210.imageHeader.Temperature = inputStream[24]
    TM22210.imageHeader.TimeTag = (inputStream[25] << 24) + (inputStream[26] << 16) + (inputStream[27] << 8) + \
                                  inputStream[28]
    TM22210.imageHeader.TotalPacketNumber = (inputStream[29] << 3) + (inputStream[30] >> 5)
    TM22210.imageHeader.PacketNumber = ((inputStream[30] & 0b11111) << 6) + (inputStream[31] >> 2)
    TM22210.imageHeader.ImageNumber_ = inputStream[31] & 0b11

    k = 32

    for i in range(pictureLength):
        for j in range(pictureLength):
            Matrix22210[i][j] = inputStream[k]
            k = k + 1

    TM22210.PacketErrorControl = (inputStream[k] << 8) + inputStream[k + 1]


##This function is used to transform the package into an array of bytes in order to be sent through the serial
# @param packageTC is the TC packet
# @return a bytearray.
def PUS17_transformIntoByteArrayTC(packageTC):
    octets = bytearray([])
    packageTC.dataFieldHeader.ServiceType = 17
    packageTC.dataFieldHeader.ServiceSubtype = 1
    octets.append((((((packageTC.packetHeader.packetID.VersionNumber << 1) +
                      packageTC.packetHeader.packetID.PacketType) << 1) +
                    packageTC.packetHeader.packetID.DataFieldHeaderFlag) << 3) +
                  (packageTC.packetHeader.PID >> 4)
                  )
    octets.append((((packageTC.packetHeader.PID & 0b1111) << 4) +
                   packageTC.packetHeader.PCAT)
                  )
    octets.append(((packageTC.packetHeader.SequenceFlags << 6) +
                   (packageTC.packetHeader.SourceSequenceCount >> 8))
                  )

    octets.append(packageTC.packetHeader.SourceSequenceCount & 0b11111111)

    octets.append(packageTC.packetHeader.PacketLength >> 8)
    octets.append(packageTC.packetHeader.PacketLength & 0b11111111)
    octets.append((((packageTC.dataFieldHeader.CCSDSsecondaryHeaderFlag << 3) +
                    packageTC.dataFieldHeader.PUSversion) << 4) +
                  packageTC.dataFieldHeader.ACK)

    octets.append(packageTC.dataFieldHeader.ServiceType)
    octets.append(packageTC.dataFieldHeader.ServiceSubtype)
    octets.append(packageTC.dataFieldHeader.SourceID)

    PacketErrorControl = crc.crc16_ccitt(octets, len(octets))

    TC.PacketErrorControl = PacketErrorControl

    octets.append(PacketErrorControl >> 8)
    octets.append(PacketErrorControl & 0b11111111)

    return octets


## This function is used to transform the input array into a package
#  @param inputStream the byte array that is the packet
def PUS1_initializeFromByteArrayTMvalid(inputStream):
    TM1.packetHeader.packetID.VersionNumber = inputStream[0] >> 5
    TM1.packetHeader.packetID.PacketType = (inputStream[0] >> 4) & 0b1
    TM1.packetHeader.packetID.DataFieldHeaderFlag = (inputStream[0] >> 3) & 0b1
    TM1.packetHeader.PID = ((inputStream[0] & 0b111) << 4) + (inputStream[1] >> 4)
    TM1.packetHeader.PCAT = inputStream[1] & 0b1111
    TM1.packetHeader.SequenceFlags = inputStream[1] >> 6
    TM1.packetHeader.SourceSequenceCount = ((inputStream[2] & 0b111111) << 8) + inputStream[3]
    TM1.packetHeader.PacketLength = (inputStream[4] << 8) + inputStream[5]
    TM1.dataFieldHeader.Spare1 = inputStream[6] >> 7
    TM1.dataFieldHeader.PUSversion = inputStream[6] >> 4 & 0b111
    TM1.dataFieldHeader.Spare2 = inputStream[6] & 0b1111
    TM1.dataFieldHeader.ServiceType = inputStream[7]
    TM1.dataFieldHeader.ServiceSubtype = inputStream[8]
    TM1.dataFieldHeader.DestinationID = inputStream[9]
    TM1.dataFieldHeader.CoarseTime = (inputStream[10] << 24) + (inputStream[11] << 16) + (inputStream[12] << 8) + \
                                     inputStream[13]
    TM1.dataFieldHeader.FineTime = (inputStream[14] << 8) + inputStream[15]
    TM1.PacketVersionNumber = inputStream[16] >> 5
    TM1.PacketType = (inputStream[16] >> 2) & 0b111
    TM1.SecondaryHeaderFlag = (inputStream[16] >> 1) & 0b1
    TM1.ApplicationProcessID = ((inputStream[16] & 0b1) << 10) + (inputStream[17] << 2) + (inputStream[18] >> 6)
    TM1.SequenceFlags = (inputStream[18] >> 4) & 0b11
    TM1.PacketSequenceCount = ((inputStream[18] & 0b1111) << 10) + inputStream[19] << 2 + inputStream[20] & 0b11
    TM1.PacketErrorControl = (inputStream[21] << 8) + inputStream[22]

## This function is used to transform the input array into a package
#  @param inputStream the byte array that is the packet
def PUS1_initializeFromByteArrayTMerror(inputStream):
    TM1E.packetHeader.packetID.VersionNumber = inputStream[0] >> 5
    TM1E.packetHeader.packetID.PacketType = (inputStream[0] >> 4) & 0b1
    TM1E.packetHeader.packetID.DataFieldHeaderFlag = (inputStream[0] >> 3) & 0b1
    TM1E.packetHeader.PID = ((inputStream[0] & 0b111) << 4) + (inputStream[1] >> 4)
    TM1E.packetHeader.PCAT = inputStream[1] & 0b1111
    TM1E.packetHeader.SequenceFlags = inputStream[1] >> 6
    TM1E.packetHeader.SourceSequenceCount = ((inputStream[2] & 0b111111) << 8) + inputStream[3]
    TM1E.packetHeader.PacketLength = (inputStream[4] << 8) + inputStream[5]
    TM1E.dataFieldHeader.Spare1 = inputStream[6] >> 7
    TM1E.dataFieldHeader.PUSversion = inputStream[6] >> 4 & 0b111
    TM1E.dataFieldHeader.Spare2 = inputStream[6] & 0b1111
    TM1E.dataFieldHeader.ServiceType = inputStream[7]
    TM1E.dataFieldHeader.ServiceSubtype = inputStream[8]
    TM1E.dataFieldHeader.DestinationID = inputStream[9]
    TM1E.dataFieldHeader.CoarseTime = (inputStream[10] << 24) + (inputStream[11] << 16) + (inputStream[12] << 8) + \
                                     inputStream[13]
    TM1E.dataFieldHeader.FineTime = (inputStream[14] << 8) + inputStream[15]
    TM1E.PacketVersionNumber = inputStream[16] >> 5
    TM1E.PacketType = (inputStream[16] >> 2) & 0b111
    TM1E.SecondaryHeaderFlag = (inputStream[16] >> 1) & 0b1
    TM1E.ApplicationProcessID = ((inputStream[16] & 0b1) << 10) + (inputStream[17] << 2) + (inputStream[18] >> 6)
    TM1E.SequenceFlags = (inputStream[18] >> 4) & 0b11
    TM1E.PacketSequenceCount = ((inputStream[18] & 0b1111) << 10) + inputStream[19] << 2 + inputStream[20] & 0b11
    TM1E.Code=(inputStream[21]>>4) & 0b11
    TM1E.Date=inputStream[21] & 0b1111
    TM1E.PacketErrorControl = (inputStream[22] << 8) + inputStream[23]

## This function is used to transform the package into an array of bytes in order to be sent through the serial
# @param packageTCHR the package that I want to transform into a byte array
# @return a bytearray.
def PUS3_transformIntoByteArrayTCHR(packageTCHR):
    octets = bytearray([])
    packageTCHR.packetHeader.PacketLength = 0b0000000000001000;
    octets.append((((((packageTCHR.packetHeader.packetID.VersionNumber << 1) +
                      packageTCHR.packetHeader.packetID.PacketType) << 1) +
                    packageTCHR.packetHeader.packetID.DataFieldHeaderFlag) << 3) +
                  (packageTCHR.packetHeader.PID >> 4)
                  )
    octets.append((((packageTCHR.packetHeader.PID & 0b1111) << 4) +
                   packageTCHR.packetHeader.PCAT)
                  )
    octets.append(((packageTCHR.packetHeader.SequenceFlags << 6) +
                   (packageTCHR.packetHeader.SourceSequenceCount >> 8))
                  )

    octets.append(packageTCHR.packetHeader.SourceSequenceCount & 0b11111111)

    octets.append(packageTCHR.packetHeader.PacketLength >> 8)
    octets.append(packageTCHR.packetHeader.PacketLength & 0b11111111)
    octets.append((((packageTCHR.dataFieldHeader.CCSDSsecondaryHeaderFlag << 3) +
                    packageTCHR.dataFieldHeader.PUSversion) << 4) +
                  packageTCHR.dataFieldHeader.ACK)

    octets.append(packageTCHR.dataFieldHeader.ServiceType)
    octets.append(packageTCHR.dataFieldHeader.ServiceSubtype)
    octets.append(packageTCHR.dataFieldHeader.SourceID)
    octets.append(packageTCHR.SID)
    octets.append(packageTCHR.DataCollectionInterval >> 8)
    octets.append(packageTCHR.DataCollectionInterval & 0b11111111)

    PacketErrorControl = crc.crc16_ccitt(octets, len(octets))

    packageTCHR.PacketErrorControl = PacketErrorControl

    octets.append(PacketErrorControl >> 8)
    octets.append(PacketErrorControl & 0b11111111)

    return octets


## This function is used to transform the input array into a telecommand package for housekeeping
#  @param inputStream a byte array of the TM packet
def PUS3_initializeFromByteArrayTMHR(inputStream):
    TMHR.packetHeader.packetID.VersionNumber = inputStream[0] >> 5
    TMHR.packetHeader.packetID.PacketType = (inputStream[0] >> 4) & 0b1
    TMHR.packetHeader.packetID.DataFieldHeaderFlag = (inputStream[0] >> 3) & 0b1
    TMHR.packetHeader.PID = ((inputStream[0] & 0b111) << 4) + (inputStream[1] >> 4)
    TMHR.packetHeader.PCAT = inputStream[1] & 0b1111
    TMHR.packetHeader.SequenceFlags = inputStream[1] >> 6
    TMHR.packetHeader.SourceSequenceCount = ((inputStream[2] & 0b111111) << 8) + inputStream[3]
    TMHR.packetHeader.PacketLength = (inputStream[4] << 8) + inputStream[5]
    TMHR.dataFieldHeader.Spare1 = inputStream[6] >> 7
    TMHR.dataFieldHeader.PUSversion = inputStream[6] >> 4 & 0b111
    TMHR.dataFieldHeader.Spare2 = inputStream[6] & 0b1111
    TMHR.dataFieldHeader.ServiceType = inputStream[7]
    TMHR.dataFieldHeader.ServiceSubtype = inputStream[8]
    TMHR.dataFieldHeader.DestinationID = inputStream[9]
    TMHR.dataFieldHeader.CoarseTime = (inputStream[10] << 24) + (inputStream[11] << 16) + (inputStream[12] << 8) + \
                                      inputStream[13]
    TMHR.dataFieldHeader.FineTime = (inputStream[14] << 8) + inputStream[15]
    TMHR.StructureID = (inputStream[16] << 8) + inputStream[17]
    TMHR.ParamHR = (inputStream[18] << 24) + (inputStream[19] << 16) + (inputStream[20] << 8) + inputStream[21]
    TMHR.PacketErrorControl = (inputStream[22] << 8) + inputStream[23]


## This function is used to transform the package into an array of bytes in order to be sent through the serial
# @param packageTCTiM a class that contains the following parameters:
# @return A bytearray of the TCTiM packet.
def PUS9_transformIntoByteArrayTCTiM(packageTCTiM):
    octets = bytearray([])
    packageTCTiM.packetHeader.PacketLength = 0b0000000000000111
    packageTCTiM.packetHeader.PID = 0b0000000
    packageTCTiM.packetHeader.PCAT = 0
    octets.append((((((packageTCTiM.packetHeader.packetID.VersionNumber << 1) +
                      packageTCTiM.packetHeader.packetID.PacketType) << 1) +
                    packageTCTiM.packetHeader.packetID.DataFieldHeaderFlag) << 3) +
                  (packageTCTiM.packetHeader.PID >> 4)
                  )
    octets.append((((packageTCTiM.packetHeader.PID & 0b1111) << 4) +
                   packageTCTiM.packetHeader.PCAT)
                  )
    octets.append(((packageTCTiM.packetHeader.SequenceFlags << 6) +
                   (packageTCTiM.packetHeader.SourceSequenceCount >> 8))
                  )

    octets.append(packageTCTiM.packetHeader.SourceSequenceCount & 0b11111111)

    octets.append(packageTCTiM.packetHeader.PacketLength >> 8)
    octets.append(packageTCTiM.packetHeader.PacketLength & 0b11111111)

    octets.append((packageTCTiM.CoarseTime << 24) & 0b11111111)
    octets.append((packageTCTiM.CoarseTime << 16) & 0b11111111)
    octets.append((packageTCTiM.CoarseTime << 8) & 0b11111111)
    octets.append(packageTCTiM.CoarseTime & 0b11111111)
    octets.append((packageTCTiM.FineTime << 8) & 0b11111111)
    octets.append(packageTCTiM.FineTime & 0b11111111)

    PacketErrorControl = crc.crc16_ccitt(octets, len(octets))

    TC.PacketErrorControl = PacketErrorControl

    octets.append(PacketErrorControl >> 8)
    octets.append(PacketErrorControl & 0b11111111)

    return octets


## Creating the packets that will be sent to the secondary.
outputStreamTC3130 = PUS3_transformIntoByteArrayTCHR(TCHR)
## Creating the packets that will be sent to the secondary.
outputStreamTC171 = PUS17_transformIntoByteArrayTC(TC)
## Creating the packets that will be sent to the secondary.
outputStreamTC9132 = PUS9_transformIntoByteArrayTCTiM(TCTiM)
## Creating the packets that will be sent to the secondary.
outputStreamTC2221 = PUS222_transformIntoByteArrayTC2221(TC2221)
## Creating the packets that will be sent to the secondary.
outputStreamTC2222 = PUS222_transformIntoByteArrayTC2222(TC2222)
## Creating the packets that will be sent to the secondary.
outputStreamTC2223 = PUS222_transformIntoByteArrayTC2223(TC2223)
## Creating a serial connection to the port COM4 with a baudrate of 115200, bytesize of 8, timeout of 4
# and stopbits of 1.
ser = serial.Serial(port="COM4", baudrate=115200, bytesize=8, timeout=4,
                    stopbits=serial.STOPBITS_ONE)  # create serial connection

## Send the output Stream to the SECONDARY
sendArray = [outputStreamTC2221, outputStreamTC2223,outputStreamTC9132, outputStreamTC171, outputStreamTC3130  ]
## A flag that is used to know if the program is sending or receiving.
send_receive = "SEND"
## The length of the packet that is received.
totalLength = 0
packet = bytearray([])
i = 1
isPacket = 0
## The above code is receiving the packet from the serial and then checking the packet length
# and then checking the packet type and then printing the packet type.
# def main():
ok = 0
while ok != 1:
    if send_receive == "SEND":
        ser.write(sendArray[0])
        print(f"Package {5 - len(sendArray)} sent: ")
        sendArray.pop(0)
        send_receive = "RECEIVE"

    if (isPacket):
        packet = bytearray([])
        isPacket = 0

    inputStream = ser.read(1)

    if len(sendArray) == 0 and len(inputStream) == 0:
        ok = 1

    if (len(inputStream) != 0):
        packet.append(inputStream[0])
        # print(packet)
    else:
        send_receive = "SEND"

    ##The above code is receiving the packet from the serial and then checking the packet length
    # and then checking the packet type and then printing the packet type.
    if send_receive == "RECEIVE":
        if len(packet) == 6:
            totalLength = (packet[4] << 8) + (packet[5] & 0b11111111) + 7

        if len(packet) == totalLength and (len(packet) != 0):
            # print(totalLength)
            if packet[7] == 1:
                if packet[8] == 1:
                    PUS1_initializeFromByteArrayTMvalid(packet)
                    print("TM[1,1]")
                    
                    isPacket = 1
            if packet[7] == 3:
                if packet[8] == 25:
                    PUS3_initializeFromByteArrayTMHR(packet)
                    print("TM[3,25]")
                    isPacket = 1
            if packet[7] == 1:
                if packet[8] == 7:
                    PUS1_initializeFromByteArrayTMvalid(packet)
                    print("TM[1,7]")
                    
                    isPacket = 1
            if packet[7] == 1:
                if packet[8] == 2:
                    PUS1_initializeFromByteArrayTMerror(packet)
                    print("TM[1,2]")
                    isPacket = 1
            if packet[7] == 1:
                if packet[8] == 8:
                    PUS1_initializeFromByteArrayTMerror(packet)
                    print("TM[1,8]")
                    isPacket = 1
            if packet[7] == 222:
                if packet[8] == 10:
                    PUS222_initializeFromByteArrayTM22210(packet)
                    print("TM[222,10]")
                    for i in Matrix22210:
                        print(i)
                    isPacket = 1
            time.sleep(1)
# main()

ser.close()
