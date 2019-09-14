/*
  ******************************************************************************
  * @file    slave.c 
  * @author  AMS - RF Application Team
  * @version V1.0.0
  * @date    05 - 10 - 2018
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
  * <h2><center>&copy; COPYRIGHT 2018 STMicroelectronics</center></h2>
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
#include "slave.h"
#include "osal.h"

#ifndef DEBUG
#define DEBUG 1
#endif

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define MAX_NUM_BONDED_DEVICES 1 //TBR

#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
  do {\
  	uuid_struct.uuid128[0] = uuid_0; uuid_struct.uuid128[1] = uuid_1; uuid_struct.uuid128[2] = uuid_2; uuid_struct.uuid128[3] = uuid_3; \
	uuid_struct.uuid128[4] = uuid_4; uuid_struct.uuid128[5] = uuid_5; uuid_struct.uuid128[6] = uuid_6; uuid_struct.uuid128[7] = uuid_7; \
	uuid_struct.uuid128[8] = uuid_8; uuid_struct.uuid128[9] = uuid_9; uuid_struct.uuid128[10] = uuid_10; uuid_struct.uuid128[11] = uuid_11; \
	uuid_struct.uuid128[12] = uuid_12; uuid_struct.uuid128[13] = uuid_13; uuid_struct.uuid128[14] = uuid_14; uuid_struct.uuid128[15] = uuid_15; \
	}while(0)
          
#define COPY_UUID_16(uuid_struct, uuid_1, uuid_0) \
do {\
  uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; \
}while(0)

static Service_UUID_t service_uuid;

static Char_UUID_t char_uuid;

static uint16_t conn_handle; 

static Bonded_Device_Entry_t bonded_device_entry_53[MAX_NUM_BONDED_DEVICES];
static  Whitelist_Identity_Entry_t whitelist_identity_entry_62;//USER ACTION IS NEEDED: Load correct value into the structure
static uint8_t num_of_addresses; 

static uint16_t char_handle;
static uint16_t service_handle;
static uint16_t counter = 0; 
static uint8_t slave_notification[2]; 
  
#define FIXED_PIN 123456
#define DEVICE_ID_LEN  8

//'pslave' (0x70,0x73,0x6c,0x61,0x76,0x65) is the code used at application level in order to allow slave selection from master
static uint8_t device_id[DEVICE_ID_LEN]  = {0x7,0x09,0x70,0x73,0x6c,0x61,0x76,0x65}; 

static uint8_t LE_Event_Mask[8] = {0x1F,0x02,0x00,0x00,0x00,0x00,0x00,0x00};

static volatile uint8_t update_char_value = 0; 
volatile int app_flags;


#define SLAVE_TIMER 1
static uint16_t slave_update_rate = 200;
static uint8_t slaveTimer_expired = FALSE;

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

  uint8_t value_6[] = {0x99,0x39,0x22,0xE1,0x80,0x02};
  
  
  PRINTF("***************** Controller Privacy: Slave device with Fixed pin (123456)\r\n");
  
  //aci_hal_write_config_data
  //status = aci_hal_write_config_data(offset,length,value);
  status = aci_hal_write_config_data(0x00,0x06,value_6);
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

  status = hci_le_set_event_mask(LE_Event_Mask); //It allows to enable HCI_LE_ENHANCED_CONNECTION_COMPLETE_EVENT
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("hci_le_set_event_mask() failed:0x%02x\r\n", status);
  }else{
    PRINTF("hci_le_set_event_mask --> SUCCESS\r\n");
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
  status = aci_gap_init(GAP_PERIPHERAL_ROLE,0x02,DEVICE_ID_LEN, &service_handle, &dev_name_char_handle, &appearance_char_handle);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_init() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gap_init --> SUCCESS\r\n");
  }
  
  status= aci_gatt_update_char_value_ext(0,0x0005,0x0006, 0, DEVICE_ID_LEN, 0x00,DEVICE_ID_LEN,device_id);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_update_char_value_ext() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_update_char_value_ext --> SUCCESS\r\n");
  }

  //aci_gap_clear_security_db
  //status = aci_gap_clear_security_db();
  status = aci_gap_clear_security_db();
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_clear_security_db() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gap_clear_security_db --> SUCCESS\r\n");
  }

  //aci_gap_set_io_capability
  //status = aci_gap_set_io_capability(io_capability);
  status = aci_gap_set_io_capability(0x00);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_set_io_capability() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gap_set_io_capability --> SUCCESS\r\n");
  }

  //aci_gap_set_authentication_requirement
  //status = aci_gap_set_authentication_requirement(bonding_mode,mitm_mode,sc_support,keypress_notification_support,min_encryption_key_size,max_encryption_key_size,use_fixed_pin,use_fixed_pin,identity_address_type);
  status = aci_gap_set_authentication_requirement(BONDING,MITM_PROTECTION_REQUIRED,SC_IS_SUPPORTED,KEYPRESS_IS_NOT_SUPPORTED,0x07,0x10,USE_FIXED_PIN_FOR_PAIRING,FIXED_PIN,0x00); //DONOT_USE_FIXED_PIN_FOR_PAIRING
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_set_authentication_requirement() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gap_set_authentication_requirement --> SUCCESS\r\n");
  }
}


/**
 * @brief  Create a GATT DATABASE
 * @param  None.
 * @retval None.
*/
void set_database(void)
{
  uint8_t status;

   /*
  UUIDs:
  57f03e20-cc5c-11e8-b568-0800200c9a66
  57f03e21-cc5c-11e8-b568-0800200c9a66
  */
  const uint8_t uuid_28[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x68,0xb5,0xe8,0x11,0x5c,0xcc,0x20,0x3e,0xf0,0x57};
  const uint8_t uuid_30[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x68,0xb5,0xe8,0x11,0x5c,0xcc,0x21,0x3e,0xf0,0x57};

  Osal_MemCpy(&service_uuid.Service_UUID_128, uuid_28,16);
  
  //aci_gatt_add_service
  //status = aci_gatt_add_service(service_uuid_type,service_uuid_16,service_type,max_attribute_records, &service_handle);
  // IMPORTANT: Please make sure the Max_Attribute_Records parameter is configured exactly with the required value.
  status = aci_gatt_add_service(UUID_TYPE_128,&service_uuid,PRIMARY_SERVICE,0x04, &service_handle);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_add_service() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_add_service --> SUCCESS; service_handle = 0x%02x \r\n",service_handle);
  }

  Osal_MemCpy(&char_uuid.Char_UUID_128, uuid_30, 16);

  //aci_gatt_add_char
  //status = aci_gatt_add_char(service_handle,char_uuid_type,char_uuid_16,char_value_length,char_properties,security_permissions,gatt_evt_mask,enc_key_size,is_variable, &char_handle);
  status = aci_gatt_add_char(service_handle,UUID_TYPE_128,&char_uuid,0x0014,0x1A,0x2D,GATT_NOTIFY_ATTRIBUTE_WRITE,0x10,0x01, &char_handle);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_add_char() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_add_char()--> SUCCESS; char_handle = 0x%02x \r\n",char_handle);
  }
  
  //aci_gatt_set_security_permission
  //status = aci_gatt_set_security_permission(serv_handle,attr_handle,security_permissions);
  status = aci_gatt_set_security_permission(0x000E,0x0011,0x28);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_set_security_permission() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_set_security_permission --> SUCCESS\r\n");
  }
}

