/* Includes ------------------------------------------------------------------*/
#include "USART.h"
#include "main.h"
#include "LED_Con.h"
#include "USART_Data.h"
#include "stm32f4xx_wwdg.h"

extern uint8_t aTxBuffer[];
extern uint8_t aRxBuffer[];
extern unsigned char Rx_Compli_Flag;
extern unsigned char Rx_SensorData_Count;
extern unsigned char Tx_Count_485;
extern uint8_t aRxBuffer [BUFFERSIZE];
extern unsigned char Tx_Flag;
extern unsigned char Tx_Flag_485;


static __IO uint32_t TimingDelay;

void main(void)
{
  SYsInit();
  
  while(1)
  {
    //////////// DUST Sensor Data Read & 485 Tx//////////////////
    SensorDataProcess();
    RS485InputProcess();
    RS485DataProcess();
    /////////////////////////////////////////////////////////////
  }
}

void SYsInit(void)
{
  LED_On_Test();
  USART6_Config();
  USART2_Config();
  SysTickConfig();  
}
  

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
