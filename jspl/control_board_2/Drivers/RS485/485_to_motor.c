#include "485_to_motor.h"
#include "FreeRTOS.h"
#include <string.h>

#include "bsp_uart.h"
#include "cmsis_os2.h"
#include "crc16.h"
uint8_t write_buf[8]={0};//写数据包
uint8_t read_buf[8]={0};//读数据包
uint16_t read_cmd=0x0000;
uint8_t write_cmd_buffer[64]={0x00};//写命令数据包
extern uint8_t motor_stop_buf[7];//电机停机命令数据包
extern mt_bk_t motor_data_management;
extern osSemaphoreId_t WriteCmdBinarySem01Handle;
extern osSemaphoreId_t ReadCmdBinarySem02Handle;
extern read_data_t current_motor_stat;



uint8_t WriteCmd(uint16_t addr,uint16_t cmd )//写数据命令
{
    uint16_t crc_temp=0;

    write_buf[0]=0x01;//站号
    write_buf[1]=0x06;//写命令
    write_buf[2]=addr>>8;
    write_buf[3]=addr&0x00FF;
    write_buf[4]=cmd>>8;
    write_buf[5]=cmd&0x00FF;
    crc_temp=crc16_modbus(write_buf,6);//得到校验值
    write_buf[6]=crc_temp&0x00FF;
    write_buf[7]=crc_temp>>8;


    for (uint8_t i=0;i<8;i++) {
        write_cmd_buffer[i]=write_buf[i];
    }
		data_dir(WRITE);//发送
		HAL_UART_Transmit(&huart2,write_cmd_buffer,8,100);
		data_dir(READ);//接收
    //osSemaphoreRelease(WriteCmdBinarySem01Handle);//释放写命令信号量
		
		

}
uint8_t ReadCmd(uint16_t addr,uint16_t cmd )//读数据命令
{
    uint16_t crc_temp=0;

    read_buf[0]=0x01;//站号
    read_buf[1]=0x03;//读命令
    read_buf[2]=addr>>8;
    read_buf[3]=addr&0x00FF;
    read_buf[4]=cmd>>8;
    read_buf[5]=cmd&0x00FF;
    crc_temp=crc16_modbus(read_buf,6);
    read_buf[6]=crc_temp&0x00FF;
    read_buf[7]=crc_temp>>8;
    
		data_dir(WRITE);//发送
		HAL_UART_Transmit(&huart2,read_buf,8,100);
		data_dir(READ);//接收
		
		memset(read_buf,0,sizeof(read_buf));
   



}

uint8_t motor_set_speed(uint16_t speed) {
    if (speed<=0) {

        speed=0;
    }
        WriteCmd(0x2001,speed);

}
void motor_start_dir(uint8_t direction)//电机启动方向 非0正向启动0反向启动
{
    if (direction==1) {
        
			WriteCmd(0x2000,0x0008);//反转
			
    }else {
        WriteCmd(0x2000,0x0007);//正转
    }


}
void motor_stop(void)//电机停止转动
{
    WriteCmd(0x2000,0x0006);//驱动停机

	
}
uint16_t motor_get_speed(void) {//得到电机速度
    read_cmd=0x1001;
  ReadCmd(read_cmd,0x0001 );//读数据命令


}

uint16_t motor_temperature(void)//得到电机温度
{
    read_cmd=0x1017;
    ReadCmd(read_cmd,0x0001 );//读数据命令



}
uint16_t motor_chip_temperature(void)//得到芯片温度
{


    read_cmd=0x1018;//
    ReadCmd(read_cmd,0x0001 );//读数据命令

}
void data_dir(uint8_t dir)//使能ttl转485进行读写切换
{
	if(dir==WRITE)
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
		
		
	}else if(dir==READ)
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
		
	}else{}

}	
void motor_emergency_stop(void)//电机急停
{
	
	
	
	
}
uint16_t motor_getSpeed(void)//获得电机当前转速
{
	
	ReadCmd(1001,0001);
	
	
}



