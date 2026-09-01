/* 
    System Configuration files for Peripheral config, GPIO Config or Communication Config.
*/
#include "main.h"
/********************************** DEVICE ID LIST **************************************/
#define ID_PC                          0x00
#define ID_GATEWAY                     0xFF
#define ID_ROOM_1                      0x01
#define ID_ROOM_2                      0x02

#define  MY_NODE_ID                    ID_GATEWAY

// Command List
#define CMD_RELAY_CTRL                 0x20
#define CMD_DHT_11_TEMPRATURE          0x10


#define  STOP_BYTE                     0xAA
#define START_BYTE                     0x55

/********************************** TELEMETRY FRAME ****************************************/
typedef struct __attribute__((packed)){
    uint8_t startByte;  
    uint8_t Sender_ID;
    uint8_t Target_ID;
    uint8_t Cmd_Type;
    uint8_t Payload_Lenght;
    uint8_t data[5];    
    uint8_t stopByte; 
}Telemetry_Frame; //11 byte


void SystemConfig();
void GPIO_Config();
void SPI_Config();
void USART_Config(void);