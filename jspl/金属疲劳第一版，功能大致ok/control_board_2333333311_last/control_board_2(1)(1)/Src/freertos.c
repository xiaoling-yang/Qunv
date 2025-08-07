/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_uart.h"
#include <string.h>
#include "485_to_motor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RX_BUFFER_SIZE 64
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

volatile uint8_t flag_motor_start=0;
 float interval=1;//计算间隔 
uint16_t Target_Turns;
volatile uint8_t sensor_flag=0;//4个传感器触发标志位,外部中断触发，未触发时为0
uint32_t data[4]={0};//存储4个实验的圈数
uint8_t uart_flag=0;
read_data_t current_motor_stat={0};
uint32_t count=0;//just test
float Transmission_ratio=1.5f;//
extern mt_bk_t motor_data_management;
extern mt_state_t lcd_send_to_motor;
extern uint8_t rx_buffer1[];
extern uint8_t rx_buffer2[];
extern uint16_t gloable_speed;
extern uint16_t target_circle;


/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for WriteConfirmTas */
osThreadId_t WriteConfirmTasHandle;
const osThreadAttr_t WriteConfirmTas_attributes = {
  .name = "WriteConfirmTas",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for ReadConfirmTask */
osThreadId_t ReadConfirmTaskHandle;
const osThreadAttr_t ReadConfirmTask_attributes = {
  .name = "ReadConfirmTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for dataProcessTask */
osThreadId_t dataProcessTaskHandle;
const osThreadAttr_t dataProcessTask_attributes = {
  .name = "dataProcessTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for WriteCmdBinarySem01 */
osSemaphoreId_t WriteCmdBinarySem01Handle;
const osSemaphoreAttr_t WriteCmdBinarySem01_attributes = {
  .name = "WriteCmdBinarySem01"
};
/* Definitions for ReadCmdBinarySem02 */
osSemaphoreId_t ReadCmdBinarySem02Handle;
const osSemaphoreAttr_t ReadCmdBinarySem02_attributes = {
  .name = "ReadCmdBinarySem02"
};
/* Definitions for uartBinarySem03 */
osSemaphoreId_t uartBinarySem03Handle;
const osSemaphoreAttr_t uartBinarySem03_attributes = {
  .name = "uartBinarySem03"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void WriteConfirmStartTask02(void *argument);
void ReadConfirmStartTask03(void *argument);
void dataProcessStartTask04(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of WriteCmdBinarySem01 */
  WriteCmdBinarySem01Handle = osSemaphoreNew(1, 1, &WriteCmdBinarySem01_attributes);

  /* creation of ReadCmdBinarySem02 */
  ReadCmdBinarySem02Handle = osSemaphoreNew(1, 1, &ReadCmdBinarySem02_attributes);

  /* creation of uartBinarySem03 */
  uartBinarySem03Handle = osSemaphoreNew(1, 1, &uartBinarySem03_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of WriteConfirmTas */
  WriteConfirmTasHandle = osThreadNew(WriteConfirmStartTask02, NULL, &WriteConfirmTas_attributes);

  /* creation of ReadConfirmTask */
  ReadConfirmTaskHandle = osThreadNew(ReadConfirmStartTask03, NULL, &ReadConfirmTask_attributes);

  /* creation of dataProcessTask */
  dataProcessTaskHandle = osThreadNew(dataProcessStartTask04, NULL, &dataProcessTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
		if(flag_motor_start==1)
		{
				//motor_get_speed();
    
	
		//count=lcd_send_to_motor.current_circle;
		count=count+(int)(gloable_speed*interval/60/Transmission_ratio);//通过积分的方式	计算得到当前的圈数
				
		char addr[4][30]={{"page2.n0.val"},{"page2.n1.val"},{"page2.n2.val"},{"page2.n3.val"}};//组件的地址
		
		for(uint8_t i=0;i<4;i++)
		{	
			update_lcd_ui(addr[i],count);//更新数据到led屏幕
		}
		}else
		{
			count=0;
			
		}
		if(count>=target_circle)
		{
			
			
			flag_motor_start=0;
			 motor_stop();
			
			
		}
		osDelay(interval*1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_WriteConfirmStartTask02 */
/**
* @brief Function implementing the WriteConfirmTas thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_WriteConfirmStartTask02 */
void WriteConfirmStartTask02(void *argument)
{
  /* USER CODE BEGIN WriteConfirmStartTask02 */
  /* Infinite loop */
  for(;;)
  {
		
		
		
		
    osDelay(1);
  }
  /* USER CODE END WriteConfirmStartTask02 */
}

/* USER CODE BEGIN Header_ReadConfirmStartTask03 */
/**
* @brief Function implementing the ReadConfirmTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ReadConfirmStartTask03 */
void ReadConfirmStartTask03(void *argument)
{
  /* USER CODE BEGIN ReadConfirmStartTask03 */
  /* Infinite loop */
  for(;;)
  {
	
	osStatus_t ret=osSemaphoreAcquire(ReadCmdBinarySem02Handle,osWaitForever);
	if(ret==osOK)
	{
		
		if(motor_data_management.function_code==0x03)//??????
		{
		//	uint16_t read_data_addr=motor_data_management.address;
	 //???????
			uint16_t temp_speed=0;
			temp_speed|= motor_data_management.motor_total_buf[4]<<8;
				temp_speed|= motor_data_management.data[3];
			 if(current_motor_stat.motor_dir==0x01)//???
			{
				
				
				current_motor_stat.speed=temp_speed;
				
			}else if(current_motor_stat.motor_dir==0x00)//???
			{
				temp_speed = ~(temp_speed-1);

        current_motor_stat.speed= temp_speed;
				
				
			}else{}			
				
		ProcessData(&huart2,motor_data_management.motor_total_buf,motor_data_management.total_data_len);
    memset(&motor_data_management,0,UART_RX_BUF_SIZE);
		
		
    //HAL_UART_Receive_DMA(&huart2,rx_buffer2,RX_BUFFER_SIZE);//??DMA??
		
		}
		
		
		
//		current_motor_stat.current_circles+=(uint16_t)(current_motor_stat.speed*interval);//通过积分的方式	计算得到当前的圈数	
//		char addr[4][30]={{"page2.n0.val"},{"page2.n1.val"},{"page2.n2.val"},{"page2.n3.val"}};//组件的地址
//		for(uint8_t i=0;i<4;i++)
//		{	
//			update_lcd_ui(addr[i],count++);//更新数据到led屏幕
//		}

			
		//11111
//				current_motor_stat.current_circles+=(uint16_t)(current_motor_stat.speed*interval);
//				
//				//更新数据到屏幕上
//				
//				char addr[4][30]={{"page2.n0.val"},{"page2.n1.val"},{"page2.n2.val"},{"page2.n3.val"}};
//		
//		
//		
//		for(uint8_t i=0;i<4;i++)
//		{
//			update_lcd_ui(addr[i],current_motor_stat.current_circles);//更新数据到led屏幕
//			
//			
//		}
//				
				
			
				
				
//					if(current_motor_stat.current_circles>=Target_Turns||sensor_flag!=0)//如果大于目标圈数或者传感器触发
//	{
//		if(current_motor_stat.current_circles>=Target_Turns)//达到目标圈数，应该停止电机
//		{
//			motor_stop();
//		}else{//传感器触发
//			data[sensor_flag-1]=current_motor_stat.current_circles;
//		}
//		
//		
//	}
		
		
		
		
	}
		
		
    osDelay(1);
  }
  /* USER CODE END ReadConfirmStartTask03 */
}


/* USER CODE BEGIN Header_dataProcessStartTask04 */
/**
* @brief Function implementing the dataProcessTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_dataProcessStartTask04 */
void dataProcessStartTask04(void *argument)
{
  /* USER CODE BEGIN dataProcessStartTask04 */
  /* Infinite loop */
  for(;;)
  {
	osStatus_t ret=osSemaphoreAcquire(uartBinarySem03Handle,osWaitForever);
	if(ret==osOK)
	{
		
		
		
		ProcessData(&huart1,motor_data_management.motor_total_buf,motor_data_management.total_data_len);
    memset(&motor_data_management,0,sizeof(motor_data_management));
		memset(&rx_buffer1,0,UART_RX_BUF_SIZE);
		
		


		
		
    HAL_UART_Receive_DMA(&huart1,rx_buffer1,RX_BUFFER_SIZE);//启用DMA接收
		
	}
	
	
	
	
    osDelay(1);
  }
  /* USER CODE END dataProcessStartTask04 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

