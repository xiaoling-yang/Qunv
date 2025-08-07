#include "crc16.h"
#include <stdint.h>


// 反转8位
uint8_t reverse8(uint8_t data) {
    data = (data >> 1 & 0x55) | (data << 1 & 0xAA);
    data = (data >> 2 & 0x33) | (data << 2 & 0xCC);
    data = (data >> 4 & 0x0F) | (data << 4 & 0xF0);
    return data;
}

// 反转16位
uint16_t reverse16(uint16_t data) {
    return (uint16_t)(reverse8(data & 0xFF) << 8 | reverse8(data >> 8));
}

// CRC16-Modbus (输入输出反转)

    unsigned short crc16_modbus(unsigned char *data, int length) {
    unsigned short crc = 0xFFFF;
    for (int i = 0; i < length; i++) {
        crc ^= (unsigned short)data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;

   
}





