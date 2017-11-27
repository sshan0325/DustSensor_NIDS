#include "stm32f4xx_conf.h"
#include "USART_Data.h"
#include "USART.h"
#include "LED_Con.h"

extern uint8_t aTxBuffer[];
extern uint8_t aRxBuffer[];
extern uint8_t aRxBuffer_485[];
extern unsigned char Rx_Compli_Flag;
extern unsigned char Rx_Compli_Flag_485;
extern unsigned char Rx_Count;
extern unsigned char Rx_Count_485;

extern unsigned char Tx_Count;
extern unsigned char Tx_Count_485;
extern uint8_t aRxBuffer [BUFFERSIZE];
extern unsigned char Tx_Flag;
extern unsigned char Tx_Flag_485;

int timecheck=0;
int timecheck_485=0;
int SensorDataValid=FALSE;
int IsMyTurn=FALSE;
int NextID=0;
int ForcedTxDne=FALSE;
int ForcedTxTimer=0;


void MakeTxData(uint8_t ucValidInput)
{
  uint8_t uiTxDataCount;
  uint8_t Crc=0;

  aTxBuffer[0]= DATAINIT;
  if (ucValidInput == TRUE)
  {
    for (uiTxDataCount=1; uiTxDataCount<=DATALENGTH ; uiTxDataCount++)
    {
      aTxBuffer[uiTxDataCount]=aRxBuffer[uiTxDataCount+DATAPOSITION];
    }
  }
  else
  {
    aTxBuffer[3]=0xFF;
    aTxBuffer[4]=0xFF;
    aTxBuffer[5]=0xFF;
    aTxBuffer[6]=0xFF;    
  }

  aTxBuffer[1]=0x00; 
  aTxBuffer[2]=MYID; 

  for(uiTxDataCount=0 ; uiTxDataCount<7 ; uiTxDataCount++)
  {
    Crc=Crc+aTxBuffer[uiTxDataCount];
  }
  
  aTxBuffer[7]=Crc;
  Tx_Count=8;
  Tx_Count_485=8;
}


/* SendData ---------------------------------------------------------------*/
void Send485Data(void)
{
  uint16_t tmp=0;
  /* USART in Transmitter mode */
  if (Tx_Flag_485==SET)
  {
    rs485_dir(1); 
    for (tmp=0; tmp<100; tmp++){}
    
    while(Tx_Count_485>0)
    {
      if(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == SET)
      {
        if (Tx_Count_485< BUFFERSIZE)
        {
          /* Send Transaction data */
          USART_SendData(USART2, aTxBuffer[8-Tx_Count_485]);
          Tx_Count_485--;
          if(Tx_Count_485==0)
          {
            Tx_Flag_485=RESET;
          }
        }
      }
    }
    for (tmp=0; tmp<10000; tmp++){}
    rs485_dir(0);
  } 
}

int rs485_dir(int rx)
{
  
   if(rx==0) GPIO_ResetBits(GPIOA, GPIO_Pin_1);
   else GPIO_SetBits(GPIOA, GPIO_Pin_1);
  
   return rx;
}
void RequestNextNode(void)
{
  int NextNode;
  uint8_t Crc=0;  
  uint8_t uiTxDataCount;
  
  NextNode=MYID+1;
  Tx_Flag_485=SET;
  Send485Data();  
  
  aTxBuffer[1]=0x55; 
  aTxBuffer[1]=0x00; 
  aTxBuffer[2]=NextNode;   
  aTxBuffer[3]=0xFF;
  aTxBuffer[4]=0xFF;
  aTxBuffer[5]=0xFF;
  aTxBuffer[6]=0xFF;        

  for(uiTxDataCount=0 ; uiTxDataCount<7 ; uiTxDataCount++)
  {
    Crc=Crc+aTxBuffer[uiTxDataCount];
  }  

  aTxBuffer[7]=Crc;
  Tx_Count=8;
  Tx_Count_485=8;  
  
  Tx_Flag_485=SET;
  Send485Data();  
}


void RS485DataProcess(void)
{
  int tmp;
  timecheck_485++;

  //  if(aRxBuffer_485[2]==8)
//  {
//    NextID=1;
//  }
//  else
//  {
    NextID=aRxBuffer_485[2]+1;
//  }
  
  if((Rx_Compli_Flag_485) == SET && (timecheck_485>100000))/*&& NextID==MYID*/
  {
    
    if (NextID==MYID )
    {
      __disable_irq();
      ForcedTxTimer=0;
      ForcedTxDne=TRUE;
      for(tmp=0; tmp<3000; tmp++) {}
      Rx_Compli_Flag_485 = RESET;
      Rx_Count_485 = 0; 
      if(SensorDataValid==TRUE)
      {
        MakeTxData(TRUE);
      }
      else
      {
        MakeTxData(FALSE);
      }
#if 0    
      for (tmp=0 ; tmp<8 ; tmp++)
      {  
        aTxBuffer[tmp]=aRxBuffer_485[tmp];
      }
#endif    
      Tx_Flag_485=SET;
      Send485Data();
      timecheck =0;
      SensorDataValid=FALSE;
      GPIO_ToggleBits(GPIOA, LED3_PIN);   

      

      //RequestNextNode();
      __enable_irq();
#if 0
      if(aRxBuffer_485[2]==0x01/*MYID*/ )
      {      
        MyTxTurn=TRUE;
      }
#endif
    }
    else
    {
      Rx_Compli_Flag_485 = RESET;
      Rx_Count_485 = 0;  
    }
      
  }
#if 0  
  else if(Rx_Compli_Flag == RESET && timecheck>10000000)
  {
    timecheck =0;
    MakeTxData(FALSE);
    Tx_Flag_485=SET;
    Send485Data();
    GPIO_ToggleBits(GPIOA, LED3_PIN);     
  }
#endif  
  else
  {
   
  }
}
void SensorDataProcess(void)
{
  timecheck++;
  if (ForcedTxDne==FALSE)
  {
    ForcedTxTimer++;
  }
  
  if (ForcedTxTimer>(400000+(MYID*20000)))
  {
    ForcedTxTimer=0;
    ForcedTxDne=TRUE;
    Rx_Compli_Flag = RESET;
    Rx_Count = 0;   
    MakeTxData(TRUE);
    Tx_Flag_485=SET;
    Send485Data();  
    timecheck =0;
  }
  
  if(Rx_Compli_Flag == SET && timecheck>100000)
  {
      if(aRxBuffer[0]==0xAA && aRxBuffer[1]==0xAA && aRxBuffer[2]==0x53 && aRxBuffer[3]==0x52)
      {      
        //Rx_Compli_Flag = RESET;
        //Rx_Count = 0;   
        //MakeTxData(TRUE);
        //Tx_Flag_485=SET;
        //Send485Data();
        SensorDataValid=TRUE;
        ForcedTxDne=FALSE;
        GPIO_ToggleBits(GPIOA, LED1_PIN);         
      }
      else 
      {
        Tx_Count=0;
        Rx_Count=0;
        Rx_Compli_Flag=RESET;
        GPIO_ToggleBits(GPIOA, LED2_PIN);    
      }
  }
  else if(Rx_Compli_Flag == RESET && timecheck>3000000)
  {
#if 1    
    timecheck =0;
    MakeTxData(FALSE);
    Tx_Flag_485=SET;
    Send485Data();
    GPIO_ToggleBits(GPIOA, LED3_PIN);     
#endif
  }
  else
  {
  }
}
