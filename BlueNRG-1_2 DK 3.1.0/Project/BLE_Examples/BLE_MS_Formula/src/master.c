/*
  ******************************************************************************
  * @file    master.c 
  * @author  AMS - RF Application Team
  * @version V1.1.0
  * @date    28 - September - 2018
  * @brief   Application functions x Master device of Master_Slave device
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
#include "master_slave.h"
#include "osal.h"
#include <math.h>
#include "ble_utils.h"

#ifndef DEBUG
#define DEBUG 1 //TBR
#endif

/* Enable connection update request to be sent to Master device after connection */
#define ENABLE_CONNECTION_UPDATE 1 
#define SUPERVISION_TIMEOUT 1000

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/* ------------------ User input parameters ------------------------ */
#ifndef MASTER_ADDRESS
#define MASTER_ADDRESS {0xBB,0xAA,0x00,0xE1,0x80,0x02}
#endif 

#ifndef MASTER_DEVICE_NAME
#define MASTER_DEVICE_NAME {'B', 'l', 'u', 'e', 'N', 'R', 'G', 'x','2'}
#endif 

/* -------------- Discovery, Connection parameters ----------------- */
#define SLAVE_LOCAL_NAME_LEN 6 //advscan

#define SCAN_INTERVAL_MS 300.0 
#define SCAN_WINDOW_MS   300.0
#define CONNECTION_INTERVAL_MS 15 
#define CE_LENGTH_MS 7.5 //Don't change it

/* Master device output connection parameters in internal time units (values to be used on BLE APIs) */
uint8_t Connection_Interval = (int) (CONNECTION_INTERVAL_MS/1.25); 
uint8_t CE_Length = (int) (CE_LENGTH_MS/0.625);  //<------ Time = N * 0.625 ---> N = Time/0.625
uint16_t Scan_Interval = (int) (SCAN_INTERVAL_MS/0.625);
uint16_t Scan_Window = (int) (SCAN_WINDOW_MS/0.625);

/* discovery procedure mode context */
typedef struct discoveryContext_s {
  uint8_t device_found_address_type;
  uint8_t device_found_address[6];
  uint16_t connection_handle; 
} discoveryContext_t;

typedef struct {
   uint8_t Identifier;
   uint16_t L2CAP_Length;
   uint16_t Interval_Min;
   uint16_t Interval_Max;
   uint16_t Slave_Latency;
   uint16_t Timeout_Multiplier;
}L2CAP_response_t;

/* Private variables ---------------------------------------------------------*/

static discoveryContext_t discovery[1]; //ony 1 slave device to be found

static L2CAP_response_t L2CAP_response;

static uint8_t slave_device_is_found = FALSE; 

volatile int app_flags = 0; 

