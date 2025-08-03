#ifndef BSP_USART_H
#define BSP_USART_H
#include <stdint.h>
#include "main.h"
#define head 0x5A //帧头
#define tail 0xA5 //帧尾
#define HALL_DMA_BUF_SIZE 5000

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

//typedef struct MOTOR_STRUCT_MANAGE {
//    uint8_t motor_dir;//当前电机方向
//    uint32_t motor_speed;//当前电机速度
//    uint32_t motor_total_turns;//电机当前总圈数
//}mot_manage_t ;



void USART1_Start_Receive(void);
void  UART_IdleCallback(void);
void ProcessData(uint8_t Data[]);//串口数据处理
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
#endif
