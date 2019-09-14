/*
  ******************************************************************************
  * @file    master.c 
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
#include "master.h"
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

#define SLAVE_LOCAL_NAME_LEN 6 

/* discovery procedure mode context */
typedef struct discoveryContext_s {
  uint8_t device_found_address_type;
  uint8_t device_found_address[6];
  uint16_t connection_handle; 
} discoveryContext_t;

/* Private variables ---------------------------------------------------------*/

static uint8_t LE_Event_Mask[8] = {0x1F,0x02,0x00,0x00,0x00,0x00,0x00,0x00};//0x000000000000021F
static uint16_t conn_handle; 
static Bonded_Device_Entry_t bonded_device_entry_53[MAX_NUM_BONDED_DEVICES];
static  Whitelist_Identity_Entry_t whitelist_identity_entry_62;//USER ACTION IS NEEDED: Load correct value into the structure
static uint8_t num_of_addresses; 


static discoveryContext_t discovery[1]; //ony 1 slave device to be found

static uint8_t slave_device_is_found = FALSE; 

static uint16_t slave_char_handle = 0x000F; //Expected characteristic handle for Slave device with 1 Service and 1 Characteristic with Controlle Privacy (Central Address characteristic) 

/* Slave local name: don't change it */
static uint8_t slave_local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'p','s','l','a','v','e'}; 


//pslave : [0x6576616C73700907]

volatile int app_flags = 0; 


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

  uint8_t value_2[] = {0x88,0x38,0x22,0xE1,0x80,0x02};
  
  PRINTF("***************** Controller Privacy: Master device with Fixed pin (123456)\r\n");

  //aci_hal_write_config_data
  //status = aci_hal_write_config_data(offset,length,value);
  status = aci_hal_write_config_data(0x00,0x06,value_2);
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
  status = aci_gap_init(GAP_CENTRAL_ROLE,0x02,0x07, &service_handle, &dev_name_char_handle, &appearance_char_handle);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_init() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gap_init --> SUCCESS\r\n");
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
  status = aci_gap_set_io_capability(0x02);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_set_io_capability() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gap_set_io_capability --> SUCCESS\r\n");
  }

  //aci_gap_set_authentication_requirement
  //status = aci_gap_set_authentication_requirement(bonding_mode,mitm_mode,sc_support,keypress_notification_support,min_encryption_key_size,max_encryption_key_size,use_fixed_pin,use_fixed_pin,identity_address_type);
  status = aci_gap_set_authentication_requirement(BONDING,MITM_PROTECTION_REQUIRED,SC_IS_MANDATORY,KEYPRESS_IS_NOT_SUPPORTED,0x07,0x10,USE_FIXED_PIN_FOR_PAIRING,123456,0x00); // DONOT_USE_FIXED_PIN_FOR_PAIRING
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_set_authentication_requirement() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gap_set_authentication_requirement --> SUCCESS\r\n");
  }
}


/**
 * @brief  Puts the device in scannable mode
 * @param  None.
 * @retval None.
*/
void device_scanning(void)
{
  uint8_t status;

  //aci_gap_start_general_connection_establish_proc
  //status = aci_gap_start_general_connection_establish_proc(le_scan_type,le_scan_interval,le_scan_window,own_address_type,scanning_filter_policy,filter_duplicates);
  status = aci_gap_start_general_connection_establish_proc(0x01,0x4000,0x4000,0x02,0x00,0x00);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_start_general_connection_establish_proc() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gap_start_general_connection_establish_proc --> SUCCESS\r\n");
  }
}


/**
 * @brief  Established a connection
 * @param  None.
 * @retval None.
*/
void set_connection(uint8_t device_type, uint8_t * device_address)
{
  uint8_t status;

  //uint8_t peer_address_40[] = {0x89,0xCA,0xC8,0x5C,0x0A,0x6E};

  //aci_gap_create_connection
  //status = aci_gap_create_connection(le_scan_interval,le_scan_window,peer_address_type,peer_address,own_address_type,conn_interval_min,conn_interval_max,conn_latency,supervision_timeout,minimum_ce_length,maximum_ce_length);
  status = aci_gap_create_connection(0x4000,
                                     0x4000,
                                     //discovery[0].device_found_address_type,
                                     //discovery[0].device_found_address,
                                     device_type,
                                     device_address,
                                     0x02,
                                     0x0006,
                                     0x0028,
                                     0x0000,
                                     0x03E8,
                                     0x0000,
                                     0x03E8);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_create_connection() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gap_create_connection --> SUCCESS\r\n");
  }
}


