/******************** (C) COPYRIGHT 2013 STMicroelectronics ********************
* File Name          : master_config.h
* Author             : AMG RF Application Team
* Version            : V1.0.0
* Date               : 15-December-2016
* Description        : Master Configuration parameters
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MASTER_CONFIG_H
#define __MASTER_CONFIG_H

/******************** Device Init Parameters *******************/

/**
 * @brief Public Address Type
 *
 */
#define PUBLIC_DEVICE_ADDRESS 0x00

/** 
 * @brief  Master public address.
 * 
 */ 
#define MASTER_PUBLIC_ADDRESS {0x22, 0x02, 0x71, 0xE1, 0x80, 0x02}

/**
 * @brief Transmit power level
 */
#define TX_POWER_LEVEL 4

/******************** Security  Parameters *******************/

#define MASTER_PASS_KEY 123456 //0x0001E240
   
#define MIN_KEY_SIZE 0x07
   
#define MAX_KEY_SIZE 0x10

/******************** Discovery Procedure Parameters *******************/

/**
 * @brief Limited Discovery Scan Interval
 */
#define LIM_DISC_SCAN_INT  0x30 // 30 ms = 48 * 0.625 ms

/**
 * @brief Limited Discovery Scan Window
 */
#define LIM_DISC_SCAN_WIND 0x30 // 30 ms = 48 * 0.625 ms

/**
 * @brief Fiter duplicates
 */
#define FILTER_DUPLICATES 0x01

/******************** Discovery Connection Parameters *******************/

/**
 * @brief Fast Scan duration 
 */
#define FAST_SCAN_DURATION 30000   // 30 sec

/**
 * @brief Fast Scan Interval to attempt fast connection.
 * This time is set to 30 ms. 
 * The expresiion is Time = N x 0.625 msec.
 */
#define FAST_SCAN_INTERVAL 48 

/**
 * @brief Fast Scan Window to attempt fast connection.
 * This time is set to 30 ms.
 * The expresiion is Time = N x 0.625 msec.
 */
#define FAST_SCAN_WINDOW   48 

/**
 * @brief Reduced power scan interval to attempt connection
 * This time is set to 2.56 s
 * The expresiion is Time = N x 0.625 msec.
 */
#define REDUCED_POWER_SCAN_INTERVAL 4096

/**
 * @brief Reduced power scan window to attempt connection
 * This time is set to 11.25 ms
 * The expresiion is Time = N x 0.625 msec.
 */
#define REDUCED_POWER_SCAN_WINDOW   11250

/**
 * @brief Fast minimum connection interval.
 * 
 * The actual value is Time = N x 1.25 msec.
 */
#define FAST_MIN_CONNECTION_INTERVAL  40

/**
 * @brief Fast maximum connection interval.
* The actual value  Time = N x 1.25 msec.
 */
#define FAST_MAX_CONNECTION_INTERVAL 56

/**
 * @brief Fasr connection latency
 */
#define FAST_CONNECTION_LATENCY 0

/**
 * @brief Supervision Timeout.
 * This time is 200 ms.
 * The expression is Timeout = N x 10 ms
 */
#define SUPERVISION_TIMEOUT 1000



/* ----------- Security MODES -------------------------------------------------------*/

#define PASSKEY_RANDOM_PIN  0
#define PASSKEY_FIXED_PIN   1
#define JUST_WORKS          2
#define NUMERIC_COMPARISON  3

/* Default security mode */
#ifndef SECURITY_MODE
#define SECURITY_MODE PASSKEY_RANDOM_PIN
#endif 


#define SECURE_CONNECTIONS_USER_MODE SC_IS_MANDATORY /* Supported &  mandatory:  it forces LE Secure connections,  if peripheral supports it with at least SC_IS_SUPPORTED option */

/* ----------- JUST WORKs configuration --------------------------------------*/
   
#if (SECURITY_MODE==JUST_WORKS) 

#define SECURITY_MODE_NAME  "Just Works"

/** 
  * @brief Peripheral device name
*/
#define PERIPHERAL_DEVICE_NAME "SlaveSec_A0"

#define PERIPHERAL_DEVICE_NAME_LEN (11)
   
/** 
  * @brief MITM Mode
*/
#define MITM_MODE MITM_PROTECTION_NOT_REQUIRED /* MITM is not enabled */

/** 
  * @brief Fixed pin usage
*/
#define FIXED_PIN_POLICY  DONOT_USE_FIXED_PIN_FOR_PAIRING /* don't use fixed pin */

/** 
  * @brief IO capability
*/
#define IO_CAPABILITY  IO_CAP_DISPLAY_ONLY /* Display only: if Peripheral sets also Display only --> Just Works*/

/** 
  * @brief Secure Connections support level
*/
#define SECURE_CONNECTION_SUPPORT  SECURE_CONNECTIONS_USER_MODE

