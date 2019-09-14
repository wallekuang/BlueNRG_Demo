/**
  ******************************************************************************
  * @file    DTM_Updater_Config.h 
  * @author  VMA RF Application Team
  * @version V1.0.0
  * @date    July-2015
  * @brief   
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
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DTM_UPDATER_CONFIG_H
#define DTM_UPDATER_CONFIG_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef void (*EntryPoint)(void);

/* Exported constants --------------------------------------------------------*/
#define BLUE_FLAG_RAM_BASE_ADDRESS      (0x20000030)
#define BLUE_FLAG_FLASH_BASE_ADDRESS    (0x10040814)

#define DTM_APP_ADDR                    (0x10040800)

#define BLUE_FLAG_RAM_RESET             (0x01010101)
#define BLUE_FLAG_RESET                 (0x00000000)
#define BLUE_FLAG_SET                   (0x424C5545)
    

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* DTM_UPDATER_CONFIG_H */

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/