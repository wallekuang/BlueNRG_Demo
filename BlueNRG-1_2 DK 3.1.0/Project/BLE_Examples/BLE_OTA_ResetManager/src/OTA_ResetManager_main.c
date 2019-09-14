
/******************** (C) COPYRIGHT 2018 STMicroelectronics ********************
* File Name          : OTA_ResetManager_main.c
* Author             : RF Application Team
* Version            : 1.1.0
* Date               : 15-January-2016
* Description        : Code demostrating the BLE OTA Reset Manager application
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/**
 * @file  OTA_ResetManager_main.c
 * @brief This application implements the OTA Reset Manager which, at reset,  
 *        passes control to the latest valid BLE application updated through the 
 *        BLE Over-The-Air (OTA) Service.
 * 

* \section KEIL_project KEIL project
  To use the project with KEIL uVision 5 for ARM, please follow the instructions below:
  -# Open the KEIL uVision 5 for ARM and select Project->Open Project menu. 
  -# Open the KEIL project
     <tt> ...\\Project\\BLE_Examples\\BLE_OTA_ResetManager\\MDK-ARM\\BlueNRG-1\\BLE_OTA_ResetManager.uvprojx </tt>
     <tt> ...\\Project\\BLE_Examples\\BLE_OTA_ResetManager\\MDK-ARM\\BlueNRG-2\\BLE_OTA_ResetManager.uvprojx </tt>
  -# Select desired configuration to build
  -# Select Project->Rebuild all target files. This will recompile and link the entire application
  -# To download the binary image, please connect STLink to JTAG connector in your board (if available).
  -# Select Project->Download to download the related binary image.
  -# Alternatively, open the BlueNRG1 Flasher utility and download the built binary image.

* \section IAR_project IAR project
  To use the project with IAR Embedded Workbench for ARM, please follow the instructions below:
  -# Open the Embedded Workbench for ARM and select File->Open->Workspace menu. 
  -# Open the IAR project
     <tt> ...\\Project\\BLE_Examples\\BLE_OTA_ResetManager\\EWARM\\BlueNRG-1\\BLE_OTA_ResetManager.eww </tt>
     <tt> ...\\Project\\BLE_Examples\\BLE_OTA_ResetManager\\EWARM\\BlueNRG-2\\BLE_OTA_ResetManager.eww </tt>
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
|  LED name  |   STEVAL-IDB007V1  |   STEVAL-IDB007V2  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |
----------------------------------------------------------------------------------------------------------------------------
|     DL1    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     DL2    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     DL3    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     DL4    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |

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

  - The OTA Reset Manager is a basic application which is is stored at BlueNRG-1,2 FLASH base address (0x10040000) and 
    it allows to transfer of control towards the new upgraded application every time we reset.
  - The new application has to add the OTA service and related characteristics defined on 
    files OTA_btl.c.
  - At device reset, the reset manager will take care of jumping to the location of the last image 
    that was successfully loaded by the OTA bootloader.

NOTEs
  - Before downloading the OTA Reset Manager performs a device Mass Erase of the selected BlueNRG-1,2 device (use IAR, Project, Download, Erase Memory). Then, open the IAR project related to a Lower Application with OTA Service and download it on the selected device. At this stage, the BlueNRG-1,2 device is ready for performing OTA upgrades.
  - Refer to BLE_Chat and SensorDemo workspaces IAR projects for related OTA examples (Lower and Higher Applications with OTA service workspaces).
  - On BlueNRG-2, BLE stack v2.1 or later, OTA FW upgrade also supports the data length extended capability. User is requested to add the OTA_EXTENDED_PACKET_LEN=1 option and to use a BLE stack configuration ioption supporting this capability (BLE_STACK_CONFIGURATION=BLE_OTA_BASIC_CONFIGURATION or BLE_STACK_CONFIGURATION=BLE_STACK_FULL_CONFIGURATION).

**/
   
/** @addtogroup BlueNRG1_demonstrations_applications
* BlueNRG-1 OTA Reset manager \see OTA_ResetManager_main.c for documentation.
*
*@{
*/

/** @} */
/** \cond DOXYGEN_SHOULD_SKIP_THIS
*/
/* Includes ------------------------------------------------------------------*/

#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"
#include "bluenrg1_api.h"
#include "OTA_btl.h"
#include "bluenrg1_it_stub.h"

typedef  void (*pFunction)(void);

#define RESET_WAKE_DEEPSLEEP_REASONS 0x05

/** @brief Get specific application tag value stored at vector table index 
* OTA_TAG_VECTOR_TABLE_ENTRY_OFFSET
*/
#define TAG_VALUE(x)     (* ((volatile uint32_t*) ((x) + OTA_TAG_VECTOR_TABLE_ENTRY_OFFSET)))

/**
* @brief  It check if  flash storage area has to be erased or not
* @param  None.
* @retval Status: 1 (erase flash); 0 (don't erase flash).
*
* @note The API code could be subject to change in future releases.
*/
static uint8_t OTA_Check_Storage_Area(uint32_t start_address, uint32_t end_address)
{
  volatile uint32_t *address; 
  uint32_t i; 
  
  for(i=start_address;i<end_address; i = i +4)
  { 
    address = (volatile uint32_t *) i;
    if (*address != 0xFFFFFFFF)
      return 1; /* do flash erase */
  }
  
  return 0; /* no flash erase is required */
}

