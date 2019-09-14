/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : RTC/Clockwatch/main.c 
* Author             : RF Application Team
* Version            : V1.0.0
* Date               : September-2015
* Description        : Code demostrating the RTC clockwatch functionality
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

/** @addtogroup RTC_Clockwatch RTC Clockwatch
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/** Date and time set to: 1st of December 2014, 23 hour 59 min and 31 seconds */
#define SET_HOUR                23      /*< Set hour to the RTC         */
#define SET_MINUTE              59      /*< Set minute to the RTC       */
#define SET_SECOND              31      /*< Set second to the RTC       */
#define SET_WEEKDAY              7      /*< Set weekday to the RTC      */
#define SET_DAY                  1      /*< Set day to the RTC          */
#define SET_MONTH               12      /*< Set month to the RTC        */
#define SET_YEAR              2014      /*< Set year to the RTC         */

/* Match event set to: 2nd of December 2014, 0 hour 0 min and 1 seconds */
#define MATCH_HOUR              00      /*< Match hour to generate interrupt    */
#define MATCH_MINUTE            00      /*< Match minute to generate interrupt  */
#define MATCH_SECOND            01      /*< Match second to generate interrupt  */
#define MATCH_WEEKDAY           01      /*< Match weekday to generate interrupt */
#define MATCH_DAY               02      /*< Match day to generate interrupt     */
#define MATCH_MONTH             12      /*< Match month to generate interrupt   */
#define MATCH_YEAR            2014      /*< Match year to generate interrupt    */

/* 500 ms period setting */
#define RTC_PERIOD_500ms        ((32768-1)/2)  

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void RTC_Timer_Configuration(void);    
void RTC_Clockwatch_Configuration(void);

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
  SdkEvalLedInit(LED2);
  
  /* Configure RTC in timer mode to generate interrupt each 0.5s */
  RTC_Timer_Configuration();
  
  /* Configure RTC in clockwatch mode to generate interrupt on time and date match */
  RTC_Clockwatch_Configuration();
  
  /* Infinite loop */
  while(1);
}


/**
  * @brief  RTC Configuration.
  * @param  None
  * @retval None
  */
void RTC_Timer_Configuration(void)
{
  RTC_InitType RTC_Init_struct;
  NVIC_InitType NVIC_InitStructure;
  
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_RTC, ENABLE);
  
  RTC_Init_struct.RTC_operatingMode = RTC_TIMER_PERIODIC;    /**< Periodic RTC mode */
  RTC_Init_struct.RTC_PATTERN_SIZE = 1 - 1;                  /**< Pattern size set to 1 */
  RTC_Init_struct.RTC_TLR1 = RTC_PERIOD_500ms;               /**< Enable 0.5s timer period */
  RTC_Init_struct.RTC_PATTERN1 = 0x00;                       /**< RTC_TLR1 selected for time generation */
  RTC_Init(&RTC_Init_struct);
  
  /* Enable RTC Timer interrupt*/
  RTC_IT_Config(RTC_IT_TIMER, ENABLE);
  RTC_IT_Clear(RTC_IT_TIMER);

  /** Delay between two write in RTC0->TCR register has to be
   *  at least 3 x 32k cycle + 2 CPU cycle. For that reason it
   *  is neccessary to add the delay. 
   */
  for (volatile uint16_t i=0; i<600; i++) {
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

/**
  * @brief  RTC clockwatch configuration.
  * @param  None
  * @retval None
  */
void RTC_Clockwatch_Configuration(void)
{
  RTC_DateTimeType RTC_DateTime;
  
  /* Set the present time and date */
  RTC_DateTime.Second = SET_SECOND;
  RTC_DateTime.Minute = SET_MINUTE;
  RTC_DateTime.Hour = SET_HOUR;
  RTC_DateTime.WeekDay = SET_WEEKDAY;
  RTC_DateTime.MonthDay = SET_DAY;
  RTC_DateTime.Month = SET_MONTH;
  RTC_DateTime.Year = SET_YEAR;
  RTC_SetTimeDate(&RTC_DateTime);
  
  /* Set the present time and date match */
  RTC_DateTime.Second = MATCH_SECOND;
  RTC_DateTime.Minute = MATCH_MINUTE;
  RTC_DateTime.Hour = MATCH_HOUR;
  RTC_DateTime.WeekDay = MATCH_WEEKDAY;
  RTC_DateTime.MonthDay = MATCH_DAY;
  RTC_DateTime.Month = MATCH_MONTH;
  RTC_DateTime.Year = MATCH_YEAR;
  RTC_SetMatchTimeDate(&RTC_DateTime);
  
  /* Enable RTC clock watch interrupt */
  RTC_IT_Config( RTC_IT_CLOCKWATCH, ENABLE);
  RTC_IT_Clear(RTC_IT_CLOCKWATCH);
        
  /* Enable RTC clockwatch */
  RTC_ClockwatchCmd(ENABLE);
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
