/**
 * @file    SDK_EVAL_Clock.h
 * @author  AMS - AAS Division
 * @version V1.0.0
 * @date    Febrary 3, 2014
 * @brief   This file contains definitions for Software Development Kit eval board clock define
 * @details
 *
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
#ifndef __SDK_EVAL_CLOCK_H
#define __SDK_EVAL_CLOCK_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"

#ifdef __cplusplus
 extern "C" {
#endif


/** @addtogroup SDK_EVAL_STM32L
 * @{
 */

/** @addtogroup SDK_EVAL_Clock           SDK EVAL Clock
 * @brief Management of Software Development Kit eval board Leds.
 * @details See the file <i>@ref SDK_EVAL_Clock.h</i> for more details.
 * @{
 */



/** @defgroup SDK_EVAL_Clock_Exported_Constants                           SDK EVAL Clock Exported Constants
 * @{
 */

#ifdef SYSCLK_MSI
#define SYSCLK_FREQ	  4000000
#else
#define SYSCLK_FREQ	  32000000
#endif
#define SYSCLK_FREQ_SLEEP	32000

/* Constants for BLE Stack's timer */
#define TIM_FREQ	    SYSCLK_FREQ
#define TIM_FREQ_SLEEP      SYSCLK_FREQ_SLEEP
   
#define TIM_PRESCALER	    (TIM_FREQ/1000)-1 //65535 
   
#define TIM_PRESCALER_SLEEP	(TIM_PRESCALER+1)*TIM_FREQ_SLEEP/TIM_FREQ - 1
/**
 * @}
 */

/**
 * @defgroup SDK_EVAL_Clcok_Exported_Macros                       SDK EVAL Clock Exported Macros
 * @{
 */

/**
 * @}
 */

/** @defgroup SDK_EVAL_Clock_Exported_Functions                   SDK EVAL Clock Exported Functions
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
