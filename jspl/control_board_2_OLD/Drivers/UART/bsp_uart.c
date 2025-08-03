#include "bsp_uart.h"
#include <string.h>
#include "cmsis_os.h"

#include "485_to_motor.h"
#include "crc16.h"
#include "main.h"
#define RX_BUFFER_SIZE  64
extern uint16_t Target_Turns;
extern uint8_t receive_byte;
extern uint8_t rx_buffer1[];
extern uint8_t rx_buffer2[];
extern osSemaphoreId_t uartBinarySem03Handle;
uint8_t lcd_data_stat=0;
lcd_t lcd_data_management={0};
mt_bk_t motor_data_management={0};
uint8_t current_data_pos=0;
uint8_t motor_driver_state=0;//驱动器状态
uint8_t uartRxBuffer[UART_RX_BUF_SIZE];
uint8_t motor_stop_buf[7]={0x01,0x03,0x02,0x00,0x03,0xF8,0x45};//电机停机命令数据包
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
//    HAL_UART_Receive_IT(&huart1,&receive_byte,1);
//    if(huart==&huart1)//lcd屏幕发来的数据
//    {

//        switch(lcd_data_stat)
//        {
//            case no_byte_st1:if(receive_byte==0x5A)lcd_data_stat=head_st1;break;
//            case head_st1:if(receive_byte>0){
//                lcd_data_management.valid_data_len=receive_byte-3;
//                lcd_data_stat=data_len_st1;
//            }else{
//                lcd_data_stat=no_byte_st1;
//            }break;
//            case data_len_st1:if(current_data_pos<=lcd_data_management.valid_data_len+1)
//            {
//                lcd_data_management.valid_data_buf[current_data_pos-3]=receive_byte;

//            }else if (current_data_pos>lcd_data_management.valid_data_len+1){

//                //结束本轮接收
//                current_data_pos=0;
//                ProcessData(&huart1,lcd_data_management.valid_data_buf,lcd_data_management.valid_data_len);//处理数据
//                lcd_data_stat=no_byte_st1;
//            }break;
//            default:break;
//        }
//        current_data_pos++;
//        if(lcd_data_stat==no_byte_st1)
//        {
//            current_data_pos=0;
//            memset(&lcd_data_management,0,sizeof(lcd_data_management));
//        }
//    }
//}






void ProcessData(UART_HandleTypeDef *huart,uint8_t rcceive_data_buf[],uint8_t data_len) {
if (huart==&huart2) {
    uint16_t crc16_temp=0x0000;
    crc16_temp|=rcceive_data_buf[data_len-1]<<8;
    crc16_temp|=rcceive_data_buf[data_len-2];//低字节在前
    motor_data_management.crc16_check=crc16_temp;
    motor_data_management.station_number= rcceive_data_buf[0];
    motor_data_management.function_code=rcceive_data_buf[1];
    if (crc16_temp== crc16_modbus(rcceive_data_buf,data_len-2)) {
        if (data_len==8) {

            motor_data_management.address|=rcceive_data_buf[2]<<8;//高位在前
            motor_data_management.address|=rcceive_data_buf[3];
            motor_data_management.data[0]|=rcceive_data_buf[4]<<8;
            motor_data_management.data[1]|=rcceive_data_buf[5];
        }else if (data_len==7) {
            motor_data_management.data_len=rcceive_data_buf[2];
            motor_data_management.data[1]=rcceive_data_buf[3];
            motor_data_management.data[0]=rcceive_data_buf[4];
        }
        }
    }else if (huart==&huart1) {





        uint16_t speed=0x0000;
        uint32_t turns=0x00000000;
        uint8_t  dir=0x00;
        uint8_t  motor_state=0x00;
        speed|=rcceive_data_buf[0] ;
        speed|=rcceive_data_buf[1]<<8;

        turns|=rcceive_data_buf[2];
        turns|=rcceive_data_buf[3]<<8;
        turns|=rcceive_data_buf[4]<<16;
        turns|=rcceive_data_buf[5]<<24;

        dir=rcceive_data_buf[6];
        motor_state=rcceive_data_buf[7];

        if (motor_state) {
                motor_set_speed(speed);//设置电机转速,0 - 10000
					      //HAL_Delay(500);
								osDelay(500);//
                motor_start_dir(dir);//设置电机运转方向 1 正 0反
                Target_Turns=turns;//设定目标转速
        }else {//停止运行

            motor_stop();

        }








    }
}
void UART1_IdleCallback() {//lcd 屏的数据暂存在这个结构体里
		HAL_UART_DMAStop(&huart1);
    motor_data_management.total_data_len = rx_buffer1[1]-3;//得到有效数据包的长度
    memcpy(motor_data_management.motor_total_buf,&rx_buffer1[2],motor_data_management.total_data_len);//得到的是有效数据包
//		memset(&rx_buffer1,0,UART_RX_BUF_SIZE);
//    ProcessData(&huart1,motor_data_management.motor_total_buf,motor_data_management.total_data_len);
//    memset(&motor_data_management,0,sizeof(motor_data_management));
//    HAL_UART_Receive_DMA(&huart1,rx_buffer1,RX_BUFFER_SIZE);//启用DMA接收
		osSemaphoreRelease(uartBinarySem03Handle);


}

void UART2_IdleCallback() {//串口二电机的数据

	
	
		HAL_UART_DMAStop(&huart2);
    motor_data_management.total_data_len = UART_RX_BUF_SIZE- __HAL_DMA_GET_COUNTER(huart2.hdmarx);//得到已经接收的字节数
    memcpy(motor_data_management.motor_total_buf,rx_buffer2,motor_data_management.total_data_len);
		memset(&rx_buffer2,0,UART_RX_BUF_SIZE);
    ProcessData(&huart2,motor_data_management.motor_total_buf,motor_data_management.total_data_len);
    memset(&motor_data_management,0,UART_RX_BUF_SIZE);
    HAL_UART_Receive_DMA(&huart2,rx_buffer2,RX_BUFFER_SIZE);//启用DMA接收

}