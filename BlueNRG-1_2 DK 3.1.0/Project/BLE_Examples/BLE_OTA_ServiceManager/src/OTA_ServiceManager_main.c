
/******************** (C) COPYRIGHT 2018 STMicroelectronics ********************
* File Name          : OTA_ServiceManager_main.c
* Author             : RF Application Team
* Version            : 1.1.0
* Date               : 15-January-2016
* Description        : Code demostrating the BLE OTA Service Manager application
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/**
 * @file  OTA_ServiceManager_main.c
 * @brief This application implements a basic standalone BLE Over The Air (OTA) firmware updgrade. 
 *        It provides the BLE Over-The-Air Service management for handling the OTA firmware upgrade
 *        of a BLE application which doesn't have any BLE OTA service. 

* \section KEIL_project KEIL project
  To use the project with KEIL uVision 5 for ARM, please follow the instructions below:
  -# Open the KEIL uVision 5 for ARM and select Project->Open Project menu. 
  -# Open the KEIL project
     <tt> ...\\Project\\BLE_Examples\\BLE_OTA_ServiceManager\\MDK-ARM\\BlueNRG-1\\BLE_OTA_ServiceManager.uvprojx </tt>
     <tt> ...\\Project\\BLE_Examples\\BLE_OTA_ServiceManager\\MDK-ARM\\BlueNRG-2\\BLE_OTA_ServiceManager.uvprojx </tt>
  -# Select desired configuration to build
  -# Select Project->Rebuild all target files. This will recompile and link the entire application
  -# To download the binary image, please connect STLink to JTAG connector in your board (if available).
  -# Select Project->Download to download the related binary image.
  -# Alternatively, open the BlueNRG1 Flasher utility and download the built binary image.

* \section IAR_project IAR project
  To use the project with IAR Embedded Workbench for ARM, please follow the instructions below:
  -# Open the Embedded Workbench for ARM and select File->Open->Workspace menu. 
  -# Open the IAR project
     <tt> ...\\Project\\BLE_Examples\\BLE_OTA_ServiceManager\\EWARM\\BlueNRG-1\\OTA_ServiceManager.eww </tt>
     <tt> ...\\Project\\BLE_Examples\\BLE_OTA_ServiceManager\\EWARM\\BlueNRG-2\\OTA_ServiceManager.eww </tt>
  -# Select desired configuration to build
  -# Select Project->Rebuild All. This will recompile and link the entire application
  -# To download the binary image, please connect STLink to JTAG connector in your board (if available).
  -# Select Project->Download and Debug to download the related binary image.
  -# Alternatively, open the BlueNRG1 Flasher utility and download the built binary image.

* \subsection Project_configurations Project configurations
- \c Release - Release configuration

     
* \section Board_supported Boards supported
- \c STEVAL-IDB007V1
- \c STEVAL-IDB007V2
- \c STEVAL-IDB008V1
- \c STEVAL-IDB008V2
- \c STEVAL-IDB009V1


 * \section Power_settings Power configuration settings
@table

==========================================================================================================
|                                         STEVAL-IDB00XV1                                                |
----------------------------------------------------------------------------------------------------------
| Jumper name |            |  Description                                                                |
| JP1         |   JP2      |                                                                             |
----------------------------------------------------------------------------------------------------------
| ON 1-2      | ON 2-3     | USB supply power                                                            |
| ON 2-3      | ON 1-2     | The supply voltage must be provided through battery pins.                   |
| ON 1-2      |            | USB supply power to STM32L1, JP2 pin 2 external power to BlueNRG1           |


@endtable 

* \section Jumper_settings Jumper settings
@table

========================================================================================================================================================================================
|                                                                             STEVAL-IDB00XV1                                                                                          |
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
| Jumper name |                                                                Description                                                                                             |
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------          
| JP1         | 1-2: to provide power from USB (JP2:2-3). 2-3: to provide power from battery holder (JP2:1-2)                                                                          |          
| JP2         | 1-2: to provide power from battery holder (JP1:2-3). 2-3: to provide power from USB (JP1:1-2). Pin2 to VDD  to provide external power supply to BlueNRG-1 (JP1: 1-2)   |
| JP3         | pin 1 and 2 UART RX and TX of MCU. pin 3 GND.                                                                                                                          |          
| JP4         | Fitted: to provide VBLUE to BlueNRG1. It can be used also for current measurement.                                                                                     |
| JP5         | Fitted : TEST pin to VBLUE. Not fitted:  TEST pin to GND                                                                                                               |


@endtable 

* \section Pin_settings Pin settings
@table
|  PIN name  |   STEVAL-IDB007V1  |   STEVAL-IDB007V2  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |
----------------------------------------------------------------------------------------------------------------------------
|    ADC1    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    ADC2    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO0    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO1    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO11    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO12    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO13    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO14    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO15    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO16    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO17    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO18    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO19    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|     IO2    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO20    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO21    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO22    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO23    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO24    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO25    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|     IO3    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO4    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO5    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO6    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO7    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO8    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    TEST1   |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |

@endtable 

* \section Serial_IO Serial I/O
@table
| Parameter name  | Value            | Unit      |
----------------------------------------------------
| Baudrate        | 115200 [default] | bit/sec   |
| Data bits       | 8                | bit       |
| Parity          | None             | bit       |
| Stop bits       | 1                | bit       |
@endtable

* \section LEDs_description LEDs description
@table
|  LED name  |          STEVAL-IDB007V1         |          STEVAL-IDB007V2         |          STEVAL-IDB008V1         |          STEVAL-IDB008V2         |          STEVAL-IDB009V1         |
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|     DL1    |             Not Used             |             Not Used             |             Not Used             |             Not Used             |             Not Used             |
|     DL2    |             Not Used             |             Not Used             |             Not Used             |             Not Used             |             Not Used             |
|     DL3    |  ON when OTA upgrade is ongoing  |  ON when OTA upgrade is ongoing  |  ON when OTA upgrade is ongoing  |  ON when OTA upgrade is ongoing  |  ON when OTA upgrade is ongoing  |
|     DL4    |             Not Used             |             Not Used             |             Not Used             |             Not Used             |             Not Used             |

@endtable


* \section Buttons_description Buttons description
@table
|   BUTTON name  |   STEVAL-IDB007V1  |   STEVAL-IDB007V2  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |
--------------------------------------------------------------------------------------------------------------------------------
|      PUSH1     |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|      PUSH2     |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|      RESET     |   Reset BlueNRG1   |   Reset BlueNRG1   |   Reset BlueNRG2   |   Reset BlueNRG2   |   Reset BlueNRG2   |

@endtable

* \section Usage Usage

 - The OTA Service Manager is a basic application which only supports the OTA Bootloader service.
 - It provides the BLE OTA bootloader service to any BLE application stored at fixed base address 
   on user Flash  which doesn't include any OTA service.
   It also includes the OTA Reset Manager functionalities in order to
   transfer the control to the proper valid application, after a BLE OTA session.
 - User is only requested to load the OTA_ServiceManager application and then build 
   any application using it with the ST_USE_OTA_SERVICE_MANAGER_APPLICATION=1 
   as preprocessor and linker option. 
 - Further, for jumping to the OTA Service Manager application, the application can call the
   OTA_Jump_To_Service_Manager_Application function (i.e. just using a platform button to 
   activate such call). 

  NOTEs: 
     - Refer to Use_OTA_ServiceManager workspaces on BLE_Beacon, BLE_Chat and SensorDemo IAR projects  for all related examples.
     

**/
   
