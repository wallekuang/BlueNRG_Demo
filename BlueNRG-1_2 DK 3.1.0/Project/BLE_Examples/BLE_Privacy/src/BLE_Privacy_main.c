
/******************** (C) COPYRIGHT 2018 STMicroelectronics ********************
* File Name          : BLE_Privacy_main.c
* Author             : RF Application Team
* Version            : 1.0.0
* Date               : 10-October-2018
* Description        : Code demostrating BlueNRG-2 BLE Controller Privacy feature.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/**
 * @file BLE_Privacy_main.c
 * @brief This application contains an example which shows how implementing a BLE Controller Privacy on a Central/Master and Peripheral/Slave device.
 * Controller privacy requires 32MHz High Speed Crystal on selected platform (BlueNRG-2 STEVAL Kits).
 * 
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
      <tt> ...\\Project\\BLE_Examples\\BLE_Privacy\\MDK-ARM\\BlueNRG-1\\BLE_Privacy.uvprojx </tt> or
      <tt> ...\\Project\\BLE_Examples\\BLE_Privacy\\MDK-ARM\\BlueNRG-2\\BLE_Privacy.uvprojx </tt>
  -# Select desired configuration to build
  -# Select Project->Rebuild all target files. This will recompile and link the entire application
  -# To download the binary image, please connect STLink to JTAG connector in your board (if available).
  -# Select Project->Download to download the related binary image.
  -# Alternatively, open the BlueNRG1 Flasher utility and download the built binary image.

* \section IAR_project IAR project
  To use the project with IAR Embedded Workbench for ARM, please follow the instructions below:
  -# Open the Embedded Workbench for ARM and select File->Open->Workspace menu. 
  -# Open the IAR project
     <tt> ...\\Project\\BLE_Examples\\BLE_Privacy\\EWARM\\BlueNRG-1\\BLE_Privacy.eww </tt> or
     <tt> ...\\Project\\BLE_Examples\\BLE_Privacy\\EWARM\\BlueNRG-2\\BLE_Privacy.eww </tt>
  -# Select desired configuration to build
  -# Select Project->Rebuild All. This will recompile and link the entire application
  -# To download the binary image, please connect STLink to JTAG connector in your board (if available).
  -# Select Project->Download and Debug to download the related binary image.
  -# Alternatively, open the BlueNRG1 Flasher utility and download the built binary image.

* \subsection Project_configurations Project configurations
- \c Master - Controller Privacy: master configuration
- \c Slave - Controller Privacy: slave configuration


* \section Board_supported Boards supported
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
|            |                            Master                             |||                             Slave                             |||
--------------------------------------------------------------------------------------------------------------------------------------------------
|  PIN name  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |
--------------------------------------------------------------------------------------------------------------------------------------------------
|    ADC1    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    ADC2    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO0    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO1    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO11    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO12    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO13    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO14    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO15    |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |      Not Used      |
|    IO16    |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |      Not Used      |
|    IO17    |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |      Not Used      |
|    IO18    |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |      Not Used      |
|    IO19    |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |      Not Used      |
|     IO2    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO20    |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |      Not Used      |
|    IO21    |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |      Not Used      |
|    IO22    |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |      Not Used      |
|    IO23    |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |      Not Used      |
|    IO24    |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |      Not Used      |
|    IO25    |        N.A.        |        N.A.        |      Not Used      |        N.A.        |        N.A.        |      Not Used      |
|     IO3    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO4    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO5    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO6    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO7    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO8    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    TEST1   |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |

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
|            |                            Master                             |||                             Slave                             |||
--------------------------------------------------------------------------------------------------------------------------------------------------
|  LED name  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |
--------------------------------------------------------------------------------------------------------------------------------------------------
|     DL1    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     DL2    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     DL3    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     DL4    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |

@endtable


* \section Buttons_description Buttons description
@table
|                |                               Master                                |||                                Slave                                |||
------------------------------------------------------------------------------------------------------------------------------------------------------------------
|   BUTTON name  |    STEVAL-IDB008V1   |    STEVAL-IDB008V2   |    STEVAL-IDB009V1   |    STEVAL-IDB008V1   |    STEVAL-IDB008V2   |    STEVAL-IDB009V1   |
------------------------------------------------------------------------------------------------------------------------------------------------------------------
|      PUSH1     |   BLE disconnection  |   BLE disconnection  |   BLE disconnection  |   BLE disconnection  |   BLE disconnection  |   BLE disconnection  |
|      PUSH2     |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |
|      RESET     |    Reset BlueNRG2    |    Reset BlueNRG2    |    Reset BlueNRG2    |    Reset BlueNRG2    |    Reset BlueNRG2    |    Reset BlueNRG2    |

@endtable

* \section Usage Usage

This demo implements a Controller Privacy scenario using Privacy 1.2 available with BlueNRG-1,2 BLE stack v2.x. 
Two devices Master, Slave, both configured with aci_gap_init(privacy flag = 0x02), which should perform these macro steps: 
 - Initially Master, Slave devices have both no info on their security database: the two devices should connect and make a paring + bonding (fixed key: 123456). 
 - Once bonding is completed, the Slave calls the aci_gap_configure_white_list() API for adding its bonded device's address into the controller's whitelist. 
 - Both devices also add their bonded device address and type to the list of resolvable addresses by using the aci_gap_add_devices_to_resolving_list().
 - Master device also enables the slave characteristic notification. After this first connection and pairing/bonding phase, devices disconnect. 
 - Slave enters in undirected connectable mode (aci_gap_set_undirected_connectable() API)  with its own address type = resolvable address and white list = 0x03 as advertising filter policy. 
 - Master device performs a direct connection to the detected slave device, which accepts the connection since master address is on its white list: the two devices reconnect and the slave starts a notification cycle to the master. 

- NOTES: 
  - When  connected, if user presses the BLE platform  button  PUSH1 on one of the two devices, device disconnect and slave enters in undirected connectable mode with filtering enabled (WHITE_LIST_FOR_ALL: Process scan and connection requests only from devices in the White List). This implies that slave device accepts connection requests only from devices on its  white list: master device is still be able to connect to the slave device; any other device connection requests are not accepted from the slave device. 
 

**/
   