/**
* @brief  It erases the new flash storage area. 
* @param  None.
* @retval None.
*
* @note The API code could be subject to change in future releases.
*/
static void OTA_Erase_Storage_Area(uint16_t startPageNumber, uint16_t endPageNumber)
{
  uint16_t k; 
  
  for(k=startPageNumber;k<=endPageNumber;k++)
  { 
    FLASH_ErasePage(k); 
  }
}

/**
* @brief  It defines the valid application address where to jump
*         by checking the OTA application validity tags for the lower and
*         higher applications
* @param  None.
* @retval None.
*
* @note The API code could be subject to change in future releases.
*/
static uint32_t OTA_Check_Application_Tags_Value(void)
{
  uint32_t appAddress = 0;
  if ( ((TAG_VALUE(APP_LOWER_ADDRESS) == OTA_INVALID_OLD_TAG) && (TAG_VALUE(APP_HIGHER_ADDRESS) == OTA_INVALID_OLD_TAG))   || /* 1 */
      ((TAG_VALUE(APP_LOWER_ADDRESS) == OTA_VALID_TAG) && (TAG_VALUE(APP_HIGHER_ADDRESS) == OTA_INVALID_OLD_TAG))         || /* 2 */
        ((TAG_VALUE(APP_LOWER_ADDRESS) == OTA_VALID_TAG) && (TAG_VALUE(APP_HIGHER_ADDRESS) == OTA_VALID_TAG))               || /* 4 */
          ((TAG_VALUE(APP_LOWER_ADDRESS) == OTA_INVALID_OLD_TAG) && (TAG_VALUE(APP_HIGHER_ADDRESS) == OTA_IN_PROGRESS_TAG))   || /* 8 */
            ((TAG_VALUE(APP_LOWER_ADDRESS) == OTA_VALID_TAG) && (TAG_VALUE(APP_HIGHER_ADDRESS) == OTA_IN_PROGRESS_TAG)))           /* 9 */  
  {
    /* Jump to Lower Application */
    appAddress = APP_LOWER_ADDRESS;
    
    if (OTA_Check_Storage_Area(APP_HIGHER_ADDRESS,APP_HIGHER_ADDRESS_END))
    {
      /* Unlock the Flash */
      flash_sw_lock = FLASH_UNLOCK_WORD;
      
      /* Erase OLD Higher application storage area */
      OTA_Erase_Storage_Area(OTA_HIGHER_APPLICATION_PAGE_NUMBER_START, OTA_HIGHER_APPLICATION_PAGE_NUMBER_END); 
      
      /* Lock the Flash */
      flash_sw_lock = FLASH_LOCK_WORD;
    }
  }
  else if ( ((TAG_VALUE(APP_LOWER_ADDRESS) == OTA_INVALID_OLD_TAG) && (TAG_VALUE(APP_HIGHER_ADDRESS) == OTA_VALID_TAG))       || /* 3 */
           ((TAG_VALUE(APP_LOWER_ADDRESS) == OTA_IN_PROGRESS_TAG) && (TAG_VALUE(APP_HIGHER_ADDRESS) == OTA_INVALID_OLD_TAG)) || /* 6 */
             ((TAG_VALUE(APP_LOWER_ADDRESS) == OTA_IN_PROGRESS_TAG) && (TAG_VALUE(APP_HIGHER_ADDRESS) == OTA_VALID_TAG)))         /* 7 */     
  {
    /* Jump to Higher Application */
    appAddress = APP_HIGHER_ADDRESS;
    
    if (OTA_Check_Storage_Area(APP_LOWER_ADDRESS,APP_LOWER_ADDRESS_END))
    {
      /* Unlock the Flash */
      flash_sw_lock = FLASH_UNLOCK_WORD;
      
      /* Erase OLD Lower application storage area */
      OTA_Erase_Storage_Area(OTA_LOWER_APPLICATION_PAGE_NUMBER_START, OTA_LOWER_APPLICATION_PAGE_NUMBER_END); 
      
      /* Lock the Flash */
      flash_sw_lock = FLASH_LOCK_WORD;
      
    }
  }
  else if ((TAG_VALUE(APP_LOWER_ADDRESS) == OTA_IN_PROGRESS_TAG) && (TAG_VALUE(APP_HIGHER_ADDRESS) == OTA_IN_PROGRESS_TAG))   /* 5 */
  {
    /* 5: Is it possible? No. What to do?*/
  }
  return appAddress;
}

/**
* @brief  OTA Reset Manager main function
* @param  None.
* @retval None.
*
* @note The code could be subject to change in future releases.
*/
int main(void) 
{
  pFunction Jump_To_Application;
  uint32_t JumpAddress, appAddress;
  
  /* Identifies the valid application where to jump based on the OTA application validity tags values placed on
  reserved vector table entry: OTA_TAG_VECTOR_TABLE_ENTRY_INDEX */
  appAddress = OTA_Check_Application_Tags_Value();
  
  if (appAddress == 0) {
    /* This case indicates that no valida application is present and normally should not happen */
    while (1);
  }
  
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
/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
/** \endcond
*/
