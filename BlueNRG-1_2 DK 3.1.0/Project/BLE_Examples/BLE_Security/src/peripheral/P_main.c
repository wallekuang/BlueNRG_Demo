
/******************** (C) COPYRIGHT 2018 STMicroelectronics ********************
* File Name          : P_main.c
* Author             : RF Application Team
* Version            : 1.1.0
* Date               : 22-June-2017
* Description        : Code demostrating BlueNRG-1 BLE security example on Peripheral device.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/**
 * @file P_main.c
 * @brief This application contains an example which shows how implementing a BLE security scenario on a peripheral device.
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
      <tt> ...\\Project\\BLE_Examples\\BLE_Security\\MDK-ARM\\BlueNRG-1\\BLE_Security.uvprojx </tt> or
      <tt> ...\\Project\\BLE_Examples\\BLE_Security\\MDK-ARM\\BlueNRG-2\\BLE_Security.uvprojx </tt>
  -# Select desired configuration to build
  -# Select Project->Rebuild all target files. This will recompile and link the entire application
  -# To download the binary image, please connect STLink to JTAG connector in your board (if available).
  -# Select Project->Download to download the related binary image.
  -# Alternatively, open the BlueNRG1 Flasher utility and download the built binary image.

* \section IAR_project IAR project
  To use the project with IAR Embedded Workbench for ARM, please follow the instructions below:
  -# Open the Embedded Workbench for ARM and select File->Open->Workspace menu. 
  -# Open the IAR project
     <tt> ...\\Project\\BLE_Examples\\BLE_Security\\EWARM\\BlueNRG-1\\BLE_Security.eww </tt> or
     <tt> ...\\Project\\BLE_Examples\\BLE_Security\\EWARM\\BlueNRG-2\\BLE_Security.eww </tt>
  -# Select desired configuration to build
  -# Select Project->Rebuild All. This will recompile and link the entire application
  -# To download the binary image, please connect STLink to JTAG connector in your board (if available).
  -# Select Project->Download and Debug to download the related binary image.
  -# Alternatively, open the BlueNRG1 Flasher utility and download the built binary image.

* \subsection Project_configurations Project configurations
- \c Slave_JustWorks - Peripheral Security Just Works configuration
- \c Slave_NumericComp - Peripheral Security Numeric Comparison configuration
- \c Slave_PassKey_Fixed - Peripheral Security Pass Key entry (fixed pin) configuration
- \c Slave_PassKey_Random - Peripheral Security Pass Key entry (no fixed pin) configuration


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
|            |                                             Slave_JustWorks                                             |||||                                                     Slave_PassKey_Fixed                                                     |||||                                                 Slave_NumericComp                                                 |||||                                                    Slave_PassKey_Random                                                     |||||
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|  PIN name  |   STEVAL-IDB007V1  |   STEVAL-IDB007V2  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |     STEVAL-IDB007V1    |     STEVAL-IDB007V2    |     STEVAL-IDB008V1    |     STEVAL-IDB008V2    |     STEVAL-IDB009V1    |    STEVAL-IDB007V1   |    STEVAL-IDB007V2   |    STEVAL-IDB008V1   |    STEVAL-IDB008V2   |    STEVAL-IDB009V1   |     STEVAL-IDB007V1    |     STEVAL-IDB007V2    |     STEVAL-IDB008V1    |     STEVAL-IDB008V2    |     STEVAL-IDB009V1    |
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|    ADC1    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |
|    ADC2    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |
|     IO0    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |
|     IO1    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |
|    IO11    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |
|    IO12    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |
|    IO13    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |
|    IO14    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |
|    IO15    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |         N.A.         |         N.A.         |         N.A.         |         N.A.         |       Not Used       |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |
|    IO16    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |         N.A.         |         N.A.         |         N.A.         |         N.A.         |       Not Used       |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |
|    IO17    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |         N.A.         |         N.A.         |         N.A.         |         N.A.         |       Not Used       |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |
|    IO18    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |         N.A.         |         N.A.         |         N.A.         |         N.A.         |       Not Used       |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |
|    IO19    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |         N.A.         |         N.A.         |         N.A.         |         N.A.         |       Not Used       |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |
|     IO2    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |
|    IO20    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |         N.A.         |         N.A.         |         N.A.         |         N.A.         |       Not Used       |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |
|    IO21    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |         N.A.         |         N.A.         |         N.A.         |         N.A.         |       Not Used       |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |
|    IO22    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |         N.A.         |         N.A.         |         N.A.         |         N.A.         |       Not Used       |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |
|    IO23    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |         N.A.         |         N.A.         |         N.A.         |         N.A.         |       Not Used       |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |
|    IO24    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |         N.A.         |         N.A.         |         N.A.         |         N.A.         |       Not Used       |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |
|    IO25    |        N.A.        |        N.A.        |        N.A.        |        N.A.        |      Not Used      |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |         N.A.         |         N.A.         |         N.A.         |         N.A.         |       Not Used       |          N.A.          |          N.A.          |          N.A.          |          N.A.          |        Not Used        |
|     IO3    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |
|     IO4    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |
|     IO5    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |
|     IO6    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |
|     IO7    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |
|     IO8    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |
|    TEST1   |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |

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
|            |                                             Slave_JustWorks                                             |||||                                                     Slave_PassKey_Fixed                                                     |||||                                                 Slave_NumericComp                                                 |||||                                                    Slave_PassKey_Random                                                     |||||
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|  LED name  |   STEVAL-IDB007V1  |   STEVAL-IDB007V2  |   STEVAL-IDB008V1  |   STEVAL-IDB008V2  |   STEVAL-IDB009V1  |     STEVAL-IDB007V1    |     STEVAL-IDB007V2    |     STEVAL-IDB008V1    |     STEVAL-IDB008V2    |     STEVAL-IDB009V1    |    STEVAL-IDB007V1   |    STEVAL-IDB007V2   |    STEVAL-IDB008V1   |    STEVAL-IDB008V2   |    STEVAL-IDB009V1   |     STEVAL-IDB007V1    |     STEVAL-IDB007V2    |     STEVAL-IDB008V1    |     STEVAL-IDB008V2    |     STEVAL-IDB009V1    |
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|     DL1    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |
|     DL2    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |
|     DL3    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |
|     DL4    |      Not Used      |      Not Used      |      Not Used      |      Not Used      |      Not Used      |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |

@endtable


* \section Buttons_description Buttons description
@table
|                |                                                  Slave_JustWorks                                                  |||||                                                     Slave_PassKey_Fixed                                                     |||||                                                 Slave_NumericComp                                                 |||||                                                    Slave_PassKey_Random                                                     |||||
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|   BUTTON name  |    STEVAL-IDB007V1   |    STEVAL-IDB007V2   |    STEVAL-IDB008V1   |    STEVAL-IDB008V2   |    STEVAL-IDB009V1   |     STEVAL-IDB007V1    |     STEVAL-IDB007V2    |     STEVAL-IDB008V1    |     STEVAL-IDB008V2    |     STEVAL-IDB009V1    |    STEVAL-IDB007V1   |    STEVAL-IDB007V2   |    STEVAL-IDB008V1   |    STEVAL-IDB008V2   |    STEVAL-IDB009V1   |     STEVAL-IDB007V1    |     STEVAL-IDB007V2    |     STEVAL-IDB008V1    |     STEVAL-IDB008V2    |     STEVAL-IDB009V1    |
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|      PUSH1     |   BLE disconnection  |   BLE disconnection  |   BLE disconnection  |   BLE disconnection  |   BLE disconnection  |    BLE disconnection   |    BLE disconnection   |    BLE disconnection   |    BLE disconnection   |    BLE disconnection   |   BLE disconnection  |   BLE disconnection  |   BLE disconnection  |   BLE disconnection  |   BLE disconnection  |    BLE disconnection   |    BLE disconnection   |    BLE disconnection   |    BLE disconnection   |    BLE disconnection   |
|      PUSH2     |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |       Not Used       |       Not Used       |       Not Used       |       Not Used       |       Not Used       |        Not Used        |        Not Used        |        Not Used        |        Not Used        |        Not Used        |
|      RESET     |    Reset BlueNRG1    |    Reset BlueNRG1    |    Reset BlueNRG2    |    Reset BlueNRG2    |    Reset BlueNRG2    |     Reset BlueNRG1     |     Reset BlueNRG1     |     Reset BlueNRG2     |     Reset BlueNRG2     |     Reset BlueNRG2     |    Reset BlueNRG1    |    Reset BlueNRG1    |    Reset BlueNRG2    |    Reset BlueNRG2    |    Reset BlueNRG2    |     Reset BlueNRG1     |     Reset BlueNRG1     |     Reset BlueNRG2     |     Reset BlueNRG2     |     Reset BlueNRG2     |

@endtable

* \section Usage Usage

This demo addresses some  examples of Bluetooth Low Energy Security scenarios:

 - Pairing with pass key entry method (no fixed pin) and bonding (select: Peripheral_PassKey_Random and Master_PassKey_Random projects configurations)
 - Pairing with pass key entry method (with fixed pin) and bonding  (select: Peripheral_PassKey_Fixed and Master_PassKey_Fixed projects configurations)
 - Pairing with just works  method and bonding (select: Peripheral_JustWorks and Master_JustWorks projects configurations)
 - Pairing with Numeric Comparison(BLE V4.2 security feature) and bondig (select: Peripheral_NumericComp and Master_NumericComp projects configurations)

In order to exercise each specific security scenario, each Peripheral project configuration must be used with the related Master project configuration as follows: 

@table Peripheral (Slave) and Central (Master) devices allowed project configurations combinations

|                                |  Slave_PassKey_Random  |  Slave_PassKey_Fixed  |  Slave_JustWorks  | Slave_NumericComp  |
----------------------------------------------------------------------------------------------------------------------------
|  Master_PassKey_Random         |           X            |                       |                   |                    |
|  Master_PassKey_Fixed          |                        |           X           |                   |                    |
|  Master_JustWorks              |                        |                       |        X          |                    |
|  Master_NumericComp            |                        |                       |                   |         X          |
@endtable

The following tables describes the projects main security settings, which combination lead to the selection of the specific Bluetooth Low Energy Security scenario
(PassKey entry with random pin, PassKey entry with fixed pin, Just works, Numeric Value Comparison).

- Peripheral (Slave) device security settings

@table 
|                                |   Slave_PassKey_Random    |  Slave_PassKey_Fixed      |   Slave_JustWorks       |  Slave_NumericComp            |
----------------------------------------------------------------------------------------------------------------------------------------------------
|  IO capability                 |  Display Only             |  Display Only             |  Display Only           |  Keyboard Display             |
|  Use Fixed Pin                 |  NO                       |  YES                      |  YES                    |  YES                          |
|  MITM mode                     |  Required                 |  Required                 |  Not Required           |  Required                     |
|  LE Secure support             |  Supported but optional   |  Supported but optional   |  Supported but optional |  Supported but optional       |
|  Key Press Notification        |  Not supported            |  Not supported            |  Not supported          |  Not supported                |
|  Identity  Address             |  Public                   |  Public                   |  Public                 |  Public                       |
@endtable

- Central (Master) device security settings

@table 
|                                |  Master_PassKey_Random    |  Master_PassKey_Fixed      |  Master_JustWorks        | Master_NumericComp          |
----------------------------------------------------------------------------------------------------------------------------------------------------
|  IO capability                 |  Keyboard Only            |  Keyboard Only             |  Display Only            |                             |
|  Use Fixed Pin                 |  NO                       |  YES                       |  NO                      |                             |
|  MITM mode                     |  Required                 |  Required                  |  Not Required            |                             |
|  LE Secure support (1)         |  Supported and mandatory  |  Supported and mandatory   |  Supported and mandatory | Supported and mandatory (2) |
|  Key Press Notification        |  Not supported            |  Not supported             |  Not supported           | Not supported               |
|  Identity  Address             |  Public                   |  Public                    |  Public                  | Public                      |
@endtable

- NOTES: 
  - (1) Set to Seupported and mandatory in order to force the BLE V4.2 LE Secure connections when Central device connects to Peripheral device.
  - (2) Set to mandatory in order to trigger Numeric Comparison (it is supported on new BLE V 4.2 LE Secure Connections mode) 

Central device is using the Central/Master library (refer to the related html documentation for more information). 

***  Pairing with pass key entry method (no fixed pin) and bonding:

On reset, after initialization, Peripheral device sets security IO capability as display only (IO_CAP_DISPLAY_ONLY) and set authentication requirements as follow:

- MITM authentication (MITM_PROTECTION_REQUIRED)
- don't use fixed pin (DONOT_USE_FIXED_PIN_FOR_PAIRING)
- Secure connection supported but optional  (SC_IS_SUPPORTED)
- Key press notification not supported (KEYPRESS_IS_NOT_SUPPORTED)
- Identity address public (0x00)
- bonding required (BONDING)
                                              
- NOTE: the security pin is randomly generated using the hci_le_rand() API. 
  The Pass Key value displayed on Peripheral hyper terminal must be inserted on Central device hyper terminal (Central device has IO capability keyboard only: IO_CAP_KEYBOARD_ONLY).

After initialization phase, Peripheral device defines a custom service with 2 proprietary characteristics (UUID 128 bits):

- TX characteristic: notification (CHAR_PROP_NOTIFY), no security (ATTR_PERMISSION_NONE); 

- RX characteristic with properties: read (CHAR_PROP_READ),  Link must be encrypted to read (ATTR_PERMISSION_ENCRY_READ), Need authentication to read (ATTR_PERMISSION_AUTHEN_READ),  
GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RES (application is notified when a read request of any type is received for this attribute). 


The Peripheral device enters in discovery mode with local name SlaveSec_A2. When a Central device starts the discovery procedure and detects the Peripheral device, the two devices connects.
After connection, Peripheral device starts a slave security request to the Central device (aci_gap_slave_security_req()) and , as consequence, Central devices starts pairing procedure.
After devices pairs and get bonded, Peripheral device displays the list of its bonded devices and configures its white list in order to add the bonded Central device to its white list (aci_gap_configure_whitelist() API).
Central devices starts the service discovery procedure to identify the Peripheral service and characteristics and, then, enabling the TX characteristic notification. 
Peripheral device starts  TX characteristic notification to the Central device at periodic interval,  and it provides the RX characteristic value to the Central device each time it reads it.
When  connected, if user presses the BLE platform  button  PUSH1,  Peripheral device disconnects and enters in undirected connectable mode with filtering enabled (WHITE_LIST_FOR_ALL: Process scan and connection requests only from devices in the White List). This implies that Peripheral device accepts connection requests only from devices on its  white list: Central device is still be able to connect to the Peripheral device; any other device connection requests are not accepted from the Peripheral device. 

***  Pairing with pass key entry method (with fixed pin) and bonding:

On reset, after initialization, Peripheral device sets security IO capability as display only (IO_CAP_DISPLAY_ONLY) and set authentication requirements as follow:

- MITM authentication (MITM_PROTECTION_REQUIRED)
- use fixed pin (USE_FIXED_PIN_FOR_PAIRING)
- Secure connection supported but optional  (SC_IS_SUPPORTED)
- Key press notification not supported (KEYPRESS_IS_NOT_SUPPORTED)
- Identity address public (0x00)
- bonding required (BONDING)
                                              
- NOTE: the security pin is fixed (PERIPHERAL_SECURITY_KEY on file BLE_Security_Peripheral.h).

After initialization phase, Peripheral device defines a custom service with 2 proprietary characteristics (UUID 128 bits):

- TX characteristic: notification (CHAR_PROP_NOTIFY), no security (ATTR_PERMISSION_NONE); 

- RX characteristic with properties: read (CHAR_PROP_READ),  Link must be encrypted to read (ATTR_PERMISSION_ENCRY_READ), Need authentication to read (ATTR_PERMISSION_AUTHEN_READ),  
GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RES (application is notified when a read request of any type is received for this attribute). 


The Peripheral device enters in discovery mode with local name SlaveSec_A1. When a Central device starts the discovery procedure and detects the Peripheral device, the two devices connects.
After connection, Peripheral device starts a slave security request to the Central device (aci_gap_slave_security_req()) and , as consequence, Central devices starts pairing procedure.
After devices pairs and get bonded, Peripheral device displays the list of its bonded devices and configures its white list in order to add the bonded Central device to its white list (aci_gap_configure_whitelist() API).
Central devices starts the service discovery procedure to identify the Peripheral service and characteristics and, then, enabling the TX characteristic notification. 
Peripheral device starts  TX characteristic notification to the Central device at periodic interval,  and it provides the RX characteristic value to the Central device each time it reads it.
When connected, if user presses the BLE platform  button  PUSH1,  Peripheral device disconnects and enters in undirected connectable mode mode with filtering enabled (WHITE_LIST_FOR_ALL: Process scan and connection requests only from devices in the White List). This implies that Peripheral device accepts connection requests only from devices on its  white list: Central device is still be able to connect to the Peripheral device; any other device connection requests are not accepted from the Peripheral device. 

***  Pairing with just works  method and bonding.

On reset, after initialization, Peripheral device sets security IO capability as display only (IO_CAP_DISPLAY_ONLY) and set authentication requirements as follow:

- MITM authentication (MITM_PROTECTION_NOT_REQUIRED)
- use fixed pin (USE_FIXED_PIN_FOR_PAIRING)
- Secure connection supported but optional  (SC_IS_SUPPORTED)
- Key press notification not supported (KEYPRESS_IS_NOT_SUPPORTED)
- Identity address public (0x00)
- bonding required (BONDING)
                                              
After initialization phase, Peripheral device defines a custom service with 2 proprietary characteristics (UUID 128 bits):

- TX characteristic: notification (CHAR_PROP_NOTIFY), no security (ATTR_PERMISSION_NONE); 

- RX characteristic with properties: read (CHAR_PROP_READ),  Link must be encrypted to read (ATTR_PERMISSION_ENCRY_READ)
GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RES (application is notified when a read request of any type is received for this attribute). 


The Peripheral device enters in discovery mode with local name SlaveSec_A0. When a Central device starts the discovery procedure and detects the Peripheral device, the two devices connects.
After connection, Peripheral device starts a slave security request to the Central device (aci_gap_slave_security_req()) and , as consequence, Central devices starts pairing procedure.
After devices pairs and get bonded, Peripheral device displays the list of its bonded devices and configures its white list in order to add the bonded Central device to its white list (aci_gap_configure_whitelist() API).
Central devices starts the service discovery procedure to identify the Peripheral service and characteristics and, then, enabling the TX characteristic notification. 
Peripheral device starts  TX characteristic notification to the Central device at periodic interval,  and it provides the RX characteristic value to the Central device each time it reads it.
When connected, if user presses the BLE platform  button  PUSH1,  Peripheral device disconnects and enters in undirected connectable mode mode with filtering enabled (WHITE_LIST_FOR_ALL: Process scan and connection requests only from devices in the White List). This implies that Peripheral device accepts connection requests only from devices on its  white list: Central device is still be able to connect to the Peripheral device; any other device connection requests are not accepted from the Peripheral device. 

***  Pairing with Numeric Comparison (BLE V4.2 only) and bonding:

On reset, after initialization, Peripheral device sets security IO capability as Keyboard, display  (IO_CAP_KEYBOARD_DISPLAY) and set authentication requirements as follow:

- MITM authentication (MITM_PROTECTION_REQUIRED)
- use fixed pin (DONOT_USE_FIXED_PIN_FOR_PAIRING)
- Secure connection supported but optional  (SC_IS_SUPPORTED)
- Key press notification not supported (KEYPRESS_IS_NOT_SUPPORTED)
- Identity address public (0x00)
- bonding required (BONDING)
                                              
After initialization phase, Peripheral device defines a custom service with 2 proprietary characteristics (UUID 128 bits):

- TX characteristic: notification (CHAR_PROP_NOTIFY), no security (ATTR_PERMISSION_NONE); 

- RX characteristic with properties: read (CHAR_PROP_READ),  Link must be encrypted to read (ATTR_PERMISSION_ENCRY_READ), Need authentication to read (ATTR_PERMISSION_AUTHEN_READ),  
GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RES (application is notified when a read request of any type is received for this attribute). 


The Peripheral device enters in discovery mode with local name SlaveSec_A3. When a Central device starts the discovery procedure and detects the Peripheral device, the two devices connects.
After connection, Peripheral device starts a slave security request to the Central device (aci_gap_slave_security_req()) and , as consequence, Central devices starts pairing procedure.

On both devices the confirm numeric value event is triggered and the related numeric value is displayed. User is requested to press key y in order to confirm the numeric value on both devices (any other key for not confirming it). 
After devices pairs and get bonded, Peripheral device displays the list of its bonded devices and configures its white list in order to add the bonded Central device to its white list (aci_gap_configure_whitelist() API).
Central devices starts the service discovery procedure to identify the Peripheral service and characteristics and, then, enabling the TX characteristic notification. 
Peripheral device starts  TX characteristic notification to the Central device at periodic interval,  and it provides the RX characteristic value to the Central device each time it reads it.
When  connected, if user presses the BLE platform  button  PUSH1,  Peripheral device disconnects and enters in undirected connectable mode mode with filtering enabled (WHITE_LIST_FOR_ALL: Process scan and connection requests only from devices in the White List). This implies that Peripheral device accepts connection requests only from devices on its  white list: Central device is still be able to connect to the Peripheral device; any other device connection requests are not accepted from the Peripheral device. 

GENERAL NOTE: TX and RX characteristics length is 20 bytes and related values are defined as follow:

  - TX characteristic value: {'S','L','A','V','E','_','S','E','C','U','R','I','T','Y','_','T','X',' ',x1,x2};  where x1, x2 are counter values
  - RX characteristic value: {'S','L','A','V','E','_','S','E','C','U','R','I','T','Y','_','R','X',' ',x1,x2};  where x1, x2 are counter values
  - When using a Smarthphone as Central device, please notice that if this device uses a random resolvable address, Periheral device is able to accept connection or scan requests coming from  it on reconnection phase.
    This is due to the fact, that when disconnecting, Peripheral device enters in undirected connectable mode with filtering enabled (WHITE_LIST_FOR_ALL: Process scan and connection requests only from devices in the White List). As consequence, it is able to accept Smarthphone's scan request or connection requests, only if the Controller Privacy is enabled on Peripheral device (please refer to the BlueNRG GUI SW package (STSW-BNRGUI) script: BlueNRG GUI x.x.x\Application\scripts\Privacy_1_2_Whitelist\Privacy_1_2_Slave_WhiteList.py for a a complete reference example).
    A possible simple option, on Peripheral device, is to replace the WHITE_LIST_FOR_ALL advertising filter policy with NO_WHITE_LIST_USE: Peripheral device doesn't enable device filtering after reconnection,  and it is able to accept connection or scan requests coming from a Smartphone using resolvable random addresses. 


**/
   