/** @addtogroup BlueNRG1_demonstrations_applications
* BlueNRG-1 OTA Service manager \see OTA_ServiceManager_main.c for documentation.
*
*@{
*/

/** @} */
/** \cond DOXYGEN_SHOULD_SKIP_THIS
*/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "BlueNRG1_conf.h"
#include "bluenrg1_stack.h"
#include "clock.h"
#include "sleep.h"

#include "OTA_ServiceManager.h"
#include "OTA_ServiceManager_config.h"
#include "ble_const.h"
#include "SDK_EVAL_Config.h"
#include "OTA_btl.h" 
#include "BluenRG1_flash.h"
#include "bluenrg1_it_stub.h"

/* Private typedef -----------------------------------------------------------*/
typedef  void (*pFunction)(void);
/* Private define ------------------------------------------------------------*/

#ifdef DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define RESET_WAKE_DEEPSLEEP_REASONS 0x05

#define BLE_OTA_SERVICE_MANAGER_VERSION_STRING "1.0.0" 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/  
extern volatile uint32_t ota_sw_activation;

/* Private function prototypes -----------------------------------------------*/

/**
* @brief  It erases destination flash erase before starting OTA upgrade session. 
* @param  None.
* @retval None.
*
* @note The API code could be subject to change in future releases.
*/
static void OTA_Erase_Flash(uint16_t startNumber, uint16_t endNumber)
{
  uint16_t k; 
  
  for(k=startNumber;k<=endNumber;k++)
  { 
    FLASH_ErasePage(k); 
  }
  
}

