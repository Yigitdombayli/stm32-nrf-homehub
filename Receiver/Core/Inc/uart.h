#include "stm32g071xx.h"
#include "stm32g0xx_ll_utils.h"
#include "main.h"

/*
    UART Funcition file.
*/

#define DHT11_STOPBYTE                  0x55
#define DHT11_STARTBYTE                 0xAA


/**
    data[2] -> Temprature Value Integer part
    data[0] -> Humidity Value Integer part
*/
typedef struct __attribute__((packed)){
    uint8_t startByte;  
    uint8_t data[5];    
    uint8_t stopByte; 
}DHT11_Frame; // 7 byte frame.


void printMessage(char *message);
