#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h"
#include "bluenrg1_stack.h"
#include "app_state.h"
#include "gatt_db.h"
#include "osal.h"
#include "SDK_EVAL_Config.h"

#ifndef DEBUG
#define DEBUG 1
#endif

#define ENABLE_SECURITY         0

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define LED_NUMBER 3

uint16_t RCServHandle, controlPointHandle;

/* UUIDs */
Service_UUID_t service_uuid;
Char_UUID_t controlPointUuid;
  
/*******************************************************************************
* Function Name  : Add_Chat_Service
* Description    : Add the 'Accelerometer' service.
* Input          : None
* Return         : Status.
*******************************************************************************/
tBleStatus Add_RC_Service(void)
{
  tBleStatus ret;

  /*
  UUIDs:
  ed0ef62e-9b0d-11e4-89d3-123b93f75cba
  ed0efb1a-9b0d-11e4-89d3-123b93f75cba
  */

  uint8_t Service_UUID_128[16] = {0xba,0x5c,0xf7,0x93,0x3b,0x12,0xd3,0x89,0xe4,0x11,0x0d,0x9b,0x2e,0xf6,0x0e,0xed};
  uint8_t ControlPoint_UUID_128[16] = {0xba,0x5c,0xf7,0x93,0x3b,0x12,0xd3,0x89,0xe4,0x11,0x0d,0x9b,0x1a,0xfb,0x0e,0xed};  

  Osal_MemCpy(&service_uuid.Service_UUID_128, Service_UUID_128, 16);
  Osal_MemCpy(&controlPointUuid.Char_UUID_128, ControlPoint_UUID_128, 16);
  
  ret = aci_gatt_add_service(UUID_TYPE_128, &service_uuid, PRIMARY_SERVICE, 3, &RCServHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;

#if ENABLE_SECURITY
  ret =  aci_gatt_add_char(RCServHandle, UUID_TYPE_128, &controlPointUuid, 1, CHAR_PROP_READ|CHAR_PROP_WRITE|CHAR_PROP_WRITE_WITHOUT_RESP|CHAR_PROP_SIGNED_WRITE, ATTR_PERMISSION_AUTHEN_READ|ATTR_PERMISSION_AUTHEN_WRITE, GATT_NOTIFY_ATTRIBUTE_WRITE,
                    16, 1, &controlPointHandle);
#else
  ret =  aci_gatt_add_char(RCServHandle, UUID_TYPE_128, &controlPointUuid, 1, CHAR_PROP_READ|CHAR_PROP_WRITE|CHAR_PROP_WRITE_WITHOUT_RESP, ATTR_PERMISSION_NONE, GATT_NOTIFY_ATTRIBUTE_WRITE,
                    16, 1, &controlPointHandle);
#endif
  if (ret != BLE_STATUS_SUCCESS) goto fail;

  PRINTF("RC Service added.\nControl Point Char Handle %04X\n", controlPointHandle);
  return BLE_STATUS_SUCCESS; 
      
fail:
  PRINTF("Error while adding RC service.\n");
  return BLE_STATUS_ERROR ;
}


void Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data)
{
  if(handle == controlPointHandle + 1){
    
    uint8_t led_num = LED_NUMBER;
    
    PRINTF("Command received.\n");
    
    for(int i = 0; i < led_num; i++){ 
       if (i !=1){ /* Only LED 1, 3 are used */
        if((att_data[0]>>i)&0x01){
          PRINTF("LED %d ON\n",i);
          SdkEvalLedOn((SdkEvalLed)i);
        }
        else {
          PRINTF("LED %d OFF\n",i);
          SdkEvalLedOff((SdkEvalLed)i); 
        }
      }
    }
  }
}


