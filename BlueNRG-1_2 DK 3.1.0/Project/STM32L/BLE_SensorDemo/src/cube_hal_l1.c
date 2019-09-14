/**
  ******************************************************************************
  * @file    cube_hal_l1.c
  * @author  RF Application Team - AMG
  * @version V1.0.0
  * @date    27-April-2015
  * @brief   
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "cube_hal.h"
#include "hal.h"
#include "clock.h"
#include "low_power.h"
#include "SDK_EVAL_Config.h"
#include "hci_parser.h"


extern UART_HandleTypeDef DTM_UartHandle;
extern uint8_t DTM_read_data[32];

/**
 * @brief  System Clock Configuration
 * @param  None
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  
  /**
  * Enable clock on PWR block
  * This is used to setup registers when entering low power mode
  */
  __PWR_CLK_ENABLE();
  
  /**
   * Set voltage scaling range
   * The voltage scaling allows optimizing the power consumption when the device is 
   * clocked below the maximum system frequency, to update the voltage scaling value 
   * regarding system frequency refer to product datasheet.
   */
#if (HCLK_32MHZ == 1)
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Poll VOSF bit of in PWR_CSR. Wait until it is reset to 0 */
  while (__HAL_PWR_GET_FLAG(PWR_FLAG_VOS) != RESET) {};
  
  /**
   *  Enable HSI oscillator and configure the PLL to reach the max system frequency 
  *  (32MHz) when using HSI oscillator as PLL clock source: (HSI * PLLMUL)/PLLDIV= (16 MHz *6/3) = 32 Mhz
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSICalibrationValue = 0x10;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;

  /* If the USB or SDIO interface is used in the application, the PLL VCO clock (defined by
     the PLL multiplication factor) must be programmed to output a 96 MHz frequency. This
     is required to provide a 48 MHz clock to the USB or SDIO (SDIOCLK or USBCLK =
     PLLVCO/2)
  */
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6; //USB support (16MHz * 6) = 96 Mhz 
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV3; //The system clock is set to 32 MHz: (HSI * PLLMUL)/PLLDIV= (16 MHz *6/3) 

  
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  /**
  *  Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers.
  *  The SysTick 1 msec interrupt is required for the HAL process (Timeout management); by default
  *  the configuration is done using the HAL_Init() API, and when the system clock configuration
  *  is updated the SysTick configuration will be adjusted by the HAL_RCC_ClockConfig() API.
  */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;

  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
 
#elif (SYSCLK_MSI == 1)
  /*  The system clock is set to 4 MHz 
  *   System Clock source = MSI
  *   MSI Range  = 0
  *   SYSCLK(Hz) = 64000
  *   HCLK(Hz)   = 32000
  */

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2); /* For STM32L1: Low power sleep mode can only be entered when VCORE is in range 2 */

  /* Enable MSI Oscillator */
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState            = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange       = RCC_MSIRANGE_6; /*!< MSI = 4.194 MHz   */ 
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  

  /* Select MSI as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType       = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider   = RCC_SYSCLK_DIV1; // RCC_SYSCLK_DIV2; 
  RCC_ClkInitStruct.APB1CLKDivider  = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider  = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
  
#else
  /**
  * Reset value is Range 2
  */
  
  /**
  *  Enable HSI oscillator and configure the system at 16MHz
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSICalibrationValue = 0x10;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;
  
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  /**
  *  Configure the HCLK, PCLK1 and PCLK2 clocks dividers  to get 8Mhz.
  *  The SysTick 1 msec interrupt is required for the HAL process (Timeout management); by default
  *  the configuration is done using the HAL_Init() API, and when the system clock configuration
  *  is updated the SysTick configuration will be adjusted by the HAL_RCC_ClockConfig() API.
  */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
  
#endif /* (HCLK_32MHZ == 1) */
  
  return;
}

