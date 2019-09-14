/*
  ******************************************************************************
  * @file    slave.h 
  * @author  AMG - RF Application Team
  * @version V1.0.0
  * @date    05 - 10 - 2018
  * @brief   Application Header functions
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2018 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 
  
/** \cond DOXYGEN_SHOULD_SKIP_THIS
 */ 
 
#ifndef _user_H_
#define _user_H_

/**
 * @name Flags for application
 * @{
 */
#define DO_SLAVE_SECURITY_REQUEST      0x01
#define GET_BONDED_DEVICES             0x02
#define DO_CONFIGURE_WHITELIST         0x04 
#define WAIT_RECONNECTION              0x08
#define WAIT_SERVICES_DISCOVERY        0x10
#define DO_TERMINATE                   0x20
#define DO_NOTIFICATIONS               0x40 
#define TX_BUFFER_FULL                 0x80
#define CONNECTED                      0x100
#define APP_ERROR                      0x200


/** 
  * @brief  Variable which contains some flags useful for application
  */ 
extern volatile int app_flags;

/* Exported macros -----------------------------------------------------------*/
#define APP_FLAG(flag) (app_flags & flag)

#define APP_FLAG_SET(flag) (app_flags |= flag)
#define APP_FLAG_CLEAR(flag) (app_flags &= ~flag)


/**
  * @brief  This function initializes the BLE GATT & GAP layers and it sets the TX power level 
  * @param  None
  * @retval None
  */
void device_initialization(void);


/**
  * @brief  This function ... 
  * @param  None
  * @retval None
  */
void set_database(void);


/**
  * @brief  This function handles the BLE advertising mode 
  * @param  None
  * @retval None
  */
void set_device_discoverable(void);


/**
  * @brief  User Application tick 
  * @param  None
  * @retval None
  */
void APP_Tick(void);

/**
  * @brief  This function handles the timing of notification
  * @param  None
  * @retval None
  */
void set_slave_update_timer(void); 

#endif /* _user_H_ */
/** \endcond 
*/
