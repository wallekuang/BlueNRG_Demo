/**
 * @file    SDK_EVAL_Timers.c
 * @author  VMA division - AMS
 * @version 3.2.1
 * @date    May 1, 2015
 * @brief   SDK EVAL timers configuration.
 * @details
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


/* Includes ------------------------------------------------------------------*/
#include "SDK_EVAL_Timers.h"



/**
 * @addtogroup SDK_EVAL_STM32L
 * @{
 */

/**
 * @addtogroup SDK_EVAL_Timers
 * @{
 */


/**
 * @defgroup SDK_EVAL_Timers_Private_TypesDefinitions           SDK EVAL Timers Private Types Definitions
 * @{
 */

/**
 *@}
 */


/**
 * @defgroup SDK_EVAL_Timers_Private_Defines                    SDK EVAL Timers Private Defines
 * @{
 */

#define SYSTEM_CLOCK    32000000

/**
 *@}
 */


/**
 * @defgroup SDK_EVAL_Timers_Private_Macros                     SDK EVAL Timers Private Macros
 * @{
 */



/**
 *@}
 */

/**
 * @defgroup SDK_EVAL_Timers_Private_Variables                  SDK EVAL Timers Private Variables
 * @{
 */

volatile uint32_t lSystickCounter=0;


/**
 *@}
 */

/**
 * @defgroup SDK_EVAL_Timers_Private_FunctionPrototypes         SDK EVAL Timers Private Function Prototypes
 * @{
 */

/**
 *@}
 */


/**
 * @defgroup SDK_EVAL_Timers_Private_Functions                  SDK EVAL Timers Private Functions
 * @{
 */


/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{
//  lSystickCounter++;
//}


/**
  * @brief  This function implements return the current
  *         systick with a step of 1 ms.
  * @param  lTimeMs desired delay expressed in ms.
  * @retval None
  */
uint32_t SdkGetCurrentSysTick(void)
{
  return lSystickCounter;
  
}

void SdkStartSysTick(void)
{
  SysTick_Config(SYSTEM_CLOCK/100);
  lSystickCounter = 0;
}

/**
  * @brief  This function implements a delay using the microcontroller
  *         Systick with a step of 100 ms.
  * @param  lTimeMs desired delay expressed in ms.
  * @retval None
  */
void SdkDelay10Ms(volatile uint32_t lTime10Ms)
{
  uint32_t nWaitPeriod = ~lSystickCounter;
  
  if(nWaitPeriod<lTime10Ms)
  {
    while( lSystickCounter != 0xFFFFFFFF);
    nWaitPeriod = lTime10Ms-nWaitPeriod;
  }
  else
    nWaitPeriod = lTime10Ms+ ~nWaitPeriod;
  
  while( lSystickCounter != nWaitPeriod ) ;

}

void SdkDelay10Ms_ISR(volatile  uint32_t lTime10Ms)
{
  uint32_t nWaitPeriod = ~(SysTick->VAL);
  
  if(nWaitPeriod<lTime10Ms)
  {
    while( (SysTick->VAL) != 0xFFFFFFFF);
    nWaitPeriod = lTime10Ms-nWaitPeriod;
  }
  else
    nWaitPeriod = lTime10Ms+ ~nWaitPeriod;
  
  while( (SysTick->VAL) != nWaitPeriod ) ;
}

/**
 *@}
 */


/**
 *@}
 */


/**
 *@}
 */


/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
