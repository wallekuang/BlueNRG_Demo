/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : WDG/Reset/main.c 
* Author             : RF Application Team
* Version            : V1.0.0
* Date               : September-2015
* Description        : Code demostrating the watchdog reset functionality
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "BlueNRG_x_device.h"
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"

/** @addtogroup BlueNRG1_StdPeriph_Examples BlueNRG1 Standard Peripheral Examples
* @{
*/

/** @addtogroup WDG_Examples Watchdog Examples
* @{
*/

/** @addtogroup WDG_Reset Watchdog Reset
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void WDG_Configuration(void);
void MFT_Configuration(void);

/* Private functions ---------------------------------------------------------*/

/**
* @brief  Main program code
* @param  None
* @retval None
*/
#include <stdio.h>
int main(void)
{
  /* System initialization function */
  SystemInit();
  
  /* Identify BlueNRG1 platform */
  SdkEvalIdentification();
  
  /* LEDs initialization function */
  SdkEvalLedInit(LED1);
  SdkEvalLedInit(LED2);
  SdkEvalLedInit(LED3);
  
  /* Push1 button initialization function */
  SdkEvalPushButtonInit(USER_BUTTON);
  SdkEvalPushButtonIrq(USER_BUTTON, IRQ_ON_RISING_EDGE);

  /* MFT configuration */
  MFT_Configuration();

  /* WDG configuration */
  WDG_Configuration();

  /* Watchdog enable */
  WDG_Enable();

  /* MFT1 enable */
  MFT_Cmd(MFT1, ENABLE);
  
  /* Infinite loop */
  while (1);
}


/**
* @brief  WDG configuration routine
* @param  None
* @retval None
*/
void WDG_Configuration(void)
{
  NVIC_InitType NVIC_InitStructure;
  
  /* Enable watchdog clocks  */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_WDG, ENABLE);  
  
  /* Set watchdog reload period at 15 seconds */
  WDG_SetReload(RELOAD_TIME(WDG_TIME));
  
  /* Enable the watchdog interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = WDG_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LOW_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


/**
  * @brief  MFT configuration routine
  * @param  None
  * @retval None
  */
void MFT_Configuration(void)
{
  NVIC_InitType NVIC_InitStructure;
  MFT_InitType timer_init;
  
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_MTFX1, ENABLE);
  
  MFT_StructInit(&timer_init);
  
  timer_init.MFT_Mode = MFT_MODE_1;
  
  /* Clock period at 10 us */
  timer_init.MFT_Prescaler = 160-1;
  
  /* MFT1 in mode 1 on Low speed clock (TIMER_8MHZ) generate 10kHz signal */
  timer_init.MFT_Clock1 = MFT_PRESCALED_CLK;
  timer_init.MFT_Clock2 = MFT_NO_CLK;
  timer_init.MFT_CRA = 50000 - 1;
  timer_init.MFT_CRB = 50000 - 1;
  MFT_Init(MFT1, &timer_init);
  
  /* Enable MFT2 Interrupt 1 */
  NVIC_InitStructure.NVIC_IRQChannel = MFT1A_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LOW_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  MFT_EnableIT(MFT1, MFT_IT_TNA, ENABLE);
}


#ifdef  USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}

#endif

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
