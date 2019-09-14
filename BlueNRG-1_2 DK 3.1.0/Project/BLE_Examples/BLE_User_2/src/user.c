/*
  ******************************************************************************
  * @file    user.c 
  * @author  AMG - RF Application Team
  * @version V1.0.0
  * @date    25 - 05 - 2019
  * @brief   Application functions
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
 
/* Includes-----------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h"
#include "bluenrg1_stack.h"
#include "gp_timer.h"
#include "SDK_EVAL_Config.h"
#include "OTA_btl.h"
#include "user.h"
#include "osal.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
Service_UUID_t service_uuid;

#define COPY_UUID_16(uuid_struct, uuid_1, uuid_0) \
do {\
  uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; \
}while(0)

Char_UUID_t char_uuid;


/**
 * @brief  Init a BlueNRG device
 * @param  None.
 * @retval None.
*/
void device_initialization(void)
{
  uint16_t service_handle;
  uint16_t dev_name_char_handle;
  uint16_t appearance_char_handle;
  uint8_t status;

  uint8_t value_8[] = {0xDE,0x71,0x00,0xE1,0x80,0x02};

  //aci_hal_write_config_data
  //status = aci_hal_write_config_data(offset,length,value);
  status = aci_hal_write_config_data(0x00,0x06,value_8);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_hal_write_config_data() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_hal_write_config_data --> SUCCESS\r\n");
  }

  //aci_hal_set_tx_power_level
  //status = aci_hal_set_tx_power_level(en_high_power,pa_level);
  status = aci_hal_set_tx_power_level(0x01,0x04);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_hal_set_tx_power_level() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_hal_set_tx_power_level --> SUCCESS\r\n");
  }

  //aci_gatt_init
  //status = aci_gatt_init();
  status = aci_gatt_init();
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_init() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_init --> SUCCESS\r\n");
  }

  //aci_gap_init
  //status = aci_gap_init(role,privacy_enabled,device_name_char_len, &service_handle, &dev_name_char_handle, &appearance_char_handle);
  status = aci_gap_init(0x0F,0x00,0x07, &service_handle, &dev_name_char_handle, &appearance_char_handle);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_init() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gap_init --> SUCCESS\r\n");
  }

  uint8_t char_value_16[] = {0x42,0x6C,0x75,0x65,0x4E,0x52,0x47};

  //aci_gatt_update_char_value
  //status = aci_gatt_update_char_value(service_handle,char_handle,val_offset,char_value_length,char_value);
  status = aci_gatt_update_char_value(0x0005,0x0006,0x00,0x07,char_value_16);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_update_char_value() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_update_char_value --> SUCCESS\r\n");
  }
}


/**
 * @brief  Create a GATT DATABASE
 * @param  None.
 * @retval None.
*/
void set_database(void)
{
  uint16_t char_handle;
  uint8_t status;
  uint16_t service_handle;

  uint8_t uuid_43[2];

  COPY_UUID_16(uuid_43, 0xFF,0xF0);

  Osal_MemCpy(&service_uuid.Service_UUID_16, uuid_43, 2);

  //aci_gatt_add_service
  //status = aci_gatt_add_service(service_uuid_type,service_uuid_16,service_type,max_attribute_records, &service_handle);
  // IMPORTANT: Please make sure the Max_Attribute_Records parameter is configured exactly with the required value.
  status = aci_gatt_add_service(UUID_TYPE_16,&service_uuid,PRIMARY_SERVICE,0x06, &service_handle);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_add_service() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_add_service --> SUCCESS\r\n");
  }

  uint8_t uuid_45[2];

  COPY_UUID_16(uuid_45, 0xFF,0xF1);

  Osal_MemCpy(&char_uuid.Char_UUID_16, uuid_45, 2);

  //aci_gatt_add_char
  //status = aci_gatt_add_char(service_handle,char_uuid_type,char_uuid_16,char_value_length,char_properties,security_permissions,gatt_evt_mask,enc_key_size,is_variable, &char_handle);
  status = aci_gatt_add_char(0x000C,UUID_TYPE_16,&char_uuid,0x0014,CHAR_PROP_WRITE,ATTR_PERMISSION_NONE,GATT_NOTIFY_ATTRIBUTE_WRITE,0x07,0x00, &char_handle);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_add_char() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_add_char --> SUCCESS\r\n");
  }

  uint8_t uuid_49[2];

  COPY_UUID_16(uuid_49, 0xFF,0xF2);

  Osal_MemCpy(&char_uuid.Char_UUID_16, uuid_49, 2);

  //aci_gatt_add_char
  //status = aci_gatt_add_char(service_handle,char_uuid_type,char_uuid_16,char_value_length,char_properties,security_permissions,gatt_evt_mask,enc_key_size,is_variable, &char_handle);
  status = aci_gatt_add_char(0x000C,UUID_TYPE_16,&char_uuid,0x0014,CHAR_PROP_NOTIFY,ATTR_PERMISSION_NONE,GATT_NOTIFY_ATTRIBUTE_WRITE,0x07,0x00, &char_handle);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_add_char() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_add_char --> SUCCESS\r\n");
  }
}


/**
 * @brief  Puts the device in connectable mode
 * @param  None.
 * @retval None.
*/
void set_device_discoverable(void)
{
  uint8_t status;

  //hci_le_set_scan_response_data
  //status = hci_le_set_scan_response_data(scan_response_data_length,scan_response_data);
  status = hci_le_set_scan_response_data(0x00,NULL);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("hci_le_set_scan_response_data() failed:0x%02x\r\n", status);
  }else{
    PRINTF("hci_le_set_scan_response_data --> SUCCESS\r\n");
  }

  uint8_t local_name_56[] = {0x09,0x42,0x6C,0x75,0x65,0x4E,0x52,0x47};

  //aci_gap_set_discoverable
  //status = aci_gap_set_discoverable(advertising_type,advertising_interval_min,advertising_interval_max,own_address_type,advertising_filter_policy,local_name_length,local_name,service_uuid_length,service_uuid_list,slave_conn_interval_min,slave_conn_interval_max);
  status = aci_gap_set_discoverable(ADV_IND,0x0064,0x0064,0x00,NO_WHITE_LIST_USE,0x08,local_name_56,0x00,NULL,0x0000,0x0000);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_set_discoverable() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gap_set_discoverable --> SUCCESS\r\n");
  }
}


/**
 * @brief  Device Demo state machine
 * @param  None.
 * @retval None.
*/
void APP_Tick(void)
{

 //USER ACTION IS NEEDED
}

/* *************** BlueNRG-1 Stack Callbacks****************/




/** \endcond 
*/