/** 
  * @brief KeyPress notification support
*/
#define KEYPRESS_NOTIFICATION  KEYPRESS_IS_NOT_SUPPORTED /* Not supported */

/** 
  * @brief Identity address tyoe
*/
#define IDENTITY_ADDRESS  0x00 /* Public address */

/* ----------- PASSKEY with fixed pin ----------------------------------------*/

#elif (SECURITY_MODE==PASSKEY_FIXED_PIN) 

#define SECURITY_MODE_NAME  "PassKey Fixed Pin"

/** 
  * @brief Peripheral device name
*/
#define PERIPHERAL_DEVICE_NAME "SlaveSec_A1"

#define PERIPHERAL_DEVICE_NAME_LEN (11)
   
/** 
  * @brief MITM Mode
*/
#define MITM_MODE MITM_PROTECTION_REQUIRED /* MITM is enabled */

/** 
  * @brief Fixed pin usage
*/
#define FIXED_PIN_POLICY  USE_FIXED_PIN_FOR_PAIRING /* use fixed pin */

/** 
  * @brief IO capability
*/
#define IO_CAPABILITY  IO_CAP_KEYBOARD_ONLY /* Keyboard only: if Peripheral sets  Display only --> Pass Key entry (fixed pin)*/

/** 
  * @brief Secure Connections support level
*/
#define SECURE_CONNECTION_SUPPORT  SECURE_CONNECTIONS_USER_MODE

/** 
  * @brief KeyPress notification support
*/
#define KEYPRESS_NOTIFICATION  KEYPRESS_IS_NOT_SUPPORTED /* Not supported */

/** 
  * @brief Identity address tyoe
*/
#define IDENTITY_ADDRESS  0x00 /* Public address  */

/*  ----------- PASS KEY ENTRY configuration with random key (default configuration)*/
   
#elif (SECURITY_MODE==PASSKEY_RANDOM_PIN) 

#define SECURITY_MODE_NAME  "PassKey Random Pin"
   
/** 
  * @brief Peripheral device name
*/
#define PERIPHERAL_DEVICE_NAME "SlaveSec_A2"

#define PERIPHERAL_DEVICE_NAME_LEN (11)   

/** 
  * @brief MITM Mode
*/
#define MITM_MODE MITM_PROTECTION_REQUIRED /* MITM is enabled */

/** 
  * @brief Fixed pin usage
*/
#define FIXED_PIN_POLICY  DONOT_USE_FIXED_PIN_FOR_PAIRING /* don't use fixed pin */

/** 
  * @brief IO capability
*/
#define IO_CAPABILITY  IO_CAP_KEYBOARD_ONLY /* Keyboard only: if Peripheral sets Display only --> Pass Key entry (no fixed pin) */

/** 
  * @brief Secure Connections support level
*/
#define SECURE_CONNECTION_SUPPORT  SECURE_CONNECTIONS_USER_MODE

/** 
  * @brief KeyPress notification support
*/
#define KEYPRESS_NOTIFICATION  KEYPRESS_IS_NOT_SUPPORTED /* Not supported */

/** 
  * @brief Identity address tyoe
*/
#define IDENTITY_ADDRESS  0x00 /* Public address */

/*  ----------- NUMERIC COMPARISON VALUE configuration (ONLY BLE 4.2 security: LE Secure connections) */

#elif (SECURITY_MODE==NUMERIC_COMPARISON) 

#define SECURITY_MODE_NAME  "Numeric Comparison"

/** 
  * @brief Peripheral device name
*/
#define PERIPHERAL_DEVICE_NAME "SlaveSec_A3"

#define PERIPHERAL_DEVICE_NAME_LEN (11)
   
/** 
  * @brief MITM Mode
*/
#define MITM_MODE MITM_PROTECTION_REQUIRED /* MITM is enabled */

/** 
  * @brief Fixed pin usage
*/
#define FIXED_PIN_POLICY  USE_FIXED_PIN_FOR_PAIRING /* use fixed pin */

/** 
  * @brief IO capability
*/
#define IO_CAPABILITY  IO_CAP_KEYBOARD_DISPLAY /* Keyboard Display */

/** 
  * @brief Secure Connections support level
*/
#define SECURE_CONNECTION_SUPPORT  SC_IS_MANDATORY /* Supported &  mandatory:  it forces LE Secure connections if peripheral supports it with at least SC_IS_SUPPORTED option (only option for triggering Numeric Comparison)  */

/** 
  * @brief KeyPress notification support
*/
#define KEYPRESS_NOTIFICATION  KEYPRESS_IS_NOT_SUPPORTED /* Not supported */

/** 
  * @brief Identity address tyoe
*/
#define IDENTITY_ADDRESS  0x00 /* Public address */

#endif 
   
#endif /*__MASTER_CONFIG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
