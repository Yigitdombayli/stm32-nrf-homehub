#include "systemConf.h"
#include "stm32g071xx.h"
#include "stm32g0xx_hal_cortex.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_spi.h"
#include "stm32g0xx_ll_usart.h"

void SystemConfig(){
    GPIO_Config();
    SPI_Config();
    USART_Config();
    NvicConfig();
}

void GPIO_Config(){
    // Colck Enable for GPIOA
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    // Colck Enable for GPIOB
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

    // SPI MOSI, MISO, SCK Config for PINs : PA5 -> SCK - PA6 -> MISO - PA7 -> MOSI
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Pin = LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    LL_GPIO_Init(GPIOA,&GPIO_InitStruct);
 
    // PB0 -> CSN Signal output for nrf -- PB1 -> CE Signal Output for nrf
    // PB11 -> Transmit Control LED for PC to ROOM1 (Output).
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_11;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // PA0 -> Transmit Successful LED -- PA1 -> Transmit Fail LED 
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;

    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    
    // PA2 -> TX (Alternate Function Push-Pull)
    // PA3 -> RX (Floating Input) 
    GPIO_InitStruct.Pin = LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void SPI_Config(){
    // Clock Enable for SPI1
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

    LL_SPI_InitTypeDef SPI_InitStruct = {0};
    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT; // Use software slave select
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV256;
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    
    LL_SPI_Init(SPI1, &SPI_InitStruct);
    LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);

    /* 4. Enable SPI Peripheral */
    LL_SPI_Enable(SPI1);
}

void USART_Config(void){
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
    LL_USART_InitTypeDef USART_InitStruct = {0};
    USART_InitStruct.BaudRate = 9600;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX; 
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE; 
    
    LL_USART_Init(USART2, &USART_InitStruct);

    LL_USART_Enable(USART2);
    LL_USART_EnableIT_RXNE(USART2);
    LL_USART_EnableIT_ERROR(USART2);
}

void NvicConfig(){
    NVIC_SetPriority(USART2_IRQn,3);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
}