/**
******************************************************************************
* @file    GPIO/InputInterrupt/BlueNRG1_it.c 
* @author  RF Application Team
* @version V1.1.0
* @date    September-2015
* @brief   Main Interrupt Service Routines.
*          This file provides template for all exceptions handler and
*          peripherals interrupt service routine.
******************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"


/** @addtogroup BlueNRG1_StdPeriph_Examples
* @{
*/

/** @addtogroup GPIO_Examples
* @{
*/ 

/** @addtogroup GPIO_InputInterrupt
* @{
*/ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define NO_BOUNCE_BUTTON_2            20

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint8_t check_push2 = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
* @brief  This function handles NMI exception.
*/
void NMI_Handler(void)
{
  while(1);
}

/**
* @brief  This function handles Hard Fault exception.
*/
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1);
}


/**
* @brief  This function handles SVCall exception.
*/
void SVC_Handler(void)
{
}


/**
* @brief  This function handles SysTick Handler.
*/
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 BlueNRG-1 Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (system_bluenrg1.c).                                               */
/******************************************************************************/

/**
* @brief  This function handles GPIO interrupt request.
* @param  None
* @retval None
*/
void GPIO_Handler(void)
{
  /* If BUTTON_1 is pressed LED1 is ON */
  if(GPIO_GetITPendingBit(Get_ButtonGpioPin(BUTTON_1)) == SET) {
    GPIO_ClearITPendingBit(Get_ButtonGpioPin(BUTTON_1));
    
    if(GPIO_ReadBit(Get_ButtonGpioPin(BUTTON_1)) == Bit_RESET) {
      GPIO_WriteBit(Get_LedGpioPin(LED1), LED_ON);
    }
    else {
      GPIO_WriteBit(Get_LedGpioPin(LED1), LED_OFF);
    }
    
  }

  /* If BUTTON_2 is pressed LED2 is toggled */
  if(GPIO_GetITPendingBit(Get_ButtonGpioPin(BUTTON_2)) == SET) {
    GPIO_ClearITPendingBit(Get_ButtonGpioPin(BUTTON_2));
    
    GPIO_ToggleBits(Get_LedGpioPin(LED2));
  }
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

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
