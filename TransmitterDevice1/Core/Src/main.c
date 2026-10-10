/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "stm32f1xx_ll_tim.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_spi.h"
#include "stm32f1xx_ll_utils.h"
#include "systemConf.h"
#include "nrf24l01.h"
#include <stdint.h>
#include <dht11.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
Telemetry_Frame reData;
Telemetry_Frame rxData;
uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
uint16_t SUM;
uint8_t Presence = 0;
uint8_t result =0;
uint32_t sensor_timer = 0;
float Kp = 30.0;
float Kd = 2.0;
float Ki = 10.0;
int target_temp = 0;
float total_error = 0.0;
float prev_error = 0.0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int Calculate_PID(int Temprature){
float error = (float)(Temprature - target_temp);

  if (error <= 0) {
      total_error = 0; 
      prev_error = 0;  
      return 0;        
  }

  total_error = total_error + error;
  
  if(total_error > 200.0) total_error = 200.0;
  if(total_error < 0.0) total_error = 0.0;

  float error_diff = error - prev_error;
  float P = Kp * error;
  float I = Ki * total_error;
  float D = Kd * error_diff;

  float total_PID = P + I + D;

  prev_error = error;
  
  if(total_PID > 999.0) total_PID = 999.0;
  if(total_PID < 0.0) total_PID = 0.0;
  
  return (int)total_PID;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  //MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
SystemConfig();
nrf24_DefaultConfiguration(); 
  nrf24_FlushRX(); 
  nrf24_FlushTX(); 
  nrf24_WriteReg(0x07, 0x70);
nrf24_AwakeForRx();           
LL_mDelay(2);

CE_Enable();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
if (nrf24_DataAvailable() == 1) {

            while ((nrf24_ReadReg(0x17) & 0x01) == 0) {

                nrf24_Receive((uint8_t *)&rxData, 11);

                if (rxData.Target_ID == MY_NODE_ID) {

                    if (rxData.Cmd_Type == CMD_SET_TARGET_TEMP) {
                        target_temp = rxData.data[0]; 
                    }
                    else if (rxData.Cmd_Type == CMD_RELAY_CTRL) {
                        if (rxData.data[0] == 0x00) {
                            if (rxData.data[1] == 0x01) {
                                LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_3);
                            } else {
                                LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_3);
                            }
                        }
                        else if (rxData.data[0] == 0x01) {
                            if (rxData.data[1] == 0x01) {
                                LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_7);
                            } else {
                                LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_7);
                            }
                        }
                    }
                    else if (rxData.Cmd_Type == CMD_DHT_11_TEMPRATURE) {

                        DHT11_Start();
                        Presence = DHT11_Check_Response();

                        if (Presence == 1) {
                            Rh_byte1   = DHT11_Read();
                            Rh_byte2   = DHT11_Read();
                            Temp_byte1 = DHT11_Read();
                            Temp_byte2 = DHT11_Read();
                            SUM        = DHT11_Read();

                            if (SUM == (Rh_byte1 + Rh_byte2 + Temp_byte1 + Temp_byte2)) {
                                
                                int new_PID_Speed = Calculate_PID(Temp_byte1);
                                LL_TIM_OC_SetCompareCH1(TIM3, new_PID_Speed);
                                
                                reData.startByte = START_BYTE;
                                reData.stopByte = STOP_BYTE;
                                reData.Cmd_Type = CMD_DHT_11_TEMPRATURE;
                                reData.Payload_Lenght = 11;
                                reData.Target_ID = ID_GATEWAY;
                                reData.Sender_ID = MY_NODE_ID;
                                reData.data[0] = Rh_byte1;
                                reData.data[1] = Rh_byte2;
                                reData.data[2] = Temp_byte1;
                                reData.data[3] = Temp_byte2;
                                reData.data[4] = SUM;
                                
                                LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_0);
                                nrf24_Transmit((uint8_t *)&reData, sizeof(Telemetry_Frame));
                                nrf24_AwakeForRx();
                                CE_Enable();
                            }
                        }
                        else {
                            LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_1);
                        }
                    }
                }
            } // RX FIFO Boşaltma Döngüsü Sonu
        }
        LL_mDelay(10);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_8);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_SetSystemCoreClock(64000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
