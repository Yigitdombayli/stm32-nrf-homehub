/*
    nrf24l01 Library File Header
*/

#include "stm32g0xx_ll_utils.h"
#include <stdint.h>

/************************* DEFINES *************************************/
#define nrf_CSN_PIN                 LL_GPIO_PIN_0  //PB0
#define nrf_SCK_PIN                 LL_GPIO_PIN_5 // PA5
#define nrf_MOSI_PIN                LL_GPIO_PIN_7 // PA7
#define nrf_MISO_PIN                LL_GPIO_PIN_6 // PA6
#define nrf_SETUPT_RETR             0x04
#define nrf_EN_AA                   0x01
#define nrf_RF_SETUP                0x06
#define nrf_RF_CH                   0x05
#define nrf_CONFIG                  0x00

/************************* FUNCTIONS *************************************/
void nrf24_WriteReg(uint8_t Reg, uint8_t Data);
void SPI1_SendByte(uint8_t data);

