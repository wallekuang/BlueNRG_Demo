
/******************** (C) COPYRIGHT 2018 STMicroelectronics ********************
* File Name          : BLE_Beacon_Flash_main.c
* Author             : RF Application Team
* Version            : 1.0.0
* Date               : 20-November-2017
* Description        : Code demostrating the BLE Beacon application and coexistence with Flash operations
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/**
 * @file BLE_Beacon_Flash_main.c
 * @brief This is a BLE beacon demo that shows how to configure a BlueNRG-1,2 device 
 * in order to advertise specific manufacturing data and performs Flash write operations
 * by synchronizing them with the planned BLE radio operations
 * 

* \section ATOLLIC_project ATOLLIC project
  To use the project with ATOLLIC TrueSTUDIO for ARM, please follow the instructions below:
  -# Open the ATOLLIC TrueSTUDIO for ARM and select File->Import... Project menu. 
  -# Select Existing Projects into Workspace. 
  -# Select the ATOLLIC project
  -# Select desired configuration to build from Project->Manage Configurations
  -# Select Project->Rebuild Project. This will recompile and link the entire application
  -# To download the binary image, please connect STLink to JTAG connector in your board (if available).
  -# Select Project->Download to download the related binary image.
  -# Alternatively, open the BlueNRG1 Flasher utility and download the built binary image.

* \section KEIL_project KEIL project
  To use the project with KEIL uVision 5 for ARM, please follow the instructions below:
  -# Open the KEIL uVision 5 for ARM and select Project->Open Project menu. 
  -# Open the KEIL project
     <tt> ...\\Project\\BLE_Examples\\BLE_Beacon_FlashManagement\\MDK-ARM\\BlueNRG-1\\BLE_Beacon_FlashManagement.uvprojx </tt> or
     <tt> ...\\Project\\BLE_Examples\\BLE_Beacon_FlashManagement\\MDK-ARM\\BlueNRG-2\\BLE_Beacon_FlashManagement.uvprojx </tt>
  -# Select desired configuration to build
  -# Select Project->Rebuild all target files. This will recompile and link the entire application
  -# To download the binary image, please connect STLink to JTAG connector in your board (if available).
  -# Select Project->Download to download the related binary image.
  -# Alternatively, open the BlueNRG1 Flasher utility and download the built binary image.

* \section IAR_project IAR project
  To use the project with IAR Embedded Workbench for ARM, please follow the instructions below:
  -# Open the Embedded Workbench for ARM and select File->Open->Workspace menu. 
  -# Open the IAR project
     <tt> ...\\Project\\BLE_Examples\\BLE_Beacon_FlashManagement\\EWARM\\BlueNRG-1\\BLE_Beacon_FlashManagement.eww </tt> or
     <tt> ...\\Project\\BLE_Examples\\BLE_Beacon_FlashManagement\\EWARM\\BlueNRG-2\\BLE_Beacon_FlashManagement.eww </tt>
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

The Beacon demo configures a BlueNRG-1,2 device in advertising mode (non-connectable mode) with specific manufacturing data.
It transmits advertisement packets at regular intervals which contain the following manufacturing data:
@table   
------------------------------------------------------------------------------------------------------------------------
| Data field              | Description                       | Notes                                                  |
------------------------------------------------------------------------------------------------------------------------
| Company identifier code | SIG company identifier (1)        | Default is 0x0030 (STMicroelectronics)                 |
| ID                      | Beacon ID                         | Fixed value                                            |
| Length                  | Length of the remaining payload   | NA                                                     |
| Location UUID           | Beacons UUID                      | It is used to distinguish specific beacons from others |
| Major number            | Identifier for a group of beacons | It is used to group a related set of beacons           |                                              
| Minor number            | Identifier for a single beacon    | It is used to identify a single beacon                 |                                       
| Tx Power                | 2's complement of the Tx power    | It is used to establish how far you are from device    |                                       
@endtable

 - (1): SIG company identifiers are available on https://www.bluetooth.org/en-us/specification/assigned-numbers/company-identifiers
 - NA : Not Applicable;

Further this application shows how to properly handling Flash operations (Erase and Write) and preserving the BLE radio activities.
This is achieved by synchronizing with the scheduled BLE radio activities through the aci_hal_end_of_radio_activity_event() event callback timing information.
It is possible to check the Flash activity by reading the Flash memory at the address 0x10066800 (for example using the BlueNRG-1 Flasher - Utility GUI).

**/
   