/*******************************************************************************
* Function Name  : Init_Device
* Description    : Device initialization steps using STM32L1 standard library
* Input          : None.
* Return         : None.
*******************************************************************************/
void Init_Device(void)
{
  
  /* STM32Cube HAL library initialization:
  */
  HAL_Init();
  
#if ENABLE_MICRO_SLEEP 
  /* Disable Prefetch Buffer */
  __HAL_FLASH_PREFETCH_BUFFER_DISABLE();
#endif
  
  /* Configure the system clock */
  SystemClock_Config(); 
  
#if ENABLE_MICRO_SLEEP //TBR
  /* Configure the system Power */
  SystemPower_Config();
#endif 
  
#ifdef SPI_INTERFACE
  /* Init SPI interface */
  SdkEvalSpiInit(SPI_MODE_EXTI);//SPI_MODE_DMA);
#endif
  
#ifndef ENABLE_MICRO_SLEEP
  /* Init the UART peripheral */
  SdkEvalComUartInit(UART_BAUDRATE);
#endif
  
#ifdef UART_INTERFACE
  DTM_IO_Config();
#endif
  
  /* Configure the BlueNRG-1 pins - RESET pin */
  Sdk_Eval_Gpio_Init();
}


/**
  * @brief UART MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{  
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  if(huart->Instance == USART2) {
  UART_TX_CLOCK();
  UART_RX_CLOCK();
  
  /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStruct.Pin       = UART_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = UART_TX_AF;
  HAL_GPIO_Init(UART_TX_PORT, &GPIO_InitStruct);
  
  /* Configure USART Rx as input floating */
  GPIO_InitStruct.Pin = UART_RX_PIN;
  GPIO_InitStruct.Alternate = UART_RX_AF;
  HAL_GPIO_Init(UART_RX_PORT, &GPIO_InitStruct);
  
  /* Configure the NVIC for SPI */  
  HAL_NVIC_SetPriority(UART_IRQ, 4, 0); //TBR Which priority < spi
  HAL_NVIC_EnableIRQ(UART_IRQ);
  }
  
  if(huart->Instance == USART1) {
    DTM_USART_TX_GPIO_CLK_ENABLE();
    DTM_USART_RX_GPIO_CLK_ENABLE();
    
    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = DTM_USART_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;      //GPIO_PULLDOWN; GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = DTM_USART_TX_AF;  
    HAL_GPIO_Init(DTM_USART_TX_GPIO_PORT, &GPIO_InitStruct);
    
    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin = DTM_USART_RX_PIN;
    GPIO_InitStruct.Alternate = DTM_USART_RX_AF;  
    HAL_GPIO_Init(DTM_USART_RX_GPIO_PORT, &GPIO_InitStruct);
    
    /* Configure the NVIC for SPI */  
    HAL_NVIC_SetPriority(DTM_USART_IRQn, 4, 0); //TBR Which priority < spi
    HAL_NVIC_EnableIRQ(DTM_USART_IRQn);
  }
  
}

/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handle
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
#ifdef UART_INTERFACE
  /* This is because if a reset occurs to BlueNRG-1
   * the UART lines generate a UART error */
  if(UartHandle==&DTM_UartHandle) {
    /* Start another reception */
    HAL_UART_Receive_IT(UartHandle, DTM_read_data, 1);
  }
#endif
#ifdef SPI_INTERFACE
  __NOP();
#endif
}


/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and 
  *         you can add your own implementation.
  * @retval None
  */
extern UART_HandleTypeDef xUsartInit;
extern uint8_t read_data;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
#ifdef UART_INTERFACE
  if(UartHandle==&DTM_UartHandle) {
   
    hci_input_event(DTM_read_data, 1); // => la packet received inserisce nella coda : list_insert_tail(&hciReadPktRxQueue, (tListNode *)DTM_read_data);
    
    /* Start another reception */
    HAL_UART_Receive_IT(UartHandle, DTM_read_data, 1);
  }
#endif
  if(UartHandle==&xUsartInit) {
  /* Trasfer complete: read data */
  SdkEval_IO_Receive_Data(&read_data,1);
  /* Start another reception */
  HAL_UART_Receive_IT(UartHandle, &read_data, 1);
}
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
