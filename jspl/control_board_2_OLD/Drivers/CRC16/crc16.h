#ifndef __CRC16_H
#define __CRC16_H
#include <stdint.h>

uint16_t CRC16_Modbus_IO_Reflected(const uint8_t *data, uint16_t length);

 unsigned short crc16_modbus(unsigned char *data, int length);
#endif