/**
 * @brief  Discovery Services, Characteristics or descriptors
 * @param  None.
 * @retval None.
*/
void discovery_services(void)
{
  uint8_t status;

  uint8_t uuid_16_51[] = {0x00,0x18};//USER ACTION IS NEEDED

  //aci_gatt_disc_primary_service_by_uuid
  //status = aci_gatt_disc_primary_service_by_uuid(connection_handle,uuid_type,uuid_16);
  status = aci_gatt_disc_primary_service_by_uuid(conn_handle,UUID_TYPE_16,(UUID_t *)uuid_16_51);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_disc_primary_service_by_uuid() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_disc_primary_service_by_uuid --> SUCCESS\r\n");
  }
}


/**
 * @brief  Read and Write Characteristics
 * @param  None.
 * @retval None.
*/
void read_write_characteristics(void)
{
  uint8_t status;

  uint8_t attribute_val_56[] = {0x01,0x00};

  //aci_gatt_write_char_desc
  //status = aci_gatt_write_char_desc(connection_handle,attr_handle,attribute_val_length,attribute_val);
  status = aci_gatt_write_char_desc(conn_handle,slave_char_handle + 2,0x02,attribute_val_56);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_write_char_desc() failed:0x%02x\r\n", status);
  }else{
    //PRINTF("aci_gatt_write_char_desc --> SUCCESS\r\n");
    PRINTF("aci_gatt_write_char_desc() --> SUCCESS; char_handle = 0x%02x\r\n", (slave_char_handle+2)); 
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
 
  if APP_FLAG(SET_CONNECTABLE)
  {
    APP_FLAG_CLEAR(SET_CONNECTABLE);
    set_connection(discovery[0].device_found_address_type,
                   discovery[0].device_found_address); 
  }
  else if APP_FLAG(WRITE_DESCRIPTOR)
  {
    APP_FLAG_CLEAR(WRITE_DESCRIPTOR);
    read_write_characteristics();
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
        
        APP_FLAG_SET(WRITE_DESCRIPTOR); 
      }
    }
  } 
  else if APP_FLAG(DO_RECONNECTION)
  {
    APP_FLAG_CLEAR(DO_RECONNECTION); 
    //aci_gap_add_devices_to_resolving_list
    //status = aci_gap_add_devices_to_resolving_list(num_of_resolving_list_entries,whitelist_identity_entry,clear_resolving_list);
    status = aci_gap_add_devices_to_resolving_list(0x01,&whitelist_identity_entry_62,0x00);
    if (status != BLE_STATUS_SUCCESS) {
      PRINTF("aci_gap_add_devices_to_resolving_list() failed:0x%02x\r\n", status);
    }else{
      PRINTF("aci_gap_add_devices_to_resolving_list --> SUCCESS\r\n");
    }
    
    set_connection(whitelist_identity_entry_62.Peer_Identity_Address_Type, whitelist_identity_entry_62.Peer_Identity_Address);
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
}

/* *************** BlueNRG-1 Stack Callbacks****************/



/*******************************************************************************
* Function Name  : Find_DeviceName.
* Description    : Extracts the device name.
* Input          : Data length.
*                  Data value
* Return         : TRUE if the local name found is the expected one, FALSE otherwise.
*******************************************************************************/
static uint8_t Find_DeviceName(uint8_t data_length, uint8_t *data_value)
{
  uint8_t index = 0;
  
  while (index < data_length) {
    /* Advertising data fields: len, type, values */
    /* Check if field is complete local name and the lenght is the expected one for BLE NEW Chat  */
    if (data_value[index+1] == AD_TYPE_COMPLETE_LOCAL_NAME) { 
      /* check if found device name is the expected one: local_name */ 
      if (memcmp(&data_value[index+1], &slave_local_name[0], SLAVE_LOCAL_NAME_LEN) == 0)
        return TRUE;
      else
        return FALSE;
    } else {
      /* move to next advertising field */
      index += (data_value[index] +1); 
    }
  }
  
  return FALSE;
}

