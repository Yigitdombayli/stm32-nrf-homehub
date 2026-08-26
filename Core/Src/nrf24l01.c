#include "nrf24l01.h"
#include "stm32g071xx.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_spi.h"
#include <stdint.h>

uint8_t SPI1_TransferByte(uint8_t data) {
    while (!LL_SPI_IsActiveFlag_TXE(SPI1));
    LL_SPI_TransmitData8(SPI1, data);
    
    // RX tamponuna cevabın gelmesini bekle ve oku
    while (!LL_SPI_IsActiveFlag_RXNE(SPI1));
    return LL_SPI_ReceiveData8(SPI1);
}

// SPI Write function for sending the data.
void SPI1_SendByte(uint8_t data) {
    while (!LL_SPI_IsActiveFlag_TXE(SPI1));
    LL_SPI_TransmitData8(SPI1, data);
    
    while (LL_SPI_IsActiveFlag_BSY(SPI1));
}

void CSN_Enable(){
    LL_GPIO_ResetOutputPin(GPIOB, nrf_CSN_PIN);
}

void CSN_Disable(){
    LL_GPIO_SetOutputPin(GPIOB, nrf_CSN_PIN);
}

/** 
   * @brief Writing Function for write single byte to nrf module.
   * @param Reg: Address of the writing register.
   * @param Data: Data to be written.
*/
void nrf24_WriteReg(uint8_t Reg, uint8_t Data){
    CSN_Enable();
    SPI1_TransferByte(0x20 | (Reg & 0x1F)); 
    SPI1_TransferByte(Data);                
    CSN_Disable();
}

/**
    * @brief Reading Reg from nrf Module.
    * @param Reg: Reg to be read.
*/
uint8_t nrf24_ReadReg(uint8_t Reg){
    uint8_t reData = 0;
    CSN_Enable();
    SPI1_TransferByte((Reg & 0x1F));
    reData = SPI1_TransferByte(0xFF);
    CSN_Disable();
    return reData;
}

/**
    * @brief Default Configuration Value for nrf Module.
             PLL Lock Disable
             2 Mbps Speed Selected
             0dBm Power Selected
             EN_AA Enable for all data pipe
             SETUP_RETR 750 uS wait and 15 Re-Tranmit on fail
*/  
void nrf24_DefaultConfiguration(){
    // Set nrf Frequency channel to operate.
    nrf24_WriteReg(nrf_RF_CH, 0x4C);

    // PLL Lock Disable
    // 2 Mbps Speed Selected
    // 0dBm Power Selected
    nrf24_WriteReg(nrf_RF_SETUP, 0x0F);

    // EN_AA Enable for all data pipe
    nrf24_WriteReg(nrf_EN_AA, 0x3F);

    // SETUP_RETR 750 uS wait and 15 Re-Tranmit on fail
    nrf24_WriteReg(nrf_SETUPT_RETR, 0x2F);
}

/**
    *@brief Wake up to module in Tx Mode, CRC Enable.
*/
void nrf24_AwakeForTx(){
    nrf24_WriteReg(nrf_CONFIG, 0x0E);
}

/**
    *@brief Wake up to module in Rx Mode, CRC Enable.
*/
void nrf24_AwakeForRx(){
    nrf24_WriteReg(nrf_CONFIG, 0x0F);
}