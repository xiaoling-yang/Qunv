#ifndef __BSP_UART_H
#define __BSP_UART_H
#include "usart.h"
#define UART_RX_BUF_SIZE 64

#define HEAD  0x5A
#define TAIL  0xA5

#define STANUMB 0X01 //站号
#define FC_WRITE 0X06//写功能码
#define FC_READ  0X03//读功能码
extern uint8_t motor_stop_buf[7];//电机停机命令数据包


extern uint8_t uartRxBuffer[UART_RX_BUF_SIZE];

enum uart1_data_state {//串口1的数据状态
    no_byte_st1,//无数据
    head_st1,//帧头
    data_len_st1,//帧长度
    uart_data_st1,//帧数据
    tail_st1//帧尾
};
enum uart2_data_state {//串口2的数据状态
    no_byte_st2,//无数据,
    station_number_st2,
    function_code_st2,//读或者写
    address_st2,//读或写的地址
    data_at2,//写入的数据或者读取数据的长度
    CRC16_st2//crc校验
};
typedef struct LCD_DATA {
    uint8_t valid_data_len;
    uint8_t valid_data_buf[128];
} lcd_t;//屏幕下发的数据
typedef struct MOTOR_BACK_DATA {
    uint8_t motor_total_buf[32];//整个数据包
    uint8_t total_data_len;
    uint8_t station_number;
    uint8_t function_code;//读或者写
    uint16_t address;//地址
    uint8_t data_len;//读数据长度
    uint8_t data[10];//读或者写的数据
    uint16_t crc16_check;//CRC16校验

} mt_bk_t;//电机驱动返回数据


/*
串口回调函数
包含了串口1和串口二的数据处理接收
*/
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
/*
同过串口2给电机驱动发送命令
function_code 功能码 读或者写 1个字节
addr 读或者写的地址 2个字节
cmd 在对应地址写的命令 2个字节
返回值 为对方是否成功接收 1 成功 0失败
*/

void UART2_IdleCallback(void);//串口2空闲中断处理回调函数

void UART1_IdleCallback(void);//串口1空闲中断处理回调函数
void ProcessData(UART_HandleTypeDef *huart,uint8_t rcceive_data_buf[],uint8_t data_len);//数据处理包含串口1和串口2的

void update_lcd_ui(char *addr,uint16_t value);//发送数据给lcd屏幕更新，更新数据

#endif
