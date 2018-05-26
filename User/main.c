/* Includes ------------------------------------------------------------------*/
#include "USART.h"
#include "main.h"
#include "LED_Con.h"
#include "USART_Data.h"
#include "stm32f4xx_wwdg.h"
#include "stdio.h"
#include "stm32f4xx.h"
#include "I2C.h"
#include "stm32f4xx_i2c.h"

extern uint8_t aTxBuffer[];
extern uint8_t aRxBuffer[];
extern unsigned char Rx_Compli_Flag;
extern unsigned char Rx_SensorData_Count;
extern unsigned char Tx_Count_485;
extern uint8_t aRxBuffer [BUFFERSIZE];
extern unsigned char Tx_Flag;
extern unsigned char Tx_Flag_485;

//#define SLAVE_ADDRESS 0x5A // the slave address (example)
#define SLAVE_ADDRESS 0x44 // the slave address (example)

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
  int temp1,temp2;
  SYsInit();
  uint8_t received_data[9];
  
  printf ("System Init\r\n");   
  

  I2C_StretchClockCmd(I2C1, DISABLE);
  I2C_start(I2C1, SLAVE_ADDRESS<<1, I2C_Direction_Transmitter); // start a transmission in Master receiver mode
  
  I2C_SendData(I2C1, 0x20);  
  I2C_SendData(I2C1, 0x32);  
  for (temp1=0 ; temp1<100 ; temp1++)
  {
    temp2++;
  }      
  I2C_stop(I2C1);
        
  while(1)
  {
    //////////// DUST Sensor Data Read & 485 Tx//////////////////
    SensorDataProcess();
    RS485InputProcess();
    RS485DataProcess();
    /////////////////////////////////////////////////////////////
    temp++;
    if (0x44 == SLAVE_ADDRESS)
    {
        if (temp>10000000)
        {
          temp=0;
          GPIO_ToggleBits(GPIOA, LED3_PIN);
          printf ("AAAA\r\n");   
          
          I2C_start(I2C1, SLAVE_ADDRESS<<1, I2C_Direction_Transmitter); // start a transmission in Master receiver mode
          I2C_SendData(I2C1, 0xE0);  
          I2C_SendData(I2C1, 0x00);  
          for (temp1=0 ; temp1<100 ; temp1++)
          {
            temp2++;
          }      
          //I2C_stop(I2C1);      
          
          for (temp1=0 ; temp1<1000 ; temp1++)
          {
            temp2++;
          }      
          
          I2C_start(I2C1, SLAVE_ADDRESS<<1, I2C_Direction_Receiver); // start a transmission in Master receiver mode
          //I2C_SendData(I2C1, 0x0);  
          received_data[0] = I2C_read_ack(I2C1); // read one byte and request another byte
          received_data[1] = I2C_read_ack(I2C1); // read one byte and request another byte
          received_data[2] = I2C_read_ack(I2C1); // read one byte and request another byte
          received_data[3] = I2C_read_ack(I2C1); // read one byte and request another byte
          received_data[4] = I2C_read_ack(I2C1); // read one byte and request another byte
          received_data[5] = I2C_read_ack(I2C1); // read one byte and request another byte
          //received_data[6] = I2C_read_ack(I2C1); // read one byte and request another byte
          //received_data[7] = I2C_read_ack(I2C1); // read one byte and request another byte
          //received_data[8] = I2C_read_ack(I2C1); // read one byte and don't request another byte, stop transmission      
          I2C_stop(I2C1);
          
          printf ("%d, %d, %d, %d, %d, %d, %d, %d, %d \r\n",received_data[0],received_data[1],received_data[2],received_data[3],received_data[4],received_data[5],received_data[6],received_data[7],received_data[8]);            
        }
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
  init_I2C1();
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
