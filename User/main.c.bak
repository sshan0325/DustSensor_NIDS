/* Includes ------------------------------------------------------------------*/
#include "USART.h"
#include "main.h"
#include "LED_Con.h"
#include "USART_Data.h"
#include "stm32f4xx_wwdg.h"
#include "stdio.h"
#include "stm32f4xx.h"

extern uint8_t aTxBuffer[];
extern uint8_t aRxBuffer[];
extern unsigned char Rx_Compli_Flag;
extern unsigned char Rx_SensorData_Count;
extern unsigned char Tx_Count_485;
extern uint8_t aRxBuffer [BUFFERSIZE];
extern unsigned char Tx_Flag;
extern unsigned char Tx_Flag_485;


static __IO uint32_t TimingDelay;

#ifdef __GNUC__
  // With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
  //   set to 'Yes') calls __io_putchar() 
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
   #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{ 
  USART_SendData(USART1, (u8) ch);
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
  return ch;
}


void main(void)
{
  int temp=0;
  SYsInit();
  
  while(1)
  {
    //////////// DUST Sensor Data Read & 485 Tx//////////////////
    SensorDataProcess();
    RS485InputProcess();
    RS485DataProcess();
    /////////////////////////////////////////////////////////////
    temp++;
    
    if (temp>1000000)
    {
      temp=0;
      GPIO_ToggleBits(GPIOA, LED3_PIN);
      printf ("AAAA\r\n");      
    }    
  }
  

}

void SYsInit(void)
{
  LED_On_Test();
  USART6_Config();
  USART2_Config();
  SysTickConfig();  
  USART1_Config(115200);
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
