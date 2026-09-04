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
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "cmsis_os2.h"
#include "spi.h"
#include "stm32g071xx.h"
#include "stm32g0xx_ll_utils.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "systemConf.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_spi.h"
#include "nrf24l01.h"
#include <stdint.h>
#include "queue.h"
#include <stdio.h>
#include "string.h"
#include "uart.h"
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
/******************************** QUEUES *************************************/
QueueHandle_t UartQueue;
QueueHandle_t UartRxQueue;
/******************************** TASKS *************************************/
TaskHandle_t UART_Task = NULL;
TaskHandle_t nrf24_ReceiveTask = NULL;
TaskHandle_t UART_Parser = NULL;


/************************* PRIVATE VARIABLES ******************************/
Telemetry_Frame telemetry_data = {0};
uint8_t result =0;
static uint8_t rx_buffer[11];
static uint8_t rx_index = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void nrf24_ReceiveTaskHandler(void *params){
  while (1) {
        if (nrf24_DataAvailable() == 1) {
        
          nrf24_Receive((uint8_t *)&telemetry_data, sizeof(Telemetry_Frame));
          if(telemetry_data.Target_ID == MY_NODE_ID)
          {
            LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_0);
            LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_1);
            xQueueSend(UartQueue, &telemetry_data, 0);
          }
        }
        else{
          LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1);
          LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_0);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
      }
}
void UART_TaskHandler(void *params){
  Telemetry_Frame received_data;
  while (1) {
               if (xQueueReceive(UartQueue, &received_data, portMAX_DELAY) == pdPASS) {
              uint8_t *frame_ptr = (uint8_t *)&received_data;
              
              for(int i = 0; i < sizeof(Telemetry_Frame); i++) {
                  while(!LL_USART_IsActiveFlag_TXE(USART2)); 
                  LL_USART_TransmitData8(USART2, frame_ptr[i]);
            }
        }
     }
}
void vTask_UartParser(void *pvParameters) {
    Telemetry_Frame incomingFrame;

    while(1){
        if (xQueueReceive(UartRxQueue, &incomingFrame, portMAX_DELAY) == pdPASS) {
            
            if(incomingFrame.startByte == 0x55 && incomingFrame.stopByte == 0xAA) {
                
                if(incomingFrame.Target_ID == ID_ROOM_1) {
                    nrf24_Transmit((uint8_t*)&incomingFrame, 11);
                    LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_11);
                }
            }
        }
    }
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
  //MX_USART2_UART_Init();
 // MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  SystemConfig();

  /************************************ Configuration nrf ************************/
  nrf24_DefaultConfiguration(); 
  nrf24_AwakeForRx();          
  CE_Enable();
  HAL_Delay(2);
  /*********************************** CREATE QUEUES *****************************/
  UartQueue = xQueueCreate(10, sizeof(Telemetry_Frame));
  UartRxQueue = xQueueCreate(5, sizeof(Telemetry_Frame));

  /************************************  TASK CREATE  ****************************/
  xTaskCreate(nrf24_ReceiveTaskHandler, "nrf24_Receive", 128, NULL, 3, &nrf24_ReceiveTask);
  xTaskCreate(UART_TaskHandler, "UART",128,NULL,2,&UART_Task);
  xTaskCreate(vTask_UartParser, "UART Parser", 128, NULL, 3, &UART_Parser);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

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
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
  {
  }

  /* HSI configuration and activation */
  LL_RCC_HSI_Enable();
  while(LL_RCC_HSI_IsReady() != 1)
  {
  }

  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_1, 8, LL_RCC_PLLR_DIV_2);
  LL_RCC_PLL_Enable();
  LL_RCC_PLL_EnableDomain_SYS();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  }

  /* Set AHB prescaler*/
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

  /* Sysclk activation on the main PLL */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  }

  /* Set APB1 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(64000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void USART2_IRQHandler(void)
{
    if (LL_USART_IsActiveFlag_RXNE(USART2) && LL_USART_IsEnabledIT_RXNE(USART2))
    {
        rx_buffer[rx_index++] = LL_USART_ReceiveData8(USART2);
        
        if (rx_index >= 11)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xQueueSendFromISR(UartRxQueue, rx_buffer, &xHigherPriorityTaskWoken);
            rx_index = 0;

            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }

    if (LL_USART_IsActiveFlag_ORE(USART2) || LL_USART_IsActiveFlag_FE(USART2) || LL_USART_IsActiveFlag_NE(USART2))
    {
        LL_USART_ClearFlag_ORE(USART2);
        LL_USART_ClearFlag_FE(USART2);
        LL_USART_ClearFlag_NE(USART2);
        rx_index = 0; 
    }
}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
