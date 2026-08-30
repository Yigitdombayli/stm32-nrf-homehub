#include "uart.h"
#include "stm32g071xx.h"
#include "stm32g0xx_ll_utils.h"
#include "usart.h"
#include <stdio.h>
#include "string.h"

/**
    @brief Print message to screen via UART2
    @param message: message you want to send
*/
void printMessage(char *message){
  for(int i=0; i< strlen(message); i++)
  {
    while(!LL_USART_IsActiveFlag_TXE(USART2));
    LL_USART_TransmitData8(USART2, message[i]);
  }
}