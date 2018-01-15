#include "stm32f4xx_conf.h"
#include "USART_Data.h"
#include "USART.h"
#include "LED_Con.h"

extern uint8_t aTxBuffer[];
extern uint8_t aRxBuffer[];
extern uint8_t aRxBuffer_485[];
extern unsigned char Rx_Compli_Flag;
extern unsigned char Rx_SensorData_Count;
extern unsigned char Rx_SensorData_Count;
extern unsigned char Rx_Count_485;

extern unsigned char Tx_Count_485;
extern uint8_t aRxBuffer [BUFFERSIZE];
extern unsigned char Tx_Flag;
extern unsigned char Tx_Flag_485;
extern unsigned char NextID;
extern unsigned char MyIdIsFirst;


int timecheck=0;
int SensorDataValid=FALSE;
int CalledID=FALSE;
int CalledByID=0;
int CurrentID=0;
int ForcedTxDne=FALSE;
int ForcedTxTimer=0;
int Rx485DataPosition=0;
int Rx485ValidDataPosition=0;
int AckWatingCount=0;
int NetworkLinked=FALSE;

unsigned int SensedValue[3][2];
unsigned int SensedCount=0;
unsigned int SensedAverageValue[2];



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
#if 1
  else
  {
    aTxBuffer[3]=0xFF;
    aTxBuffer[4]=0xFF;
    aTxBuffer[5]=0xFF;
    aTxBuffer[6]=0xFF;    
  }
#else
  else
  {
    for (uiTxDataCount=0; uiTxDataCount<=7 ; uiTxDataCount++)
    {
      aTxBuffer[uiTxDataCount]=aRxBuffer_485[Rx485DataPosition-8+uiTxDataCount];
    }   
  }  
#endif

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
  uint8_t uiTxDataCountForNextNodeCall;

  aTxBuffer[0]=0x55; 
  aTxBuffer[1]=0x00; 
  aTxBuffer[2]=NextID;   
  aTxBuffer[3]=0x00;
  aTxBuffer[4]=MYID;
  aTxBuffer[5]=0xFF;
  aTxBuffer[6]=0xFF;        

  for(uiTxDataCountForNextNodeCall=0 ; uiTxDataCountForNextNodeCall<7 ; uiTxDataCountForNextNodeCall++)
  {
    Crc=Crc+aTxBuffer[uiTxDataCountForNextNodeCall];
  }  
  aTxBuffer[7]=Crc;
  
  Tx_Count_485=8;  
  Tx_Flag_485=SET;
  Send485Data();  
  GPIO_ToggleBits(GPIOA, LED1_PIN);
  
  AckWatingCount++;
  if (AckWatingCount>5)
    NextID=127;
}

/******************************************************************************/
/*** @brief  RS-485 Data Rx/Tx Propcess                                                     ***/
/*** @param  None                                                                                     ***/
/*** @retval None                                                                                       ***/
/*****************************************************************************/
void SENDACK(void)
{
  uint8_t Crc=0;  
  uint8_t uiTxDataCountForSendACK;

  aTxBuffer[0]=0x55; 
  aTxBuffer[1]=0x55; 
  aTxBuffer[2]=CalledByID;   
  aTxBuffer[3]=0x00;
  aTxBuffer[4]=MYID;
  aTxBuffer[5]=0xFF;
  aTxBuffer[6]=0xFF;        

  for(uiTxDataCountForSendACK=0 ; uiTxDataCountForSendACK<7 ; uiTxDataCountForSendACK++)
  {
    Crc=Crc+aTxBuffer[uiTxDataCountForSendACK];
  }  
  aTxBuffer[7]=Crc;
  
  Tx_Count_485=8;  
  Tx_Flag_485=SET;
  Send485Data();    
  
  CalledByID=0;
}


