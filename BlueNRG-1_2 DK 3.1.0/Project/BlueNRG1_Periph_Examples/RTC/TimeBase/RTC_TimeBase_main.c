/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : RTC/TimeBase/main.c 
* Author             : RF Application Team
* Version            : V1.0.0
* Date               : September-2015
* Description        : Code demostrating the RTC timer periodic functionality
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

/** @addtogroup BlueNRG1_StdPeriph_Examples
  * @{
  */

/** @addtogroup RTC_Examples
  * @{
  */

/** @addtogroup RTC_TimeBase RTC TimeBase
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define RTC_PERIOD_1s          (32767)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void RTC_Configuration(void);

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
  
  /* LED initialization */
  SdkEvalLedInit(LED1);
  
  /* RTC configuration */
  RTC_Configuration();
  
  /* Infinite loop */
  while(1);
}
  

/**
  * @brief  RTC Configuration.
  * @param  None
  * @retval None
  */
void RTC_Configuration(void)
{
  RTC_InitType RTC_Init_struct;
  NVIC_InitType NVIC_InitStructure;
  
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_RTC, ENABLE);
  
  /* RTC configuration */  
  RTC_Init_struct.RTC_operatingMode = RTC_TIMER_PERIODIC;     /**< Periodic RTC mode */
  RTC_Init_struct.RTC_PATTERN_SIZE = 1 - 1;                   /**< Pattern size set to 1 */
  RTC_Init_struct.RTC_TLR1 = RTC_PERIOD_1s;                   /**< Enable 1s timer period */
  RTC_Init_struct.RTC_TLR2 = 0;                               /**< Enable 1s timer period */
  RTC_Init_struct.RTC_PATTERN1 = 0x00;                        /**< RTC_TLR1 selected for time generation */
  RTC_Init_struct.RTC_PATTERN2 = 0x00;                        /**< RTC_TLR1 selected for time generation */
  RTC_Init_struct.RTC_PATTERN3 = 0x00;                        /**< RTC_TLR1 selected for time generation */
  RTC_Init_struct.RTC_PATTERN4 = 0x00;                        /**< RTC_TLR1 selected for time generation */
  RTC_Init(&RTC_Init_struct);
  
  /* Enable RTC Timer interrupt*/
  RTC_IT_Config(RTC_IT_TIMER, ENABLE);
  RTC_IT_Clear(RTC_IT_TIMER);
  
  /** Delay between two write in RTC->TCR register has to be
   *  at least 3 x 32k cycle + 2 CPU cycle. For that reason it
   *  is neccessary to add the delay. 
   */
  for (volatile uint32_t i=0; i<600; i++) {
    __asm("NOP");
  }
  
  /* Set the RTC_IRQn interrupt priority and enable it */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LOW_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable RTC */
  RTC_Cmd(ENABLE);
    
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
