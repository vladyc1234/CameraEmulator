## The function takes a byte and a 16-bit CRC value, and returns the updated CRC value   
# @param byte The byte to be added to the CRC.
# @param crc The current CRC value.
# @return The CRC value.
def crc16_ccitt_update(byte, crc):
    crc = ((crc << 8) | byte) ^ 0
    return crc & 0b1111111111111111

## It takes the CRC value and XORs it with 0xFFFF twice
# @param crc The current CRC value.
# @return The CRC value.
def crc16_ccitt_finalize(crc):
    for i in range(2):
        crc = crc16_ccitt_update(0, crc)
    return crc


def crc16_ccitt_init():
    return 0xFFFF

##The function takes a byte array and returns a 16-bit CRC value.  
# @param data The data to be used to calculate the CRC
# @param len length of the data
# @return The CRC value
def crc_calculate_table(data, len):
    b = data
    crc = crc16_ccitt_init()
    # Update the CRC using the data
    for i in range(0, len):
        crc = crc16_ccitt_update(b[i], crc)
    return crc

## It takes a string of bytes and returns a 16-bit CRC value
# @param data The data to be calculated.
# @param n the number of bytes in the data
# @return The CRC value
def crc16_ccitt(data, n):
    crc = crc_calculate_table(data, n)
    return crc
