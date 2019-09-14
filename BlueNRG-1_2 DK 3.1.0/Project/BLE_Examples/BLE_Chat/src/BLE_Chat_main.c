
/******************** (C) COPYRIGHT 2018 STMicroelectronics ********************
* File Name          : BLE_Chat_main.c
* Author             : RF Application Team
* Version            : 1.1.0
* Date               : 15-January-2016
* Description        : Code demostrating the BLE Chat application
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/**
 * @file BLE_Chat_main.c
 * @brief This is a Chat demo that shows how to implement a simple 2-way communication between two BlueNRG-1,2 devices.
 * It also provides a reference example about how using the 
 * BLE Over-The-Air (OTA) firmware upgrade capability with the BLE Chat Demo.
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
     <tt> ...\\Project\\BLE_Examples\\BLE_Chat\\MDK-ARM\\BlueNRG-1\\BLE_Chat.uvprojx </tt> or
     <tt> ...\\Project\\BLE_Examples\\BLE_Chat\\MDK-ARM\\BlueNRG-2\\BLE_Chat.uvprojx </tt>
  -# Select desired configuration to build
  -# Select Project->Rebuild all target files. This will recompile and link the entire application
  -# To download the binary image, please connect STLink to JTAG connector in your board (if available).
  -# Select Project->Download to download the related binary image.
  -# Alternatively, open the BlueNRG1 Flasher utility and download the built binary image.

* \section IAR_project IAR project
  To use the project with IAR Embedded Workbench for ARM, please follow the instructions below:
  -# Open the Embedded Workbench for ARM and select File->Open->Workspace menu. 
  -# Open the IAR project
     <tt> ...\\Project\\BLE_Examples\\BLE_Chat\\EWARM\\BlueNRG-1\\BLE_Chat.eww </tt> or
     <tt> ...\\Project\\BLE_Examples\\BLE_Chat\\EWARM\\BlueNRG-2\\BLE_Chat.eww </tt>
  -# Select desired configuration to build
  -# Select Project->Rebuild All. This will recompile and link the entire application
  -# To download the binary image, please connect STLink to JTAG connector in your board (if available).
  -# Select Project->Download and Debug to download the related binary image.
  -# Alternatively, open the BlueNRG1 Flasher utility and download the built binary image.

* \subsection Project_configurations Project configurations
- \c Client - Client role configuration
- \c Server - Server role configuration
- \c Server_HigherApp_OTA - Server role configuration for Higher Application with OTA Service
- \c Server_LowerApp_OTA - Server role configuration for Lower Application with OTA Service
- \c Server_Use_OTA_ServiceManager - Server role configuration for Application using OTA Service Manager


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
|            |                                                    Server_HigherApp_OTA                                                     |||||                                                                         Server_Use_OTA_ServiceManager                                                                         |||||                                                 Client                                                  |||||                                                     Server_LowerApp_OTA                                                     |||||                                                 Server                                                  |||||
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|  PIN name  |     STEVAL-IDB007V1    |     STEVAL-IDB007V2    |     STEVAL-IDB008V1    |     STEVAL-IDB008V2    |     STEVAL-IDB009V1    |          STEVAL-IDB007V1         |          STEVAL-IDB007V2         |          STEVAL-IDB008V1         |          STEVAL-IDB008V2         |          STEVAL-IDB009V1         |   STEVAL-IDB007V1  |   STEVAL-IDB007V2  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |     STEVAL-IDB007V1    |     STEVAL-IDB007V2    |     STEVAL-IDB008V1    |     STEVAL-IDB008V2    |     STEVAL-IDB009V1    |   STEVAL-IDB007V1  |   STEVAL-IDB007V2  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|    ADC1    |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |             Not Used             |             Not Used             |             Not Used             |             Not Used             |             Not Used             |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    ADC2    |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |             Not Used             |             Not Used             |             Not Used             |             Not Used             |             Not Used             |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO0    |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |             Not Used             |             Not Used             |             Not Used             |             Not Used             |             Not Used             |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO1    |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |             Not Used             |             Not Used             |             Not Used             |             Not Used             |             Not Used             |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO11    |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |             Not Used             |             Not Used             |             Not Used             |             Not Used             |             Not Used             |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO12    |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |             Not Used             |             Not Used             |             Not Used             |             Not Used             |             Not Used             |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO13    |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |             Not Used             |             Not Used             |             Not Used             |             Not Used             |             Not Used             |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO14    |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |             Not Used             |             Not Used             |             Not Used             |             Not Used             |             Not Used             |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO15    |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |               N.A.               |               N.A.               |               N.A.               |               N.A.               |             Not Used             |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO16    |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |               N.A.               |               N.A.               |               N.A.               |               N.A.               |             Not Used             |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO17    |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |               N.A.               |               N.A.               |               N.A.               |               N.A.               |             Not Used             |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO18    |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |               N.A.               |               N.A.               |               N.A.               |               N.A.               |             Not Used             |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO19    |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |               N.A.               |               N.A.               |               N.A.               |               N.A.               |             Not Used             |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|     IO2    |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |             Not Used             |             Not Used             |             Not Used             |             Not Used             |             Not Used             |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    IO20    |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |               N.A.               |               N.A.               |               N.A.               |               N.A.               |             Not Used             |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO21    |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |               N.A.               |               N.A.               |               N.A.               |               N.A.               |             Not Used             |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO22    |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |               N.A.               |               N.A.               |               N.A.               |               N.A.               |             Not Used             |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO23    |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |               N.A.               |               N.A.               |               N.A.               |               N.A.               |             Not Used             |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO24    |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |               N.A.               |               N.A.               |               N.A.               |               N.A.               |             Not Used             |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|    IO25    |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |               N.A.               |               N.A.               |               N.A.               |               N.A.               |             Not Used             |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |
|     IO3    |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |             Not Used             |             Not Used             |             Not Used             |             Not Used             |             Not Used             |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO4    |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |             Not Used             |             Not Used             |             Not Used             |             Not Used             |             Not Used             |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO5    |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |             Not Used             |             Not Used             |             Not Used             |             Not Used             |             Not Used             |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO6    |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |             Not Used             |             Not Used             |             Not Used             |             Not Used             |             Not Used             |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO7    |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |             Not Used             |             Not Used             |             Not Used             |             Not Used             |             Not Used             |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     IO8    |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |             Not Used             |             Not Used             |             Not Used             |             Not Used             |             Not Used             |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|    TEST1   |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |             Not Used             |             Not Used             |             Not Used             |             Not Used             |             Not Used             |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |

@endtable 

* \section Serial_IO Serial I/O
  The application will listen for keys typed in one node and, on return press, it will send them to the remote node.
  The remote node will listen for RF messages and it will output them in the serial port.
  In other words everything typed in one node will be visible to the other node and viceversa.
@table
| Parameter name  | Value               | Unit      |
------------------------------------------------------
| Baudrate        | 115200 [default]    | bit/sec   |
| Data bits       | 8                   | bit       |
| Parity          | None                | bit       |
| Stop bits       | 1                   | bit       |
@endtable

* \section LEDs_description LEDs description
@table
|            |                                                                                                      Server_HigherApp_OTA                                                                                                       |||||                                                                                                  Server_Use_OTA_ServiceManager                                                                                                  |||||                                                 Client                                                  |||||                                                                                                       Server_LowerApp_OTA                                                                                                       |||||                                                 Server                                                  |||||
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|  LED name  |               STEVAL-IDB007V1              |               STEVAL-IDB007V2              |               STEVAL-IDB008V1              |               STEVAL-IDB008V2              |               STEVAL-IDB009V1              |               STEVAL-IDB007V1              |               STEVAL-IDB007V2              |               STEVAL-IDB008V1              |               STEVAL-IDB008V2              |               STEVAL-IDB009V1              |   STEVAL-IDB007V1  |   STEVAL-IDB007V2  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |               STEVAL-IDB007V1              |               STEVAL-IDB007V2              |               STEVAL-IDB008V1              |               STEVAL-IDB008V2              |               STEVAL-IDB009V1              |   STEVAL-IDB007V1  |   STEVAL-IDB007V2  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|     DL1    |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     DL2    |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     DL3    |   ON when OTA firmware upgrade is ongoing  |   ON when OTA firmware upgrade is ongoing  |   ON when OTA firmware upgrade is ongoing  |   ON when OTA firmware upgrade is ongoing  |   ON when OTA firmware upgrade is ongoing  |   ON when OTA firmware upgrade is ongoing  |   ON when OTA firmware upgrade is ongoing  |   ON when OTA firmware upgrade is ongoing  |   ON when OTA firmware upgrade is ongoing  |   ON when OTA firmware upgrade is ongoing  |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |   ON when OTA firmware upgrade is ongoing  |   ON when OTA firmware upgrade is ongoing  |   ON when OTA firmware upgrade is ongoing  |   ON when OTA firmware upgrade is ongoing  |   ON when OTA firmware upgrade is ongoing  |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|     DL4    |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |                  Not Used                  |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |

@endtable


* \section Buttons_description Buttons description
@table
|                |                                                    Server_HigherApp_OTA                                                     |||||                                                                         Server_Use_OTA_ServiceManager                                                                         |||||                                                 Client                                                  |||||                                                     Server_LowerApp_OTA                                                     |||||                                                 Server                                                  |||||
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|   BUTTON name  |     STEVAL-IDB007V1    |     STEVAL-IDB007V2    |     STEVAL-IDB008V1    |     STEVAL-IDB008V2    |     STEVAL-IDB009V1    |          STEVAL-IDB007V1         |          STEVAL-IDB007V2         |          STEVAL-IDB008V1         |          STEVAL-IDB008V2         |          STEVAL-IDB009V1         |   STEVAL-IDB007V1  |   STEVAL-IDB007V2  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |     STEVAL-IDB007V1    |     STEVAL-IDB007V2    |     STEVAL-IDB008V1    |     STEVAL-IDB008V2    |     STEVAL-IDB009V1    |   STEVAL-IDB007V1  |   STEVAL-IDB007V2  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|      PUSH1     |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |    Jump to OTA Service manager   |    Jump to OTA Service manager   |    Jump to OTA Service manager   |    Jump to OTA Service manager   |    Jump to OTA Service manager   |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|      PUSH2     |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |             Not Used             |             Not Used             |             Not Used             |             Not Used             |             Not Used             |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |
|      RESET     |     Reset BlueNRG1     |     Reset BlueNRG1     |     Reset BlueNRG2     |     Reset BlueNRG2     |     Reset BlueNRG2     |          Reset BlueNRG1          |          Reset BlueNRG1          |          Reset BlueNRG2          |          Reset BlueNRG2          |          Reset BlueNRG2          |   Reset BlueNRG1   |   Reset BlueNRG1   |   Reset BlueNRG2   |   Reset BlueNRG2   |   Reset BlueNRG2   |     Reset BlueNRG1     |     Reset BlueNRG1     |     Reset BlueNRG2     |     Reset BlueNRG2     |     Reset BlueNRG2     |   Reset BlueNRG1   |   Reset BlueNRG1   |   Reset BlueNRG2   |   Reset BlueNRG2   |   Reset BlueNRG1   |

@endtable

* \section Usage Usage

This Chat demo has 2 roles:
 - The server that expose the Chat service. It is the slave.
 - The client that uses the Chat service. It is the master.

The Chat Service contains 2 Characteristics:
 -# The TX Characteristic: the client can enable notifications on this characteristic. When the server has data to be sent, it will send notifications which will contains the value of the TX Characteristic
 -# The RX Characteristic: it is a writable caracteristic. When the client has data to be sent to the server, it will write a value into this characteristic.

The maximum length of the characteristic value is 20 bytes.

NOTES:
 - OTA service support for lower or higher application is enabled, respectively,  through ST_OTA_LOWER_APPLICATION=1 or ST_OTA_HIGHER_APPLICATION=1(preprocessor, linker) options and files: OTA_btl.[ch] (refer to Server_LowerApp_OTA and  Server_HigherApp_OTA IAR workspaces).
 - OTA service manager support is enabled, respectively,  through ST_USE_OTA_SERVICE_MANAGER_APPLICATION (preprocessor, linker) options and files: OTA_btl.[ch] (refer to Use_OTA_ServiceManager IAR workspace).   
 - OTA FW upgrade feature is supported only on BlueNRG-2, BLE stack v2.x.     

**/
    
