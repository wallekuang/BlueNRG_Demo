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
#define HS_STARTUP_TIME         (uint16_t)(1)  /* High Speed start up time min value */
#define OTA_ACCESS_ADDRESS      (uint32_t)(0x88D188DA)
#define OTA_CHANNEL             24
#define RX_WAKEUP_TIME          300
#define TX_WAKEUP_TIME          500

#define RX_TIMEOUT_DATA         5000
#define RX_TIMEOUT_ACK          500

#define BYTE_IN_FRAME   16 //64
    
typedef enum {
  OTA_CONNECTION = 0,
  OTA_SIZE = 1,
  OTA_START = 2,
  OTA_DATAREQ = 3,
  OTA_GETDATA = 4,
  OTA_SENDATA = 4,
  OTA_FLASHDATA = 5,
  OTA_COMPLETE = 6,
  OTA_ONGOING = 7,
} ota_state_machine_t;


#define HEADER_CONNECTION       0xA0
#define HEADER_SIZE             0xB0
#define HEADER_START            0xC0
#define HEADER_DATAREQ          0xD0
#define HEADER_GETDATA          0xE0
#define HEADER_SENDATA          0xE0
#define HEADER_NOTSTART         0xF0


/** @brief Get specific application tag value stored at vector table index 
  * OTA_TAG_VECTOR_TABLE_ENTRY_OFFSET
  */
#define TAG_VALUE(x)     (* ((volatile uint32_t*) ((x) + OTA_TAG_VECTOR_TABLE_ENTRY_OFFSET)))

/** @brief Application base address */
extern volatile uint32_t appAddress; 


#define SERVICE_MANAGER_OFFSET     (0x0)
#define FLASH_PAGE_SIZE N_BYTES_PAGE 


/** @brief OTA in progress tag: it is sets on vector table during OTA upgrade */
#define OTA_IN_PROGRESS_TAG      (0xFFFFFFFF)

/** @brief  OTA  invalid, old tag: it tags old application as invalid/old 
  * (OTA upgrade done and jump to new application) 
  */
#define OTA_INVALID_OLD_TAG      (0x00000000)

/** @brief  OTA valid tag: it tags new application as valid 
  * (It is done after a OTA upgrade process is completed with success:  
  * as consequence of a SW reset to OTA Reset Manager) */
#define OTA_VALID_TAG            (0xAA5555AA)

/** @brief  OTA Service Manager valid tag: It tags OTA Service manager as valid
  */
#define OTA_SERVICE_MANAGER_TAG  (0xAABBCCDD)


/** @brief  Vector table entry used to register OTA application validity tag*/
#define OTA_TAG_VECTOR_TABLE_ENTRY_INDEX  (4)
/** @brief  Address offset for vector table entry used to register OTA application validity tag */
#define OTA_TAG_VECTOR_TABLE_ENTRY_OFFSET (OTA_TAG_VECTOR_TABLE_ENTRY_INDEX * 4)


#define PAGE_SIZE_ROUND(size) (FLASH_PAGE_SIZE*(((size)+FLASH_PAGE_SIZE-1)/FLASH_PAGE_SIZE))
#define PAGE_SIZE_TRUNC(size) (FLASH_PAGE_SIZE*((size)/FLASH_PAGE_SIZE))


#define NVM_SIZE                   PAGE_SIZE_ROUND(4 * 1024)
#define SERVICE_MANAGER_SIZE       PAGE_SIZE_ROUND(8 * 1024)


/** @brief OTA application with OTA Service manager address: don't change them */
#define SM_APP_OFFSET              (SERVICE_MANAGER_OFFSET + SERVICE_MANAGER_SIZE)
#define APP_WITH_OTA_SERVICE_ADDRESS             (_MEMORY_FLASH_BEGIN_ + SM_APP_OFFSET)

#define APP_OTA_SERVICE_ADDRESS                  (_MEMORY_FLASH_BEGIN_ + SERVICE_MANAGER_OFFSET) 

#define SM_APP_SIZE                PAGE_SIZE_TRUNC((_MEMORY_FLASH_SIZE_-SERVICE_MANAGER_SIZE-NVM_SIZE))

#define APP_WITH_OTA_SERVICE_PAGE_NUMBER_START   (SM_APP_OFFSET/FLASH_PAGE_SIZE)
#define APP_WITH_OTA_SERVICE_PAGE_NUMBER_END     (APP_WITH_OTA_SERVICE_PAGE_NUMBER_START + (SM_APP_SIZE/FLASH_PAGE_SIZE) - 1)

#define OTA_NO_OPERATION                                 (0x11)
#define OTA_APP_SWITCH_OP_CODE_NO_OPERATION              (0xb0014211)
#define OTA_APP_SWITCH_OP_CODE_GO_TO_OTA_SERVICE_MANAGER (OTA_APP_SWITCH_OP_CODE_NO_OPERATION + (OTA_NO_OPERATION*4))



/* Exported functions ------------------------------------------------------- */


#endif /* MAIN_COMMON_H */

/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/