void set_slave_update_timer(void)
{
  uint8_t ret; 
   /* Start the slave Timer */
  ret = HAL_VTimerStart_ms(SLAVE_TIMER, slave_update_rate);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("HAL_VTimerStart_ms() failed; 0x%02x\r\n", ret);
  } else {
    slaveTimer_expired = FALSE;
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

  uint8_t scan_response_data_34[] = {0x07,0x09,0x70,0x73,0x6C,0x61,0x76,0x65,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

  //hci_le_set_scan_response_data
  //status = hci_le_set_scan_response_data(scan_response_data_length,scan_response_data);
  status = hci_le_set_scan_response_data(0x08,scan_response_data_34);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("hci_le_set_scan_response_data() failed:0x%02x\r\n", status);
  }else{
    PRINTF("hci_le_set_scan_response_data --> SUCCESS\r\n");
  }
  
  //aci_gap_set_undirected_connectable
  //status = aci_gap_set_undirected_connectable(advertising_interval_min,advertising_interval_max,own_address_type,adv_filter_policy);
  status = aci_gap_set_undirected_connectable(0x0100,0x0200,0x02,NO_WHITE_LIST_USE);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_set_undirected_connectable(NO_WHITE_LIST_USE) failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gap_set_undirected_connectable(NO_WHITE_LIST_USE) --> SUCCESS\r\n");
  }
}