/** @addtogroup BlueNRG1_demonstrations_applications
 *  BlueNRG-1 Beacon demo \see BLE_Beacon_Flash_main.c for documentation.
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
#include "sleep.h"
#include "SDK_EVAL_Config.h"
#include "Beacon_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BLE_BEACON_VERSION_STRING "1.0.0"

/* Set to 1 for enabling Flags AD Type position at the beginning 
   of the advertising packet */
#define ENABLE_FLAGS_AD_TYPE_AT_BEGINNING 1

#define FLASH_ERASE_GUARD_TIME   25  // 25 ms
#define FLASH_WRITE_GUARD_TIME   1  // 1 ms

#define BLE_BEACON_TEST_PAGE_ADDRESS          0x10066800

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t Next_Advertising_SysTime = 0;

/* Private function prototypes -----------------------------------------------*/
void FlashRoutine(void);

/* Private functions ---------------------------------------------------------*/

void Device_Init(void)
{
  uint8_t ret;
  uint16_t service_handle;
  uint16_t dev_name_char_handle;
  uint16_t appearance_char_handle;
  
  /* Set the TX Power to -2 dBm */
  ret = aci_hal_set_tx_power_level(1,4);
  if(ret != 0) {
    printf ("Error in aci_hal_set_tx_power_level() 0x%04xr\n", ret);
    while(1);
  }

  /* Init the GATT */
  ret = aci_gatt_init();
  if (ret != 0)
    printf ("Error in aci_gatt_init() 0x%04xr\n", ret);
  else
    printf ("aci_gatt_init() --> SUCCESS\r\n");
  
  /* Init the GAP */
  ret = aci_gap_init(0x01, 0x00, 0x08, &service_handle, 
                     &dev_name_char_handle, &appearance_char_handle);
  if (ret != 0)
    printf ("Error in aci_gap_init() 0x%04x\r\n", ret);
  else
    printf ("aci_gap_init() --> SUCCESS\r\n");
}


/**
* @brief  Start beaconing
* @param  None 
* @retval None
*/
static void Start_Beaconing(void)
{  
  uint8_t ret = BLE_STATUS_SUCCESS;

#if ENABLE_FLAGS_AD_TYPE_AT_BEGINNING
  /* Set AD Type Flags at beginning on Advertising packet  */
  uint8_t adv_data[] = {
      /* Advertising data: Flags AD Type */
      0x02, 
      0x01, 
      0x06, 
      /* Advertising data: manufacturer specific data */
      26, //len
      AD_TYPE_MANUFACTURER_SPECIFIC_DATA,  //manufacturer type
      0x30, 0x00, //Company identifier code (Default is 0x0030 - STMicroelectronics: To be customized for specific identifier)
      0x02,       // ID
      0x15,       //Length of the remaining payload
      0xE2, 0x0A, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4, //Location UUID
      0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61,
      0x00, 0x00, // Major number 
      0x00, 0x00, // Minor number 
      0xC8        //2's complement of the Tx power (-56dB)};      
   };
#else
   uint8_t manuf_data[] = {
      26, //len
      AD_TYPE_MANUFACTURER_SPECIFIC_DATA, //manufacturer type
      0x30, 0x00, //Company identifier code (Default is 0x0030 - STMicroelectronics: To be customized for specific identifier)
      0x02,       // ID
      0x15,       //Length of the remaining payload
      0xE2, 0x0A, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4, //Location UUID
      0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61,
      0x00, 0x00, // Major number 
      0x00, 0x00, // Minor number 
      0xC8        //2's complement of the Tx power (-56dB)};      
   };
#endif
   
  /* disable scan response */
  ret = hci_le_set_scan_response_data(0,NULL);
  if (ret != BLE_STATUS_SUCCESS)
  {
    printf ("Error in hci_le_set_scan_resp_data() 0x%04x\r\n", ret);
    return;
  }
  else
    printf ("hci_le_set_scan_resp_data() --> SUCCESS\r\n");

  /* put device in non connectable mode */
  ret = aci_gap_set_discoverable(ADV_NONCONN_IND, 160, 160, PUBLIC_ADDR, NO_WHITE_LIST_USE,
                                 0, NULL, 0, NULL, 0, 0); 
  if (ret != BLE_STATUS_SUCCESS)
  {
    printf ("Error in aci_gap_set_discoverable() 0x%04x\r\n", ret);
    return;
  }
  else
    printf ("aci_gap_set_discoverable() --> SUCCESS\r\n");

#if ENABLE_FLAGS_AD_TYPE_AT_BEGINNING
  /* Set the  ADV data with the Flags AD Type at beginning of the 
     advertsing packet,  followed by the beacon manufacturer specific data */
  ret = hci_le_set_advertising_data (sizeof(adv_data), adv_data);
  if (ret != BLE_STATUS_SUCCESS)
  {
    printf ("Error in hci_le_set_advertising_data() 0x%04x\r\n", ret);
    return;
  }
  else
    printf ("hci_le_set_advertising_data() --> SUCCESS\r\n");
#else
  /* Delete the TX power level information */
  ret = aci_gap_delete_ad_type(AD_TYPE_TX_POWER_LEVEL); 
  if (ret != BLE_STATUS_SUCCESS)
  {
    printf ("Error in aci_gap_delete_ad_type() 0x%04x\r\n", ret);
    return;
  }
  else
    printf ("aci_gap_delete_ad_type() --> SUCCESS\r\n");

  /* Update the ADV data with the BEACON manufacturing data */
  ret = aci_gap_update_adv_data(27, manuf_data);  
  if (ret != BLE_STATUS_SUCCESS)
  {
    printf ("Error in aci_gap_update_adv_data() 0x%04x\r\n", ret);
    return;
  }
  else
    printf ("aci_gap_update_adv_data() --> SUCCESS\r\n");
#endif
}

