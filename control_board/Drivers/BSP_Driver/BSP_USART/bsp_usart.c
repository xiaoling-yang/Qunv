#include "bsp_usart.h"
#include "main.h"
#include "string.h"
#include "stdio.h"
#include "bsp_motor.h"
#include "tim.h"
#include "dma.h"


#define UART_RX_BUF_SIZE  512  //缓冲区大小
#define UART_FRAME_BUF_SIZE   512   // 单帧最大长度
#define UART_RINGBUF_SIZE     20   // 最多缓存10帧
extern uint32_t hall_dma_buffer[];
uint8_t uartRxBuffer[UART_RX_BUF_SIZE];  // DMA接收缓冲区
uint8_t uartRingBuffer[UART_RINGBUF_SIZE][UART_FRAME_BUF_SIZE];  // 环形缓冲
extern DMA_HandleTypeDef hdma_tim2_up_ch3;
uint32_t Target_Turns=0;
uint16_t curr_pos=0;
uint16_t old_pos=0;
uint8_t uartRingHead = 0, uartRingTail = 0;
//int8_t dataState=error_dataState;//数据状态
uint32_t hall_dma_buffer[HALL_DMA_BUF_SIZE];
static uint16_t last_index = 0;
static uint16_t last_capture = 0;
uint32_t motor_cnt[4]={0};
volatile uint8_t Sensorflag1;
volatile uint8_t Sensorflag2;
volatile uint8_t Sensorflag3;
volatile uint8_t Sensorflag4;
extern UART_HandleTypeDef huart1;
extern uint8_t receive_byte;
uint8_t data_len=0;
uint8_t rcceive_data_buf[64]={0};
uint8_t data_stat=0;
uint8_t current_data_pos=0;
//struct Usart_Data_Manage Data_Manage_Unity={
//    .valid_data_len = 0,
//    .valid_data ={0}
//};
typedef struct MOTOR_STRUCT_MANAGE {
    uint8_t motor_dir;//当前电机方向
    uint32_t motor_speed;//当前电机速度
    uint32_t motor_total_turns;//电机当前总圈数
}mot_manage_t ;
mot_manage_t motor_data={0};

void USART1_Start_Receive(void) {


HAL_UART_Receive_DMA(&huart1,uartRxBuffer,UART_RX_BUF_SIZE);//启用DMA接收

__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);//启用空闲中断接收
}
//void  UART_IdleCallback(void) {
//    uint16_t data_len;
//    uint8_t data_byte=0;
//    curr_pos = UART_RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(huart1.hdmarx);//数据总长读




//    if (curr_pos>=old_pos) {
//        data_len=curr_pos-old_pos;




//    }else  {
//        data_len=UART_RX_BUF_SIZE-old_pos+curr_pos;


//    }
//    if (data_len > 0 && data_len <= UART_FRAME_BUF_SIZE) {// 将数据拷贝到环形缓冲区

//        uint8_t *frame_buf = uartRingBuffer[uartRingHead];
//        for (uint16_t i = 0; i < data_len; ++i) {
//            frame_buf[i] = uartRxBuffer[(old_pos + i) % UART_RX_BUF_SIZE];
//        }

//        // 更新头指针
//        uartRingHead = (uartRingHead + 1) % UART_RINGBUF_SIZE;
//				old_pos = curr_pos;
//        // 可选：通知任务处理（事件标志 / 消息队列等）
//    }
//    uint16_t i=0;
//		uint8_t temp=0;
//    while (1) {
//        uint8_t *farm;
//        if (uartRingTail==uartRingHead) {
//            return;
//        }

