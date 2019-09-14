/******************** (C) COPYRIGHT 2017 STMicroelectronics ********************
* File Name          : RTC/RTC_VirtualTimer/BLE_RTC_main.c
* Author             : AMG - RF Application Team
* Version            : V1.1.0
* Date               : 10-July-2018
* Description        : RTC example using the VTimer functionality. The VTimer is
*                      used to wait for 30 seconds, then the LED2 turns on and 
*                      the application stop. Sleep is used.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h"
#include "bluenrg1_stack.h"
#include "sleep.h"
#include "SDK_EVAL_Config.h"
#include "RTC_config.h"

/** @addtogroup BlueNRG1_StdPeriph_Examples
 *@{
 */

/** @addtogroup RTC_Examples
  * @{
  */

/** @addtogroup RTC_VirtualTimer RTC VirtualTimer
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Virtual Timer */
#define VTIMER  1
#define VTIMER_TIMEBASE_TARGET_MAX_SEC  (int32_t)(5200)


/** 
* Brief User target timeout
 */
#define VTIMER_USER_TARGET_SEC          (int32_t)(60)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static volatile uint8_t VTimer_expired = FALSE;
static int32_t vtimer_user_target_sec = VTIMER_USER_TARGET_SEC;
static int32_t vtimer_user_target2 = 0;
static volatile int32_t vtimer_user_cycle = 1;
static volatile uint32_t vtimer_get_current_time = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void HAL_VTimerTimeoutCallback(uint8_t timerNum)
{
  if(timerNum == VTIMER) {
    vtimer_get_current_time = HAL_VTimerGetCurrentTime_sysT32();
    vtimer_user_cycle--;
    VTimer_expired = TRUE;
  }
}


int main(void) 
{
  uint8_t ret;
  
  /* System Init */
  SystemInit();
  
  /* Identify BlueNRG-1 platform */
  SdkEvalIdentification();
  
  /* Initialize the LEDs */
  SdkEvalLedInit(LED1); /* Activity LED */
  SdkEvalLedInit(LED2);
  SdkEvalLedOn(LED1);
  
  /* BlueNRG-1 stack init */
  ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);
  if (ret != BLE_STATUS_SUCCESS) {
    while(1);
  }
  
  if(vtimer_user_target_sec > VTIMER_TIMEBASE_TARGET_MAX_SEC) {
    vtimer_user_cycle += (vtimer_user_target_sec / VTIMER_TIMEBASE_TARGET_MAX_SEC);
    vtimer_user_target2 = vtimer_user_target_sec % VTIMER_TIMEBASE_TARGET_MAX_SEC;
    vtimer_user_target_sec = VTIMER_TIMEBASE_TARGET_MAX_SEC;
  }

  /* Start the VTimer */
  ret = HAL_VTimerStart_ms(VTIMER, vtimer_user_target_sec*1000);
  if (ret != BLE_STATUS_SUCCESS) {
    while(1);
  }
  
  while(1) {
    /* BlueNRG-1 stack tick */
    BTLE_StackTick();
    
    if(VTimer_expired==TRUE) {
        
      if((vtimer_user_cycle == 1) || (vtimer_user_cycle == 0)) {
        
        if(vtimer_user_target2) {
          vtimer_get_current_time = HAL_VTimerAcc_sysT32_ms(vtimer_get_current_time, vtimer_user_target2*1000);
          HAL_VTimerStart_sysT32(VTIMER, vtimer_get_current_time);
          vtimer_user_target2 = 0;
        }
        else {
          SdkEvalLedOn(LED2);
          while(1);
        }
      }
      else {
        vtimer_get_current_time = HAL_VTimerAcc_sysT32_ms(vtimer_get_current_time, vtimer_user_target_sec*1000);
        HAL_VTimerStart_sysT32(VTIMER, vtimer_get_current_time); 
      }
      
      VTimer_expired = FALSE;
    }
    /* Enable Power Save according the Advertising Interval */
    BlueNRG_Sleep(SLEEPMODE_NOTIMER, 0, 0);
  }
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
