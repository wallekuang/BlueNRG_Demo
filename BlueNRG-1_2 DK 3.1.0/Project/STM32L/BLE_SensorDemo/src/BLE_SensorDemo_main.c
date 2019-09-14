/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
* File Name          : SensorDemo_main.c
* Author             : RF Application Team - AMG
* Version            : V1.0.0
* Date               : 20-November-2016
* Description        : NUCLEO-L152RE network coprocessor main file for sensor demo
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/** @addtogroup BlueNRG1_demonstrations_applications
 * BlueNRG-1 SensorDemo \see SensorDemo_main.c for documentation.
 *
 *@{
 */

/** @} */
/** \cond DOXYGEN_SHOULD_SKIP_THIS
 */
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "hal_types.h"
#include "hci.h"
#include "gp_timer.h"
#include "hal.h"
#include "osal.h"
#include "hci_const.h"
#include "user_timer.h"
#include "bluenrg1_types.h"
#include "bluenrg1_gap.h"
#include "bluenrg1_aci.h"
#include "bluenrg1_hci_le.h"
#include "SDK_EVAL_Config.h"
#include "sensor.h"
#if ENABLE_MICRO_SLEEP
#include "low_power.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define BLE_SENSOR_VERSION_STRING "1.0.0" 

/** 
* @brief  Enable febug printf's
*/ 
#ifndef DEBUG
#define DEBUG 0
#endif

/* Private macros ------------------------------------------------------------*/
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
uint8_t App_SleepMode_Check(void)
{
  if(HCI_Queue_Empty() && !user_timer_expired){
    return 1;
  }
  return 0;
}

/*******************************************************************************
* Function Name  : main.
* Description    : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int main(void)
{
  uint8_t ret;
  
  /* Device Initialization */
  Init_Device();  
  
  /* BlueNRG-1 stack init */
  ret = BlueNRG_Stack_Initialization();
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error in BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
    while(1);
  }

  /* Application demo Led Init */
  SdkEvalLedInit(LED1);
  
  PRINTF("BlueNRG-1 BLE Sensor Demo Application (version: %s)\r\n", BLE_SENSOR_VERSION_STRING); 
  
  /* Sensor Device Init */
  ret = Sensor_DeviceInit();
  if (ret != BLE_STATUS_SUCCESS) {
    SdkEvalLedOn(LED1);
    PRINTF("Sensor_DeviceInit failed 0x%02x\r\n", ret); 
    while(1);
  }

  while(1) {
    BTLE_StackTick();
    APP_Tick();
    
#if ENABLE_MICRO_SLEEP
    System_Sleep();
#endif
  }
}



#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {}
}
#endif

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
/** \endcond
*/
