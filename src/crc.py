# data=b'\x1a\xdc\xc0\x00\x00\x05\x11\x11\x01\x00\x00\x00'
print(oct)


def crc16_ccitt_update(byte, crc):
    crc = ((crc << 8) | byte) ^ 0
    return crc & 0b1111111111111111


def crc16_ccitt_finalize(crc):
    # Augment 16 zero-bits
    for i in range(2):
        crc = crc16_ccitt_update(0, crc)
    # print(" 0x%04hX\n" % crc)
    return crc


def crc16_ccitt_init():
    return 0xFFFF


def crc_calculate_table(data, len):
    b = data
    crc = crc16_ccitt_init()
    # Update the CRC using the data
    for i in range(0, len):
        crc = crc16_ccitt_update(b[i], crc)
        # print(" 0x%04hX\n" % crc)
    # crc = crc16_ccitt_finalize(crc)
    return crc


def crc16_ccitt(data, n):
    crc = crc_calculate_table(data, n)
    # print("Table-based CRC check: 0x%04hX\n" % crc)
    return crc