/******************************************************************************/
/*** @brief  RS-485 Data Rx/Tx Propcess                                                     ***/
/*** @param  None                                                                                     ***/
/*** @retval None                                                                                       ***/
/*****************************************************************************/
void RS485DataProcess(void)
{
  int tmp;
  
  /////////////////////////// 수신 센서 데이터 -> RS-485  Tx ////////////////////////
  // ForcedTxDne == TRUE : 센서로부터 정상데이터가 들어온 상태로, Tx 대기 상태//
  // 일정 시간 내에 자신에게 Pool이 수신되지 않으면 강제 Rx 수행              //
  /////////////////////////////////////////////////////////////////////////////////////////
  if( ((SensorDataValid == TRUE) && (MyIdIsFirst==TRUE)) || (CalledID==TRUE) )
  {
    if (MyIdIsFirst==TRUE)
    {
      GPIO_ToggleBits(GPIOA, LED4_PIN);
    }
    if(CalledID==TRUE) 
    {
      for(tmp=0; tmp<350000; tmp++) {}
      SENDACK();
    }
      
    CalledID=FALSE;
    ForcedTxTimer=0;
    ForcedTxDne=TRUE;
    for(tmp=0; tmp<450000; tmp++) {}
    
    for(tmp=0; tmp<250000; tmp++) {}

    if (SensorDataValid==TRUE)
    {
      MakeTxData(TRUE);
      Tx_Flag_485=SET;
      Send485Data();      
    }
    else
    {
      //MakeTxData(FALSE);
      MakeTxData(TRUE);
    }
    

    Rx_SensorData_Count = 0;
    timecheck =0;
    SensorDataValid=FALSE;


    for(tmp=0; tmp<350000; tmp++) {}
    Tx_Flag_485=SET;      
    RequestNextNode();
  } 

  else if(timecheck>(5000000+(50000*MYID))  )
  {
    timecheck =0;
    
    MyIdIsFirst=TRUE;
      
    if (SensorDataValid==TRUE)
      MakeTxData(TRUE);
    else
      MakeTxData(FALSE);
    
    Tx_Flag_485=SET;
    Send485Data();
    Rx_SensorData_Count = 0;
    
    for(tmp=0; tmp<350000; tmp++) {}
    Tx_Flag_485=SET;      
    RequestNextNode();    
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

  ////////// 정상 센서 데이터 수신 확인, RS-485 TX Trigger//////////////////////
  // ForcedTxDne = TRUE 강제 전송이 가능하도록 설정                              //
  // 센서로 부터 수신한 데이터가 정상이 아닌경우, 버림                              //
  // 일정시간 센서 데이터가 없는 경우 Invalid Value Tx Trigger                    //
  ////////////////////////////////////////////////////////////////////////////////////  
  if(Rx_Compli_Flag == SET && timecheck>100000)
  {
      if(aRxBuffer[0]==0xAA && aRxBuffer[1]==0xAA         // 정상 데이터 여부 확인
         && aRxBuffer[2]==0x53 && aRxBuffer[3]==0x52)
      {
        if (SensedCount == 0)
        {
          SensedCount=1;
          SensedValue[0][0] = (aRxBuffer[12]*256) + aRxBuffer[13];              //첫번째 PM2.5 센싱 데이터
          SensedValue[0][1] = (aRxBuffer[14]*256) + aRxBuffer[15];              //첫번째 PM10 센싱 데이터
        }
        
        else if  (SensedCount == 1)
        {
          SensedCount=0;
          SensedValue[1][0] = (aRxBuffer[12]*256) + aRxBuffer[13];              //두번째 PM2.5 센싱 데이터
          SensedValue[1][1] = (aRxBuffer[14]*256) + aRxBuffer[15];              //두번째 PM10 센싱 데이터          
          
          SensedAverageValue[0]=(SensedValue[0][0]+SensedValue[1][0]) / 2;      //PM2.5 센싱 데이터 평균
          SensedAverageValue[1]=(SensedValue[0][1]+SensedValue[1][1]) / 2;      //PM10 센싱 데이터 평균
          
          SensorDataValid=TRUE;
          aRxBuffer[12]= SensedAverageValue[0]/256;
          aRxBuffer[13]= SensedAverageValue[0]%256;
          aRxBuffer[14]= SensedAverageValue[1]/256;
          aRxBuffer[15]= SensedAverageValue[1]%256;
            
        }
        else
        {
          SensedCount=0;
        }
      }
      else  // 비정상 데이터 -> 버림
      {
        Rx_Compli_Flag=RESET;
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
  while(Rx_Count_485>=8)
  {
    if((aRxBuffer_485[Rx485DataPosition]==0xAA) && (aRxBuffer_485[Rx485DataPosition+1]==0x00) )
    {
      if ( (aRxBuffer_485[Rx485DataPosition+2]<MYID) )
        MyIdIsFirst = 0;

      if ( (aRxBuffer_485[Rx485DataPosition+2]<NextID) && (aRxBuffer_485[Rx485DataPosition+2]>MYID) )
        NextID=aRxBuffer_485[Rx485DataPosition+2];
      
      Rx485DataPosition+=8;
      Rx_Count_485-=8;      
      if (Rx485DataPosition>=BUFFERSIZE)
        Rx485DataPosition=0;      
    }
    else if( (aRxBuffer_485[Rx485DataPosition] == 0x55)  && (aRxBuffer_485[Rx485DataPosition+1] == 0x00) )
    {
      if (aRxBuffer_485[Rx485DataPosition+2] == MYID)
      {
        Rx485ValidDataPosition=Rx485DataPosition;
        CalledID=TRUE;
        CalledByID=aRxBuffer_485[Rx485DataPosition+4];
        NetworkLinked=TRUE;
      }
      
      Rx485DataPosition+=8;
      Rx_Count_485-=8;      
      if (Rx485DataPosition>=BUFFERSIZE)
        Rx485DataPosition=0;
    }
    else if( (aRxBuffer_485[Rx485DataPosition] == 0x55)  && (aRxBuffer_485[Rx485DataPosition+1] == 0x55) )
    {
      if (aRxBuffer_485[Rx485DataPosition+2] == MYID)
      {
        GPIO_ToggleBits(GPIOA, LED2_PIN);
        AckWatingCount=0;
      }
      
      Rx485DataPosition+=8;
      Rx_Count_485-=8;      
      if (Rx485DataPosition>=BUFFERSIZE)
        Rx485DataPosition=0;
    }    
    else 
    {
      Rx485DataPosition++;
      Rx_Count_485--;
      if (Rx485DataPosition>=BUFFERSIZE)
        Rx485DataPosition=0;
    }
  }
  ////////////////////////////////////////////////////////////////////////////////////  
}
