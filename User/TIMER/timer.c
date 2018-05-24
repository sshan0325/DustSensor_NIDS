#include "timer.h"
#include "stm32f4xx_tim.h"

//////////////////////////////////// TIMMER//////////////////////////////////////////////////
void TIM_Config(void)
{
  TIM_TimeBaseInitTypeDef         TIM_TimeBaseStructure;
  TIM_OCInitTypeDef               TIM_OCInitStructure;
  NVIC_InitTypeDef                NVIC_InitStructure;
  
  /* Timer 14 Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 480;     // 48000000 / 480 =100000 Hz = 10us
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 1000; //   10 ms == 1000us
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
  
    /* TIM Interrupts enable */
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

  /* TIM4 enable counter */
  TIM_Cmd(TIM4, ENABLE);
  
  //Enable the TIM14 gloabal Interrupt 
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 2 ;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
}