/** @addtogroup BlueNRG1_demonstrations_applications
 * BlueNRG-1 Security peripheral \see P_main.c for documentation.
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
#include "BLE_Security_Peripheral.h"
#include "SDK_EVAL_Config.h"
#include "BLE_Security_Peripheral_config.h"
#include "sleep.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
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

/* Private macro -------------------------------------------------------------*/
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

/* Private functions ---------------------------------------------------------*/


int main(void) 
{
  uint8_t ret;
 
  /* System Init */
  SystemInit();
  
  /* Identify BlueNRG1 platform */
  SdkEvalIdentification();

  /* Configure I/O communication channel:
  */
  SdkEvalComUartInit(UART_BAUDRATE);
  
  /* Configure I/O communication channel  */
  SdkEvalComIOConfig(SdkEvalComIOProcessInputData);
  
  /* BlueNRG-1 stack init */
  ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);
  if (ret != BLE_STATUS_SUCCESS) {
    printf("Error in BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
    while(1);
  }
  
  /* Init Device */
  ret = DeviceInit();
  if (ret != BLE_STATUS_SUCCESS) {
    printf("DeviceInit()--> Failed 0x%02x\r\n", ret);
    while(1);
  }
  
  /* Initialize the button */
  SdkEvalPushButtonInit(USER_BUTTON); 

  user_set_wakeup_source(USER_BUTTON); 
  
  PRINTF("BlueNRG-1 BLE Security Peripheral Application (version: %s, security mode: %s; button: %d)\r\n", BLE_APPLICATION_VERSION_STRING,Security_Configuration,USER_BUTTON);
  
  while(1) {
    
    /* BlueNRG-1 stack tick */
    BTLE_StackTick();
    
    /* Application tick */
    APP_Tick();
    
    /* Power Save management: no timer and wakeup on UART RX, PUSH button */
    BlueNRG_Sleep(SLEEPMODE_NOTIMER, wakeup_source, wakeup_level);
    
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
