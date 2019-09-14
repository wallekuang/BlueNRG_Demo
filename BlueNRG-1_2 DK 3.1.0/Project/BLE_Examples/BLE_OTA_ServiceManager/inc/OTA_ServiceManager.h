
#ifndef _OTA_SERVICE_MANAGER_H_
#define _OTA_SERVICE_MANAGER_H_

/** @brief Get specific application tag value stored at vector table index 
  * OTA_TAG_VECTOR_TABLE_ENTRY_OFFSET
  */
#define TAG_VALUE(x)     (* ((volatile uint32_t*) ((x) + OTA_TAG_VECTOR_TABLE_ENTRY_OFFSET)))

/** @brief Application base address */
extern volatile uint32_t appAddress; 

uint8_t OTA_ServiceManager_DeviceInit(void);
void APP_Tick(void);

#endif // _OTA_SERVICE_MANAGER_H_
