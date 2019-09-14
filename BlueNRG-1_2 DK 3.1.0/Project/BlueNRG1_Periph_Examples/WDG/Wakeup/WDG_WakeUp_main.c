/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : WDG/WakeUp/main.c 
* Author             : RF Application Team
* Version            : V1.0.0
* Date               : September-2015
* Description        : Code demostrating the watchdog wake-up functionality
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
#include "SDK_EVAL_Config.h"

/** @addtogroup BlueNRG1_StdPeriph_Examples BlueNRG1 Standard Peripheral Examples
* @{
*/

/** @addtogroup WDG_Examples Watchdog Examples
* @{
*/

/** @addtogroup Watchdog_WakeUp Watchdog WakeUp
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/

/** Set the watchdog reload interval in [s] = (WDT_LOAD + 3) / (clock frequency in Hz). */
#define RC32K_FREQ		32768
#define RELOAD_TIME(sec)        ((sec*RC32K_FREQ)-3)

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void WDG_Configuration(void);

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
  
  /* LEDs initialization function */
  SdkEvalLedInit(LED1);
  
  /* WDG configuration */
  WDG_Configuration();
  
  /* Watchdog enable */
  WDG_ITConfig(ENABLE);
  
  /* Cortex will enter in deep sleep mode when WFI command start */
  SystemSleepCmd(ENABLE);
  __WFI();
  
  /* Once waked-up enter the sleep mode again */
  while(1) {
    /* Toggle LED1  */
    SdkEvalLedToggle(LED1);
    
    /* Go to sleep mode again */
    __WFI();
  }
  
}

/**
* @brief  NVIC configuration
* @param  None
* @retval None
*/
void WDG_Configuration(void)
{
  NVIC_InitType NVIC_InitStructure;
  
  /* Enable watchdog clock */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_WDG, ENABLE);
  
  /* WDG reload time configuration */
  WDG_SetReload(RELOAD_TIME(1));
  
  /* Clear pending interrupt on cortex */
  NVIC->ICPR[0] = 0xFFFFFFFF;
  
  /* Enable the RTC interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = WDG_IRQn;
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
