
/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : MTFX/Mode_2/main.c 
* Author             : RF Application Team
* Version            : V1.0.0
* Date               : September-2015
* Description        : Code demostrating the MFT Mode 2 functionality
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
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"


/** @addtogroup BlueNRG1_StdPeriph_Examples BlueNRG1 Standard Peripheral Examples
* @{
*/


/** @addtogroup MTFX_Examples MTFX Examples
* @{
*/

/** @addtogroup MTFX_Mode2 MTFX Mode 2 Example
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define IO_CAPTURE_PIN_MFT1_TIMERA             2 /* IO2 */
#define IO_CAPTURE_PIN_MFT1_TIMERB             3 /* IO3 */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
FlagStatus fA_found = RESET;
FlagStatus fB_found = RESET;

/* Private function prototypes -----------------------------------------------*/
void MFT_Configuration(void);

/* Private functions ---------------------------------------------------------*/



/**
* @brief  Main program code
* @param  None
* @retval None
*/
int main(void)
{
  /* System initialization function */
  SystemInit();
  
  /* Identify BlueNRG1 platform */
  SdkEvalIdentification();
  
  /* LEDs initialization */
  SdkEvalLedInit(LED1);
  SdkEvalLedInit(LED2);
  
  /* IO2 for TnCRA (TnA) */
  MFT_SelectCapturePin(MFT1_TIMERA, IO_CAPTURE_PIN_MFT1_TIMERA);
  
  /* IO3 for TnCRB (TnB) */
  MFT_SelectCapturePin(MFT1_TIMERB, IO_CAPTURE_PIN_MFT1_TIMERB);
  
  /* MFT configuration */
  MFT_Configuration();
  
  /* Configure edge detection on falling edge for TnA and TnB */
  MFT_TnEDGES(MFT1, MFT_FALLING, MFT_FALLING);
  
  /* Disable the preset (to 0xFFFF) of the TnCNT1 register on TnA and TnB event.
   * In this way, the TnCNT1 value is not changed by hardware. 
   * This allows to use its underflow in order to compute the frequency of 
   * signals greater than the max tick counter. */
  MFT_TnXEN(MFT1, MFT_TnA, DISABLE);
  MFT_TnXEN(MFT1, MFT_TnB, DISABLE);
  
  /* Enable all MFT interrupt */
  MFT_EnableIT(MFT1, MFT_IT_TNA | MFT_IT_TNB | MFT_IT_TNC, ENABLE);
  
  /* Start MFT1 timer */
  MFT_Cmd(MFT1, ENABLE);
  
  /* Infinite loop */
  while(1) {

    /* If input signal frequency on IN_PIN1 is 1kHz OUT_PIN2 is in LOW state */  
    if (fA_found == SET) {
      SdkEvalLedOn(LED1);
    }
    else {
      SdkEvalLedOff(LED1);
    }
    
    /* If input signal frequency on IN_PIN2 is 10kHz OUT_PIN3 is in LOW state */
    if (fB_found == SET) {
      SdkEvalLedOn(LED2);
    }
    else {
      SdkEvalLedOff(LED2);
    }
    
  }
}



/**
* @brief  MFT_Configuration.
* @param  None
* @retval None
*/
void MFT_Configuration()
{
  MFT_InitType timer_init;
  NVIC_InitType NVIC_InitStructure;
  
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_MTFX1, ENABLE);
  
  /* Init MFT1 in mode 2 */
  timer_init.MFT_Clock1 = MFT_PRESCALED_CLK;
  timer_init.MFT_Clock2 = MFT_NO_CLK;
  timer_init.MFT_Mode = MFT_MODE_2;
  timer_init.MFT_CRA = 0xFFFF;
  timer_init.MFT_CRB = 0xFFFF;
  
 /* If a square waveform is @ input with a frequency of 10kHz => T = 100 us
  *
  * with Prescaler = 16 - 1; => 1us sampling period
  * the counter number within a period is 100.
  * 
  * with Prescaler = 1 - 1; => 62.5ns sampling period
  * the counter number within a period is 16000. More resolution.
  */
  timer_init.MFT_Prescaler = 1-1;
  
  MFT_Init(MFT1, &timer_init);
  
  /* Set the TnCNT1 at the max value. No matter about the TnCNT2 since it is not used. */
  MFT_SetCounter(MFT1, 0xFFFF, 0xFFFF);
  
  /* Enable MFT irq */
  NVIC_InitStructure.NVIC_IRQChannel = MFT1A_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LOW_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
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