/**
 * @brief  Device Demo state machine
 * @param  None.
 * @retval None.
*/
void APP_Tick(void)
{
  uint8_t status; 
  
  if APP_FLAG(DO_SLAVE_SECURITY_REQUEST)
  {
    APP_FLAG_CLEAR(DO_SLAVE_SECURITY_REQUEST);
    //aci_gap_slave_security_req
    //status = aci_gap_slave_security_req(connection_handle);
    status = aci_gap_slave_security_req(conn_handle);
    if (status != BLE_STATUS_SUCCESS) {
      PRINTF("aci_gap_slave_security_req() failed:0x%02x\r\n", status);
    }else{
      PRINTF("aci_gap_slave_security_req --> SUCCESS\r\n");
    }
  }
  else if APP_FLAG(GET_BONDED_DEVICES)
  {
    APP_FLAG_CLEAR(GET_BONDED_DEVICES);

    //aci_gap_get_bonded_devices
    //status = aci_gap_get_bonded_devices(&num_of_addresses, bonded_device_entry_53);
    status = aci_gap_get_bonded_devices(&num_of_addresses, bonded_device_entry_53);
    if (status != BLE_STATUS_SUCCESS) {
      PRINTF("aci_gap_get_bonded_devices() failed:0x%02x\r\n", status);
    }
    else
    {
      PRINTF("aci_gap_get_bonded_devices --> SUCCESS; N: %d \r\n", num_of_addresses);
      if (num_of_addresses>=1)
      {
        whitelist_identity_entry_62.Peer_Identity_Address_Type = bonded_device_entry_53[0].Address_Type;
        
        memcpy(&(whitelist_identity_entry_62.Peer_Identity_Address[0]), &(bonded_device_entry_53[0].Address), 6);  
        if (!update_char_value) 
        {
          APP_FLAG_SET(WAIT_SERVICES_DISCOVERY); 
        }
        else
        {
          APP_FLAG_SET(DO_NOTIFICATIONS); 
        }
      }
    }
  } 
  else if APP_FLAG(DO_TERMINATE)
  {
      APP_FLAG_CLEAR(DO_TERMINATE);
     //aci_gap_terminate
      //status = aci_gap_terminate(connection_handle,reason);
      status = aci_gap_terminate(conn_handle,0x13);
      if (status != BLE_STATUS_SUCCESS) {
        PRINTF("aci_gap_terminate() failed:0x%02x\r\n", status);
      }else{
        PRINTF("aci_gap_terminate --> SUCCESS\r\n");
      }
  }                
  else if APP_FLAG(DO_CONFIGURE_WHITELIST)
  {
    APP_FLAG_CLEAR(DO_CONFIGURE_WHITELIST);
      //aci_gap_configure_whitelist
    //status = aci_gap_configure_whitelist();
    status = aci_gap_configure_whitelist();
    if (status != BLE_STATUS_SUCCESS) {
      PRINTF("aci_gap_configure_whitelist() failed:0x%02x\r\n", status);
    }else
    {
      PRINTF("aci_gap_configure_whitelist --> SUCCESS\r\n");
      
      //aci_gap_add_devices_to_resolving_list
      //status = aci_gap_add_devices_to_resolving_list(num_of_resolving_list_entries,whitelist_identity_entry,clear_resolving_list);
      status = aci_gap_add_devices_to_resolving_list(0x01,&whitelist_identity_entry_62,0x00);
      if (status != BLE_STATUS_SUCCESS) {
        PRINTF("aci_gap_add_devices_to_resolving_list() failed:0x%02x\r\n", status);
      }else{
        PRINTF("aci_gap_add_devices_to_resolving_list --> SUCCESS\r\n");
      }

      //aci_gap_set_undirected_connectable
      //status = aci_gap_set_undirected_connectable(advertising_interval_min,advertising_interval_max,own_address_type,adv_filter_policy);
      status = aci_gap_set_undirected_connectable(0x0100,0x0200,0x02,WHITE_LIST_FOR_ALL);
      if (status != BLE_STATUS_SUCCESS) {
        PRINTF("aci_gap_set_undirected_connectable(WHITE_LIST_FOR_ALL) failed:0x%02x\r\n", status);
      }else{
        PRINTF("aci_gap_set_undirected_connectable(WHITE_LIST_FOR_ALL) --> SUCCESS\r\n");
        APP_FLAG_SET(WAIT_RECONNECTION);
      }
    }
  }
  else if (APP_FLAG(DO_NOTIFICATIONS) && !APP_FLAG(TX_BUFFER_FULL))
  {

    if (slaveTimer_expired) 
    {
      slaveTimer_expired = FALSE;
      if (HAL_VTimerStart_ms(SLAVE_TIMER, slave_update_rate) != BLE_STATUS_SUCCESS)
      {
        slaveTimer_expired = TRUE;
      }
        //aci_gatt_update_char_value_ext
      //status = aci_gatt_update_char_value_ext(conn_handle_to_notify,service_handle,char_handle,update_type,char_length,value_offset,value_length,value);
      slave_notification[1] = (uint8_t) ((counter)  >> 8);
      slave_notification[0] = (uint8_t) (counter & 0xFF); 
      status = aci_gatt_update_char_value_ext(conn_handle,service_handle,char_handle,0x01,0x2,0x0000,0x2,slave_notification);
      if (status == BLE_STATUS_INSUFFICIENT_RESOURCES)
      {
        // Radio is busy (buffer full).
        APP_FLAG_SET(TX_BUFFER_FULL);
      }
      else if (status != BLE_STATUS_SUCCESS) {
        PRINTF("aci_gatt_update_char_value_ext() failed:0x%02x\r\n", status);
      }else{
        //PRINTF("aci_gatt_update_char_value_ext() OK: S0: 0x%02x, S1: 0x%02x, counter: 0x%04x\r\n", slave_notification[0], slave_notification[1], counter);
        PRINTF("Send Notification. Counter: %d\r\n", counter);
        counter+=1;
      }
    }    
  }
}

