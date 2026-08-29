#include "dht11.h"
#include "main.h"
#include "stm32f103xb.h"
#include "stm32f1xx_ll_gpio.h"
#include <stdint.h>
#include "stm32f1xx_ll_tim.h"  
#include "stm32f1xx_ll_utils.h"

void delay_us(uint16_t us) {
    LL_TIM_SetCounter(TIM1, 0); 
    while (LL_TIM_GetCounter(TIM1) < us); 
}

void Set_Pin_Output(GPIO_TypeDef *GPIOx, uint32_t PinMask) {
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = PinMask;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    LL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void Set_Pin_Input(GPIO_TypeDef *GPIOx, uint32_t PinMask) {
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = PinMask;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    LL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

/**
    *@brief DHT11 Sensor start function.
*/
void DHT11_Start(void) {
    Set_Pin_Output(DHT11_PORT, DHT11_PIN);
    LL_GPIO_ResetOutputPin(DHT11_PORT, DHT11_PIN); 
    delay_us(18);                                                                
    Set_Pin_Input(DHT11_PORT, DHT11_PIN);         
}


/**
    *@brief Response Control function for DHT11
*/
uint8_t DHT11_Check_Response(void) {
    uint8_t response = 0;
    delay_us(40);
    if (!(LL_GPIO_IsInputPinSet(DHT11_PORT, DHT11_PIN))) {
        delay_us(80);
        if (LL_GPIO_IsInputPinSet(DHT11_PORT, DHT11_PIN)) {
            response = 1;
        }
        else {
            response = -1;
        }
    }
    while (LL_GPIO_IsInputPinSet(DHT11_PORT, DHT11_PIN));
    return response;
}

/**
    @brief Read sensor value from DHT11.
*/
uint8_t DHT11_Read(void) {
    uint8_t j = 0;
    uint8_t response = 0;
    for(j = 0; j < 8; j++)
    {
        while(!(LL_GPIO_IsInputPinSet(DHT11_PORT, DHT11_PIN)));
        delay_us(40);
        if(LL_GPIO_IsInputPinSet(DHT11_PORT, DHT11_PIN))
        {
            response |= (1 << (7 - j));
            while(LL_GPIO_IsInputPinSet(DHT11_PORT, DHT11_PIN));
        }
        else
        {
            response &= ~(1 << (7 - j));
        }
    }
    
    return response;
}