/* Slave local name: don't change it */
static uint8_t slave_local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'a','d','v','s','c','a','n'}; 

          
/******************************************************************************
 * Function Name  : device_initialization.
 * Description    : Init a BlueNRG device.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void device_initialization(void)
{
  uint16_t gap_service_handle;
  uint16_t dev_name_char_handle;
  uint16_t appearance_char_handle;
  uint8_t status = BLE_STATUS_SUCCESS;

  uint8_t master_address[] = MASTER_ADDRESS; 
  
  uint8_t device_name[] = MASTER_DEVICE_NAME; 

  PRINTF("************** Master device \r\n");
  //aci_hal_write_config_data
  //status = aci_hal_write_config_data(offset,length,value);
  status = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN,master_address);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_hal_write_config_data() failed:0x%02x\r\n", status);
    return;
  }else{
    PRINTF("aci_hal_write_config_data --> SUCCESS\r\n");
  }

  //aci_hal_set_tx_power_level
  //status = aci_hal_set_tx_power_level(en_high_power,pa_level);
  status = aci_hal_set_tx_power_level(0x01,0x04);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_hal_set_tx_power_level() failed:0x%02x\r\n", status);
    return;
  }else{
    PRINTF("aci_hal_set_tx_power_level --> SUCCESS\r\n");
  }

  //aci_gatt_init
  //status = aci_gatt_init();
  status = aci_gatt_init();
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_init() failed:0x%02x\r\n", status);
    return;
  }else{
    PRINTF("aci_gatt_init --> SUCCESS\r\n");
  }

  //aci_gap_init
  status = aci_gap_init(GAP_CENTRAL_ROLE,0x00,sizeof(device_name), &gap_service_handle, &dev_name_char_handle, &appearance_char_handle);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_init() failed:0x%02x\r\n", status);
    return;
  }else{
    PRINTF("aci_gap_init --> SUCCESS\r\n");
  }

  status = aci_gatt_update_char_value_ext(0,gap_service_handle,dev_name_char_handle,0,sizeof(device_name),0x00,sizeof(device_name),device_name);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_update_char_value_ext() failed:0x%02x\r\n", status);
    return;
  }else{
    PRINTF("aci_gatt_update_char_value_ext() --> SUCCESS\r\n");
  }
  
  //hci_le_set_scan_response_data
  //status = hci_le_set_scan_response_data(scan_response_data_length,scan_response_data);
  status = hci_le_set_scan_response_data(0x00,NULL);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("hci_le_set_scan_response_data() failed:0x%02x\r\n", status);
  }else{
    PRINTF("hci_le_set_scan_response_data --> SUCCESS\r\n");
  }
  
  /* Starts discovery procedure */
  status = device_scanning();
  if (status == BLE_STATUS_SUCCESS) 
  {
    APP_FLAG_SET(DO_SLAVES_CONNECTION);
  }
  
} /* end device_initialization() */


/******************************************************************************
 * Function Name  : set_database.
 * Description    : Create a GATT DATABASE.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void set_database(void)
{
}


/******************************************************************************
 * Function Name  : set_device_discoverable.
 * Description    : Puts the device in connectable mode.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void set_device_discoverable(void)
{
}

/******************************************************************************
 * Function Name  : device_scanning.
 * Description    : Puts the device in scannable mode.
 * Input          : None.
 * Output         : None.
 * Return         : Status code.
******************************************************************************/
uint8_t device_scanning(void)
{
  uint8_t status;

  //aci_gap_start_general_discovery_proc
  //status = aci_gap_start_general_discovery_proc(le_scan_interval,le_scan_window,own_address_type,filter_duplicates);
  status = aci_gap_start_general_discovery_proc(Scan_Interval,
                                                Scan_Window,
                                                0x00,
                                                0x00);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_start_general_discovery_proc() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gap_start_general_discovery_proc --> SUCCESS\r\n");
   
  }
  return (status);
}


/******************************************************************************
 * Function Name  : set_connection.
 * Description    : Established a connection.
 * Input          : None.
 * Output         : None.
 * Return         : Status.
******************************************************************************/
uint8_t set_connection(void)
{
  uint8_t status;

  
  //aci_gap_create_connection
  //status = aci_gap_create_connection(le_scan_interval,le_scan_window,peer_address_type,peer_address,own_address_type,conn_interval_min,conn_interval_max,conn_latency,supervision_timeout,minimum_ce_length,maximum_ce_length);
  status = aci_gap_create_connection(Scan_Interval,
                                     Scan_Window,
                                     discovery[0].device_found_address_type,
                                     discovery[0].device_found_address,
                                     0x00,
                                     Connection_Interval,
                                     Connection_Interval,
                                     0x0000,
                                     (int) (Connection_Interval * 1.25), //TBR SUPERVISION_TIMEOUT
                                     CE_Length,
                                     CE_Length);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_create_connection() failed:0x%02x\r\n", status);
  }else{
    APP_FLAG_SET(WAIT_SLAVE_CONNECTION); 
    PRINTF("aci_gap_create_connection --> SUCCESS\r\n");
  }
  return (status); 
}


