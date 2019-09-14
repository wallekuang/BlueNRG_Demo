
/******************** (C) COPYRIGHT 2018 STMicroelectronics ********************
* File Name          : Power_Consumption_Test.c
* Author             : RF Application Team
* Version            : 1.0.0
* Date               : 12-April-2017
* Description        : Code demostrating the BlueNRG-1 current consumption in BLE normal operating mode
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/**
 * @file Power_Consumption_Test.c
 * @brief This demonstration application allows to properly configure the BlueNRG-1,2 devices and show the related current consumption  during two BLE operating modes: advertising and connection.


 * 

* \section KEIL_project KEIL project
  To use the project with KEIL uVision 5 for ARM, please follow the instructions below:
  -# Open the KEIL uVision 5 for ARM and select Project->Open Project menu. 
  -# Open the KEIL project
     <tt> ...\\Project\\BLE_Examples\\BLE_Power_Consumption\\MDK-ARM\\BlueNRG-1\\BLE_Power_Consumption.uvprojx </tt> or
     <tt> ...\\Project\\BLE_Examples\\BLE_Power_Consumption\\MDK-ARM\\BlueNRG-2\\BLE_Power_Consumption.uvprojx </tt>
  -# Select desired configuration to build
  -# Select Project->Rebuild all target files. This will recompile and link the entire application
  -# To download the binary image, please connect STLink to JTAG connector in your board (if available).
  -# Select Project->Download to download the related binary image.
  -# Alternatively, open the BlueNRG1 Flasher utility and download the built binary image.

* \section IAR_project IAR project
  To use the project with IAR Embedded Workbench for ARM, please follow the instructions below:
  -# Open the Embedded Workbench for ARM and select File->Open->Workspace menu. 
  -# Open the IAR project
     <tt> ...\\Project\\BLE_Examples\\BLE_Power_Consumption\\EWARM\\BlueNRG-1\\BLE_Power_Consumption.eww </tt> or
     <tt> ...\\Project\\BLE_Examples\\BLE_Power_Consumption\\EWARM\\BlueNRG-2\\BLE_Power_Consumption.eww </tt>
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
  The application will listen for keys typed and it will execute the associated command.
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
|      RESET     |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |

@endtable

* \section Usage Usage

The application allow to put the selected BLE device in discovery mode: user can select from a test menu which advertising interval to use (100 ms or 1000 ms). To take the BlueNRG-1,2 current consumption is necessary to connect a DC power analyzer to the JP4 connector of the STEVAL-IDB007V1, STEVAL-IDB008V1 kit platforms.
Further, user can setup a connection with another device configured as a master and take the related power consumption measurements. 
The master role can be covered from another BlueNRG-1, BlueNRG-2 kit platform configured with the DTM FW application (DTM_UART.hex), and running a specific script through BlueNRG GUI or Script launcher PC applications. 
On BLE_Power_Consumption demo application project folder, two scripts are provided for configuring the master device and create a connection with the BlueNRG-1,2 kit platform under test.
The two scripts allow to establish a connection with, respectively, 100 ms and 1000 ms as connection interval.
 
The power consumption demo supports some test commands:
- <b>f</b>: Device in Discoverable mode with fast interval 100 ms.
- <b>s</b>: Device in discoverable mode with slow interval 1000 ms.
- <b>r</b>: Reset the BlueNRG-1.
- <b>?</b>: Display this help menu.


The following table provides the BlueNRG-1, BlueNRG-2 current consumption values on the highlighted tests scenarios: 

@table
| BLE Stack version   | Advertising 100 ms       | Advertising 1000  ms      | Connection 100  ms      | Connection 1000  ms      |     
-----------------------------------------------------------------------------------------------------------------------------------
| V2.0                | 136.931 uA               | 15.342 uA                 | 50.423 uA               | 7.059 uA                 |
| V2.1 Basic          | 146.06 uA	         | 17.326 uA	             | 53.966 uA               | 7.439 uA                 |
| V2.1 Full           | 145.96 uA                | 15.645 uA	             | 54.156 uA	       | 7.492 uA                 |
@endtable

**/
   

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "sleep.h"
#include "bluenrg1_stack.h"
#include "ble_status.h"
#include "ble_const.h"
#include "SDK_EVAL_Config.h"
#include "Power_Consumption_Test_config.h"

