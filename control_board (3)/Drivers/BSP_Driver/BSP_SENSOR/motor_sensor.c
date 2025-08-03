#include "motor_sensor.h"
#include "stdio.h"
#include "usart.h"
Sensor_t sensors[SENSOR_COUNT] = {
    {GPIOD, GPIO_PIN_8, 0},
    {GPIOD, GPIO_PIN_9, 0},
    {GPIOD, GPIO_PIN_10, 0},
    {GPIOD, GPIO_PIN_4, 0},
};
uint8_t motor_sensor_level(uint8_t motor) {
    uint8_t tem_level=0;
switch (motor) {
    case motor1: tem_level=HAL_GPIO_ReadPin(sensors[0].port, sensors[0].pin);break;
    case motor2: tem_level=HAL_GPIO_ReadPin(sensors[1].port, sensors[1].pin);break;
    case motor3: tem_level=HAL_GPIO_ReadPin(sensors[2].port, sensors[2].pin);break;
    case motor4: tem_level=HAL_GPIO_ReadPin(sensors[3].port, sensors[3].pin);break;
default:break;
}
return tem_level;

}
void motor_sensor_sw(uint8_t motor,uint8_t state) {


    switch (motor) {
        case motor1: if (state) {
            HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);

            }else {
                HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);
             }
        case motor2: if (state) {
                    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET);
            }else {
                    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET);
            }
        case motor3: if (state) {
                    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET);
            }else {
                    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET);
            }
        case motor4: if (state) {
                    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_RESET);
            }else {
                    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,GPIO_PIN_SET);
            }


    }

}
void motor_sensor_init(void){
	//将4个传感器打开
	motor_sensor_sw(motor1,1);
	motor_sensor_sw(motor2,1);
	motor_sensor_sw(motor3,1);
	motor_sensor_sw(motor4,1);
	
	
	
	char txt[4][64]={
						{"page2.n4.val="},
						{"page2.n5.val="},
						{"page2.n6.val="},
						{"page2.n7.val="}
					};
				uint8_t tail[3]={0xff,0xff,0xff};
				uint8_t txt_len[4]={0}; 
				txt_len[0]=sprintf(txt[0],"%d",0x01);
				txt_len[1]=sprintf(txt[1],"%d",0x01);
				txt_len[2]=sprintf(txt[2],"%d",0x01);
				txt_len[3]=sprintf(txt[3],"%d",0x01);	
		for(uint8_t i=0;i<4;i++)
				{
					//分两次发送一个数据包
					HAL_UART_Transmit(&huart1,(uint8_t*)txt[i],txt_len[i],100);
					HAL_UART_Transmit(&huart1,tail,3,100);
				}
				
}
