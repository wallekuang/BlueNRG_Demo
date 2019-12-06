/*
  ******************************************************************************
  * @file    BLE_User_main.c 
  * @author  AMG - RF Application Team
  * @version V1.0.0
  * @date    06 - 11 - 2019
  * @brief   User application initialization functions.
	This C file has been created automatically by BlueNRG GUI.
	This code has to be reviewed.
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
  * <h2><center>&copy; COPYRIGHT 2019 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 
  
/** \cond DOXYGEN_SHOULD_SKIP_THIS
 */ 
 
/* Includes-----------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h"
#include "bluenrg1_stack.h"
#include "SDK_EVAL_Config.h"
#include "sleep.h"
#include "user.h"
#include "osal.h"

#define BLE_STACK_V_1_X 0

#include "user_config.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

int main(void)
{
  int ret;
  uint8_t status;
  uint8_t hci_version;
  uint16_t hci_revision;
  uint8_t lmp_pal_version;
  uint16_t manufacturer_name;
  uint16_t lmp_pal_subversion;


  /* System Init */
  SystemInit();

  /* Identify BlueNRG/BlueNRG-MS/BleNRG-1 or BlueNRG-2 platform */
  SdkEvalIdentification();

  /* Configure I/O communication channel */
  //SdkEvalComUartInit(UART_BAUDRATE);
	SdkEvalComIOConfig(Process_InputData);

  /* BlueNRG-1 stack init */
  ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);

  if (ret != BLE_STATUS_SUCCESS) {
    printf("Error in BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
    while(1);
    }

  /* Application demo Led Init */
  SdkEvalLedInit(LED1);
  SdkEvalLedInit(LED3);
  SdkEvalLedOn(LED1);
  SdkEvalLedOn(LED3);


  /*NOTE: In order to generate an user-friendly C code and helping user on his coding post-build activities, 
          the generated BLE stack APIs are placed within some predefined C functions (device_initialization(), set_database(), device_scanning(), 
          set_device_discoverable(), set_connection(), set_connection_update(), discovery(), read_write_characteristics() and update_characterists()).
  
          Once C code has been generated, user can move and modify APIs sequence order, within and outside these default user functions, 
          according to his specific needs.
  */

  //hci_read_local_version_information
  //status = hci_read_local_version_information(&hci_version, &hci_revision, &lmp_pal_version, &manufacturer_name, &lmp_pal_subversion);
  status = hci_read_local_version_information(&hci_version, &hci_revision, &lmp_pal_version, &manufacturer_name, &lmp_pal_subversion);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("hci_read_local_version_information() failed:0x%02x\r\n", status);
  }else{
    PRINTF("hci_read_local_version_information --> SUCCESS\r\n");
  }

  //hci_reset
  //status = hci_reset();
  status = hci_reset();
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("hci_reset() failed:0x%02x\r\n", status);
  }else{
    PRINTF("hci_reset --> SUCCESS\r\n");
  }

  //device_initialization
  device_initialization();


  while(1) {
    /* BlueNRG-1 stack tick */
    BTLE_StackTick();


    /* Application Tick */
    APP_Tick();


    /* Power Save management */
#if BLE_STACK_V_1_X ==1
    //BlueNRG_Sleep(SLEEPMODE_NOTIMER, 0, 0,0);
#else
    //BlueNRG_Sleep(SLEEPMODE_NOTIMER, 0, 0);
#endif


  }
}

/****************** BlueNRG-1 Sleep Management Callback ********************************/

SleepModes App_SleepMode_Check(SleepModes sleepMode)
{
  if(SdkEvalComIOTxFifoNotEmpty())
    return SLEEPMODE_RUNNING;

  return SLEEPMODE_NOTIMER;
}
/***************************************************************************************/


#ifdef USE_FULL_ASSERT


/**
 * @brief  Reports the name of the source file and the source line number where the assert_param error has occurred
 * @param  file	pointer to the source file name.
 * @param  line	assert_param error line source number.
 * @retval None.
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/** \endcond 
*/
