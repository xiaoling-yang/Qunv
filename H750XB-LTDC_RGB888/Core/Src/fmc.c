/**
  ******************************************************************************
  * File Name          : FMC.c
  * Description        : This file provides code for the configuration
  *                      of the FMC peripheral.
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

/* Includes ------------------------------------------------------------------*/
#include "fmc.h"

/* USER CODE BEGIN 0 */

FMC_SDRAM_CommandTypeDef command;	// ����ָ��

/******************************************************************************************************
*	�� �� ��: SDRAM_delay
*	��ڲ���: ��
*	�� �� ֵ: ��
*	��������: ������ʱ��������λԼΪ5ms
*	˵    ��: ��
*******************************************************************************************************/
void SDRAM_delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
	
  for(index = (200000 * nCount); index != 0; index--);
}

/******************************************************************************************************
*	�� �� ��: SDRAM_Initialization_Sequence
*	��ڲ���: hsdram - SDRAM_HandleTypeDef����ı���������ʾ�����sdram
*				 Command	- ����ָ��
*	�� �� ֵ: ��
*	��������: SDRAM ��������
*	˵    ��: ����SDRAM���ʱ��Ϳ��Ʒ�ʽ
*******************************************************************************************************/
void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command)
{
  __IO uint32_t tmpmrd = 0;
  
  /* Configure a clock configuration enable command */
  Command->CommandMode 					= FMC_SDRAM_CMD_CLK_ENABLE;	// ����SDRAMʱ�� 
  Command->CommandTarget 				= FMC_COMMAND_TARGET_BANK; 	// ѡ��Ҫ���Ƶ�����
  Command->AutoRefreshNumber 			= 1;
  Command->ModeRegisterDefinition 	= 0;
  
  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);	// ���Ϳ���ָ��
  SDRAM_delay(5);		// ��ʱ�ȴ�
  
  /* Configure a PALL (precharge all) command */ 
  Command->CommandMode 					= FMC_SDRAM_CMD_PALL;		// Ԥ�������
  Command->CommandTarget 				= FMC_COMMAND_TARGET_BANK;	// ѡ��Ҫ���Ƶ�����
  Command->AutoRefreshNumber 			= 1;
  Command->ModeRegisterDefinition 	= 0;
  
  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);  // ���Ϳ���ָ��
  
  /* Configure a Auto-Refresh command */ 
  Command->CommandMode 					= FMC_SDRAM_CMD_AUTOREFRESH_MODE;	// ʹ���Զ�ˢ��
  Command->CommandTarget 				= FMC_COMMAND_TARGET_BANK;          // ѡ��Ҫ���Ƶ�����
  Command->AutoRefreshNumber			= 8;                                // �Զ�ˢ�´���
  Command->ModeRegisterDefinition 	= 0;
  
  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);	// ���Ϳ���ָ��
  
  /* Program the external memory mode register */
  tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_2          |
                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
                     SDRAM_MODEREG_CAS_LATENCY_3           |
                     SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
  
  Command->CommandMode					= FMC_SDRAM_CMD_LOAD_MODE;	// ����ģʽ�Ĵ�������
  Command->CommandTarget 				= FMC_COMMAND_TARGET_BANK;	// ѡ��Ҫ���Ƶ�����
  Command->AutoRefreshNumber 			= 1;
  Command->ModeRegisterDefinition 	= tmpmrd;
  
  HAL_SDRAM_SendCommand(hsdram, Command, 0x1000);	// ���Ϳ���ָ��
  
  HAL_SDRAM_ProgramRefreshRate(hsdram, 1586); 	// ����ˢ�¼����� 
}

void SDRAM_Device_Init(void)
{
	SDRAM_Initialization_Sequence(&hsdram1, &command);
}