//        farm=uartRingBuffer[uartRingTail];
//        data_byte=farm[i];
//        switch (dataState) {
//            case error_dataState: if (data_byte == head) {
//                dataState=head_st;
//							temp=i;
//                 };break;
//            case head_st:if (data_byte>0) {
//                dataState=length_st;
//                Data_Manage_Unity.valid_data_len=data_byte-3;//总长度要减去头尾和长度的字节
//                break;
//            }
//            case length_st:
//                if (Data_Manage_Unity.valid_data_len+2!=i && data_byte!=tail ) {
//                    Data_Manage_Unity.valid_data[i-2]=data_byte;
//                }else if (Data_Manage_Unity.valid_data_len+2+temp==i && data_byte==tail) {
//                    dataState=tail_st;
//                }break;
//            case tail_st:dataState=error_dataState;//恢复初始状态
//                ProcessData();//数据接收完毕，进行数据处理
//								memset(&Data_Manage_Unity,0,sizeof(Data_Manage_Unity));//数据清0
//								
//                 uartRingTail = (uartRingTail + 1) % UART_RINGBUF_SIZE;
//                return;
//                        default: break;//理论走不到这里
//                }
//				
//            i++;
//    }
//}
void ProcessData(uint8_t Data[]) {//数据长度固定
	
	uint16_t speed=0x0000;
	uint32_t turns=0x00000000;
	uint8_t  dir=0x00;
	uint8_t  motor_state=0x00;
	uint8_t index=0;
	
	
	speed|=Data[index++];
	speed|=Data[index++]<<8;
				
	turns|=Data[index++];
	turns|=Data[index++]<<8;
	turns|=Data[index++]<<16;
	turns|=Data[index++]<<24;
				 
	dir=Data[index++];
	motor_state=Data[index++];
	
	
	
	Motor_State(motor_state);//设置电机运d行状态， 启动或停止 1启动 0停止
	Motor_SetSpeed(speed);//设置电机转速,0 - 1000
  Motor_SetDirection(dir);//设置电机运转方向 1 正 0反
	Target_Turns=turns;//设定目标转速




}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==&htim4)
	{
		  uint32_t motor_cnt_temp=(uint32_t)__HAL_TIM_GET_COUNTER(&htim2);
      uint32_t curr_index = HALL_DMA_BUF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_tim2_up_ch3);
      float turns=0;
      // 数据更新了吗？
      while (last_index != curr_index) {
				
        uint32_t capture = hall_dma_buffer[last_index];//得到上次的ccr计数值
        uint16_t delta = capture - last_capture;//两次的ccr之差，用于计算时间
        last_capture = capture;//更新上次的ccr捕获值

        // 速度计算，单位 RPM
        float time_us = (float)delta;
        float rpm = (1.0f / (time_us / 1e6f)) * 60.0f / 6.0f/1.5;//计算得到一分钟多少圈 1.5是考虑的传动比

        // 方向判断
        uint8_t is_reverse = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2);//得到电机的方向

        // 输出调试
       // printf("RPM = %.1f, Dir = %s\n", rpm, is_reverse ? "Reverse" : "Forward");

        // 移动到下一个
        last_index = (last_index + 1) % HALL_DMA_BUF_SIZE;

        motor_data.motor_dir=is_reverse;
        motor_data.motor_speed=rpm;
        turns=motor_cnt_temp/6.0/1.5;
        motor_data.motor_total_turns=turns;

      }
			uint8_t i;
			for(i=0;i<4;i++)
			{
				if(motor_cnt[i]==0)break;
			}
			if(i==3)
			{
				Motor_DataReset();//发送现有的数据，然后所有数据清0，关闭传感器	
			}
			if(Target_Turns==motor_data.motor_total_turns && Target_Turns!=0)//如果目标圈数
			{
					Motor_DataReset();//发送现有的数据，然后所有数据清0，关闭传感器
			}
		
		
		
		
	}
	

}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
		HAL_UART_Receive_IT(&huart1,&receive_byte,1);
	if(huart==&huart1)
	{
		
		switch(data_stat)
		{
			case 0:if(receive_byte==0x5A)data_stat=1;break;
			case 1:if(receive_byte>=0){
			data_len=receive_byte;
				data_stat=2;
			}else{
				data_stat=0;
			}break;
			case 2:if(current_data_pos<=data_len-4)
			{
				rcceive_data_buf[current_data_pos]=receive_byte;
				current_data_pos++;
			}else{
				
			//结束本轮接收
				ProcessData(rcceive_data_buf);

				data_stat=0;
				current_data_pos=0;
				data_len=0;
			}break;
			default:break;	
		}
	if(data_stat==0)
	{
						memset(rcceive_data_buf,0,sizeof(rcceive_data_buf));
	}
		
		}
		
		
	}
	
	