/** @addtogroup BlueNRG1_StdPeriph_Examples BlueNRG1 Standard Peripheral Examples
  * @{
  */

/** @addtogroup Micro_Examples Micro Examples
  * @{
  */

/** @addtogroup Micro_SleepTest  Micro Sleep Test Example
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define ADV_INTERVAL_FAST_MS  100
#define ADV_INTERVAL_SLOW_MS  1000

#define DEVICE_IDLE_STATE         0
#define DEVICE_ADV_STATE          1
#define DEVICE_CONN_STATE         2
#define DEVICE_SLEEP_STATE        3
#define DEVICE_DISCONNECTED_STATE 4

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t device_state;
uint16_t connection_interval;

/*******************************************************************************
 * Function Name  : help.
 * Description    : Prints the test option.
 * Input          : None.
 * Return         : None.
 *******************************************************************************/
void help(void)
{
  printf("\r\n");
  printf("f:   Device in Discoverable mode with fast interval 100 ms \r\n");
  printf("s:   Device in discoverable mode with slow interval 1000 ms \r\n");
  printf("r:   Reset the BlueNRG-1\r\n");
  printf("?:   Display this help menu\r\n");
  printf("\r\n> ");
}

/*******************************************************************************
 * Function Name  : DeviceInit.
 * Description    : Init the device.
 * Input          : None.
 * Return         : None.
 *******************************************************************************/
void DeviceInit(void)
{
  uint8_t bdaddr[] = {0xBC, 0xEC, 0x00, 0xE1, 0x80, 0x02};
  uint8_t ret;
  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
  uint8_t device_name[] = {'P', 'o', 'w', 'e', 'r', 'C', 'o', 'n', 's', 'u', 'm', 'p', 't', 'i', 'o', 'n', 'T', 'e', 's', 't'};

  /* Set the device public address */
  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN, bdaddr);  
  if(ret != BLE_STATUS_SUCCESS) {
    printf("aci_hal_write_config_data() failed: 0x%02x\r\n", ret);
  }
  
  /* Set the TX power -2 dBm */
  aci_hal_set_tx_power_level(1, 4);
  
  /* GATT Init */
  ret = aci_gatt_init();
  if (ret != BLE_STATUS_SUCCESS) {
    printf("aci_gatt_init() failed: 0x%02x\r\n", ret);
  }
  
  /* GAP Init */
  ret = aci_gap_init(GAP_PERIPHERAL_ROLE, 0, sizeof(device_name), &service_handle,
                     &dev_name_char_handle, &appearance_char_handle);
  if (ret != BLE_STATUS_SUCCESS) {
    printf("aci_gap_init() failed: 0x%02x\r\n", ret);
  }
 
  /* Update device name */
  ret = aci_gatt_update_char_value_ext(0,service_handle, dev_name_char_handle,0,sizeof(device_name),0, sizeof(device_name), device_name);
  if(ret != BLE_STATUS_SUCCESS) {
    printf("aci_gatt_update_char_value_ext() failed: 0x%02x\r\n", ret);
  }
}