/** @addtogroup BlueNRG1_demonstrations_applications
 * BlueNRG-1 Chat demo \see BLE_Chat_main.c for documentation.
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
#include "gp_timer.h"
#include "app_state.h"
#include "chat.h"
#include "SDK_EVAL_Config.h"
#include "Chat_config.h"
#include "OTA_btl.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BLE_CHAT_VERSION_STRING "1.0.0" 


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

int main(void) 
{
  uint8_t ret;

  /* System Init */
  SystemInit();
  
  /* Identify BlueNRG1 platform */
  SdkEvalIdentification();
  
  /* Init Clock */
  Clock_Init();

  /* Configure I/O communication channel:
       It requires the void IO_Receive_Data(uint8_t * rx_data, uint16_t data_size) function
       where user received data should be processed */
  SdkEvalComIOConfig(Process_InputData);

  /* BlueNRG-1 stack init */
  ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);
  if (ret != BLE_STATUS_SUCCESS) {
    printf("Error in BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
    while(1);
  }
  
#if SERVER
  printf("BlueNRG-1 BLE Chat Server Application (version: %s)\r\n", BLE_CHAT_VERSION_STRING);
#else
  printf("BlueNRG-1 BLE Chat Client Application (version: %s)\r\n", BLE_CHAT_VERSION_STRING); 
#endif

  /* Init Chat Device */
  ret = CHAT_DeviceInit();
  if (ret != BLE_STATUS_SUCCESS) {
    printf("CHAT_DeviceInit()--> Failed 0x%02x\r\n", ret);
    while(1);
  }
  
  printf("BLE Stack Initialized \n");
  
#if ST_USE_OTA_SERVICE_MANAGER_APPLICATION
  /* Initialize the button */
  SdkEvalPushButtonInit(USER_BUTTON); 
#endif /* ST_USE_OTA_SERVICE_MANAGER_APPLICATION */
  
  while(1) {
    
    /* BlueNRG-1 stack tick */
    BTLE_StackTick();
    
    /* Application tick */
    APP_Tick();
    
#if ST_OTA_FIRMWARE_UPGRADE_SUPPORT
    /* Check if the OTA firmware upgrade session has been completed */
    if (OTA_Tick() == 1)
    {
      /* Jump to the new application */
      OTA_Jump_To_New_Application();
    }
#endif  /* ST_OTA_FIRMWARE_UPGRADE_SUPPORT */

#if ST_USE_OTA_SERVICE_MANAGER_APPLICATION
    if (SdkEvalPushButtonGetState(USER_BUTTON) == RESET)
    {
      OTA_Jump_To_Service_Manager_Application();
    }
#endif /* ST_USE_OTA_SERVICE_MANAGER_APPLICATION */
  }
  
} /* end main() */

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
