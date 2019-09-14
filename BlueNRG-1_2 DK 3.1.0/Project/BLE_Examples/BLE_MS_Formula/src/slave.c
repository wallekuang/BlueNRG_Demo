/*
  ******************************************************************************
  * @file    user.c 
  * @author  AMG - RF Application Team
  * @version V1.0.0
  * @date    21 - 09 - 2018
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
#include "OTA_btl.h"
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
Service_UUID_t service_uuid;

#define COPY_UUID_16(uuid_struct, uuid_1, uuid_0) \
do {\
  uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; \
}while(0)

#define NUM_PACKETS 500 
/**************************************************************************** */

#ifdef BLUENRG2_DEVICE
#define SLAVE_MAX_ATT_MTU DEFAULT_MAX_ATT_MTU /* Set to max only on BlueNRG-2, BLE stack v2.1 with extended data length */
#else
#define SLAVE_MAX_ATT_MTU DEFAULT_ATT_MTU
#endif 

#define  TX_OCTECTS  (SLAVE_MAX_ATT_MTU + 4)  //+4  for L2CAP header 
#define  TX_TIME    ((TX_OCTECTS +14)*8) //Don't modify it `

static Char_UUID_t char_uuid;

static uint16_t conn_handle; 

static uint16_t update_len = DEFAULT_ATT_MTU -3; 

static  uint16_t char_handle;
static   uint16_t service_handle;

volatile int slave_app_flags = SET_CONNECTABLE;

#ifndef SLAVE_INDEX

#define SLAVE_INDEX 1

#endif 

static uint16_t slave_counter = 0;
static uint8_t slave_notification[SLAVE_MAX_ATT_MTU-3] = {0}; 

static slave_address_type slave_address[MAX_SUPPORTED_SLAVES] = {{0x01, 0x00, 0x00, 0xE1, 0x80, 0x02}, \
                                                          {0x02, 0x00, 0x00, 0xE1, 0x80, 0x02}, \
                                                          {0x03, 0x00, 0x00, 0xE1, 0x80, 0x02}};


/**
 * @brief  Create a GATT DATABASE
 * @param  None.
 * @retval None.
*/
void set_database(void)
{
  uint8_t status;

  uint8_t uuid_19[2];

  COPY_UUID_16(uuid_19, 0xA0,0x01);

  Osal_MemCpy(&service_uuid.Service_UUID_16, uuid_19, 2);

  //aci_gatt_add_service
  //status = aci_gatt_add_service(service_uuid_type,service_uuid_16,service_type,max_attribute_records, &service_handle);
  // IMPORTANT: Please make sure the Max_Attribute_Records parameter is configured exactly with the required value.
  status = aci_gatt_add_service(UUID_TYPE_16,&service_uuid,PRIMARY_SERVICE,0x04, &service_handle);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_add_service() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_add_service --> SUCCESS\r\n");
  }

  uint8_t uuid_21[2];

  COPY_UUID_16(uuid_21, 0xA0,0x02);

  Osal_MemCpy(&char_uuid.Char_UUID_16, uuid_21, 2);

  //aci_gatt_add_char
  //status = aci_gatt_add_char(service_handle,char_uuid_type,char_uuid_16,char_value_length,char_properties,security_permissions,gatt_evt_mask,enc_key_size,is_variable, &char_handle);
 
  status = aci_gatt_add_char(service_handle,UUID_TYPE_16,&char_uuid,SLAVE_MAX_ATT_MTU - 3,CHAR_PROP_NOTIFY,ATTR_PERMISSION_NONE,GATT_NOTIFY_ATTRIBUTE_WRITE,0x07,0x01, &char_handle);

  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_add_char() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_add_char --> SUCCESS\r\n");
  }
 
#ifdef BLUENRG2_DEVICE
  
    status = hci_le_write_suggested_default_data_length(TX_OCTECTS,  TX_TIME);
    if (status != BLE_STATUS_SUCCESS) 
    {
      PRINTF("hci_le_write_suggested_default_data_length() failed:0x%02x\r\n", status);
    }
    else
    {
      PRINTF("hci_le_write_suggested_default_data_length --> SUCCESS\r\n");
    }
