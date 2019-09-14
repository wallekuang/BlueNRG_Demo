/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : OTA_ResetManager_main.c
* Author             : AMS - VMA RF application team
* Version            : V1.1.0
* Date               : 15-January-2016
* Description        : BlueNRG-1 OTA Reset manager file
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
/** @addtogroup BlueNRG1_demonstrations_applications
* BlueNRG-1 OTA Reset manager \see OTA_ResetManager_main.c for documentation.
*
*@{
*/

/** @} */
/** \cond DOXYGEN_SHOULD_SKIP_THIS
*/
/* Includes ------------------------------------------------------------------*/

#include "BlueNRG1_conf.h"
#include "OTA_btl.h"

typedef  void (*pFunction)(void);

#define RESET_WAKE_DEEPSLEEP_REASONS 0x05

/** @brief Get specific application tag value stored at vector table index 
* OTA_TAG_VECTOR_TABLE_ENTRY_OFFSET
*/
#define TAG_VALUE(x)     (* ((volatile uint32_t*) ((x) + OTA_TAG_VECTOR_TABLE_ENTRY_OFFSET)))

/**
* @brief  It check if  flash storage area has to be erased or not
* @param  None.
* @retval Status: 1 (erase flash); 0 (don't erase flash).
*
* @note The API code could be subject to change in future releases.
*/
static uint8_t OTA_Check_Storage_Area(uint32_t start_address, uint32_t end_address)
{
  volatile uint32_t *address; 
  uint32_t i; 
  
  for(i=start_address;i<end_address; i = i +4)
  { 
    address = (volatile uint32_t *) i;
    if (*address != 0xFFFFFFFF)
      return 1; /* do flash erase */
  }
  
  return 0; /* no flash erase is required */
}

/**
* @brief  It erases the new flash storage area. 
* @param  None.
* @retval None.
*
* @note The API code could be subject to change in future releases.
*/
static void OTA_Erase_Storage_Area(uint16_t startPageNumber, uint16_t endPageNumber)
{
  uint16_t k; 
  
  for(k=startPageNumber;k<=endPageNumber;k++)
  { 
    FLASH_ErasePage(k); 
  }
}

/**
* @brief  It defines the valid application address where to jump
*         by checking the OTA application validity tags for the lower and
*         higher applications
* @param  None.
* @retval None.
*
* @note The API code could be subject to change in future releases.
*/
static uint32_t OTA_Check_Application_Tags_Value(void)
{
  uint32_t appAddress = 0;
  if ( ((TAG_VALUE(APP_LOWER_ADDRESS) == OTA_INVALID_OLD_TAG) && (TAG_VALUE(APP_HIGHER_ADDRESS) == OTA_INVALID_OLD_TAG))   || /* 1 */
      ((TAG_VALUE(APP_LOWER_ADDRESS) == OTA_VALID_TAG) && (TAG_VALUE(APP_HIGHER_ADDRESS) == OTA_INVALID_OLD_TAG))         || /* 2 */
        ((TAG_VALUE(APP_LOWER_ADDRESS) == OTA_VALID_TAG) && (TAG_VALUE(APP_HIGHER_ADDRESS) == OTA_VALID_TAG))               || /* 4 */
          ((TAG_VALUE(APP_LOWER_ADDRESS) == OTA_INVALID_OLD_TAG) && (TAG_VALUE(APP_HIGHER_ADDRESS) == OTA_IN_PROGRESS_TAG))   || /* 8 */
            ((TAG_VALUE(APP_LOWER_ADDRESS) == OTA_VALID_TAG) && (TAG_VALUE(APP_HIGHER_ADDRESS) == OTA_IN_PROGRESS_TAG)))           /* 9 */  
  {
    /* Jump to Lower Application */
    appAddress = APP_LOWER_ADDRESS;
    
    if (OTA_Check_Storage_Area(APP_HIGHER_ADDRESS,APP_HIGHER_ADDRESS_END))
    {
      /* Unlock the Flash */
      flash_sw_lock = FLASH_UNLOCK_WORD;
      
      /* Erase OLD Higher application storage area */
      OTA_Erase_Storage_Area(OTA_HIGHER_APPLICATION_PAGE_NUMBER_START, OTA_HIGHER_APPLICATION_PAGE_NUMBER_END); 
      
      /* Lock the Flash */
      flash_sw_lock = FLASH_LOCK_WORD;
    }
  }
  else if ( ((TAG_VALUE(APP_LOWER_ADDRESS) == OTA_INVALID_OLD_TAG) && (TAG_VALUE(APP_HIGHER_ADDRESS) == OTA_VALID_TAG))       || /* 3 */
           ((TAG_VALUE(APP_LOWER_ADDRESS) == OTA_IN_PROGRESS_TAG) && (TAG_VALUE(APP_HIGHER_ADDRESS) == OTA_INVALID_OLD_TAG)) || /* 6 */
             ((TAG_VALUE(APP_LOWER_ADDRESS) == OTA_IN_PROGRESS_TAG) && (TAG_VALUE(APP_HIGHER_ADDRESS) == OTA_VALID_TAG)))         /* 7 */     
  {
    /* Jump to Higher Application */
    appAddress = APP_HIGHER_ADDRESS;
    
    if (OTA_Check_Storage_Area(APP_LOWER_ADDRESS,APP_LOWER_ADDRESS_END))
    {
      /* Unlock the Flash */
      flash_sw_lock = FLASH_UNLOCK_WORD;
      
      /* Erase OLD Lower application storage area */
      OTA_Erase_Storage_Area(OTA_LOWER_APPLICATION_PAGE_NUMBER_START, OTA_LOWER_APPLICATION_PAGE_NUMBER_END); 
      
      /* Lock the Flash */
      flash_sw_lock = FLASH_LOCK_WORD;
      
    }
  }
  else if ((TAG_VALUE(APP_LOWER_ADDRESS) == OTA_IN_PROGRESS_TAG) && (TAG_VALUE(APP_HIGHER_ADDRESS) == OTA_IN_PROGRESS_TAG))   /* 5 */
  {
    /* 5: Is it possible? No. What to do?*/
  }
  return appAddress;
}

/**
* @brief  OTA Reset Manager main function
* @param  None.
* @retval None.
*
* @note The code could be subject to change in future releases.
*/
int main(void) 
{
  pFunction Jump_To_Application;
  uint32_t JumpAddress, appAddress;
  
  /* Identifies the valid application where to jump based on the OTA application validity tags values placed on
  reserved vector table entry: OTA_TAG_VECTOR_TABLE_ENTRY_INDEX */
  appAddress = OTA_Check_Application_Tags_Value();
  
  if (appAddress == 0) {
    /* This case indicates that no valid application is present and this normally should not happen */
    while (1);
  }
  
  /* Jump to user application */
  JumpAddress = *(__IO uint32_t*) (appAddress + 4);
  Jump_To_Application = (pFunction) JumpAddress;
  /* Initialize user application's Stack Pointer */
  __set_MSP(*(__IO uint32_t*) appAddress);
  Jump_To_Application();
  
  /* Infinite loop */
  while (1)
  {
  }
}
/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
/** \endcond
*/
