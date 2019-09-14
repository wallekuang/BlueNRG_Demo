/**
* @file    SDK_EVAL_Timers.c
* @author  VMA division - AMS
* @version V3.2.0
* @date    February 1, 2015
* @brief   SDK EVAL timers configuration.
* @details
*
* This module allows the user to easily configure the STM32L timers.
* The functions that are provided are limited to the generation of an
* IRQ every time the timer elapses.
*
* <b>Example:</b>
* @code
*
*   ...
*
*   SdkEvalTimersTimConfig_ms(TIM2,60.0);
*
*   ...
*
*   SdkEvalTimersState(TIM2, ENABLE);          // the timer starts counting here
*
*   ...
*
*   SdkEvalTimersState(TIM2, DISABLE);         // timer stopped
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
* <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SDK_EVAL_TIMERS_H
#define __SDK_EVAL_TIMERS_H

/* Includes ------------------------------------------------------------------*/

#include "stm32l1xx.h"


#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @addtogroup SDK_EVAL_STM32L
 * @{
 */

/**
 * @defgroup SDK_EVAL_Timers            SDK EVAL Timers
 * @brief Management of STM32L timers.
 * @details See the file <i>@ref SDK_EVAL_Timers.h</i> for more details.
 * @{
 */



/**
 * @defgroup SDK_EVAL_Timers_Exported_Types             SDK EVAL Timers Exported Types
 * @{
 */


/**
 *@}
 */


/**
 * @defgroup SDK_EVAL_Timers_Exported_Constants         SDK EVAL Timers Exported Constants
 * @{
 */

/**
 *@}
 */


/**
 * @defgroup SDK_EVAL_Timers_Exported_Macros            SDK EVAL Timers Exported Macros
 * @{
 */


/**
 *@}
 */


/**
 * @defgroup SDK_EVAL_Timers_Exported_Functions         SDK EVAL Timers Exported Functions
 * @{
 */
   
//void SysTick_Handler(void);
void SdkDelay10Ms(volatile uint32_t lTime10Ms);
uint32_t SdkGetCurrentSysTick(void);
void SdkStartSysTick(void);
void SdkDelay10Ms_ISR(volatile  uint32_t lTime10Ms);

/**
 *@}
 */

/**
 *@}
 */

/**
 *@}
 */

#ifdef __cplusplus
}
#endif

#endif

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
