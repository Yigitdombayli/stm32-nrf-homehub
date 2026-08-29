
/*  
          Configuration file for dht11 sensor.            

*/
#include "main.h"
#include <stdint.h>

#define DHT11_PORT                      GPIOA
#define DHT11_PIN                       LL_GPIO_PIN_2



void delay_us(uint16_t us);
void Set_Pin_Output(GPIO_TypeDef *GPIOx, uint32_t PinMask);
void Set_Pin_Input(GPIO_TypeDef *GPIOx, uint32_t PinMask);
void DHT11_Start(void);
uint8_t DHT11_Check_Response(void);
uint8_t DHT11_Read(void);