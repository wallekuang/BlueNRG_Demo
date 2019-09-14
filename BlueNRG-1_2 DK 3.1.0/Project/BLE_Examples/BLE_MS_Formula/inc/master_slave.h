/*
  ******************************************************************************
  * @file    master_slave.h 
  * @author  AMS - RF Application Team
  * @version V1.1.0
  * @date    17 - September - 2018
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
  * <h2><center>&copy; COPYRIGHT 2017 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 
  
/** \cond DOXYGEN_SHOULD_SKIP_THIS
 */ 
 
#ifndef _MASTER_SLAVE_H_
#define _MASTER_SLAVE_H_

/**
 * @name Flags for application
 * @{
 */
#define DO_SLAVES_CONNECTION      0x01
#define WAIT_SLAVE_CONNECTION     0x02
#define SET_CONNECTABLE           0x04 
#define NOTIFICATIONS_ENABLED     0x08
#define WAIT_PARAM_UPD_SENT       0x10
#define CONN_PARAM_UPD_SENT       0x20
#define GET_NOTIFICATION          0x40
#define GATT_EXCHANGE_CONFIG_SENT 0x80
#define TX_BUFFER_FULL            0x100
#define APP_ERROR                 0x200


/* Exported variables ------------------------------------------------------- */  
     
/** 
  * @brief  Variable which contains some flags useful for application
  */ 
extern volatile int app_flags;

/**
 * @}
 */

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
  * @brief  This function ... 
  * @param  None
  * @retval None
  */
uint8_t  device_scanning(void);


/**
  * @brief  This function ... 
  * @param  None
  * @retval Status
  */
uint8_t set_connection(void);


/**
  * @brief  This function update the characteristic with  slave_index value
  * @param  slave_index: index of the slave device to be provided to master/s device
*         char_len: length of characteristic to be updated
  * @retval None
  */
void update_characterists(uint8_t slave_index,uint8_t char_len);


/**
  * @brief  User Application tick 
  * @param  None
  * @retval None
  */
void APP_Tick(void);



#endif /* _MASTER_SLAVE_H_ */
/** \endcond 
*/
