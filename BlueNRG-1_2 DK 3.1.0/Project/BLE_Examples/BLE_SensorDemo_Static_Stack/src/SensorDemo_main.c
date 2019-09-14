/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : SensorDemo_main.c
* Author             : AMS - VMA RF Application team
* Version            : V1.1.0
* Date               : 15-January-2016
* Description        : BlueNRG-1 Sensor Demo main file 
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
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h" 
#include "bluenrg1_stack.h"
#include "SDK_EVAL_Config.h"
#include "sleep.h"
#include "sensor.h"
#include "SensorDemo_config.h"
#include "OTA_btl.h"  
#include "gatt_db.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define BLE_SENSOR_VERSION_STRING "1.0.0" 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Set the Application Service Max number of attributes records with init parameters coming from application *.config.h file */
uint8_t Services_Max_Attribute_Records[NUMBER_OF_APPLICATION_SERVICES] = {MAX_NUMBER_ATTRIBUTES_RECORDS_SERVICE_1, MAX_NUMBER_ATTRIBUTES_RECORDS_SERVICE_2};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

int main(void) 
{
  uint8_t ret;

  /* System Init */
  SystemInit();
  
  /* Identify BlueNRG1 platform */
  SdkEvalIdentification();

  /* Configure I/O communication channel */
  SdkEvalComUartInit(UART_BAUDRATE);

  /* BlueNRG-1 stack init */
  ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error in BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
    while(1);
  }
  
  /* Application demo Led Init */
  SdkEvalLedInit(LED1); //Activity led 
  SdkEvalLedInit(LED3); //Error led 
  SdkEvalLedOn(LED1);
  SdkEvalLedOff(LED3);
  
  PRINTF("BlueNRG-1 BLE Sensor Demo Application (version: %s)\r\n", BLE_SENSOR_VERSION_STRING); 
  
  /* Sensor Device Init */
  ret = Sensor_DeviceInit();
  if (ret != BLE_STATUS_SUCCESS) {
    SdkEvalLedOn(LED3);
    while(1);
  }
  
  while(1)
  {
    /* BLE Stack Tick */
    BTLE_StackTick();

    /* Application Tick */
    APP_Tick();
    
    /* Power Save management */
    BlueNRG_Sleep(SLEEPMODE_NOTIMER, 0, 0); 
    
#if ST_OTA_FIRMWARE_UPGRADE_SUPPORT
    /* Check if the OTA firmware upgrade session has been completed */
    if (OTA_Tick() == 1)
    {
      /* Jump to the new application */
      OTA_Jump_To_New_Application();
    }
#endif /* ST_OTA_FIRMWARE_UPGRADE_SUPPORT */

  }/* while (1) */
}

/****************** BlueNRG-1 Sleep Management Callback ********************************/

SleepModes App_SleepMode_Check(SleepModes sleepMode)
{
  if(request_free_fall_notify || SdkEvalComIOTxFifoNotEmpty() || SdkEvalComUARTBusy())
    return SLEEPMODE_RUNNING;
  
  return SLEEPMODE_NOTIMER;
}

/***************************************************************************************/

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