/* *************** BlueNRG-1 Stack Callbacks****************/




/**
 * @brief  The LE Enhanced Connection Complete event indicates to both of the Hosts
forming the connection that a new connection has been created. Upon the
creation of the connection a Connection_Handle shall be assigned by the
Controller, and passed to the Host in this event. If the connection establishment
fails, this event shall be provided to the Host that had issued the
LE_Create_Connection command.
If this event is unmasked and LE Connection Complete event is unmasked,
only the LE Enhanced Connection Complete event is sent when a new
connection has been completed.
This event indicates to the Host that issued a LE_Create_Connection
command and received a Command Status event if the connection
establishment failed or was successful.
The Master_Clock_Accuracy parameter is only valid for a slave. On a master,
this parameter shall be set to 0x00.
 * @param  param See file bluenrg1_events.h.
 * @retval See file bluenrg1_events.h.
*/
void hci_le_enhanced_connection_complete_event(uint8_t status,uint16_t connection_handle,uint8_t role,uint8_t peer_address_type,uint8_t peer_address[6],uint8_t local_resolvable_private_address[6],uint8_t peer_resolvable_private_address[6],uint16_t conn_interval,uint16_t conn_latency,uint16_t supervision_timeout,uint8_t master_clock_accuracy)
{
  //USER ACTION IS NEEDED
  PRINTF("hci_le_enhanced_connection_complete_event --> EVENT\r\n");
  
  conn_handle = connection_handle;
  APP_FLAG_SET(DO_SLAVE_SECURITY_REQUEST); 
  APP_FLAG_SET(CONNECTED);
  
  if APP_FLAG(WAIT_RECONNECTION)
  {
    APP_FLAG_CLEAR(WAIT_RECONNECTION); 
    APP_FLAG_SET(DO_SLAVE_SECURITY_REQUEST); 
    
  }
}


/**
 * @brief  This event is generated when the slave security request is successfully sent to the master.
 * @param  param See file bluenrg1_events.h.
 * @retval retVal See file bluenrg1_events.h.
*/
void aci_gap_slave_security_initiated_event()
{
  //USER ACTION IS NEEDED
  PRINTF("aci_gap_slave_security_initiated_event --> EVENT\r\n");
}


/**
 * @brief  This event is generated by the Security manager to the application when a passkey is
required for pairing. When this event is received, the application has to respond with the
@ref ACI_GAP_PASS_KEY_RESP command.
 * @param  param See file bluenrg1_events.h.
 * @retval retVal See file bluenrg1_events.h.
*/
void aci_gap_pass_key_req_event(uint16_t connection_handle)
{
  //USER ACTION IS NEEDED
  PRINTF("aci_gap_pass_key_req_event --> EVENT\r\n");
}