#endif 
}

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

  //uint8_t s_address[] = slave_address[SLAVE_INDEX-1]; //{0xCD,0x3C,0x00,0xE1,0x80,0x02}; {0xaa, 0x00, 0x00, 0xE1, 0x80, 0x02};
  
  uint8_t device_name[] = {0x73,0x6C,0x61,0x76,0x65,0x6D}; //slavem
  
   PRINTF("************** Slave: %d\r\n", SLAVE_INDEX);

  //Osal_MemCpy(s_address, slave_address[SLAVE_INDEX-1], 6);
  //aci_hal_write_config_data
  //status = aci_hal_write_config_data(offset,length,value);
  status = aci_hal_write_config_data(0x00,0x06,slave_address[SLAVE_INDEX-1]);
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
  status = aci_gap_init(GAP_PERIPHERAL_ROLE,0x00,sizeof(device_name), &service_handle, &dev_name_char_handle, &appearance_char_handle);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_init() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gap_init --> SUCCESS\r\n");
  }
  
  //aci_gatt_update_char_value_ext
  //status = aci_gatt_update_char_value_ext(conn_handle_to_notify,service_handle,char_handle,update_type,char_length,value_offset,value_length,value);
  status = aci_gatt_update_char_value_ext(0x0000,0x0005,0x0006,0x00,sizeof(device_name),0x0000,sizeof(device_name),device_name);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_update_char_value_ext() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_update_char_value_ext --> SUCCESS\r\n");
  }
  
   //Add database
    set_database(); 
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

  uint8_t local_name_26[] = {0x09,0x73,0x6C,0x61,0x76,0x65,0x6D}; //slavem

  //aci_gap_set_discoverable
  //status = aci_gap_set_discoverable(advertising_type,advertising_interval_min,advertising_interval_max,own_address_type,advertising_filter_policy,local_name_length,local_name,service_uuid_length,service_uuid_list,slave_conn_interval_min,slave_conn_interval_max);
  status = aci_gap_set_discoverable(ADV_IND,0x0100,0x0100,0x00,NO_WHITE_LIST_USE,0x07,local_name_26,0x00,NULL,0x0006,0x0008);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_set_discoverable() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gap_set_discoverable --> SUCCESS\r\n");
  }
}

