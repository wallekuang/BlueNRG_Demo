/**
  ******************************************************************************
  * @file    MFT/Mode_2/BlueNRG1_it.c 
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
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"

/** @addtogroup BlueNRG1_StdPeriph_Examples
  * @{
  */

/** @addtogroup MTFX Mode 2 Example
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t status_a = 0;
uint8_t status_b = 0;
uint32_t time1_a = 0;
uint32_t time1_b = 0;
uint32_t underflow_counter_a = 0;
uint32_t underflow_counter_b = 0;
uint32_t period_counter_a;
uint32_t period_counter_b;

extern FlagStatus fA_found;
extern FlagStatus fB_found;

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
  * @brief  This function handles MFT1A interrupt request.
  * @param  None
  * @retval None
  */
void MFT1A_Handler(void)
{
  if (MFT_StatusIT(MFT1,MFT_IT_TNA) != RESET )
  {
    /* Clear the interrupt */
    MFT_ClearIT(MFT1, MFT_IT_TNA);
    
    if (status_a == 0)
    {
      /* Save the time stamp 1 */
      time1_a =  MFT1->TNCRA;
      
      /* Clear underflow counter */
      underflow_counter_a = 0;
      
      status_a = 1;
    }
    else
    {
      /* Count the numper of CPU clock perion in one period of input signal */
      period_counter_a = time1_a - MFT1->TNCRA + (underflow_counter_a * 0xFFFF);
      
      /* Clear underflow counter and reset the counter to avoid underflow */
      MFT1->TNCNT2 = 0xFFFF;
      underflow_counter_a = 0;
      
      /* Threshold calculated in number of counter as how many count in the target period
       * with a target period of 1 ms, and a tick counter of 62.5 ns,
       * then the target counter is 16000.
       */
      if ((period_counter_a > 15500) && (period_counter_a < 16500)) {
        fA_found = SET;
      }
      else {
        fA_found = RESET;
      }
      
      status_a = 0;
    }
  }
  
  if (MFT_StatusIT(MFT1,MFT_IT_TNB) != RESET )
  {
    /* Clear the interrupt */
    MFT_ClearIT(MFT1, MFT_IT_TNB);
    
    if (status_b == 0)
    {
      /* Save the time stamp 1 */
      time1_b =  MFT1->TNCRB;
      
      /* Clear underflow counter */
      underflow_counter_b = 0;
      
      status_b = 1;
    }
    else
    {
      /* Count the numper of CPU clock perion in one period of input signal */
      period_counter_b = time1_b - MFT1->TNCRB + (underflow_counter_b * 0xFFFF);
      
      /* Clear underflow counter and reset the counter to avoid underflow */
      MFT1->TNCNT1 = 0xFFFF;
      underflow_counter_b = 0;
      
      /* Threshold calculated in number of counter as how many count in the target period
       * with a target period of 100 us, and a tick counter of 62.5 ns,
       * then the target counter is 1600.
       */
      if ((period_counter_b > 1550) && (period_counter_b < 1650)) {
        fB_found = SET;
      }
      else {
        fB_found = RESET;
      }
      status_b = 0;
    }
  }
  
  if (MFT_StatusIT(MFT1,MFT_IT_TNC) != RESET )
  {
    /* Clear the interrupt */
    MFT_ClearIT(MFT1, MFT_IT_TNC);
    
    /* Increment underflow counters */
    underflow_counter_a++;
    underflow_counter_b++;
  }
  
}


/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