/******************************************************************************
 * Function Name  : hci_le_advertising_report_event.
 * Description    : The LE Advertising Report event indicates that a Bluetooth device or multiple
Bluetooth devices have responded to an active scan or received some information
during a passive scan. The Controller may queue these advertising reports
and send information from multiple devices in one LE Advertising Report event..
 * Input          : See file bluenrg1_events.h.
 * Output         : See file bluenrg1_events.h.
 * Return         : See file bluenrg1_events.h.
******************************************************************************/
void hci_le_advertising_report_event(uint8_t num_reports,Advertising_Report_t advertising_report[])
{
  //PRINTF("hci_le_advertising_report_event --> EVENT\r\n");
  uint8_t evt_type = advertising_report[0].Event_Type ;
  uint8_t data_length = advertising_report[0].Length_Data;
  uint8_t bdaddr_type = advertising_report[0].Address_Type;
  
  if (!slave_device_is_found)
  {
    /* Advertising_Report contains all the expected parameters */

    uint8_t bdaddr[6];

    Osal_MemCpy(bdaddr, advertising_report[0].Address,6);
    /*  check current found device */
    if ((evt_type == SCAN_RSP) &&  Find_DeviceName(data_length, advertising_report[0].Data) )
    {
       /* store first device found:  address type and address value */
        discovery[0].device_found_address_type = bdaddr_type;
        Osal_MemCpy(discovery[0].device_found_address, bdaddr, 6);
        slave_device_is_found = TRUE;
        
        /* Connect to found slave device */
        APP_FLAG_SET(SET_CONNECTABLE);
    }
  }
     
}/* end hci_le_advertising_report_event() */



/**
 * @brief  This event is sent by the GAP to the upper layers when a procedure previously started has
been terminated by the upper layer or has completed for any other reason
 * @param  param See file bluenrg1_events.h.
 * @retval retVal See file bluenrg1_events.h.
*/
void aci_gap_proc_complete_event(uint8_t procedure_code,uint8_t status,uint8_t data_length,uint8_t data[])
{
  //USER ACTION IS NEEDED
  PRINTF("aci_gap_proc_complete_event --> EVENT\r\n");
}


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
  APP_FLAG_SET(CONNECTED);
  
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
 * @brief  This event is generated in response to a @ref ACI_ATT_FIND_BY_TYPE_VALUE_REQ
 * @param  param See file bluenrg1_events.h.
 * @retval retVal See file bluenrg1_events.h.
*/
void aci_att_find_by_type_value_resp_event(uint16_t connection_handle,uint8_t num_of_handle_pair,Attribute_Group_Handle_Pair_t attribute_group_handle_pair[])
{
  //USER ACTION IS NEEDED
  PRINTF("aci_att_find_by_type_value_resp_event --> EVENT\r\n");
}


/**
 * @brief  This event is generated when an Error Response is received from the server. The error
response can be given by the server at the end of one of the GATT discovery procedures.
This does not mean that the procedure ended with an error, but this error event is part of the
procedure itself.
 * @param  param See file bluenrg1_events.h.
 * @retval retVal See file bluenrg1_events.h.
*/
void aci_gatt_error_resp_event(uint16_t connection_handle,uint8_t req_opcode,uint16_t attribute_handle,uint8_t error_code)
{
  //USER ACTION IS NEEDED
  PRINTF("aci_gatt_error_resp_event --> connection_handle: 0x%02x, attribute_handle: 0x%02x, req_opcode: 0x%02x,  error_code: 0x%02x\r\n",connection_handle, attribute_handle, req_opcode,error_code);
}


/**
 * @brief  This event is generated when a GATT client procedure completes either with error or
successfully.
 * @param  param See file bluenrg1_events.h.
 * @retval retVal See file bluenrg1_events.h.
*/
void aci_gatt_proc_complete_event(uint16_t connection_handle,uint8_t error_code)
{
  //USER ACTION IS NEEDED
  PRINTF("aci_gatt_proc_complete_event --> EVENT\r\n");
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
  
  APP_FLAG_SET(DO_RECONNECTION); 
  APP_FLAG_CLEAR(CONNECTED);
}


/**
 * @brief  This event is generated when a notification is received from the server.
 * @param  param See file bluenrg1_events.h.
 * @retval retVal See file bluenrg1_events.h.
*/
void aci_gatt_notification_event(uint16_t connection_handle,uint16_t attribute_handle,uint8_t attribute_value_length,uint8_t attribute_value[])
{
  //USER ACTION IS NEEDED
  
  uint16_t value; 
 
  /* Receive Notifications from Slaves and send them to the Master */

  /* value = slave index */
  value = ((uint16_t) (attribute_value[0] & 0xFF)) | ((uint16_t) (attribute_value[1] <<8));
  
  PRINTF("Notification Received. Counter : %d\r\n", value);
}


/** \endcond 
*/