void do_notifications()
{
  uint8_t status;
  
#if THROUGHPUT_TEST //TBR
    static uint32_t packets = 0;
    static tClockTime time, time2; 
    
    //HOST_TO_LE_32(data+16, packets);
    
    if(packets==0){
      //PRINTF("TX test start\n");
      time = Clock_Time(); 
    }
#endif		
    //if(aci_gatt_update_char_value_ext(connection_handle,  ServHandle, TXCharHandle, 1,20, 0, 20, data)==BLE_STATUS_INSUFFICIENT_RESOURCES)
    status  = aci_gatt_update_char_value_ext(conn_handle,service_handle,char_handle,0x01,update_len ,0x0000,update_len,slave_notification);//SLAVE_MAX_ATT_MTU - 3
    if(status==BLE_STATUS_INSUFFICIENT_RESOURCES)
    {
      // Radio is busy (buffer full).
      APP_SLAVE_FLAG_SET(TX_BUFFER_FULL);
    }
    else// if (status == 0)
    {
#if THROUGHPUT_TEST //TBR
    
      packets++;
      if(packets != 0 && packets%NUM_PACKETS == 0)
      {
        time2 = Clock_Time();
        tClockTime diff = time2-time;
        PRINTF("%d TX packets. Elapsed time: %d ms. App throughput: %.1f kbps.\n", NUM_PACKETS, (int)diff, (float)NUM_PACKETS*update_len*8/diff);
        time = Clock_Time();
        
      }
#else
      PRINTF("Slave: %d, value: 0x%02x\r\n", SLAVE_INDEX, slave_counter);
      
#endif 
      slave_counter +=1; 
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
 //USER ACTION IS NEEDED
  
   if(APP_SLAVE_FLAG(SET_CONNECTABLE))
  {
    set_device_discoverable();
    APP_SLAVE_FLAG_CLEAR(SET_CONNECTABLE);
  }
  else if (APP_SLAVE_FLAG(DO_EXCHANGE_CONFIG))
  {
      //aci_gatt_exchange_config
    //status = aci_gatt_exchange_config(connection_handle);
    status = aci_gatt_exchange_config(conn_handle);
    if (status != BLE_STATUS_SUCCESS) 
    {
      PRINTF("aci_gatt_exchange_config() failed: 0x%02x, conn_handle: 0x%04x\r\n", status,conn_handle);
    }else
    {
      PRINTF("aci_gatt_exchange_config --> SUCCESS\r\n");
      APP_SLAVE_FLAG_CLEAR(DO_EXCHANGE_CONFIG);
    }  
  }
  else if (APP_SLAVE_FLAG(SEND_DATA) && APP_SLAVE_FLAG(NOTIFICATIONS_ENABLED) && !APP_SLAVE_FLAG(TX_BUFFER_FULL))
  //else if(APP_SLAVE_FLAG(CONNECTED) && APP_SLAVE_FLAG(NOTIFICATIONS_ENABLED) && !APP_SLAVE_FLAG(TX_BUFFER_FULL))
  {
    slave_notification[2] = SLAVE_INDEX;
    
    //slave_value  = (uint16_t)(((uint16_t) attribute_value[0])  | (((uint16_t) attribute_value[1])  << 8)); 
    
    slave_notification[1] = (uint8_t) ((slave_counter)  >> 8);
    slave_notification[0] = (uint8_t) (slave_counter & 0xFF); 
    
    do_notifications();

  }
    
}

/* *************** BlueNRG-1 Stack Callbacks****************/




/**
 * @brief  The LE Connection Complete event indicates to both of the Hosts forming the
connection that a new connection has been created. Upon the creation of the
connection a Connection_Handle shall be assigned by the Controller, and
passed to the Host in this event. If the connection establishment fails this event
shall be provided to the Host that had issued the LE_Create_Connection command.
This event indicates to the Host which issued a LE_Create_Connection
command and received a Command Status event if the connection
establishment failed or was successful.
The Master_Clock_Accuracy parameter is only valid for a slave. On a master,
this parameter shall be set to 0x00.
 * @param  param See file bluenrg1_events.h.
 * @retval See file bluenrg1_events.h.
*/
void hci_le_connection_complete_event(uint8_t status,uint16_t connection_handle,uint8_t role,uint8_t peer_address_type,uint8_t peer_address[6],uint16_t conn_interval,uint16_t conn_latency,uint16_t supervision_timeout,uint8_t master_clock_accuracy)
{
  //USER ACTION IS NEEDED
  PRINTF("hci_le_connection_complete_event --> EVENT: 0x%04x\r\n", connection_handle);
  conn_handle = connection_handle;
  
  APP_SLAVE_FLAG_SET(CONNECTED);
  
}


/**
 * @brief  The LE Data Length Change event notifies the Host of a change to either the maximum Payload length or the maximum transmission time of Data Channel PDUs in either direction. The values reported are the maximum that will actually be used on the connection following the change. 
 * @param  param See file bluenrg1_events.h.
 * @retval See file bluenrg1_events.h.
*/
void hci_le_data_length_change_event(uint16_t connection_handle,uint16_t maxtxoctets,uint16_t maxtxtime,uint16_t maxrxoctets,uint16_t maxrxtime)
{
  //USER ACTION IS NEEDED
  
  PRINTF("hci_le_data_length_change_event --> EVENT: TX: %d, TXTime: %d, RX: %d , RXTime: %d \r\n",maxtxoctets,maxtxtime,maxrxoctets,maxrxtime);
  /* If this event is raised,  slave and master_slave device support both data length extension: increase ATT_MTU for take benefit from
     ATT packet sent on a single LL PDU packet */
  APP_SLAVE_FLAG_SET(DO_EXCHANGE_CONFIG); 
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
void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason)
{
  APP_SLAVE_FLAG_CLEAR(CONNECTED);
  /* Make the device connectable again. */
  APP_SLAVE_FLAG_SET(SET_CONNECTABLE);
  
  APP_SLAVE_FLAG_CLEAR(SEND_DATA);
  APP_SLAVE_FLAG_CLEAR(DO_EXCHANGE_CONFIG);
  
  APP_SLAVE_FLAG_CLEAR(NOTIFICATIONS_ENABLED);
  APP_SLAVE_FLAG_CLEAR(TX_BUFFER_FULL);
 
  
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
  
  //PRINTF("aci_gatt_attribute_modified_event(): attr_handle =  0x%04x\r\n\r\n",attr_handle);
  
  if(attr_handle == char_handle + 2)
  {        
    if(attr_data[0] == 0x01)
    {
      APP_SLAVE_FLAG_SET(NOTIFICATIONS_ENABLED);
      //APP_SLAVE_FLAG_SET(DO_EXCHANGE_CONFIG); //TBR
			
#ifndef BLUENRG2_DEVICE
      /* if slave is not a BlueNRG-2 device, no data length extension is supported and, as consequence,
	  hci_le_data_length_change_event() is not raised and no aci_gatt_exchange_config() is called for increasing ATT_MTU size: 
	  start notification  loop */
      APP_SLAVE_FLAG_SET(SEND_DATA);
			
#endif 
      
      PRINTF("aci_gatt_attribute_modified_event(): enabled characteristic notification\r\n");
    }
  }
}


/**
 * @brief  This event is generated in response to an Exchange MTU request. See
@ref ACI_GATT_EXCHANGE_CONFIG.
 * @param  param See file bluenrg1_events.h.
 * @retval retVal See file bluenrg1_events.h.
*/
void aci_att_exchange_mtu_resp_event(uint16_t connection_handle,uint16_t server_rx_mtu)
{
  //USER ACTION IS NEEDED
  
  /* In this demo application, the update_len is increased only for device supporting data length extension */
#ifdef BLUENRG2_DEVICE
  
  update_len = server_rx_mtu - 3; 
  
  PRINTF("aci_att_exchange_mtu_resp_event(%d) : update_len: %d --> EVENT\r\n",server_rx_mtu,update_len);
	
  /* Start notification  (if enabled) */
  APP_SLAVE_FLAG_SET(SEND_DATA);
#endif 
  
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


/*******************************************************************************
 * Function Name  : aci_gatt_tx_pool_available_event.
 * Description    : This event occurs when a TX pool available is received.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_tx_pool_available_event(uint16_t Connection_Handle,
                                      uint16_t Available_Buffers)
{       
  /* It allows to notify when at least 2 GATT TX buffers are available */
  APP_SLAVE_FLAG_CLEAR(TX_BUFFER_FULL);
} 
  
/** \endcond 
*/
