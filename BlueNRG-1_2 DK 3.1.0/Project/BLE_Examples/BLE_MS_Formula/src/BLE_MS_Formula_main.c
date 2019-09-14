
/******************** (C) COPYRIGHT 2018 STMicroelectronics ********************
* File Name          : BLE_MS_Formula_main.c
* Author             : RF Application Team
* Version            : 1.0.0
* Date               : 30-October-2017
* Description        : Code demostrating a Master and Slave device using the multiple connection formula
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/**
 * @file BLE_MS_Formula_main.c
 * @brief  This application provides a basic example of multiple connections, simultaneously Master and Slave scenario using the related formula for calculating the related proper parameters.
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
     <tt> ...\\Project\\BLE_Examples\\BLE_MS_Formula\\MDK-ARM\\BlueNRG-1\\BLE_MS_Formula.uvprojx </tt> or
     <tt> ...\\Project\\BLE_Examples\\BLE_MS_Formula\\MDK-ARM\\BlueNRG-2\\BLE_MS_Formula.uvprojx </tt>
  -# Select desired configuration to build
  -# Select Project->Rebuild all target files. This will recompile and link the entire application
  -# To download the binary image, please connect STLink to JTAG connector in your board (if available).
  -# Select Project->Download to download the related binary image.
  -# Alternatively, open the BlueNRG1 Flasher utility and download the built binary image.

* \section IAR_project IAR project
  To use the project with IAR Embedded Workbench for ARM, please follow the instructions below:
  -# Open the Embedded Workbench for ARM and select File->Open->Workspace menu. 
  -# Open the IAR project
     <tt> ...\\Project\\BLE_Examples\\BLE_MS_Formula\\EWARM\\BlueNRG-1\\BLE_MS_Formula.eww </tt> or
     <tt> ...\\Project\\BLE_Examples\\BLE_MS_Formula\\EWARM\\BlueNRG-2\\BLE_MS_Formula.eww </tt>
  -# Select desired configuration to build
  -# Select Project->Rebuild All. This will recompile and link the entire application
  -# To download the binary image, please connect STLink to JTAG connector in your board (if available).
  -# Select Project->Download and Debug to download the related binary image.
  -# Alternatively, open the BlueNRG1 Flasher utility and download the built binary image.

* \subsection Project_configurations Project configurations
- \c Master - Master configuration
- \c Master_Slave - Master, Slave configuration


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
|            |                                              Master_Slave                                               |||||                                                 Master                                                  |||||
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|  PIN name  |   STEVAL-IDB007V1  |   STEVAL-IDB007V2  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |   STEVAL-IDB007V1  |   STEVAL-IDB007V2  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|    ADC1    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    ADC2    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO0    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO1    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO11    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO12    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO13    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO14    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO15    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO16    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO17    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO18    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO19    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|     IO2    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO20    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO21    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO22    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO23    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO24    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO25    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|     IO3    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO4    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO5    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO6    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO7    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO8    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    TEST1   |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |

@endtable 

* \section Serial_IO Serial I/O
  The application will listen for keys typed and it will send back in the serial port.
  In other words everything typed in serial port will be send back.
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
|            |                                              Master_Slave                                               |||||                                                 Master                                                  |||||
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|  LED name  |   STEVAL-IDB007V1  |   STEVAL-IDB007V2  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |   STEVAL-IDB007V1  |   STEVAL-IDB007V2  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|     DL1    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     DL2    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     DL3    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     DL4    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |

@endtable


* \section Buttons_description Buttons description
@table
|                |                                              Master_Slave                                               |||||                                                 Master                                                  |||||
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|   BUTTON name  |   STEVAL-IDB007V1  |   STEVAL-IDB007V2  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |   STEVAL-IDB007V1  |   STEVAL-IDB007V2  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|      PUSH1     |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|      PUSH2     |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|      RESET     |   Reset BlueNRG1   |   Reset BlueNRG1   |   Reset BlueNRG2   |   Reset BlueNRG2   |   Reset BlueNRG2   |   Reset BlueNRG1   |   Reset BlueNRG1   |   Reset BlueNRG2   |   Reset BlueNRG2   |   Reset BlueNRG2   |

@endtable

* \section Usage Usage

<b>Master_Slave device role</b>

Master_Slave device role allows to test a multiple connection, simultaneously Master and  Slave scenario using the GET_Master_Slave_device_connection_parameters() formula provided within the ble_utils.c file. 
It configures the Master_Slave device as Central and Peripheral, with a service and one characteristic and it performs simultaneously advertising and scanning in order to connect respectively, to up to Num_Slaves BLE Peripheral/Slave devices Slave1, Slave2, ..... (which have defined the same service and characteristic) and then to  up to Num_Masters Central/Master devices. The number Num_Slaves of Slave device depends on max number of supported multiple connections (8) and the number Num_Masters [0-2] of selected Master devices: Num_Slaves = 8 - Num_Masters. User must define the expected number of slaves and master devices,  by setting the preprocessor options: 

- MASTER_SLAVE_NUM_MASTERS
- MASTER_SLAVE_NUM_SLAVES

It can also set the requested minimal scan window and additional sleep time, respectively, through the preprocessor options: 

- MASTER_SLAVE_SCAN_WINDOW
- MASTER_SLAVE_SLEEP_TIME

- NOTE: Default configuration is: Num_Masters = 1; Num_Slaves = 6; Slave_Scan_Window_Length = 20; Slave_Sleep_time = 0. 

Once the connections with slaves and devices are performed, BLE Master_Slave device receives characteristics notifications from Num_Slaves devices and it also notifies characteristics (as Peripheral)  to the Num_Masters BLE Master devices (if any), which display the related received slave index value. 
Num_Slaves devices notified characteristic value is as follow: <slave_index> <counter_value>, where slave_index is one byte in the range [1 - Num_Slaves], and counter_value is a 2 bytes inter counter starting from 0. 

NOTE: if both Master_Slave and Slave(i) device supports data length extension, the ATT_MTU size is increased to the common max supported value (Att_Mtu), using ACI_GATT_EXCHANGE_CONFIG() API, and the slave(i) notifies a characteristic of Att_Mtu - 3 bytes. Same path is followed for the Master_Slave to Master(i) communication, if both devices supports the data length extension feature (the notification length is also tailored to the actual notification length received from each Slave and to the max number of octets for data length extension handled by connected Master devices).

How to run? Follow these steps:

1) Configure Master_Slave device 
 - Download the Project\\BLE_Examples\\BLE_MS_Formula\\EWARM\\[BlueNRG-1/BlueNRG-2]\\Master_Slave\\Exe\\BLE_Master_Slave.hex, respectively, on the selected BlueNRG-1 or BlueNRG-2 STEVAL kit.
 - Open a hyper terminal with the described settings
 - Reset the selected BlueNRG-1 or BlueNRG-2 STEVAL kit: a discovery procedure starts for finding the Num_Slaves slaves devices

2) Configure Num_Slaves GAP Peripheral devices Slave1, ...   using the Slaves.py script
 
 - <b>BlueNRG/BlueNRG-MS platform configuration steps</b>:
	 - Connect the selected platform to a PC USB port
	 - Put selected platform in DFU mode and download the "C:\\Program Files (x86)\STMicroelectronics\BlueNRG GUI x.x.x\Firmware\STM32L1_prebuilt_images\BlueNRG_VCOM_x_x.hex" using the BlueNRG GUI, Tools, Flash Motherboard FW... utility 
 - <b> BlueNRG-1,2 platforms configuration steps</b>: 
	 - Connect the selected Num_Slaves platforms to a Num_Slaves PC USB ports
	 - BlueNRG-1 platform: download the "C:\\\Program Files (x86)\STMicroelectronics\\BlueNRG GUI x.x.x\\Firmware\\DTM\\BlueNRG1\\DTM_UART.hex or DTM_SPI.hex" file, using the BlueNRG Flasher utility available on  BlueNRG-1_2 DK SW package (STSW-BLUENRG1-DK)
	 - BlueNRG-2 platform: download the "C:\\Program Files (x86)\\STMicroelectronics\\BlueNRG GUI x.x.x\\Firmware\\DTM\\BlueNRG1\\DTM_UART.hex or DTM_SPI.hex" file, using the BlueNRG Flasher utility available on  BlueNRG-1_2 DK SW package (STSW-BLUENRG1-DK) 
	 - <b>NOTES</b>: SPI Network coprocessor configuration file (DTM_SPI.hex) requires some HW changes to be applied to the BlueNRG-1 development platform. Refer to STSW-BNRGUI user manual UM2058. 
 - Open a DOS shell and go to the BlueNRG GUI installation folder : C:\\Program Files (x86)\\STMicroelectronics\\BlueNRG GUI x.x.x\\Application
 - On DOS shell launch the script launcher utility:  BlueNRG_Script_Launcher.exe -s < script_user_folder > Slaves_Num_Slaves.py
 - Script requests user to add the number of expected Master devices (masters of Master_Slave device) and the number of slaves devices Num_Slaves and to confirm that Num_Slaves devices are actually connected to PC USB ports
 - Slaves.py puts each of Num_Slaves device in discovey mode with advertising name = slavem, waiting for Master_Slave device to connect it and to enable characteristic notification
 - At this stage, Slaves.py script starts Num_Slave characteristic notifications.

3) Configure Num_Masters devices as follow:

<b>Master device role</b>

Master device role simply configures a BlueNRG-1, BlueNRG-2 device as a Master device looking for the Master_Slave device in advertising with advertising name = advscan.
Once Master device finds advscan device it connects to it,  and it enables the characteristic notification. Notifications from Num_Slaves devices will be notified to Master device through the Master_Slave device. 

 How to configure a Master device? 

 - Download the Project\\BLE_Examples\\BLE_MS_Formula\\EWARM\\[BlueNRG-1/BlueNRG-2]\\Master\\Exe\\BLE_Master.hex, respectively, on the selected BlueNRG-1 or BlueNRG-2 STEVAL kit.
 - Open a hyper terminal with the described settings
 - Reset the selected BlueNRG-1 or BlueNRG-2 STEVAL kit: a discovery procedure starts for finding the Master_Slave device and connect to it.

NOTE: Alternatevely user can configure a Master device running the Master_Slave.py script 

 - Open BlueNRG GUI 
 - <b>BlueNRG-1,2 platforms configuration steps</b>: 
	   - Connect the selected platform to a PC USB port
	   -  BlueNRG-1 platform: download the "C:\\Program Files (x86)\\STMicroelectronics\\BlueNRG GUI x.x.x\\Firmware\\DTM\\BlueNRG1\\DTM_UART.hex or DTM_SPI.hex" file, using the BlueNRG Flasher utility available on  BlueNRG-1_2 DK SW package (STSW-BLUENRG1-DK)
	   - BlueNRG-2 platform: download the "C:\\Program Files (x86)\\STMicroelectronics\\BlueNRG GUI x.x.x\\Firmware\DTM\BlueNRG2\\DTM_UART.hex or DTM_SPI.hex" file, using the BlueNRG Flasher utility available on  BlueNRG-1_2 DK SW package (STSW-BLUENRG1-DK) 
	   - <b>NOTES</b>: SPI Network coprocessor configuration file (DTM_SPI.hex) could require some HW changes to be applied to the BlueNRG-1 development platform. Refer to STSW-BNRGUI user manual UM2058. 
 - On BlueNRG GUI, open the USB Port associated to the plugged BlueNRG-1,2 platform
 - On BlueNRG GUI, select the Scripts window 
 - On BlueNRG GUI, Scripts window, Scripts Engine section, click on ... tab and browse to the Master.py script location and select it
 - On BlueNRG GUI, click on  Run Script tab for running the selected script
 - Master device is configured as Central and it performs a connection procedure for connecting to the Master_Slave device (advertising name = advscan)
 - Then Master device  enables the characteristic notification on connected device.
 - At this stage, Master receives and displays the received characteristic values (slave_index component).
 - NOTE: Master device can also be a Smarthphone device running a simple BLE app (i.e. B-BLE on Android).
 
How to use an Smartphone as second master ? (if Num_Masters = 2), (i.e. Android and B-BLE app as reference example)

 - Select B-BLE app
 - Look for devices and selecte advscan
 - Once connected, selected characteristic with notify property and enable characteristic notification
 - Smarthphone app recevies notifications coming from Num_Slaves devices through the Master_Slave device	 

**/
   