/** @addtogroup BlueNRG1_demonstrations_applications
 * BlueNRG-2 Controller Privacy demo  \see BLE_Privacy_main.c for documentation.
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
   
#if SLAVE
#include "slave.h"
#elif MASTER
#include "master.h"  
#endif 
   
#include "sleep.h"
#include "osal.h"

#define BLE_STACK_V_1_X 0

#include "user_config.h"

#ifndef DEBUG
#define DEBUG 1
#endif

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


static uint8_t wakeup_source = 0;
static uint8_t wakeup_level = 0;
   
static void user_set_wakeup_source(SdkEvalButton Button)
{
  if (Button == BUTTON_1)
  {
     /* IRQ_ON_FALLING_EDGE is set since wakeup level is low (mandatory setup for wakeup sources);  
        IRQ_ON_RISING_EDGE is the specific application level */ 
     SdkEvalPushButtonIrq(USER_BUTTON, IRQ_ON_BOTH_EDGE); 
     wakeup_source = WAKEUP_IO13;
     wakeup_level = (WAKEUP_IOx_LOW << WAKEUP_IO13_SHIFT_MASK);
  }
}

int main(void)
{
  int ret;

  /* System Init */
  SystemInit();

  /* Identify BlueNRG/BlueNRG-MS/BleNRG-1 or BlueNRG-2 platform */
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

  device_initialization();
  
#if SLAVE
  //set_database
  set_database();
  
  // Set timer for notification frequency
  set_slave_update_timer();

  //set_device_discoverable
  set_device_discoverable();
#elif MASTER
  // start slave discovery procedure
  device_scanning(); 
#else
#error "MASTER or SLAVE macro must be selected"
#endif

  /* Initialize the button */
  SdkEvalPushButtonInit(USER_BUTTON); 
  
  user_set_wakeup_source(USER_BUTTON);

  while(1) {
    /* BlueNRG-1 stack tick */
    BTLE_StackTick();


    /* Application Tick */
    APP_Tick();

    /* Power Save management */
    BlueNRG_Sleep(SLEEPMODE_NOTIMER, wakeup_source, wakeup_level);

  }
}

/****************** BlueNRG-1 Sleep Management Callback ********************************/

SleepModes App_SleepMode_Check(SleepModes sleepMode)
{
  if(APP_FLAG(DO_TERMINATE) || SdkEvalComIOTxFifoNotEmpty() || SdkEvalComUARTBusy())
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
