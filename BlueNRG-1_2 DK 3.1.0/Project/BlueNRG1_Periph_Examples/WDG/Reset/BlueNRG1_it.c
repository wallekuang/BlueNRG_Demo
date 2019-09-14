/**
  ******************************************************************************
  * @file    WDG/WDG_Reset/BlueNRG1_it.c 
  * @author  RF Application Team
  * @version V1.0.0
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
#include "SDK_EVAL_Config.h"

/** @addtogroup BlueNRG1_StdPeriph_Examples
  * @{
  */

/** @addtogroup WDG_Examples
  * @{
  */

/** @addtogroup WDG_Reset
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const uint16_t mft1a_cr[] = {50000 - 1, 25000 - 1, 10000 - 1, 5000 - 1};
uint8_t cnt = 0;

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
}

/**
  * @brief  This function handles Hard Fault exception.
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
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
  * @brief  This function handles WATCHDOG interrupt request.
  * @param  None
  * @retval None
  */
void WDG_Handler(void)
{
  /** The WDG interrupt flag is not cleared inside of the interrupt handler.
    * If the interrupt status is not cleared and a new interrupt is generated, 
    * then a watchdog reset is generated, rebooting the system.
    */
    SdkEvalLedOff(LED1);
    SdkEvalLedOff(LED2);
    SdkEvalLedOff(LED3);
}


/**
  * @brief  This function handles MFTTIMER2A interrupt request.
  * @param  None
  * @retval None
  */
void MFT1A_Handler(void)
{
  if ( MFT_StatusIT(MFT1,MFT_IT_TNA) != RESET )
  {
    cnt++;
    if(cnt < 5) {      
      MFT1->TNCRA = mft1a_cr[0];
      MFT1->TNCRB = mft1a_cr[0];
    }
    else if(cnt < 11) {
      MFT1->TNCRA = mft1a_cr[1];
      MFT1->TNCRB = mft1a_cr[1];
    }    
    else if(cnt < 21) {
      MFT1->TNCRA = mft1a_cr[2];
      MFT1->TNCRB = mft1a_cr[2];
    }
    else {
      MFT1->TNCRA = mft1a_cr[3];
      MFT1->TNCRB = mft1a_cr[3];
    }
    SdkEvalLedToggle(LED1);
    SdkEvalLedToggle(LED2);
    SdkEvalLedToggle(LED3);
    
    /** Clear MFT2 pending interrupt A */
    MFT_ClearIT(MFT1, MFT_IT_TNA);
  }
}

void GPIO_Handler(void)
{
  /** If INT1_PIN interrupt event occured toggle the OUT1_PIN */
  if(SdkEvalPushButtonGetITPendingBit(USER_BUTTON) == SET) {
    
    cnt = 0;
    WDG_SetReload(RELOAD_TIME(WDG_TIME));
    
    SdkEvalPushButtonClearITPendingBit(USER_BUTTON);
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
