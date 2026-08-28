#include "nrf24l01.h"
#include "stm32f1xx.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_spi.h"
#include <stdint.h>

uint8_t SPI1_TransferByte(uint8_t data) {
    while (!LL_SPI_IsActiveFlag_TXE(SPI1));
    LL_SPI_TransmitData8(SPI1, data);
    

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

void CE_Enable(){
    LL_GPIO_SetOutputPin(GPIOB, nrf_CE_PIN);
}

void CE_Disable(){
    LL_GPIO_ResetOutputPin(GPIOB, nrf_CE_PIN);
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

    // Byte 0 Payload width 5 byte
    nrf24_WriteReg(0x11, 5);
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

/**
 * @brief nRF24L01 Transmit Function.
 * @param *payload: Tranmitted Data Array
 * @param size: Byte Lenght
 */
void nrf24_Transmit(uint8_t *payload, uint8_t size){
    
    CSN_Enable();
    SPI1_TransferByte(0xA0); 
    for(uint8_t i = 0; i < size; i++) {
        SPI1_TransferByte(payload[i]);
    }
    CSN_Disable(); 

    CE_Enable();
    for(volatile int wait = 0; wait < 100; wait++) {
        __NOP(); 
    }
    
    CE_Disable(); 
}

/**
 * @brief It checks whether the sent data has been received.
 * @retval 1: Done (ACK received)
 * @retval 0: Failed 
 * @retval 2: Waiting for operation
 */
uint8_t nrf24_CheckTransmission(void){
    uint8_t status = nrf24_ReadReg(nrf_STATUS); 
    
    if(status & (1 << 5)) { 
        nrf24_WriteReg(nrf_STATUS, (1 << 5)); 
        return 1; 
    }
    
    if(status & (1 << 4)) { 
        nrf24_WriteReg(nrf_STATUS, (1 << 4)); 
        
        CSN_Enable();
        SPI1_TransferByte(0xE1); 
        CSN_Disable();
        
        return 0; 
    }
    return 2; 
}

/**
 * @brief Check the data at RX
 * @retval 1: Avaiable
 * @retval 0: Empty
 */
uint8_t nrf24_DataAvailable(void){
    uint8_t status = nrf24_ReadReg(0x07);
    
    if(status & (1 << 6)) {
        nrf24_WriteReg(0x07, (1 << 6)); 
        return 1; 
    }
    
    return 0; 
}

/**
 * @brief It reads the data coming into the chip 
 * @param *data: The array where the read data will be saved.
 * @param size: Size of Data to be read.
 */
void nrf24_Receive(uint8_t *data, uint8_t size){
    CSN_Enable(); 
    SPI1_TransferByte(0x61); 
    for(uint8_t i = 0; i < size; i++) {
        data[i] = SPI1_TransferByte(0xFF);
    }
    CSN_Disable(); 
}