/** @addtogroup BlueNRG1_demonstrations_applications
 *  BlueNRG-1,2 Multiple Connections and  Formula \see BLE_MS_Formula_main.c for documentation.
 *
 *@{
 */

/** @} */
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
#if SLAVE
#include "slave.h"
#else
#include "master_slave.h"
#endif
#include "clock.h"
#include "osal.h"
#include "ble_utils.h"

#define BLE_STACK_V_1_X 0

#if BLE_STACK_V_1_X == 1
#include "user_config_1_x.h"
#else
#include "user_config.h"
#endif

#ifndef DEBUG
#define DEBUG 1 
#endif

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define APP_SLEEP_MODE SLEEPMODE_NOTIMER

int main(void)
{
  uint8_t ret = 0; 

  /* System Init */
  SystemInit();

  /* Identify BlueNRG1 platform */
  SdkEvalIdentification();
  
  /* Configure I/O communication channel */
  SdkEvalComUartInit(UART_BAUDRATE);


  /* BlueNRG-1 stack init */
  ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);

  if (ret != BLE_STATUS_SUCCESS) {
    printf("Error in BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
    while(1);
    }
  
  /* Application demo Led Init */
  SdkEvalLedInit(LED1); //Activity led 
  SdkEvalLedOn(LED1);

  //device_initialization
  device_initialization();
  
  printf("BLE device initialized\r\n");
  
  while(1) {
    /* BlueNRG-1 stack tick */
    BTLE_StackTick();


    /* Application Tick */
    APP_Tick();


    /* Power Save management */
#if BLE_STACK_V_1_X ==1
    BlueNRG_Sleep(APP_SLEEP_MODE, 0, 0,0);
#else
   BlueNRG_Sleep(APP_SLEEP_MODE, 0, 0); 
#endif


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


/******************************************************************************
 * Function Name  : assert_failed.
 * Description    : Reports the name of the source file and the source line number
                    where the assert_param error has occurred.
 * Input          : - file: pointer to the source file name,
                    - line: assert_param error line source number.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/******************* (C) COPYRIGHT 2018 STMicroelectronics *****END OF FILE****/
/** \endcond 
*/
