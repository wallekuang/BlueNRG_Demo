/**
  ******************************************************************************
  * @file    main_common.h 
  * @author  RF Application Team
  * @version V1.0.0
  * @date    September-2017
  * @brief   Library configuration file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAIN_COMMON_H
#define MAIN_COMMON_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define ------------------------------------------------------------*/
#define NSLAVES        2

#define SLAVE_DEV1              (uint32_t)(0x8888A1A1)
#define SLAVE_DEV2              (uint32_t)(0x8888D3D3)
#define SLAVE_DEV3              (uint32_t)(0x8888C5C5)
#define SLAVE_DEV4              (uint32_t)(0x8888B7B7)
    
#define APP_CHANNEL             (uint8_t)(24)
#define HS_STARTUP_TIME         (uint16_t)(1)  /* High Speed start up time min value */
#define WKP_OFFSET              300
#define MAX_RETRY_TEST          3
#define RX_TIMEOUT_DATA         10000 
#define RX_TIMEOUT_ACK          1000

/* Exported functions ------------------------------------------------------- */


#endif /* MAIN_COMMON_H */

/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/
