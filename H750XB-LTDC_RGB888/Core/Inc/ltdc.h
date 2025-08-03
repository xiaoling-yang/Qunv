/**
  ******************************************************************************
  * File Name          : LTDC.h
  * Description        : This file provides code for the configuration
  *                      of the LTDC instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ltdc_H
#define __ltdc_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern LTDC_HandleTypeDef hltdc;

/* USER CODE BEGIN Private defines */

#define LCD_Width     	800				//LCD的像素长度
#define LCD_Height    	480				//LCD的像素宽度
#define LCD_MemoryAdd   0xC0000000 	//显存的起始地址  
#define LCD_MemoryAdd_OFFSET    ((uint32_t)LCD_Width * LCD_Height * 3)    

#define  	LTDC_Black_PIN									GPIO_PIN_4								
#define		LTDC_Black_PORT									GPIOD									
#define 	GPIO_LTDC_Black_CLK_ENABLE  		__HAL_RCC_GPIOD_CLK_ENABLE()	 

void  LCD_Clear(uint32_t color); 		//	清屏
void  LCD_CopyBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,uint32_t *color);
void  LCD_RGB_Init(void);

/* USER CODE END Private defines */

void MX_LTDC_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ ltdc_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