/******************************************************************************
 * Function Name  : write_characteristic_.
 * Description    : Write Characteristics.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void write_characteristic_descriptor(uint16_t Connection_Handle)
{
  uint8_t status;

  uint8_t attribute_val_133[] = {0x01,0x00};

  //aci_gatt_write_char_desc
  //status = aci_gatt_write_char_desc(connection_handle,attr_handle,attribute_val_length,attribute_val);
  status = aci_gatt_write_char_desc(Connection_Handle,
                                    0x000F,
                                    0x02,
                                    attribute_val_133);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_write_char_desc() failed:0x%02x\r\n", status);
    
  }else{
    PRINTF("aci_gatt_write_char_desc --> SUCCESS\r\n");
    APP_FLAG_CLEAR(WAIT_SLAVE_CONNECTION); //DO after enabling notification
  }
  
  APP_FLAG_CLEAR(NOTIFICATIONS_ENABLED); //Clear notification flag 
}


/******************************************************************************
 * Function Name  : app_tick.
 * Description    : Device Demo state machine.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void APP_Tick(void)
{
  uint8_t status; 
  
 if APP_FLAG(SET_CONNECTABLE) 
  {
    /* A slave has been found: connect to it */
    status = set_connection();
    if (status == 0)
    {
      APP_FLAG_CLEAR(SET_CONNECTABLE);
     
    }
  }
  else if APP_FLAG(NOTIFICATIONS_ENABLED)
  {
    /* Enable slave device characteristic notification */
    write_characteristic_descriptor(discovery[0].connection_handle);
  }
 else if APP_FLAG(CONN_PARAM_UPD_SENT)
 {
   /* Sent L2CAP response */
    status = aci_l2cap_connection_parameter_update_resp(discovery[0].connection_handle,
                                                        L2CAP_response.Interval_Min,
                                                        L2CAP_response.Interval_Max,
                                                        L2CAP_response.Slave_Latency,
                                                        L2CAP_response.Timeout_Multiplier,
                                                        CE_Length,
                                                        CE_Length,
                                                        L2CAP_response.Identifier,
                                                        0x01); //Accept
    if (status == 0)
    {
      PRINTF("aci_l2cap_connection_parameter_update_resp() OK \r\n");
      APP_FLAG_CLEAR(CONN_PARAM_UPD_SENT);
    }
    else 
    {
      //PRINTF("aci_l2cap_connection_parameter_update_resp() issue, Status: 0x%02x\r\n", status);
      
      PRINTF("aci_l2cap_connection_parameter_update_resp() issue, Status: 0x%02x\r\n", status);
    }
 }
    
} /* end APP_Tick() */
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
    if ((evt_type == ADV_IND) &&  Find_DeviceName(data_length, advertising_report[0].Data) )
    {
       /* store first device found:  address type and address value */
        discovery[0].device_found_address_type = bdaddr_type;
        Osal_MemCpy(discovery[0].device_found_address, bdaddr, 6);
        slave_device_is_found = TRUE;
    }
  }
     
}/* end hci_le_advertising_report_event() */


/******************************************************************************
 * Function Name  : aci_gap_proc_complete_event.
 * Description    : This event is sent by the GAP to the upper layers when a procedure previously started has
been terminated by the upper layer or has completed for any other reason.
 * Input          : See file bluenrg1_events.h.
 * Output         : See file bluenrg1_events.h.
 * Return         : See file bluenrg1_events.h.
******************************************************************************/
void aci_gap_proc_complete_event(uint8_t procedure_code,uint8_t status,uint8_t data_length,uint8_t data[])
{
  if (APP_FLAG(DO_SLAVES_CONNECTION)) 
  {
    /* aci_gap_proc_complete_event triggererd as end of create connection procedure */
    /* Connection as Master to discovered Slaves devices: Nothing to be done */
    PRINTF("-- END OF MASTER CREATE CONNECTION PROCEDURE: number of found slaves: %d\r\n", slave_device_is_found);
    if (!slave_device_is_found)
    {
      /* restart discovery */
      status = device_scanning();
      if (status != BLE_STATUS_SUCCESS) 
      {
        APP_FLAG_CLEAR(DO_SLAVES_CONNECTION); 
      }
    }
    else
    {
      APP_FLAG_CLEAR(DO_SLAVES_CONNECTION);
      
      /* Connect to found slave device */
      APP_FLAG_SET(SET_CONNECTABLE);
    }
  }
}/* end aci_gap_proc_complete_event() */


