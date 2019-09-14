/**
 * @file    slave_config.h
 * @author  AMS - VMA RF Application Team
 * @version V1.0.0
 * @date    October 5, 2015
 * @brief   This file provides all the configuration parameters for the HID/HOGP BLE peripheral.
 * @details
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
 *
 * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SLAVE_CONFIG_H
#define __SLAVE_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#ifdef PTS_AUTOMATING
#define PERIPHERAL_PUBLIC_ADDRESS  {0x23, 0x02, 0x00, 0xE1, 0x80, 0x02}
#else

#ifdef HID_MOUSE
/** 
  * @brief  Peripheral public address.
  * 
  */ 
#define PERIPHERAL_PUBLIC_ADDRESS {0xEC, 0xFC, 0x00, 0xE1, 0x80, 0x02}
#else
/** 
  * @brief  Peripheral public address.
  * 
  */ 
#define PERIPHERAL_PUBLIC_ADDRESS {0xBC, 0xFC, 0x00, 0xE1, 0x80, 0x02}
#endif

#endif

/**
 * @brief Discoverable Advertising Interval Min.
 * Value suggested by HOGP profile specification is
 * 30 ms = 48 * 0.625 ms.
 */
#define DISC_ADV_MIN  0x30 // 30 ms = 48 * 0.625 ms

/**
 * @brief Discoverable Advertising Interval Max.
 * Value suggested by HOGP profile specification is
 * 50 ms = 80 * 0.625 ms.
 */
#define DISC_ADV_MAX 0x50 // 50 ms = 80 * 0.625 ms

/**
 * @brief Discoverable Advertising Higher Latency Interval Min.
 * Value suggested by HOGP profile specification is
 * 20 ms = 32 * 0.625 ms.
 */
#define DISC_ADV_HIGH_LATENCY_MIN  0x20 // 20 ms = 32 * 0.625 ms

/**
 * @brief Discoverable Advertising Higher Latency Interval Max.
 * Value suggested by HOGP profile specification is 
 * 30 ms = 48 * 0.625 ms.
 */
#define DISC_ADV_HIGH_LATENCY_MAX 0x30 // 30 ms = 48 * 0.625 ms

/**
 * @brief Discoverable Advertising Reduced Power Interval Min.
 * Value suggested by HOGP profile specification is
 * 1 s = 1600 * 0.625 ms.
 */
#define DISC_ADV_REDUCED_POWER_MIN 0x640 // 1 s = 1600 * 0.625 ms

/**
 * @brief Discoverable Advertising Reduced Power Interval Max.
 * Value suggested by HOGP profile specification is
 * 2.5 s = 4000 * 0.625 ms.
 */
#define DISC_ADV_REDUCED_POWER_MAX 0xFA0 // 2.5 s = 4000 * 0.625 ms

/**
 * @brief Connection Interval Min.
 * Value suggested by HOGP profile specification is 
 * 11.25 ms = 9 * 1.25 ms.
 */
#define CONN_INTERVAL_MIN 0x09 // 11.25 ms = 9 * 1.25 ms

/**
 * @brief Connection Interval Max
 * Value suggested by HOGP profile specification is
 * 50 ms = 40 * 1.25 ms
 */
#define CONN_INTERVAL_MAX 0x28 // 50 ms = 40 * 1.25 ms

/**
 * @brief Advertising duration during the reconnection
 * Value suggested by HOGP profile specification.
 * Value expressed in milliseconds.
 */
#define DISC_ADV_HIGH_LATENCY_DURATION 30000 // 30 sec.

/**
 * @brief Manufacturer name string length
 */
#define MANUFAC_NAME_LEN 9

/**
 * @brief Model number string length
 */
#define MODEL_NUMB_LEN 4

/**
 * @brief Firmware revision string length
 */
#define FW_REV_LEN 4

/**
 * @brief Software revision string length
 */
#define SW_REV_LEN 4

/**
 * @brief PNP ID string length
 */
#define PNP_ID_LEN 7

#ifdef HID_MOUSE
/**
 * @brief Number of Reports data in the HID device
 */
#define REPORT_NUMBER 1
#else
/**
 * @brief Number of Reports data in the HID device
 */
#define REPORT_NUMBER 2
#endif

/**
 * @brief Number of external reports in the report map
 */
#define EXTERNAL_REPORT_NUMBER 1

/**
 * @brief Number of included service in the HID service
 */
#define INCLUDED_SERVICE_NUMBER 1

#endif
