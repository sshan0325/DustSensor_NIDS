#include "stm32f4xx.h"
#include "stm32f4xx_i2c.h"
#include "I2C.h"
#include "LED_Con.h"
#include "stdio.h"



void init_I2C2(void){
	
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;
	
	// enable APB1 peripheral clock for I2C2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	// enable clock for SCL and SDA pins
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	/* setup SCL and SDA pins
	 * You can connect I2C2 to two different
	 * pairs of pins:
	 * 1. SCL on PB10 and SDA on PB9
	 * 2. SCL on PB8 and SDA on PB9
	 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9; // we are going to use PB6 and PB7
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;			// set pins to alternate function
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		// set GPIO speed
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;			// set output to open drain --> the line has to be only pulled low, not driven high
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;			// enable pull up resistors
	GPIO_Init(GPIOB, &GPIO_InitStruct);					// init GPIOB
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10; // we are going to use PB6 and PB7
        GPIO_Init(GPIOB, &GPIO_InitStruct);					// init GPIOB
	
	// Connect I2C2 pins to AF  
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);	// SCL
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C2); // SDA
	
        I2C_Cmd(I2C2, DISABLE);
        I2C_SoftwareResetCmd(I2C2, ENABLE);
        I2C_SoftwareResetCmd(I2C2, DISABLE);
        
        
	// configure I2C2
	I2C_InitStruct.I2C_ClockSpeed = 100000; 		// 100kHz
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;			// I2C mode
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;	// 50% duty cycle --> standard
	I2C_InitStruct.I2C_OwnAddress1 = 0xAA;			// own address, not relevant in master mode
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;		// disable acknowledge when reading (can be changed later on)
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // set address length to 7 bit addresses
	I2C_Init(I2C2, &I2C_InitStruct);				// init I2C2
	
	// enable I2C2
	I2C_Cmd(I2C2, ENABLE);
}

void init_I2C1(void){
	
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;
	
	// enable APB1 peripheral clock for I2C2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	// enable clock for SCL and SDA pins
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	/* setup SCL and SDA pins
	 * You can connect I2C1 to two different
	 * pairs of pins:
	 * 1. SCL on PB10 and SDA on PB9
	 * 2. SCL on PB8 and SDA on PB9
	 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6; // we are going to use PB6 and PB7
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;			// set pins to alternate function
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		// set GPIO speed
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;			// set output to open drain --> the line has to be only pulled low, not driven high
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;			// enable pull up resistors
	GPIO_Init(GPIOB, &GPIO_InitStruct);					// init GPIOB
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7; // we are going to use PB6 and PB7
        GPIO_Init(GPIOB, &GPIO_InitStruct);					// init GPIOB
	
	// Connect I2C2 pins to AF  
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);	// SCL
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1); // SDA
	
        I2C_Cmd(I2C1, DISABLE);
        I2C_SoftwareResetCmd(I2C1, ENABLE);
        I2C_SoftwareResetCmd(I2C1, DISABLE);
        
        
	// configure I2C1
	I2C_InitStruct.I2C_ClockSpeed = 100000; 		// 100kHz
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;			// I2C mode
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;	// 50% duty cycle --> standard
	I2C_InitStruct.I2C_OwnAddress1 = 0x55;			// own address, not relevant in master mode
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;		// disable acknowledge when reading (can be changed later on)
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // set address length to 7 bit addresses
	I2C_Init(I2C1, &I2C_InitStruct);				// init I2C2
	
	// enable I2C2
	I2C_Cmd(I2C1, ENABLE);
}

/* This function issues a start condition and 
 * transmits the slave address + R/W bit
 * 
 * Parameters:
 * 		I2Cx --> the I2C peripheral e.g. I2C2
 * 		address --> the 7 bit slave address
 * 		direction --> the tranmission direction can be:
 * 						I2C_Direction_Tranmitter for Master transmitter mode
 * 						I2C_Direction_Receiver for Master receiver
 */
void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction){
	// wait until I2C2 is not busy anymore
        printf ("Chaeck I2C State is Busy \r\n");
#if 1  
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
#else
        if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
        {
          printf ("I2C State is Busy \r\n");      
          GPIO_ToggleBits(GPIOA, LED2_PIN);
          //break;
        }
#endif        
                 
	// Send I2C2 START condition 
	I2C_GenerateSTART(I2Cx, ENABLE);
	  
	// wait for I2C2 EV5 --> Slave has acknowledged start condition
#if 1        
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
#else        
        if (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
        {
          //break;
          printf ("I2C Event is not master Mode \r\n");  
          GPIO_ToggleBits(GPIOA, LED3_PIN);
        }
#endif
	// Send slave Address for write 
	I2C_Send7bitAddress(I2Cx, address, direction);
	  
	/* wait for I2C2 EV6, check if 
	 * either Slave has acknowledged Master transmitter or
	 * Master receiver mode, depending on the transmission
	 * direction
	 */ 
	if(direction == I2C_Direction_Transmitter){
                printf ("Check I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED \r\n"); 
#if 1          
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
                 printf ("Check I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED DOne \r\n"); 
#else
                if (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
                {
                  printf ("I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED is not Set \r\n"); 
                  GPIO_ToggleBits(GPIOA, LED4_PIN);
                }
#endif                
                
	}
	else if(direction == I2C_Direction_Receiver){
#if 0          
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
#else          
                if (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
                {
                  GPIO_ToggleBits(GPIOA, LED4_PIN);
                  printf ("I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED is not Set \r\n"); 
                }
#endif                
	}
}

/* This function transmits one byte to the slave device
 * Parameters:
 *		I2Cx --> the I2C peripheral e.g. I2C2
 *		data --> the data byte to be transmitted
 */
void I2C_write(I2C_TypeDef* I2Cx, uint8_t data)
{
	I2C_SendData(I2Cx, data);
	// wait for I2C2 EV8_2 --> byte has been transmitted
#if 0        
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
#else
        if(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
          printf ("I2C_EVENT_MASTER_BYTE_TRANSMITTED is not Set \r\n"); 
#endif        
}

/* This function reads one byte from the slave device 
 * and acknowledges the byte (requests another byte)
 */
uint8_t I2C_read_ack(I2C_TypeDef* I2Cx){
        
	// enable acknowledge of recieved data
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	// wait until one byte has been received
#if 0        
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
#else        
        int tmp=0;
        for (tmp=0 ; tmp<5000 ; tmp++) {}
#endif        
	// read data from I2C data register and return data byte
	uint8_t data = I2C_ReceiveData(I2Cx);
	return data;
}

/* This function reads one byte from the slave device
 * and doesn't acknowledge the recieved data 
 */
uint8_t I2C_read_nack(I2C_TypeDef* I2Cx){
	// disabe acknowledge of received data
	// nack also generates stop condition after last byte received
	// see reference manual for more info
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	I2C_GenerateSTOP(I2Cx, ENABLE);
	// wait until one byte has been received
	//while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
        int tmp=0;
        for (tmp=0 ; tmp<5000 ; tmp++) {}        
	// read data from I2C data register and return data byte
	uint8_t data = I2C_ReceiveData(I2Cx);
	return data;
}

/* This funtion issues a stop condition and therefore
 * releases the bus
 */
void I2C_stop(I2C_TypeDef* I2Cx){
	// Send I2C2 STOP Condition 
	I2C_GenerateSTOP(I2Cx, ENABLE);
}