#ifndef __485_TO_MOTOR_H
#define __485_TO_MOTOR_H
#include "main.h"
#define WRITE  1
#define READ   0

typedef struct  motor_state {
    int16_t speed;
    int16_t dir;
    uint16_t motor_temperature;//电机温度
    uint16_t chip_temperature;//芯片温度


}mt_state_t;
extern uint8_t write_cmd_buffer[64];//写命令数据包

uint8_t WriteCmd(uint16_t addr,uint16_t cmd );//写数据命令
uint8_t ReadCmd(uint16_t addr,uint16_t cmd );//读数据命令

uint8_t motor_set_speed(uint16_t speed);

void motor_start_dir(uint8_t direction);//电机启动方向 1正向启动0反向启动
void motor_stop(void);//电机停止转动
uint16_t motor_get_speed(void);//得到电机速度
uint16_t motor_temperature(void);//得到电机温度
uint16_t motor_chip_temperature(void);//得到芯片温度
void data_dir(uint8_t dir);//1 是写数据 0是读数据 转换芯片需要使能所以需要控制该IO口




#endif
