/**
  ******************************************************************************
  * @file    USART/USART_TwoBoards/USART_DataExchangeInterrupt/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "USART.h"
#include "stm32f4xx_it.h"
#include "LED_Con.h"
#include "main.h"
#include "stm32f4xx_conf.h"    
#include "USART_Data.h"

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_DataExchangeInterrupt
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint8_t aTxBuffer[];
extern uint8_t aRxBuffer[];
extern uint8_t aRxBuffer_485[BUFFERSIZE];
extern uint8_t aCmdBuffer[];    //?
extern uint8_t aAckBuffer[];    //?

extern __IO uint8_t ubUsartTransactionType;     //?
extern __IO uint8_t ubUsartMode;                //?

extern int timecheck;

unsigned char Rx_Compli_Flag = RESET;
unsigned char Tx_Flag = RESET;
unsigned char Tx_Flag_485 = RESET;
unsigned char Rx_SensorData_Count = 0 ;
unsigned char Rx_Count_485 = 0 ;
unsigned char Tx_Count_485 =0 ;
unsigned char NextID=127;
unsigned char MyIdIsFirst=TRUE;
unsigned int RS485DataSavePosition=0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{

}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s/startup_stm32f427x.s).                         */
/******************************************************************************/

/**
* @brief  This function handles USRAT interrupt request.
* @param  None
* @retval None
*/
void USART2_IRQHandler(void)
{
  /* USART in Recirve mode */  
  if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
  {
    aRxBuffer_485[RS485DataSavePosition] = USART_ReceiveData(USART2);	
    Rx_Count_485++;  
    RS485DataSavePosition++;
    
    if(RS485DataSavePosition>=BUFFERSIZE)
      RS485DataSavePosition=0;
  } 
}
void USART6_IRQHandler(void)
{
  if (USART_GetITStatus(USART6, USART_IT_RXNE) == SET)
  {
    aRxBuffer[Rx_SensorData_Count] = USART_ReceiveData(USART6);	
    Rx_SensorData_Count++; 
    if (Rx_SensorData_Count>7)
    {
      Rx_Compli_Flag = SET ; 
    }
    timecheck =0;
  } 
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
