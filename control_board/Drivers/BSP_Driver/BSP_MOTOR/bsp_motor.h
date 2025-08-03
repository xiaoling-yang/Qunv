#ifndef BSP_MOTOR_H
#define BSP_MOTOR_H
#define  MOTOR_RUN 1
#define  MOTOR_STOP 0
#include "main.h"


void Motor_State(uint8_t state);//设置电机运行状态， 启动或停止 1启动 0停止
void Motor_SetSpeed(uint16_t speed);//设置电机转速,0 - 1000
void Motor_SetDirection(uint8_t direction);//设置电机运转方向 1 正 0反
void Motor_DataReset(void);//清除电机数据，关闭传感器




#endif