/**
 * @brief  The Encryption Change event is used to indicate that the change of the encryption
mode has been completed. The Connection_Handle will be a Connection_Handle
for an ACL connection. The Encryption_Enabled event parameter
specifies the new Encryption_Enabled parameter for the Connection_Handle
specified by the Connection_Handle event parameter. This event will occur on
both devices to notify the Hosts when Encryption has changed for the specified
Connection_Handle between two devices. Note: This event shall not be generated
if encryption is paused or resumed; during a role switch, for example.
The meaning of the Encryption_Enabled parameter depends on whether the
Host has indicated support for Secure Connections in the Secure_Connections_Host_Support
parameter. When Secure_Connections_Host_Support is
'disabled' or the Connection_Handle refers to an LE link, the Controller shall
only use Encryption_Enabled values 0x00 (OFF) and 0x01 (ON).
(See Bluetooth Specification v.4.1, Vol. 2, Part E, 7.7.8)
 * @param  param See file bluenrg1_events.h.
 * @retval See file bluenrg1_events.h.
*/
void hci_encryption_change_event(uint8_t status,uint16_t connection_handle,uint8_t encryption_enabled)
{
  //USER ACTION IS NEEDED
  PRINTF("hci_encryption_change_event --> EVENT\r\n");
}


/**
 * @brief  This event is generated when the pairing process has completed successfully or a pairing
procedure timeout has occurred or the pairing has failed. This is to notify the application that
we have paired with a remote device so that it can take further actions or to notify that a
timeout has occurred so that the upper layer can decide to disconnect the link.
 * @param  param See file bluenrg1_events.h.
 * @retval retVal See file bluenrg1_events.h.
*/
void aci_gap_pairing_complete_event(uint16_t connection_handle,uint8_t status,uint8_t reason)
{
  //USER ACTION IS NEEDED
  PRINTF("aci_gap_pairing_complete_event --> EVENT\r\n");
  
  APP_FLAG_SET(GET_BONDED_DEVICES); 
}


/**
 * @brief  This event is generated to the application by the GATT server when a client modifies any
attribute on the server, as consequence of one of the following GATT procedures:
- write without response
- signed write without response
- write characteristic value
- write long characteristic value
- reliable write.
 * @param  param See file bluenrg1_events.h.
 * @retval retVal See file bluenrg1_events.h.
*/
void aci_gatt_attribute_modified_event(uint16_t connection_handle,uint16_t attr_handle,uint16_t offset,uint16_t attr_data_length,uint8_t attr_data[])
{
  //USER ACTION IS NEEDED
  PRINTF("aci_gatt_attribute_modified_event --> EVENT\r\n");
  
  if APP_FLAG(WAIT_SERVICES_DISCOVERY)
  {
    APP_FLAG_CLEAR(WAIT_SERVICES_DISCOVERY);
    APP_FLAG_SET(DO_TERMINATE);
    update_char_value = 1; 
  }
}


/**
 * @brief  The Disconnection Complete event occurs when a connection is terminated.
The status parameter indicates if the disconnection was successful or not. The
reason parameter indicates the reason for the disconnection if the disconnection
was successful. If the disconnection was not successful, the value of the
reason parameter can be ignored by the Host. For example, this can be the
case if the Host has issued the Disconnect command and there was a parameter
error, or the command was not presently allowed, or a Connection_Handle
that didn't correspond to a connection was given.
 * @param  param See file bluenrg1_events.h.
 * @retval See file bluenrg1_events.h.
*/
void hci_disconnection_complete_event(uint8_t status,uint16_t connection_handle,uint8_t reason)
{
  //USER ACTION IS NEEDED
  PRINTF("hci_disconnection_complete_event --> EVENT\r\n");
  APP_FLAG_CLEAR(CONNECTED);
  APP_FLAG_CLEAR(DO_NOTIFICATIONS); 
  if (num_of_addresses>=1)
  {
    APP_FLAG_SET(DO_CONFIGURE_WHITELIST);
  }
}

void aci_gatt_tx_pool_available_event(uint16_t Connection_Handle,
                                      uint16_t Available_Buffers)
{       
  /* It allows to notify when at least 2 GATT TX buffers are available */
  APP_FLAG_CLEAR(TX_BUFFER_FULL);
} 

void HAL_VTimerTimeoutCallback(uint8_t timerNum)
{
  if (timerNum == SLAVE_TIMER) {
    slaveTimer_expired = TRUE;
  }
}
/** \endcond 
*/
