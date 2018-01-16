/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported typedef -----------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Uncomment the line below if you will use the USART in Transmitter Mode */
/* #define USART_TRANSMITTER */
/* Uncomment the line below if you will use the USART in Receiver Mode */
#define USART_RECEIVER 

#define BUFFERSIZE                     512

/* Private function prototypes -----------------------------------------------*/


#ifdef USART_RECEIVER
static TestStatus Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);
#endif

/* Private functions ---------------------------------------------------------*/
void USART6_Config(void);
void USART2_Config(void);
void USART1_Config(int baudrate);
void SysTickConfig(void);
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
void assert_failed(uint8_t* file, uint32_t line);

#endif /* __MAIN_H */