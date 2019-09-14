/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
* File Name          : app.h
* Author             : AMG - RF Application Team
* Version            : V1.0.0
* Date               : 05-December-2016
* Description        : Header file wich contains variable used for application.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_H
#define __APP_H

#ifdef __cplusplus
 extern "C" {
#endif


/* Exported variables ------------------------------------------------------- */  
     
/** 
  * @brief  Variable which contains some flags useful for application
  */ 
extern volatile int app_flags;

/**
 * @name Flags for application
 * @{
 */
#define CONNECTED               0x01
#define SET_CONNECTABLE         0x02
#define NOTIFICATIONS_ENABLED   0x04
#define CONN_PARAM_UPD_SENT     0x08
#define L2CAP_PARAM_UPD_SENT    0x10
#define TX_BUFFER_FULL          0x20
/* Added flags for handling security steps */
#define START_GAP_SLAVE_SECURITY_REQUEST    0x100
#define HCI_ENCRYPTION_CHANGE_EVENT_FLAG    0x200 
#define ACI_GAP_PASS_KEY_REQ_EVENT_FLAG     0x400
#define ACI_GAP_PAIRING_COMPLETE_EVENT_FLAG 0x800
#define START_TERMINATE_LINK_FLAG           0x1000
#define PRINT_CONNECTED_DATA                0x2000

/* BLE Security v4.2 is supported: BLE stack FW version >= 2.x */
#define START_CONFIRM_NUMERIC_VALUE         0x4000

/**
 * @}
 */

/* Exported macros -----------------------------------------------------------*/
#define APP_FLAG(flag) (app_flags & flag)

#define APP_FLAG_SET(flag) (app_flags |= flag)
#define APP_FLAG_CLEAR(flag) (app_flags &= ~flag)

#ifdef __cplusplus
}
#endif

#endif /*__APP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