/*******************************************************************************
 * Function Name  : Set_DeviceConnectable.
 * Description    : Puts the device in connectable mode.
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
void Set_DeviceConnectable(uint16_t adv_interval)
{  
  uint8_t ret;
  uint8_t local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME, 'P', 'o', 'w', 'e', 'r', 'C', 'o', 'n', 's', 'u', 'm', 'p', 't', 'i', 'o', 'n', 'T', 'e', 's', 't'};

  if (adv_interval == ADV_INTERVAL_SLOW_MS)
    printf("Set Discoverable Mode Slow Interval 1000 ms");
  else
    printf("Set Discoverable Mode Fast Interval 100 ms");
  
  hci_le_set_scan_response_data(0,NULL);
  
  ret = aci_gap_set_discoverable(ADV_IND,
                                (adv_interval*1000)/625,(adv_interval*1000)/625,
                                 PUBLIC_ADDR, NO_WHITE_LIST_USE,
                                 sizeof(local_name), local_name, 0, NULL, 0, 0); 
  if(ret != BLE_STATUS_SUCCESS) {
    printf("--> FAILED: 0x%02x\r\n",ret);
  } else {
    device_state = DEVICE_ADV_STATE;
    printf("--> SUCCESS\r\n");
  }
}


int main(void)
{
  uint8_t charRead, ret;
  
  /* System Init */
  SystemInit();
  
  /* Identify BlueNRG1 platform */
  SdkEvalIdentification();
  
  /* Enable the UART */
  SdkEvalComUartInit(UART_BAUDRATE);
  
  /* Disable the PKA peripheral clock to reduce the power consumption */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_PKA, DISABLE);
  
  /* BlueNRG-1 stack init */
  ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);
  if (ret != BLE_STATUS_SUCCESS) {
    printf("Error during BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
    while(1);
  }
  
  /* Setup the BLE GAP and GATT layer*/
  DeviceInit();

  printf("Power Consumption Test\r\n");
  printf("Enter ? for list of commands\r\n");
 
  device_state = DEVICE_IDLE_STATE;
  while(1) {
    BTLE_StackTick();

    if (__io_getcharNonBlocking(&charRead)) {
      switch (charRead) {
      case 'f':
        if (device_state == DEVICE_IDLE_STATE) {
          /* Device in discoverable mode with fast adv interval 100 ms */
          Set_DeviceConnectable(ADV_INTERVAL_FAST_MS);
          device_state = DEVICE_ADV_STATE;
        } else {
          printf ("Device already in discoverable or connected state. Please reset the device.\r\n");
        }
        break;
      case 's':
        if (device_state == DEVICE_IDLE_STATE) {
          /* Device in discoverable mode with slow adv interval 1000 ms */
          Set_DeviceConnectable(ADV_INTERVAL_SLOW_MS);
          device_state = DEVICE_ADV_STATE;
        } else {
          printf ("Device already in discoverable or connected state. Please reset the device.\r\n");
        }
        break;
      case '?':
        help();
        break;
      case 'r':
        NVIC_SystemReset();
        break;
      case '\r':
      case '\n':
        // ignored
        break;
      default:
        printf("Unknown Command\r\n");
      }
    }
  
    switch(device_state) {
    case DEVICE_DISCONNECTED_STATE:
      /* Enable the UART */
      SdkEvalComUartInit(UART_BAUDRATE);
      printf ("Device Disconnected!!!\r\n");
      device_state = DEVICE_IDLE_STATE;
      break;
    case DEVICE_CONN_STATE:
      /* Enable the UART */
      SdkEvalComUartInit(UART_BAUDRATE);
      printf("Device Connected with Interval %d * 1.25 ms\r\n", connection_interval);
      device_state = DEVICE_SLEEP_STATE;
      break;
    case DEVICE_IDLE_STATE:
      /* Nothing to Do */
      break;
    default:
      /* Disable the UART to save power */
      UART_Cmd(DISABLE);
      SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_UART, DISABLE);
      SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_GPIO, DISABLE);
      
      ret = BlueNRG_Sleep(SLEEPMODE_NOTIMER, 0, 0);
      if (ret != BLE_STATUS_SUCCESS) {
        printf("BlueNRG_Sleep() error 0x%02x\r\n", ret);
        while(1);
      }
    }
  }
}

/* ***************** BlueNRG-1 Stack Callbacks ********************************/

/*******************************************************************************
 * Function Name  : hci_le_connection_complete_event.
 * Description    : This event indicates that a new connection has been created.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void hci_le_connection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Role,
                                      uint8_t Peer_Address_Type,
                                      uint8_t Peer_Address[6],
                                      uint16_t Conn_Interval,
                                      uint16_t Conn_Latency,
                                      uint16_t Supervision_Timeout,
                                      uint8_t Master_Clock_Accuracy)
{ 
  if (Status == BLE_STATUS_SUCCESS) {
    device_state = DEVICE_CONN_STATE;
    connection_interval = Conn_Interval;
  }
}/* end hci_le_connection_complete_event() */

/*******************************************************************************
 * Function Name  : hci_disconnection_complete_event.
 * Description    : This event occurs when a connection is terminated.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason)
{
  device_state = DEVICE_DISCONNECTED_STATE;
}/* end hci_disconnection_complete_event() */

/*******************************************************************************
 * Function Name  : HAL_VTimerTimeoutCallback.
 * Description    : This function will be called on the expiry of 
 *                  a one-shot virtual timer.
 * Input          : See file bluenrg1_stack.h
 * Output         : See file bluenrg1_stack.h
 * Return         : See file bluenrg1_stack.h
 *******************************************************************************/
void HAL_VTimerTimeoutCallback(uint8_t timerNum)
{
  /* Add app code to execute @ Sleep timeout */
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


