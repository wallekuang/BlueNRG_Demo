
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ble_const.h" 
#include "bluenrg1_stack.h"
#include "osal.h"
#include "app_state.h"
#include "SDK_EVAL_Config.h"
#include "gatt_db.h"
#include "BLE_Security_Peripheral.h"

uint16_t ServHandle, TXCharHandle, RXCharHandle;

/* UUIDs */
Service_UUID_t service_uuid;
Char_UUID_t char_uuid;

static uint16_t rx_counter = 0; 
static uint16_t tx_counter = 0; 
extern volatile uint16_t connection_handle;

/*******************************************************************************
* Function Name  : Add_Service
* Description    : Add the Slave Security service.
* Input          : None
* Return         : Status.
*******************************************************************************/
uint8_t Add_Service(void)
{
  uint8_t ret;

  /*
  UUIDs:
  D973F2E0-B19E-11E2-9E96-0800200C9A66
  D973F2E1-B19E-11E2-9E96-0800200C9A66
  D973F2E2-B19E-11E2-9E96-0800200C9A66
  */

  const uint8_t uuid[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe0,0xf2,0x73,0xd9};
  const uint8_t charUuidTX[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe1,0xf2,0x73,0xd9};
  const uint8_t charUuidRX[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe2,0xf2,0x73,0xd9};

  Osal_MemCpy(&service_uuid.Service_UUID_128, uuid, 16);
  ret = aci_gatt_add_service(UUID_TYPE_128, &service_uuid, PRIMARY_SERVICE, 6, &ServHandle); 
  if (ret != BLE_STATUS_SUCCESS) goto fail;    

  Osal_MemCpy(&char_uuid.Char_UUID_128, charUuidTX, 16);
  /* TX characteristic: notification property */
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 20, CHAR_PROP_NOTIFY, ATTR_PERMISSION_NONE, 0,
                16, 1, &TXCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;

  Osal_MemCpy(&char_uuid.Char_UUID_128, charUuidRX, 16);
  
  /* RX characteristic: read with encryption & authentication properties */
  ret =  aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, 20, CHAR_PROP_READ, RX_CHAR_SECURITY_PERMISSIONS, GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
              16, 1, &RXCharHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;

  printf("Device Service added.\nTX Char Handle %04X, RX Char Handle %04X\n", TXCharHandle, RXCharHandle);
  
#if (MITM_MODE == MITM_PROTECTION_REQUIRED)
  /* Set security permissions on the TX client characteristic descriptor */
  ret =  aci_gatt_set_security_permission(ServHandle,
                                          TXCharHandle+2,
                                          ATTR_PERMISSION_ENCRY_WRITE|ATTR_PERMISSION_AUTHEN_WRITE);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
#endif 
  
   return BLE_STATUS_SUCCESS; 
fail:
  printf("Error while adding device ervice.\n");
  return BLE_STATUS_ERROR ;
}

/*******************************************************************************
* Function Name  : read_data_handler
* Description    : Updated RX char value for the Client read
* Input          : None.
* Return         : None.
*******************************************************************************/
uint8_t read_data_handler(void)
{
  /* read data handler */
  uint8_t ret= BLE_STATUS_SUCCESS; 
  uint8_t buff_rx[20] = {'S','L','A','V','E','_','S','E','C','U','R','I','T','Y','_','R','X',' ',0,0};
  
  STORE_VALUE(&buff_rx[18],rx_counter);
  rx_counter +=1;
  
  ret = aci_gatt_update_char_value_ext(connection_handle, ServHandle, RXCharHandle, 1,20, 0, 20, buff_rx);
  if (ret != BLE_STATUS_SUCCESS){
    printf("Error while updating RXCharHandle characteristic: 0x%02X\n",ret) ;
    return BLE_STATUS_ERROR ;
  }
  return ret;
}

/*******************************************************************************
* Function Name  : notification_data_handler
* Description    : Updated TX char value x notification to Client
* Input          : None.
* Return         : None.
*******************************************************************************/
uint8_t notification_data_handler(void)
{
  uint8_t ret= BLE_STATUS_SUCCESS; 
  
  uint8_t buff_tx[20] = {'S','L','A','V','E','_','S','E','C','U','R','I','T','Y','_', 'T','X',' ',0,0};
  
  /* notification */
  STORE_VALUE(&buff_tx[18],tx_counter);
  tx_counter = tx_counter +1;

  ret = aci_gatt_update_char_value_ext(connection_handle, ServHandle, TXCharHandle, 1,20,0, 20, buff_tx);
  if (ret != BLE_STATUS_SUCCESS){
    printf("Error while updating TXCharHandle characteristic: 0x%02X\r\n",ret) ;
    return BLE_STATUS_ERROR ;
  }
  return ret;
}
  

