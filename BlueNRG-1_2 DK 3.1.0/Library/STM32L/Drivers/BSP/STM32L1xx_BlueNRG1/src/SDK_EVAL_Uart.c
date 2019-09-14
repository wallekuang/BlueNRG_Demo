/**
* @file    SDK_EVAL_UART_Driver.c
* @author  AMS VMA RF application team
* @version V1.0.0
* @date    February 28, 2017
* @brief   This file provides all the low level UART API to access to BlueNRG module
* @details It uses STM32L1 Cube APIs + Optimized APIs for UART Transmit, Receive, 
*          Transmit & Receive (these APIs are needed for BLE communication
*          throughput) 
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOURCE CODE IS PROTECTED BY A LICENSE.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
* IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*
* <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
*
*/

/* Includes ------------------------------------------------------------------*/
#include "SDK_EVAL_Config.h"
#include "osal.h"

/** @addtogroup SDK_EVAL_UART_STM32L
* @{
*/

/** @addtogroup SDK_EVAL_Uart                    SDK EVAL Uart
* @brief SPI functions implementation.
* @details This file implements the BlueNRG Library UART interface functions. 
* @{
*/

/** @defgroup STM32_BLUENRG_BLE_Private_Defines 
* @{
*/ 


/**
* @}
*/

/** @defgroup UART_Private_Variables
* @{
*/
UART_HandleTypeDef DTM_UartHandle;
uint8_t DTM_read_data[32];

/**
* @}
*/

/** @defgroup UART_Functions
* @{
*/


/**
 * @brief  Configure the selected I/O communication channel
 * @param  None.
 * @retval None.
 */
void DTM_IO_Config(void)
{  
  /* Init USART2 port */
  DTM_USART_CLK_ENABLE();   
  DTM_UartHandle.Instance        = DTM_USART;
  DTM_UartHandle.Init.BaudRate   = 115200;
  DTM_UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  DTM_UartHandle.Init.StopBits   = UART_STOPBITS_1;
  DTM_UartHandle.Init.Parity     = UART_PARITY_NONE;
#ifdef DTM_UART_HW_FLOW_CTRL
  DTM_UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_RTS_CTS;
#else
  DTM_UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
#endif
  DTM_UartHandle.Init.Mode       = UART_MODE_TX_RX;
  if(HAL_UART_DeInit(&DTM_UartHandle) != HAL_OK) {
    while(1);
  }  

  /* Small delay to avoid issue during reset of the BlueNRG-1 
   * because the UART lines during reset can generates UART Error */
  for(volatile uint32_t i=0; i<0xFFFFF; i++);
  if(HAL_UART_Init(&DTM_UartHandle) != HAL_OK) {
    while(1);
  }
  
  /* Prepare for next reception - this can be handle as header frame as SPI protocol... */
  HAL_UART_Receive_IT(&DTM_UartHandle, DTM_read_data, 1);
#ifdef DTM_UART_HW_FLOW_CTRL
  /* Configure the RTS pin as OUTPUT high level */
  DTM_UART_RTS_OUTPUT_HIGH();
  DTM_Config_GPIO_Output_RTS();
  
  /* Configure the CTS as INPUT IRQ */
  DTM_Config_GPIO_InputIrq_CTS();
#endif  
}/* end SdkEval_IO_Config() */


/**
  * @brief UART MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void DTM_Config_GPIO_Output_RTS(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  GPIO_InitStruct.Pin       = DTM_USART_RTS_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;      //GPIO_PULLDOWN; GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = DTM_USART_TX_AF;  
  HAL_GPIO_Init(DTM_USART_RTS_GPIO_PORT, &GPIO_InitStruct);
  
}



void DTM_Config_UART_RTS(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;  
  
  GPIO_InitStruct.Pin       = DTM_USART_RTS_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;      //GPIO_PULLDOWN; GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = DTM_USART_TX_AF;
  
  HAL_GPIO_Init(DTM_USART_RTS_GPIO_PORT, &GPIO_InitStruct);
  
}


/**
  * @brief UART MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void DTM_Config_GPIO_InputIrq_CTS(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  GPIO_InitStruct.Pin       = DTM_USART_CTS_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;      //GPIO_PULLDOWN; GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = DTM_USART_TX_AF;  
  HAL_GPIO_Init(DTM_USART_CTS_GPIO_PORT, &GPIO_InitStruct);
  
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}


void DTM_Config_UART_CTS(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
      
  GPIO_InitStruct.Pin       = DTM_USART_CTS_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;      //GPIO_PULLDOWN; GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = DTM_USART_TX_AF;  
  HAL_GPIO_Init(DTM_USART_CTS_GPIO_PORT, &GPIO_InitStruct);

}


void DTM_UART_CTS_Input(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
  
  GPIO_InitStruct.Pin       = DTM_USART_CTS_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;      //GPIO_PULLDOWN; GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = DTM_USART_TX_AF;  
  HAL_GPIO_Init(DTM_USART_CTS_GPIO_PORT, &GPIO_InitStruct);
}



void Hal_Write_Serial(const void* data1, const void* data2, uint16_t n_bytes1, uint16_t n_bytes2)
{
  uint8_t command_fifo[264];
  
  Osal_MemCpy(command_fifo, data1, n_bytes1);
  Osal_MemCpy(&command_fifo[n_bytes1], data2, n_bytes2);

#ifdef DTM_UART_HW_FLOW_CTRL
  DTM_Config_UART_CTS();
  DTM_UART_RTS_OUTPUT_LOW();
#endif

  if(HAL_UART_Transmit(&DTM_UartHandle, (uint8_t*)command_fifo, n_bytes1+n_bytes2, 1000)!= HAL_OK) {
    while(1);   
  }

#ifdef DTM_UART_HW_FLOW_CTRL  
  DTM_UART_RTS_OUTPUT_HIGH();
  DTM_UART_CTS_Input();
  while((HAL_GPIO_ReadPin(DTM_USART_CTS_GPIO_PORT, DTM_USART_CTS_PIN) == GPIO_PIN_RESET));
  DTM_Config_GPIO_InputIrq_CTS();
#endif
}


/**
* @brief  Disable SPI IRQ
* @param  None
* @retval None
*/
void Disable_UART_IRQ(void)
{  
  HAL_NVIC_DisableIRQ(DTM_USART_IRQn);  
}/* end Disable_SPI_IRQ() */

/**
* @brief  Enable SPI IRQ
* @param  None
* @retval None
*/
void Enable_UART_IRQ(void)
{  
  HAL_NVIC_EnableIRQ(DTM_USART_IRQn);
}/* end Enable_SPI_IRQ() */

/**
* @}
*/


/**
* @}
*/


/**
* @}
*/



/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
