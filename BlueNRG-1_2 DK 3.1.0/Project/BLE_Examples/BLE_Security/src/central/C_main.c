/******************** (C) COPYRIGHT 2018 STMicroelectronics ********************
* File Name          : C_main.c
* Author             : AMS RF Application Team
* Version            : V1.2.0
* Date               : 12-December-2018
* Description        : BlueNRG-1,2 Security central role main file
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/** @addtogroup BlueNRG1_demonstrations_applications
 * BlueNRG-1 Security Central \see C_main.c for documentation.
 *
 *@{
 */

/** @} */
/** \cond DOXYGEN_SHOULD_SKIP_THIS
 */
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h"
#include "bluenrg1_stack.h"
#include "SDK_EVAL_Config.h"
#include "sleep.h"
#include "gatt_db.h"
#include "master_basic_profile.h"
#include "master_config.h" 
#include "BLE_Security_Central_config.h"

#define BLE_APPLICATION_VERSION_STRING "1.0.0" 

#ifndef DEBUG
#define DEBUG 1
#endif

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/* Private variables ---------------------------------------------------------*/
static char Security_Configuration[] = SECURITY_MODE_NAME; 

/* Uart RX */
static uint8_t wakeup_source = WAKEUP_IO11; 
static uint8_t wakeup_level = (WAKEUP_IOx_LOW << WAKEUP_IO11_SHIFT_MASK); 

/* Private function prototypes -----------------------------------------------*/

static void user_set_wakeup_source(SdkEvalButton Button)
{
  if (Button == BUTTON_1)
  {
     /* IRQ_ON_FALLING_EDGE is set since wakeup level is low (mandatory setup for wakeup sources);  
       IRQ_ON_RISING_EDGE is the specific application level */ 
     SdkEvalPushButtonIrq(USER_BUTTON, IRQ_ON_BOTH_EDGE); 
     
     wakeup_source |= WAKEUP_IO13;
     wakeup_level |= (WAKEUP_IOx_LOW << WAKEUP_IO13_SHIFT_MASK);
  }
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
  uint32_t powersave_state = 0; 
  SleepModes sleepMode = SLEEPMODE_NOTIMER; 
	
  /* System Init */
  SystemInit();
  
  /* Identify BlueNRG1 platform */
  SdkEvalIdentification(); 
   
  /* Configure I/O communication channel */
  SdkEvalComUartInit(UART_BAUDRATE);
  
  /* Configure I/O communication channel  */
  SdkEvalComIOConfig(SdkEvalComIOProcessInputData);
 
  /* BlueNRG-1 stack init */
  ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error in BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
    while(1);
  }
     
  /* Master Init procedure */
  if (deviceInit() != BLE_STATUS_SUCCESS) {
    PRINTF("Error during the device init procedure\r\n");
  }
  
  /* Master set security */
  ret = Device_Security();
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error in Device_Security() 0x%02x\r\n", ret);
    while(1);
  }
    
  /* Initialize the button */
  SdkEvalPushButtonInit(USER_BUTTON);
  user_set_wakeup_source(USER_BUTTON);
  
  PRINTF("BlueNRG-1 BLE Security Central Application (version: %s, security mode: %s, button: %d)\r\n", BLE_APPLICATION_VERSION_STRING,Security_Configuration, USER_BUTTON);
   
  /* Start Master Device Discovery procedure */
  if (deviceDiscovery() != BLE_STATUS_SUCCESS) {
    PRINTF("Error during the device discovery procedure\r\n");
  }
  
  /* Main loop */
  while(1) {
    
    /* BLE Stack Tick */
    BTLE_StackTick();
    
    /* Application Tick */
    APP_Tick();
		
    /* Master Tick */
    powersave_state = Master_Process(); 

    /* Check master library power state LSB bit: if 1 micro can enter in low powe mode */
    (powersave_state & 0x1) ? (sleepMode = SLEEPMODE_NOTIMER) : (sleepMode = SLEEPMODE_RUNNING);  
    
    /* Power Save management: no timer and wakeup on UART RX, PUSH button */
    BlueNRG_Sleep(sleepMode, wakeup_source, wakeup_level);
    
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
