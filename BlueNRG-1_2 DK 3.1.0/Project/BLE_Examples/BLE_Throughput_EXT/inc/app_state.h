/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
* File Name          : app_state.h
* Author             : AMS - VMA, RF Application Team
* Version            : V1.0.0
* Date               : 08-February-2016
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
#ifndef __APP_STATE__H
#define __APP_STATE_H

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
#define IDX_CONNECTED                           (0)
#define IDX_SET_CONNECTABLE                     (1)
#define IDX_NOTIFICATIONS_ENABLED               (2)
#define IDX_CONN_PARAM_UPD_SENT                 (3)
#define IDX_L2CAP_PARAM_UPD_SENT                (4)
#define IDX_TX_BUFFER_FULL                      (5)
#define IDX_ATT_MTU_START_EXCHANGE              (6)
#define IDX_ATT_MTU_EXCHANGED                   (7)
#define IDX_START_READ_TX_CHAR_HANDLE           (8)
#define IDX_END_READ_TX_CHAR_HANDLE             (9)
#define IDX_START_READ_RX_CHAR_HANDLE           (10)
#define IDX_END_READ_RX_CHAR_HANDLE             (11)
#define IDX_DATA_LEN_EXCH                       (12)
#define IDX_SEND_COMMAND                        (13)
#define IDX_INDICATION_PENDING                  (14)
#define IDX_TIMER_STARTED                       (15)
#define IDX_DEV_PAIRING_REQ                     (16)
#define IDX_DEV_PAIRED                          (17)

#define CONNECTED                               (1 << IDX_CONNECTED)
#define SET_CONNECTABLE                         (1 << IDX_SET_CONNECTABLE)
#define NOTIFICATIONS_ENABLED                   (1 << IDX_NOTIFICATIONS_ENABLED)
#define CONN_PARAM_UPD_SENT                     (1 << IDX_CONN_PARAM_UPD_SENT)
#define L2CAP_PARAM_UPD_SENT                    (1 << IDX_L2CAP_PARAM_UPD_SENT)
#define TX_BUFFER_FULL                          (1 << IDX_TX_BUFFER_FULL)
#define ATT_MTU_START_EXCHANGE                  (1 << IDX_ATT_MTU_START_EXCHANGE)
#define ATT_MTU_EXCHANGED                       (1 << IDX_ATT_MTU_EXCHANGED)
#define START_READ_TX_CHAR_HANDLE               (1 << IDX_START_READ_TX_CHAR_HANDLE)
#define END_READ_TX_CHAR_HANDLE                 (1 << IDX_END_READ_TX_CHAR_HANDLE)
#define START_READ_RX_CHAR_HANDLE               (1 << IDX_START_READ_RX_CHAR_HANDLE)
#define END_READ_RX_CHAR_HANDLE                 (1 << IDX_END_READ_RX_CHAR_HANDLE)
#define DATA_LEN_EXCH                           (1 << IDX_DATA_LEN_EXCH)
#define SEND_COMMAND                            (1 << IDX_SEND_COMMAND)
#define INDICATION_PENDING                      (1 << IDX_INDICATION_PENDING)
#define TIMER_STARTED                           (1 << IDX_TIMER_STARTED)
#define DEV_PAIRING_REQ                         (1 << IDX_DEV_PAIRING_REQ)
#define DEV_PAIRED                              (1 << IDX_DEV_PAIRED)

/**
 * @}
 */



/* Exported macros -----------------------------------------------------------*/
#define APP_FLAG(flag)          (app_flags & flag)
#define APP_FLAG_SET(flag)      (app_flags |= flag)
#define APP_FLAG_CLEAR(flag)    (app_flags &= ~flag)
#define APP_FLAG_RESET()        (app_flags = 0)


#ifdef __cplusplus
}
#endif

#endif /*__APP_STATE__H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