/******************************************************************************
 * Function Name  : hci_le_connection_complete_event.
 * Description    : The LE Connection Complete event indicates to both of the Hosts forming the
connection that a new connection has been created. Upon the creation of the
connection a Connection_Handle shall be assigned by the Controller, and
passed to the Host in this event. If the connection establishment fails this event
shall be provided to the Host that had issued the LE_Create_Connection command.
This event indicates to the Host which issued a LE_Create_Connection
command and received a Command Status event if the connection
establishment failed or was successful.
The Master_Clock_Accuracy parameter is only valid for a slave. On a master,
this parameter shall be set to 0x00..
 * Input          : See file bluenrg1_events.h.
 * Output         : See file bluenrg1_events.h.
 * Return         : See file bluenrg1_events.h.
******************************************************************************/
void hci_le_connection_complete_event(uint8_t status,uint16_t connection_handle,uint8_t role,uint8_t peer_address_type,uint8_t peer_address[6],uint16_t conn_interval,uint16_t conn_latency,uint16_t supervision_timeout,uint8_t master_clock_accuracy)
{  
  if (status==0x00)
  {
    if (APP_FLAG(WAIT_SLAVE_CONNECTION)) /* CONNECTED as Master with Slaves devices */
    {
      PRINTF("--> CONNECTED as Master with one of the Discovered Slaves device: connection handle : 0x%2x\r\n",connection_handle);

      discovery[0].connection_handle  = connection_handle; 
#if !(ENABLE_CONNECTION_UPDATE == 1)
      
      APP_FLAG_SET(NOTIFICATIONS_ENABLED);
#endif 
    }
                  
  } /* end if (Status == 0) */
  else
  {
     PRINTF("--> MASTER_SLAVE connection ERROR with Slave!!! \r\n");
  }
             
}/* hci_le_connection_complete_event() */


/******************************************************************************
 * Function Name  : aci_gatt_proc_complete_event.
 * Description    : This event is generated when a GATT client procedure completes either with error or
successfully..
 * Input          : See file bluenrg1_events.h.
 * Output         : See file bluenrg1_events.h.
 * Return         : See file bluenrg1_events.h.
******************************************************************************/
void aci_gatt_proc_complete_event(uint16_t connection_handle,uint8_t error_code)
{
  PRINTF("aci_gatt_proc_complete_event --> EVENT\r\n");
}    

/******************************************************************************
 * Function Name  : aci_gatt_notification_event.
 * Description    : This event is generated when a notification is received from the server..
 * Input          : See file bluenrg1_events.h.
 * Output         : See file bluenrg1_events.h.
 * Return         : See file bluenrg1_events.h.
******************************************************************************/
void aci_gatt_notification_event(uint16_t connection_handle,uint16_t attribute_handle,uint8_t attribute_value_length,uint8_t attribute_value[])
{
  uint8_t slave_index; 
 
  /* Receive Notifications from Slaves and send them to the Master */

  /* value = slave index */
  slave_index = attribute_value[2]; 
  
  PRINTF("Notification of %d bytes from Slave: %d, Connection handle: 0x%02x \r\n", attribute_value_length, slave_index,  connection_handle); 
}


/******************************************************************************
 * Function Name  : aci_l2cap_connection_update_resp_event.
 * Description    : This event is generated when the master responds to the connection update request packet
with a connection update response packet..
 * Input          : See file bluenrg1_events.h.
 * Output         : See file bluenrg1_events.h.
 * Return         : See file bluenrg1_events.h.
******************************************************************************/
void aci_l2cap_connection_update_resp_event(uint16_t connection_handle,uint16_t result)
{
  //PRINTF("aci_l2cap_connection_update_resp_event --> EVENT\r\n");
}