int main(void) 
{
  uint8_t ret;
  
  /* System Init */
  SystemInit();
  
  /* Identify BlueNRG-1 platform */
  SdkEvalIdentification();

  /* Init the UART peripheral */
  SdkEvalComUartInit(UART_BAUDRATE); 

  /* Erase a page of the Flash memory */
  FLASH_ErasePage((BLE_BEACON_TEST_PAGE_ADDRESS-_MEMORY_FLASH_BEGIN_)/_MEMORY_BYTES_PER_PAGE_);
  
  /* BlueNRG-1 stack init */
  ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);
  if (ret != BLE_STATUS_SUCCESS) {
    printf("Error in BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
    while(1);
  }
  
  /* Init the BlueNRG-1 device */
  Device_Init();
  
  /* Enable the callback end_of_radio_activity_event for advertising */
  aci_hal_set_radio_activity_mask(0x0002);
  
  /* Start Beacon Non Connectable Mode*/
  Start_Beaconing();
  
  printf("BlueNRG-1 BLE Beacon Application (version: %s)\r\n", BLE_BEACON_VERSION_STRING); 
  
  while(1) 
  {
    /* BlueNRG-1 stack tick */
    BTLE_StackTick();
    
    /* Flash operation */
    FlashRoutine();
        
    /* Enable Power Save according the Advertising Interval */
    BlueNRG_Sleep(SLEEPMODE_NOTIMER, 0, 0);
  }
}


void aci_hal_end_of_radio_activity_event(uint8_t Last_State,
                                         uint8_t Next_State,
                                         uint32_t Next_State_SysTime)
{
  if (Next_State == 0x01) /* 0x01: Advertising */
  {  /* Check if data for flash write are ready: do write only if there is enough time before next radio activity */
    Next_Advertising_SysTime = Next_State_SysTime;
  }
}


void FlashRoutine(void)
{
  static uint32_t flash_counter = 0;
  static uint32_t flash_pattern = 0xAAAAAAAA;
  
  if(flash_counter == 512) {
    if (HAL_VTimerDiff_ms_sysT32(Next_Advertising_SysTime, HAL_VTimerGetCurrentTime_sysT32()) > FLASH_ERASE_GUARD_TIME) {
      if(FLASH->IRQRAW_b.CMDDONE == SET) {
        FLASH_ErasePage((BLE_BEACON_TEST_PAGE_ADDRESS-_MEMORY_FLASH_BEGIN_)/_MEMORY_BYTES_PER_PAGE_);
        flash_pattern = ~flash_pattern;
        flash_counter = 0;
      }
    }
  }
  else {
    if (HAL_VTimerDiff_ms_sysT32(Next_Advertising_SysTime, HAL_VTimerGetCurrentTime_sysT32()) > FLASH_WRITE_GUARD_TIME) {
      if(FLASH->IRQRAW_b.CMDDONE == SET) {
        FLASH_ProgramWord((BLE_BEACON_TEST_PAGE_ADDRESS + flash_counter*4), flash_pattern);
        flash_counter++;
      }
    }
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

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
/** \endcond
 */
