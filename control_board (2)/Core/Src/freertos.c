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
#include  "time.h"
#include  "bsp_motor.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
#define HALL_DMA_BUF_SIZE 5000
mot_manage_t motor_manage_unity={
  .motor_dir =0,
  .motor_speed = 0,
  .motor_total_turns = 0
};//初始化电机管理单元

volatile uint32_t Target_Turns=0;
volatile uint8_t Sensorflag1=0;
volatile uint8_t Sensorflag2=0;
volatile uint8_t Sensorflag3=0;
volatile uint8_t Sensorflag4=0;
uint32_t motor_cnt[4]={0};//4个电机计数值;
static uint16_t last_index = 0;
static uint16_t last_capture = 0;
extern uint32_t hall_dma_buffer[HALL_DMA_BUF_SIZE];
extern DMA_HandleTypeDef hdma_tim2_up_ch3;
extern TIM_HandleTypeDef htim2;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for HallDecodeTask */
osThreadId_t HallDecodeTaskHandle;
const osThreadAttr_t HallDecodeTask_attributes = {
  .name = "HallDecodeTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh7,
};
/* Definitions for sensor2Task04 */
osThreadId_t sensor2Task04Handle;
const osThreadAttr_t sensor2Task04_attributes = {
  .name = "sensor2Task04",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for sensor3Task05 */
osThreadId_t sensor3Task05Handle;
const osThreadAttr_t sensor3Task05_attributes = {
  .name = "sensor3Task05",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for sensor4Task06 */
osThreadId_t sensor4Task06Handle;
const osThreadAttr_t sensor4Task06_attributes = {
  .name = "sensor4Task06",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for myMutex01 */
osMutexId_t myMutex01Handle;
const osMutexAttr_t myMutex01_attributes = {
  .name = "myMutex01"
};
/* Definitions for sensor1BinarySem01 */
osSemaphoreId_t sensor1BinarySem01Handle;
const osSemaphoreAttr_t sensor1BinarySem01_attributes = {
  .name = "sensor1BinarySem01"
};
/* Definitions for sensor2BinarySem02 */
osSemaphoreId_t sensor2BinarySem02Handle;
const osSemaphoreAttr_t sensor2BinarySem02_attributes = {
  .name = "sensor2BinarySem02"
};
/* Definitions for sensor3BinarySem03 */
osSemaphoreId_t sensor3BinarySem03Handle;
const osSemaphoreAttr_t sensor3BinarySem03_attributes = {
  .name = "sensor3BinarySem03"
};
/* Definitions for sensor4BinarySem04 */
osSemaphoreId_t sensor4BinarySem04Handle;
const osSemaphoreAttr_t sensor4BinarySem04_attributes = {
  .name = "sensor4BinarySem04"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTask02(void *argument);
void StartTask04(void *argument);
void StartTask05(void *argument);
void StartTask06(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of myMutex01 */
  myMutex01Handle = osMutexNew(&myMutex01_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of sensor1BinarySem01 */
  sensor1BinarySem01Handle = osSemaphoreNew(1, 1, &sensor1BinarySem01_attributes);

  /* creation of sensor2BinarySem02 */
  sensor2BinarySem02Handle = osSemaphoreNew(1, 1, &sensor2BinarySem02_attributes);

  /* creation of sensor3BinarySem03 */
  sensor3BinarySem03Handle = osSemaphoreNew(1, 1, &sensor3BinarySem03_attributes);

  /* creation of sensor4BinarySem04 */
  sensor4BinarySem04Handle = osSemaphoreNew(1, 1, &sensor4BinarySem04_attributes);

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

  /* creation of HallDecodeTask */
  HallDecodeTaskHandle = osThreadNew(StartTask02, NULL, &HallDecodeTask_attributes);

  /* creation of sensor2Task04 */
  sensor2Task04Handle = osThreadNew(StartTask04, NULL, &sensor2Task04_attributes);

  /* creation of sensor3Task05 */
  sensor3Task05Handle = osThreadNew(StartTask05, NULL, &sensor3Task05_attributes);

  /* creation of sensor4Task06 */
  sensor4Task06Handle = osThreadNew(StartTask06, NULL, &sensor4Task06_attributes);

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
  {//电机1传感器数据
		
    if (osSemaphoreAcquire(sensor1BinarySem01Handle, 1000) == osOK)
    {
      Sensorflag1=0;
      // 收到中断信号，开始执行

    }else {
			motor_cnt[0]=motor_manage_unity.motor_total_turns;
      //获取当前霍尔变量cnt的值
    }


    osDelay(100);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the HallDecodeTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
    //该任务能得到电机当前的方向，速度，总圈数
			
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

        motor_manage_unity.motor_dir=is_reverse;
        motor_manage_unity.motor_speed=rpm;
        turns=motor_cnt_temp/6.0/1.5;
        motor_manage_unity.motor_total_turns=turns;

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
			if(Target_Turns==motor_manage_unity.motor_total_turns && Target_Turns!=0)//如果目标圈数
			{
					Motor_DataReset();//发送现有的数据，然后所有数据清0，关闭传感器
			}
			

    osDelay(10); // 可根据转速调整频率
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask04 */
/**
* @brief Function implementing the sensor2Task04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask04 */
void StartTask04(void *argument)
{
  /* USER CODE BEGIN StartTask04 */
  /* Infinite loop */
  for(;;)
  {
		  
  //电机2传感器数据
	
    if (osSemaphoreAcquire(sensor2BinarySem02Handle, 1000) == osOK)
    {
      Sensorflag2=0;
      // 收到中断信号，开始执行

    }else {
			motor_cnt[1]=motor_manage_unity.motor_total_turns;
      //获取当前霍尔变量cnt的值
    }
    osDelay(1000);
  }
  /* USER CODE END StartTask04 */
}

/* USER CODE BEGIN Header_StartTask05 */
/**
* @brief Function implementing the sensor3Task05 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask05 */
void StartTask05(void *argument)
{
  /* USER CODE BEGIN StartTask05 */
  /* Infinite loop */
  for(;;)
  {
		
		  if (osSemaphoreAcquire(sensor3BinarySem03Handle, 1000) == osOK)
    {
      Sensorflag3=0;
      // 收到中断信号，开始执行

    }else {
			motor_cnt[2]=motor_manage_unity.motor_total_turns;
      //获取当前霍尔变量cnt的值
    }
    osDelay(100);
  }
  /* USER CODE END StartTask05 */
}

/* USER CODE BEGIN Header_StartTask06 */
/**
* @brief Function implementing the sensor4Task06 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask06 */
void StartTask06(void *argument)
{
  /* USER CODE BEGIN StartTask06 */
  /* Infinite loop */
  for(;;)
  {
		
		  if (osSemaphoreAcquire(sensor4BinarySem04Handle, 1000) == osOK)//最多等待1s
    {
      Sensorflag2=0;
      // 收到中断信号，开始执行

    }else {
			motor_cnt[3]=motor_manage_unity.motor_total_turns;
      //获取当前霍尔变量cnt的值
    }
    osDelay(100);//100ms检测一次
  }
  /* USER CODE END StartTask06 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

