/**
 * @file    SDK_EVAL_Uart_Driver.h
 * @author  AMS - AAS Division
 * @version V1.0.1
 * @date    February, 28 2017
 * @brief   This file contains definitions for Software Development Kit eval SPI devices
 * @details
 *
 * In this module there are API for the management of the SPI devices on the SDK Eval
 * motherboards.
 *
 * <b>Example:</b>
 * @code
 *
 *   SdkEvalSpiInit();
 *
 *   ...
 *
 * @endcode
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
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SDK_EVAL_UART_DRIVER_H
#define __SDK_EVAL_UART_DRIVER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"

#ifdef __cplusplus
 extern "C" {
#endif


/** @addtogroup SDK_EVAL_UART_STM32L
* @{
*/
   
   
/** @defgroup SPI_Private_TypesDefinitions
* @{
*/

/**
* @}
*/


/** @addtogroup SDK_EVAL_Uart                    SDK EVAL Uart
* @brief SPI functions implementation.
* @details This file implements the BlueNRG Library UART interface functions. 
* @{
*/

   /** @addtogroup SDK_EVAL_UART_Exported_Types          SDK EVAL Uart Exported Types
 * @{
 */


/**
* @}
*/

/** @defgroup UART_Private_Defines
* @{
*/


#define DTM_UART_RTS_OUTPUT_LOW()       HAL_GPIO_WritePin(DTM_USART_RTS_GPIO_PORT, DTM_USART_RTS_PIN, GPIO_PIN_RESET)
#define DTM_UART_RTS_OUTPUT_HIGH()      HAL_GPIO_WritePin(DTM_USART_RTS_GPIO_PORT, DTM_USART_RTS_PIN, GPIO_PIN_SET)


/**
* @}
*/


/** @defgroup SDK_EVAL_Uart_Peripheral_Gpio
* @{
*/

/**
* @brief BlueNRG User Platform UART & GPIO lines: to be customized for addressing user platform.
*        Example configuration: STM32L152 Nucleo.
 */
#define DTM_USART                           USART1
#define DTM_USART_IRQn                      USART1_IRQn
#define DTM_USART_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
#define DTM_USART_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define DTM_USART_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define DTM_USART_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define DTM_USART_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

/* Definition for DTM_USART Pins */
#define DTM_USART_TX_PIN                    GPIO_PIN_9
#define DTM_USART_TX_GPIO_PORT              GPIOA
#define DTM_USART_TX_AF                     GPIO_AF7_USART1
#define DTM_USART_RX_PIN                    GPIO_PIN_10
#define DTM_USART_RX_GPIO_PORT              GPIOA
#define DTM_USART_RX_AF                     GPIO_AF7_USART1
#define DTM_USART_CTS_PIN                    GPIO_PIN_11
#define DTM_USART_CTS_GPIO_PORT              GPIOA
#define DTM_USART_CTS_AF                     GPIO_AF7_USART1
#define DTM_USART_RTS_PIN                    GPIO_PIN_12
#define DTM_USART_RTS_GPIO_PORT              GPIOA
#define DTM_USART_RTS_AF                     GPIO_AF7_USART1

/**
* @}
*/

/**
* @}
*/

void DTM_IO_Config(void);
void DTM_Config_GPIO_Output_RTS(void);
void DTM_Config_UART_RTS(void);
void DTM_Config_GPIO_InputIrq_CTS(void);
void DTM_Config_UART_CTS(void);
void DTM_UART_CTS_Input(void);

void Disable_UART_IRQ(void);
void Enable_UART_IRQ(void);

/**
* @}
*/


/**
* @}
*/


/**
* @}
*/


#ifdef __cplusplus
}
#endif

#endif

/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