/******************************************************************************************************
*	�� �� ��: SDRAM_Test
*	��ڲ���: ��
*	�� �� ֵ: SUCCESS - �ɹ���ERROR - ʧ��
*	��������: SDRAM����
*	˵    ��: ����16λ�����ݿ��д�����ݣ��ٶ�ȡ����һһ���бȽϣ������8λ�����ݿ��д�룬
*				 ������֤NBL0��NBL1�������ŵ������Ƿ�������          
*******************************************************************************************************/
uint8_t SDRAM_Test(void)
{
	uint32_t i = 0;		// ��������
	uint16_t ReadData = 0; 	// ��ȡ��������
	uint8_t  ReadData_8b;
	
	printf("SDRAM���Կ�ʼ����16λ���ݿ��д������...\r\n");	
	
	for (i = 0; i < SDRAM_Size/2; i++)
	{
 		*(__IO uint16_t*) (SDRAM_BANK_ADDR + 2*i) = (uint16_t)i;		// д������
	}
	
	printf("д����ϣ���ȡ���ݲ��Ƚ�...\r\n");
	for(i = 0; i < SDRAM_Size/2;i++ )
	{
		ReadData = *(__IO uint16_t*)(SDRAM_BANK_ADDR + 2 * i );  // ��SDRAM��������	
		if( ReadData != (uint16_t)i )      //������ݣ�������ȣ���������,���ؼ��ʧ�ܽ����
		{
			printf("SDRAM����ʧ�ܣ���\r\n");
			return ERROR;	 // ����ʧ�ܱ�־
		}
	}
	
	printf("16λ���ݿ�ȶ�дͨ������8λ���ݿ��д������\r\n");
	for (i = 0; i < 255; i++)
	{
 		*(__IO uint8_t*) (SDRAM_BANK_ADDR + i) =  (uint8_t)i;
	}	
	printf("д����ϣ���ȡ���ݲ��Ƚ�...\r\n");
	for (i = 0; i < 255; i++)
	{
		ReadData_8b = *(__IO uint8_t*) (SDRAM_BANK_ADDR + i);
		if( ReadData_8b != (uint8_t)i )      //������ݣ�������ȣ���������,���ؼ��ʧ�ܽ����
		{
			printf("8λ���ݿ�ȶ�д����ʧ�ܣ���\r\n");
			printf("����NBL0��NBL1������\r\n");	
			return ERROR;	 // ����ʧ�ܱ�־
		}
	}		
	printf("8λ���ݿ�ȶ�дͨ��\r\n");
	printf("SDRAM��д����ͨ����ϵͳ����\r\n");
	return SUCCESS;	 // ���سɹ���־
}


/* USER CODE END 0 */

SDRAM_HandleTypeDef hsdram1;

/* FMC initialization function */
void MX_FMC_Init(void)
{
  FMC_SDRAM_TimingTypeDef SdramTiming = {0};

  /** Perform the SDRAM1 memory initialization sequence
  */
  hsdram1.Instance = FMC_SDRAM_DEVICE;
  /* hsdram1.Init */
  hsdram1.Init.SDBank = FMC_SDRAM_BANK1;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_1;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 2;
  SdramTiming.ExitSelfRefreshDelay = 6;
  SdramTiming.SelfRefreshTime = 4;
  SdramTiming.RowCycleDelay = 6;
  SdramTiming.WriteRecoveryTime = 3;
  SdramTiming.RPDelay = 2;
  SdramTiming.RCDDelay = 2;

  if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK)
  {
    Error_Handler( );
  }

}

static uint32_t FMC_Initialized = 0;

static void HAL_FMC_MspInit(void){
  /* USER CODE BEGIN FMC_MspInit 0 */

  /* USER CODE END FMC_MspInit 0 */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (FMC_Initialized) {
    return;
  }
  FMC_Initialized = 1;

  /* Peripheral clock enable */
  __HAL_RCC_FMC_CLK_ENABLE();

  /** FMC GPIO Configuration
  PE1   ------> FMC_NBL1
  PE0   ------> FMC_NBL0
  PG15   ------> FMC_SDNCAS
  PD0   ------> FMC_D2
  PD1   ------> FMC_D3
  PG8   ------> FMC_SDCLK
  PF2   ------> FMC_A2
  PF1   ------> FMC_A1
  PF0   ------> FMC_A0
  PG5   ------> FMC_BA1
  PF3   ------> FMC_A3
  PG4   ------> FMC_BA0
  PG2   ------> FMC_A12
  PF5   ------> FMC_A5
  PF4   ------> FMC_A4
  PC2   ------> FMC_SDNE0
  PC3   ------> FMC_SDCKE0
  PE10   ------> FMC_D7
  PH5   ------> FMC_SDNWE
  PF13   ------> FMC_A7
  PF14   ------> FMC_A8
  PE9   ------> FMC_D6
  PE11   ------> FMC_D8
  PD15   ------> FMC_D1
  PD14   ------> FMC_D0
  PF12   ------> FMC_A6
  PF15   ------> FMC_A9
  PE12   ------> FMC_D9
  PE15   ------> FMC_D12
  PF11   ------> FMC_SDNRAS
  PG0   ------> FMC_A10
  PE8   ------> FMC_D5
  PE13   ------> FMC_D10
  PD10   ------> FMC_D15
  PD9   ------> FMC_D14
  PG1   ------> FMC_A11
  PE7   ------> FMC_D4
  PE14   ------> FMC_D11
  PD8   ------> FMC_D13
  */
  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_0|GPIO_PIN_10|GPIO_PIN_9
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15|GPIO_PIN_8
                          |GPIO_PIN_13|GPIO_PIN_7|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_8|GPIO_PIN_5|GPIO_PIN_4
                          |GPIO_PIN_2|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_15|GPIO_PIN_14
                          |GPIO_PIN_10|GPIO_PIN_9|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0|GPIO_PIN_3
                          |GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_12|GPIO_PIN_15|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* GPIO_InitStruct */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /* USER CODE BEGIN FMC_MspInit 1 */

  /* USER CODE END FMC_MspInit 1 */
}

