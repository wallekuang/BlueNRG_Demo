/**
  ******************************************************************************
  * @file    main_common.h 
  * @author  RF Application Team
* @version V1.0.1
* @date    April-2018
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
#define ADDRESS_DEV1                    (uint32_t)(0x8888D1D1)  /* Address of RX device 1 used by TX device */
#define ADDRESS_DEV2                    (uint32_t)(0x8888DDDD)  /* Address of RX device 2 used by TX device */

#define ACCESS_ADDRESS                  ADDRESS_DEV1  /* Address of RX device */
//#define ACCESS_ADDRESS                 ADDRESS_DEV2  /* Address of RX device */

#define FREQUENCY_CHANNEL       (uint8_t)(24)    // RF channel 22
#define HS_STARTUP_TIME         (uint16_t)(1)    /* High Speed start up time min value */
#define TX_WAKEUP_TIME           500        /* 400 us */
#define RX_WAKEUP_TIME           400        /* 300 us */
#define RX_TIMEOUT_1ST_PART      300000      /* 300 ms */
#define RX_TIMEOUT_2ND_PART      1000      /* 1 ms */
#define RX_TIMEOUT_ACK           1000      /* 1 ms */

/* Exported functions ------------------------------------------------------- */


#endif /* MAIN_COMMON_H */

/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/