/**
* @brief  It checks the runtime operation type and set the related OTA tags 
*         for handling the proper jumping to the valid application. 
* @param  None
* @retval None
*
* @note The API code could be subject to change in future releases.
*/
static void OTA_Check_ServiceManager_Operation(void) 
{
  if (ota_sw_activation == OTA_APP_SWITCH_OP_CODE_GO_TO_OTA_SERVICE_MANAGER) //Go to OTA Service manager
  {

    /* Unlock the Flash */
    flash_sw_lock = FLASH_UNLOCK_WORD;
    
    /* Set Invalid valid tag x OTA Application with OTA Service Manager  for allowing jumping to OTA Service manager */
    FLASH_ProgramWord(APP_WITH_OTA_SERVICE_ADDRESS + OTA_TAG_VECTOR_TABLE_ENTRY_OFFSET, OTA_INVALID_OLD_TAG);

    /* Lock the Flash */
    flash_sw_lock = FLASH_LOCK_WORD;
    
    /* Reset Service Manager ram location */ 
    ota_sw_activation = OTA_INVALID_OLD_TAG; 
  }
}

/**
* @brief  It defines the valid application address where to jump
*         by checking the OTA application validity tags for the lower and
*         higher applications
* @param  None.
* @retval appaddress: the application base address where to jump
*
* @note The API code could be subject to change in future releases.
*/
static uint32_t OTA_Check_Application_Tags_Value(void)
{
  uint32_t appAddress = 0;
  
  if (((TAG_VALUE(APP_OTA_SERVICE_ADDRESS) == OTA_SERVICE_MANAGER_TAG) && (TAG_VALUE(APP_WITH_OTA_SERVICE_ADDRESS) == OTA_IN_PROGRESS_TAG))|| /* 10 */
      ((TAG_VALUE(APP_OTA_SERVICE_ADDRESS) == OTA_SERVICE_MANAGER_TAG) && (TAG_VALUE(APP_WITH_OTA_SERVICE_ADDRESS) == OTA_INVALID_OLD_TAG))) /* 11 */ 
  {
    /* Jump to OTA Service Manager Application */
    appAddress = APP_OTA_SERVICE_ADDRESS;
  }
  else if ((TAG_VALUE(APP_OTA_SERVICE_ADDRESS) == OTA_SERVICE_MANAGER_TAG) && (TAG_VALUE(APP_WITH_OTA_SERVICE_ADDRESS) == OTA_VALID_TAG)) /* 12 */
  {
    /* Jump to Application using OTA Service Manager */
    appAddress = APP_WITH_OTA_SERVICE_ADDRESS;
  }  
  
  return appAddress;
}

int main(void)
{
  pFunction Jump_To_Application;
  uint32_t JumpAddress, appAddress;
  uint8_t ret; 
    
  /* Check Service manager RAM Location to verify if a jump to Service Manager has been set from the Application */
  OTA_Check_ServiceManager_Operation();
  
  /* Identifies the valid application where to jump based on the OTA application validity tags values placed on
  reserved vector table entry: OTA_TAG_VECTOR_TABLE_ENTRY_INDEX */
  appAddress = OTA_Check_Application_Tags_Value();
  
  /* Check if there is a valid application where to jump */
  if (appAddress == APP_WITH_OTA_SERVICE_ADDRESS)
  {
    /* Jump to user application */
    JumpAddress = *(__IO uint32_t*) (appAddress + 4);
    Jump_To_Application = (pFunction) JumpAddress;
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t*) appAddress);
    Jump_To_Application();
    
    /* Infinite loop */
    while (1)
    {
    }
  }
  
  /* Here Ota Service Manager Application is started */
  
  /* System Init */
  SystemInit();
  
  /* Identify BlueNRG1 platform */
  SdkEvalIdentification();
  
  Clock_Init();  
  SdkEvalComUartInit(UART_BAUDRATE);
  
  /* Erase the storage area from start page to end page */
  OTA_Erase_Flash(APP_WITH_OTA_SERVICE_PAGE_NUMBER_START,APP_WITH_OTA_SERVICE_PAGE_NUMBER_END);
  
  /* BlueNRG-1 stack init */
  ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error in BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
    while(1);
  }
  
  PRINTF("\r\nBlueNRG-1 BLE OTA Service Manager (version: %s)\r\n", BLE_OTA_SERVICE_MANAGER_VERSION_STRING); 
  
  /* Init OTA Service Manager Device */
  ret = OTA_ServiceManager_DeviceInit();
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("OTA_ServiceManager_DeviceInit()--> Failed 0x%02x\r\n", ret);
    while(1);
  }
  
  while(1)
  {
    /* BlueNRG-1 stack tick */
    BTLE_StackTick();
    
    /* Application tick */
    APP_Tick();
    
    /* Check if the OTA firmware upgrade session has been completed */
    if (OTA_Tick() == 1)
    {
      /* Jump to the new application */
      OTA_Jump_To_New_Application();
    }
    BlueNRG_Sleep(SLEEPMODE_NOTIMER, 0, 0);
  }
  
}

/****************** BlueNRG-1 Sleep Management Callback ********************************/

SleepModes App_SleepMode_Check(SleepModes sleepMode)
{
  if(SdkEvalComIOTxFifoNotEmpty() || SdkEvalComUARTBusy())
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
