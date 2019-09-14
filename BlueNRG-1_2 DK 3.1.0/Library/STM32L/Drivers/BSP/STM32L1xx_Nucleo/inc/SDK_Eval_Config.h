/**
* @file    SDK_EVAL_Config.h
* @author  AMS VMA RF application team
* @version V1.0.1
* @date    March 10, 2014
* @brief   This file contains SDK EVAL configuration and useful defines.
* @details
*
* This file is used to include all or a part of the SDK Eval
* libraries into the application program which will be used.
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
#ifndef __SDK_EVAL_CONFIG_H
#define __SDK_EVAL_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "SDK_EVAL_Com.h"
#include "SDK_EVAL_Spi.h"
#include "SDK_EVAL_Uart.h"
#include "SDK_EVAL_Led.h"
#include "SDK_EVAL_Timers.h"
#include "SDK_EVAL_Clock.h"
#include "SDK_EVAL_Gpio.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SDK_EVAL_Config         SDK EVAL Config
* @brief SDK EVAL configuration.
* @details See the file <i>@ref SDK_EVAL_Config.h</i> for more details.
* @{
*/

/** @addtogroup SDK_EVAL_Config_Exported_Types          SDK EVAL Config Exported Types
* @{
*/

/**
* @}
*/

/** @defgroup SDK_EVAL_Config_Exported_Constants        SDK EVAL Config Exported Constants
* @{
*/


/* USART list */
#define UART_PORT          USART2
#define UART_IRQ  USART2_IRQn
#define UART_CLOCK() __USART2_CLK_ENABLE()
#define UART_BAUDRATE     115200

/* TX GPIO USART */
#define UART_TX_PORT GPIOA
#define UART_TX_PIN GPIO_PIN_2
#define UART_TX_AF GPIO_AF7_USART2
#define UART_TX_CLOCK() __HAL_RCC_GPIOA_CLK_ENABLE()

/* RX GPIO USART */
#define UART_RX_PORT GPIOA
#define UART_RX_PIN GPIO_PIN_3
#define UART_RX_AF  GPIO_AF7_USART2
#define UART_RX_CLOCK() __HAL_RCC_GPIOA_CLK_ENABLE()

/**
* @}
*/
  
/** @defgroup SDK_EVAL_Config_Exported_Macros           SDK EVAL Config Exported Macros
* @{
*/


/**
* @}
*/


/** @defgroup SDK_EVAL_Config_Exported_Functions        SDK EVAL Config Exported Functions
* @{
*/

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
