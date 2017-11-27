
#define DATAINIT        0xAA
#define DATAINITLENGTH  1
#define DATAPOSITION    9
#define DATALENGTH      6

#define TRUE            1
#define FALSE           0

#define MYID            3

void MakeTxData(uint8_t ucValidInput);
void Send485Data(void);
int rs485_dir(int rx);
void SensorDataProcess(void);
void RS485DataProcess(void);
void RequestNextNode(void);