void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef* sdramHandle){
  /* USER CODE BEGIN SDRAM_MspInit 0 */

  /* USER CODE END SDRAM_MspInit 0 */
  HAL_FMC_MspInit();
  /* USER CODE BEGIN SDRAM_MspInit 1 */

  /* USER CODE END SDRAM_MspInit 1 */
}

static uint32_t FMC_DeInitialized = 0;

static void HAL_FMC_MspDeInit(void){
  /* USER CODE BEGIN FMC_MspDeInit 0 */

  /* USER CODE END FMC_MspDeInit 0 */
  if (FMC_DeInitialized) {
    return;
  }
  FMC_DeInitialized = 1;
  /* Peripheral clock enable */
  __HAL_RCC_FMC_CLK_DISABLE();

  /** FMC GPIO Configuration
  PE1   ------> FMC_NBL1
  PE0   ------> FMC_NBL0
  PG15   ------> FMC_SDNCAS
  PD0   ------> FMC_D2
  PD1   ------> FMC_D3
  PG8   ------> FMC_SDCLK
  PF2   ------> FMC_A2
  PF1   ------> FMC_A1
  PF0   ------> FMC_A0
  PG5   ------> FMC_BA1
  PF3   ------> FMC_A3
  PG4   ------> FMC_BA0
  PG2   ------> FMC_A12
  PF5   ------> FMC_A5
  PF4   ------> FMC_A4
  PC2   ------> FMC_SDNE0
  PC3   ------> FMC_SDCKE0
  PE10   ------> FMC_D7
  PH5   ------> FMC_SDNWE
  PF13   ------> FMC_A7
  PF14   ------> FMC_A8
  PE9   ------> FMC_D6
  PE11   ------> FMC_D8
  PD15   ------> FMC_D1
  PD14   ------> FMC_D0
  PF12   ------> FMC_A6
  PF15   ------> FMC_A9
  PE12   ------> FMC_D9
  PE15   ------> FMC_D12
  PF11   ------> FMC_SDNRAS
  PG0   ------> FMC_A10
  PE8   ------> FMC_D5
  PE13   ------> FMC_D10
  PD10   ------> FMC_D15
  PD9   ------> FMC_D14
  PG1   ------> FMC_A11
  PE7   ------> FMC_D4
  PE14   ------> FMC_D11
  PD8   ------> FMC_D13
  */

  HAL_GPIO_DeInit(GPIOE, GPIO_PIN_1|GPIO_PIN_0|GPIO_PIN_10|GPIO_PIN_9
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15|GPIO_PIN_8
                          |GPIO_PIN_13|GPIO_PIN_7|GPIO_PIN_14);

  HAL_GPIO_DeInit(GPIOG, GPIO_PIN_15|GPIO_PIN_8|GPIO_PIN_5|GPIO_PIN_4
                          |GPIO_PIN_2|GPIO_PIN_0|GPIO_PIN_1);

  HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_15|GPIO_PIN_14
                          |GPIO_PIN_10|GPIO_PIN_9|GPIO_PIN_8);

  HAL_GPIO_DeInit(GPIOF, GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0|GPIO_PIN_3
                          |GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_12|GPIO_PIN_15|GPIO_PIN_11);

  HAL_GPIO_DeInit(GPIOC, GPIO_PIN_2|GPIO_PIN_3);

  HAL_GPIO_DeInit(GPIOH, GPIO_PIN_5);

  /* USER CODE BEGIN FMC_MspDeInit 1 */

  /* USER CODE END FMC_MspDeInit 1 */
}

void HAL_SDRAM_MspDeInit(SDRAM_HandleTypeDef* sdramHandle){
  /* USER CODE BEGIN SDRAM_MspDeInit 0 */

  /* USER CODE END SDRAM_MspDeInit 0 */
  HAL_FMC_MspDeInit();
  /* USER CODE BEGIN SDRAM_MspDeInit 1 */

  /* USER CODE END SDRAM_MspDeInit 1 */
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