/******************************************************************************
 * Function Name  : aci_l2cap_connection_update_req_event.
 * Description    : This event is generated when the master receive a connection update request packet
 * from a slave device
 * Input          : See file bluenrg1_events.h.
 * Output         : See file bluenrg1_events.h.
 * Return         : See file bluenrg1_events.h.
******************************************************************************/
void aci_l2cap_connection_update_req_event(uint16_t Connection_Handle,
                                           uint8_t Identifier,
                                           uint16_t L2CAP_Length,
                                           uint16_t Interval_Min,
                                           uint16_t Interval_Max,
                                           uint16_t Slave_Latency,
                                           uint16_t Timeout_Multiplier)
{
  
#if ENABLE_CONNECTION_UPDATE == 1
  
  if (Connection_Handle == discovery[0].connection_handle)
  {
      /* Store L2CAp parameters */
      L2CAP_response.Identifier = Identifier; 
      L2CAP_response.L2CAP_Length = L2CAP_Length;
      L2CAP_response.Interval_Min = Interval_Min;
      L2CAP_response.Interval_Max = Interval_Max; 
      L2CAP_response.Slave_Latency = Slave_Latency;
      L2CAP_response.Timeout_Multiplier = Timeout_Multiplier;
      /* Set state for sending L2CAP response */
      APP_FLAG_SET(CONN_PARAM_UPD_SENT);
    }
#endif 
  
}

/******************************************************************************
 * Function Name  : aci_gatt_attribute_modified_event.
 * Description    : This event is generated to the application by the GATT server when a client modifies any
attribute on the server, as consequence of one of the following GATT procedures:
- write without response
- signed write without response
- write characteristic value
- write long characteristic value
- reliable write..
 * Input          : See file bluenrg1_events.h.
 * Output         : See file bluenrg1_events.h.
 * Return         : See file bluenrg1_events.h.
******************************************************************************/
void aci_gatt_attribute_modified_event(uint16_t connection_handle,uint16_t attr_handle,uint16_t offset,uint16_t attr_data_length,uint8_t attr_data[])
{
  PRINTF("aci_gatt_attribute_modified_event --> EVENT\r\n");
}


/******************************************************************************
 * Function Name  : hci_disconnection_complete_event.
 * Description    : The Disconnection Complete event occurs when a connection is terminated.
The status parameter indicates if the disconnection was successful or not. The
reason parameter indicates the reason for the disconnection if the disconnection
was successful. If the disconnection was not successful, the value of the
reason parameter can be ignored by the Host. For example, this can be the
case if the Host has issued the Disconnect command and there was a parameter
error, or the command was not presently allowed, or a Connection_Handle
that didn't correspond to a connection was given..
 * Input          : See file bluenrg1_events.h.
 * Output         : See file bluenrg1_events.h.
 * Return         : See file bluenrg1_events.h.
******************************************************************************/
void hci_disconnection_complete_event(uint8_t status,uint16_t connection_handle,uint8_t reason)
{
  PRINTF("hci_disconnection_complete_event --> EVENT\r\n");
}

/******************************************************************************
 * Function Name  : hci_le_connection_update_complete_event.
 * Description    : The LE Connection Update Complete event is used to indicate that the Controller
process to update the connection has completed.
On a slave, if no connection parameters are updated, then this event shall not be issued.
On a master, this event shall be issued if the Connection_Update command was sent..
 * Input          : See file bluenrg1_events.h.
 * Output         : See file bluenrg1_events.h.
 * Return         : See file bluenrg1_events.h.
******************************************************************************/
void hci_le_connection_update_complete_event(uint8_t Status,
                                             uint16_t Connection_Handle,
                                             uint16_t Conn_Interval,
                                             uint16_t Conn_Latency,
                                             uint16_t Supervision_Timeout)
{
#if (ENABLE_CONNECTION_UPDATE == 1) 
  if (Status == 0)
  {
    PRINTF("hci_le_connection_update_complete_event() OK \r\n");
    APP_FLAG_SET(NOTIFICATIONS_ENABLED); 

  }
  else 
  {
    PRINTF("hci_le_connection_update_complete_event() issue, Status: 0x%02x\r\n", Status);
  }
#endif
}

/** \endcond 
*/
