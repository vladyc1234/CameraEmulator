# pip install pyserial

import serial
import packets
import time
import crc

TC = packets._TC()  # declare telecomand
TM1 = packets._TMvalid()  # declare telemetry

"""
* Function:    PUS17_transformIntoByteArrayTC

* Description: This function is used to transform the package into an array of bytes in order to be sent through the serial

* Params:      packageTC

* Return:      bytearray octets

"""
def PUS17_transformIntoByteArrayTC(packageTC):
    octets = bytearray([])
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

"""
* Function:    PUS1_initializeFromByteArrayTMvalid

* Description: This function is used to transform the input array into a package

* Params:      inputStream

* Return:      -

"""
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
    TM1.dataFieldHeader.CoarseTime = (inputStream[10] << 24) + (inputStream[11] << 16) + (inputStream[12] << 8) + inputStream[13]
    TM1.dataFieldHeader.FineTime = (inputStream[14] << 8) + inputStream[15]
    TM1.PacketVersionNumber = inputStream[16] >> 5
    TM1.PacketType = (inputStream[16] >> 2) & 0b111
    TM1.SecondaryHeaderFlag = (inputStream[16] >> 1) & 0b1
    TM1.ApplicationProcessID = ((inputStream[16] & 0b1) << 10) + (inputStream[17] << 2) + (inputStream[18] >> 6)
    TM1.SequenceFlags = (inputStream[18] >> 4) & 0b11
    TM1.PacketSequenceCount = ((inputStream[18] & 0b1111) << 10) + inputStream[19] << 2 + inputStream[20] & 0b11
    TM1.PacketErrorControl = (inputStream[21] << 8) + inputStream[22]

outputStream = PUS17_transformIntoByteArrayTC(TC)

ser = serial.Serial(port="COM4", baudrate=115200, bytesize=8, timeout=4,
                    stopbits=serial.STOPBITS_ONE)  # create serial connection

# Send the output Stream to the SECONDARY

send_receive = "SEND"
while True and send_receive != "DONE":
    if send_receive == "SEND":
        ser.write(outputStream)
        send_receive = "RECEIVE"

    time.sleep(1)

    inputStream = ser.read(30)
    print(inputStream)

    if send_receive == "RECEIVE":
        if len(inputStream) != 0:
            if inputStream[7] == 1:
                if inputStream[8] == 1:
                    PUS1_initializeFromByteArrayTMvalid(inputStream)
            send_receive = "DONE"

    print(send_receive)

ser.close()
