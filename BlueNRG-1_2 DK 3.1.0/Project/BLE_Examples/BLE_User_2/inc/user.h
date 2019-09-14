/*
  ******************************************************************************
  * @file    user.h 
  * @author  AMG - RF Application Team
  * @version V1.0.0
  * @date    25 - 05 - 2019
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
  * <h2><center>&copy; COPYRIGHT 2019 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 
  
/** \cond DOXYGEN_SHOULD_SKIP_THIS
 */ 
 
#ifndef _user_H_
#define _user_H_

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



#endif /* _user_H_ */
/** \endcond 
*/
