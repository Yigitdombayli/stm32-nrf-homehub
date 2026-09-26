#include "systemConf.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_spi.h"
#include "stm32f1xx_ll_tim.h"

void SystemConfig(){
    GPIO_Config();
    SPI_Config();
    Timer_Config();
}

void GPIO_Config(){
    // Clock Enable for GPIOA and GPIOB (STM32F1 uses APB2 for GPIOs)
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

    // SPI MOSI, MISO, SCK Config for PINs : PA5 -> SCK - PA6 -> MISO - PA7 -> MOSI
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pin = LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    LL_SPI_Enable(SPI1);
 
    // PB0 -> CSN Signal output for nrf -- PB1 -> CE Signal Output for nrf
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_7;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // PA0 -> Receive Successful LED -- PA1 -> Receive Fail LED --- PA3 -> ROOM1 LED1 CMD
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_3;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
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
    

    /* Enable SPI Peripheral */
    LL_SPI_Enable(SPI1);
}

// Timer count up every 1 us
void Timer_Config(){
    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
	TIM_InitStruct.Prescaler = 63;
	TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_InitStruct.RepetitionCounter = 0;
	TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	TIM_InitStruct.Autoreload = 0xFFFF-1;
	LL_TIM_Init(TIM1, &TIM_InitStruct);
	LL_TIM_EnableCounter(TIM1);
}

