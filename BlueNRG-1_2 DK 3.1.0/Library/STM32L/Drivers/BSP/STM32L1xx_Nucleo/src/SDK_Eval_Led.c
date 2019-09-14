/**
 * @file    SDK_EVAL_Led.c
 * @author  AMS VMA RF application team
 * @version V1.0.1
 * @date    March 10, 2015
 * @brief   This file provides all the low level API to manage SDK LEDs.
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
 * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
 */


/* Includes ------------------------------------------------------------------*/
#include "SDK_EVAL_Led.h"
/** @addtogroup SDK_EVAL_STM32L
 * @{
 */


/** @addtogroup SDK_EVAL_Led
 * @{
 */

/** @defgroup SDK_EVAL_Led_Private_TypesDefinitions             SDK EVAL Led Private Types Definitions
 * @{
 */

/**
 * @}
 */


/** @defgroup SDK_EVAL_Led_Private_Defines                      SDK EVAL Led Private Defines
 * @{
 */

/**
 * @}
 */


/** @defgroup SDK_EVAL_Led_Private_Macros                       SDK EVAL Led Private Macros
 * @{
 */

/**
 * @}
 */

/**
 * @}
 */


/**
 * @defgroup SDK_EVAL_Led_Private_FunctionPrototypes                    SDK EVAL Led Private Function Prototypes
 * @{
 */

/**
 * @}
 */


/**
 * @defgroup SDK_EVAL_Led_Private_Functions                             SDK EVAL Led Private Functions
 * @{
 */



/**
 * @brief  Configures LED GPIO.
 * @param  xLed Specifies the Led to be configured.
 *         This parameter can be one of following parameters:
 *         @arg LED1
 *         @arg LED2
 *         @arg LED3
 *         @arg LED4
 *         @arg LED5
 * @retval None.
 */
void SdkEvalLedInit(SdkEvalLed xLed)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /* Enable the GPIO_LED Clock */
  __GPIOA_CLK_ENABLE(); 
  
  /* Configure the GPIO_LED pin */
  GPIO_InitStruct.Pin = USER_LED1_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(USER_LED1_GPIO_PORT, &GPIO_InitStruct);

  /* Reset PIN to switch off the LED */
  HAL_GPIO_WritePin(USER_LED1_GPIO_PORT,USER_LED1_GPIO_PIN, GPIO_PIN_RESET);
  
  SdkEvalLedOff(xLed);
}

/**
 * @brief  Turns selected LED On.
 * @param  xLed Specifies the Led to be set on.
 *         This parameter can be one of following parameters:
 *         @arg LED1
 *         @arg LED2
 *         @arg LED3
 *         @arg LED4
 *         @arg LED5
 * @retval None.
 */
void SdkEvalLedOn(SdkEvalLed xLed)
{
   HAL_GPIO_WritePin(USER_LED1_GPIO_PORT,USER_LED1_GPIO_PIN, LED_ON);
}

/**
 * @brief  Turns selected LED Off.
 * @param  xLed Specifies the Led to be set off.
 *         This parameter can be one of following parameters:
 *         @arg LED1
 *         @arg LED2
 *         @arg LED3
 *         @arg LED4
 *         @arg LED5
 * @retval None.
 */
void SdkEvalLedOff(SdkEvalLed xLed)
{
   HAL_GPIO_WritePin(USER_LED1_GPIO_PORT,USER_LED1_GPIO_PIN, LED_OFF);
}

/**
 * @brief  Toggles the selected LED.
 * @param  xLed Specifies the Led to be toggled.
 *         This parameter can be one of following parameters:
 *         @arg LED1
 *         @arg LED2
 *         @arg LED3
 *         @arg LED4
 *         @arg LED5
 * @retval None.
 */
void SdkEvalLedToggle(SdkEvalLed xLed)
{
  HAL_GPIO_TogglePin(USER_LED1_GPIO_PORT,USER_LED1_GPIO_PIN);
}

/**
 * @brief  Returns the status of a specified led.
 * @param  xLed Specifies the Led to be read.
 *         This parameter can be one of following parameters:
 *         @arg LED1
 *         @arg LED2
 *         @arg LED3
 *         @arg LED4
 *         @arg LED5
 * @retval FlagStatus return the status of the LED. This parameter can be:
 *         SET or RESET.
 */
FlagStatus SdkEvalLedGetState(SdkEvalLed xLed)
{
  if(USER_LED1_GPIO_PORT->IDR & USER_LED1_GPIO_PIN)
    return RESET;
  else
    return SET;
}


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
