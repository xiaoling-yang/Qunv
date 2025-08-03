#ifndef BSP_USART_H
#define BSP_USART_H
#include <stdint.h>
#include "main.h"
#define head 0x5A //帧头
#define tail 0xA5 //帧尾

enum dataState {
    error_dataState,
    head_st,
    length_st,
    tail_st
};
struct Usart_Data_Manage {
    uint8_t valid_data_len;//有效数据长度
    uint8_t valid_data[100];//有效数据包




};


void USART1_Start_Receive(void);
void  UART_IdleCallback(void);
void ProcessData(void);//串口数据处理

#endif
