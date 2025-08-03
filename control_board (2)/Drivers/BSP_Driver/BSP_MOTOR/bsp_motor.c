#include "bsp_motor.h"
#include "motor_sensor.h"
#include "tim.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"
extern volatile uint32_t Target_Turns;
extern uint32_t motor_cnt[4];
void Motor_State(uint8_t state)//设置电机运行状态， 启动或停止
{
if (state == 1) {
	
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_SET);//电平拉高，启动电机
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
		motor_sensor_init();//开启传感器
}else {

        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
				Motor_DataReset();//关闭传感器关闭电机
    }



}
void Motor_SetSpeed(uint16_t speed)//设置电机转速
{
    uint16_t speed_tem=speed;
    if (speed_tem>=1000 ) {
        speed_tem=1000;
    }
    __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, speed_tem-1);
}
void Motor_SetDirection(uint8_t direction)//设置电机运转方向
{
    if (direction == 1) {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET);
    }else {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET);
    }


}
void Motor_DataReset(void){//传感器关闭，数据清0
	
	
	char txt[8][64]={
						{"page2.n0.val="},
						{"page2.n1.val="},
						{"page2.n2.val="},
						{"page2.n3.val="},
						{"page2.n4.val="},
						{"page2.n5.val="},
						{"page2.n6.val="},
						{"page2.n7.val="}
					};
				uint8_t tail[3]={0xff,0xff,0xff};
				uint8_t txt_len[8]={0};  
				txt_len[0]=sprintf(txt[0],"%d",motor_cnt[0]);
				txt_len[1]=sprintf(txt[1],"%d",motor_cnt[1]);
				txt_len[2]=sprintf(txt[2],"%d",motor_cnt[2]);
				txt_len[3]=sprintf(txt[3],"%d",motor_cnt[3]);
				txt_len[4]=sprintf(txt[4],"%d",0x00);
				txt_len[5]=sprintf(txt[5],"%d",0x00);
				txt_len[6]=sprintf(txt[6],"%d",0x00);
				txt_len[7]=sprintf(txt[7],"%d",0x00);
				for(uint8_t i=0;i<8;i++)
				{
					//分两次发送一个数据包
					HAL_UART_Transmit(&huart1,(uint8_t*)txt[i],txt_len[i],100);
					HAL_UART_Transmit(&huart1,tail,3,100);
				}
				
			
	
	
	
	
	motor_sensor_sw(motor1,0);
	motor_sensor_sw(motor2,0);
	motor_sensor_sw(motor3,0);
	motor_sensor_sw(motor4,0);
	memset(motor_cnt,0,sizeof(motor_cnt));
	htim2.Instance->CNT=0;
	htim2.Instance->CCR1=0;
	Target_Turns=0;
	
}