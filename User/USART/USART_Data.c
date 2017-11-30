#include "stm32f4xx_conf.h"
#include "USART_Data.h"
#include "USART.h"
#include "LED_Con.h"

extern uint8_t aTxBuffer[];
extern uint8_t aRxBuffer[];
extern uint8_t aRxBuffer_485[];
extern unsigned char Rx_Compli_Flag;
extern unsigned char Rx_Compli_Flag_485;
extern unsigned char Rx_SensorData_Count;
extern unsigned char Rx_Count_485;

extern unsigned char Tx_Count_485;
extern uint8_t aRxBuffer [BUFFERSIZE];
extern unsigned char Tx_Flag;
extern unsigned char Tx_Flag_485;
extern unsigned char NextID;
extern unsigned char MyIdIdFirst;


int timecheck=0;
int timecheck_485=0;
int SensorDataValid=FALSE;
int IsMyTurn=FALSE;
int CalledID=0;
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
  Tx_Count_485=8;
}


/******************************************************************************/
/*** @brief  Send RS-485 Data                                                                    ***/
/*** @param  None                                                                                     ***/
/*** @retval None                                                                                       ***/
/*****************************************************************************/
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
        /* Send Transaction data */
        USART_SendData(USART2, aTxBuffer[8-Tx_Count_485]);
        Tx_Count_485--;
        if(Tx_Count_485==0)
        {
          Tx_Flag_485=RESET;
        }
      }
    }
    for (tmp=0; tmp<10000; tmp++){}
    rs485_dir(0);
  } 
}


/******************************************************************************/
/*** @brief  RS-485 Tx Enable                                                                     ***/
/*** @param  None                                                                                     ***/
/*** @retval None                                                                                       ***/
/*****************************************************************************/
int rs485_dir(int rx)
{
  
   if(rx==0) GPIO_ResetBits(GPIOA, GPIO_Pin_1);
   else GPIO_SetBits(GPIOA, GPIO_Pin_1);
  
   return rx;
}


/******************************************************************************/
/*** @brief  Next Node Call                                                                          ***/
/*** @param  None                                                                                     ***/
/*** @retval None                                                                                       ***/
/*****************************************************************************/
void RequestNextNode(void)
{
  uint8_t Crc=0;  
  uint8_t uiTxDataCount;

  aTxBuffer[0]=0x55; 
  aTxBuffer[1]=0x00; 
  aTxBuffer[2]=MYID;   
  aTxBuffer[3]=0x00;
  aTxBuffer[4]=NextID;
  aTxBuffer[5]=0xFF;
  aTxBuffer[6]=0xFF;        

  for(uiTxDataCount=0 ; uiTxDataCount<7 ; uiTxDataCount++)
  {
    Crc=Crc+aTxBuffer[uiTxDataCount];
  }  
  aTxBuffer[7]=Crc;
  
  Tx_Count_485=8;  
  Tx_Flag_485=SET;
  Send485Data();  
    
  GPIO_ToggleBits(GPIOA, LED4_PIN);
}

/******************************************************************************/
/*** @brief  RS-485 Data Rx/Tx Propcess                                                     ***/
/*** @param  None                                                                                     ***/
/*** @retval None                                                                                       ***/
/*****************************************************************************/
void RS485DataProcess(void)
{
  int tmp;
  timecheck_485++;

  CalledID=aRxBuffer_485[4];

  /////////////////////////// 수신 센서 데이터 -> RS-485  Tx ////////////////////////
  // ForcedTxDne == TRUE : 센서로부터 정상데이터가 들어온 상태로, Tx 대기 상태//
  // 일정 시간 내에 자신에게 Pool이 수신되지 않으면 강제 Rx 수행              //
  /////////////////////////////////////////////////////////////////////////////////////////
  if( (SensorDataValid == TRUE) && (Rx_SensorData_Count >6)  )
  {
    if (CalledID==MYID  || MyIdIdFirst==TRUE)
    {

      ForcedTxTimer=0;
      ForcedTxDne=TRUE;
      for(tmp=0; tmp<3000; tmp++) {}
      
      Rx_SensorData_Count = 0;   
      MakeTxData(TRUE);
      Tx_Flag_485=SET;
      Send485Data();
      timecheck =0;
      SensorDataValid=FALSE;
      
      GPIO_ToggleBits(GPIOA, LED3_PIN);    
      
//      if (NextID > MYID)
//      {
        NextID=2;
        for(tmp=0; tmp<300000; tmp++) {}
        MakeTxData(TRUE);
        Tx_Flag_485=SET;      
        RequestNextNode();
//      }

    }
    else
    {
    }
  } 
  else if(Rx_Compli_Flag == RESET && timecheck>3000000)  // 센서 데이터 없음
  {
    timecheck =0;
    MakeTxData(FALSE);
    Tx_Flag_485=SET;
    Send485Data();
    GPIO_ToggleBits(GPIOA, LED3_PIN);     
  }  
  else
  {
   
  }

}

/******************************************************************************/
/*** @brief  센서 데이터 체크 및 RS-485 Tx Trigger                                         ***/
/*** @param  None                                                                                     ***/
/*** @retval None                                                                                       ***/
/*****************************************************************************/
void SensorDataProcess(void)
{
  timecheck++;
  
  ////////////////////////////////////// ForcedTxDne //////////////////////////////////////
  // ForcedTxDne == TRUE : 센서로부터 정상데이터가 들어온 상태로, Tx 대기 상태//
  // 일정 시간 내에 자신에게 Pool이 수신되지 않으면 강제 Rx 수행                        //
  ////////////////////////////////////////////////////////////////////////////////////////////
  if (ForcedTxDne==FALSE)        
  {
    ForcedTxTimer++;
  }
#if 0 
  if (ForcedTxTimer>(400000+(MYID*20000)))
  {
    ForcedTxTimer=0;
    ForcedTxDne=TRUE;
    Rx_SensorData_Count = 0;   
    MakeTxData(TRUE);
    Tx_Flag_485=SET;
    Send485Data();  
    timecheck =0;
  }
#endif  

  ////////// 정상 센서 데이터 수신 확인, RS-485 TX Trigger//////////////////////
  // ForcedTxDne = TRUE 강제 전송이 가능하도록 설정                              //
  // 센서로 부터 수신한 데이터가 정상이 아닌경우, 버림                              //
  // 일정시간 센서 데이터가 없는 경우 Invalid Value Tx Trigger                    //
  ////////////////////////////////////////////////////////////////////////////////////  
  if(Rx_Compli_Flag == SET && timecheck>100000)
  {
      if(aRxBuffer[0]==0xAA && aRxBuffer[1]==0xAA         // 정상 데이터 확인
         && aRxBuffer[2]==0x53 && aRxBuffer[3]==0x52)
      {      
        SensorDataValid=TRUE;
        ForcedTxDne=FALSE;
        GPIO_ToggleBits(GPIOA, LED1_PIN);         
      }
      else  // 비정상 데이터 -> 버림
      {
        Rx_SensorData_Count=0;
        Rx_Compli_Flag=RESET;
        GPIO_ToggleBits(GPIOA, LED2_PIN);    
      }
      Rx_Compli_Flag=RESET;
  }
  else  //함수 커버리지
  {
  }
  ////////////////////////////////////////////////////////////////////////////////////  
}



/******************************************************************************/
/*** @brief  RS-485 Input 처리                                                                     ***/
/*** @param  None                                                                                     ***/
/*** @retval None                                                                                       ***/
/*****************************************************************************/
void RS485InputProcess(void)
{
  timecheck_485++;

  if(Rx_Compli_Flag_485 == SET && timecheck_485>100000)
  {
      Rx_Count_485=0;
      Rx_Compli_Flag_485=RESET;
      
      if(aRxBuffer_485[0]==0x55 && aRxBuffer_485[1]==0x00)
      {
        GPIO_ToggleBits(GPIOA, LED1_PIN);    
      }
      if(aRxBuffer_485[4]==0x02 )
      {
        GPIO_ToggleBits(GPIOA, LED2_PIN);    
      }
  }
  else  //함수 커버리지
  {
  }
  ////////////////////////////////////////////////////////////////////////////////////  
}
