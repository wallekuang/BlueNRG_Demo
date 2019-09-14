/*
  ******************************************************************************
  * @file    master_slave.c
  * @author  AMS - RF Application Team
  * @version V1.1.0
  * @date    28 - September - 2018
  * @brief   Application functions x Master_Slave device using multiple connection
  *          formula 
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

/* Enable connection update request to be dent to Master device after connection */
#define ENABLE_CONNECTION_UPDATE 1 

/* Do scan first */
#define SCAN_FIRST  1 

#define MASTER_SLAVE_MAX_NUM_MASTERS 2 
   
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
   
#define SLAVES_LOCAL_NAME_LEN 6 
#define MAX_SLAVES_NUMBER 8 
   
#define COPY_UUID_16(uuid_struct, uuid_1, uuid_0) \
do {\
  uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; \
}while(0)

#define SUPERVISION_TIMEOUT 1000

/* SDK 3.0.0 or later: BlueNRG-2, BLE stack v2.1x (extended data length is supported) */
#ifdef BLUENRG2_DEVICE 

/* Charactestic length */
#define CHAR_LEN (DEFAULT_MAX_ATT_MTU - 3)

/* Extended data length parameters */
#define TX_OCTECTS  (DEFAULT_MAX_ATT_MTU + 4)
#define TX_TIME     ((TX_OCTECTS +14)*8)

#else 
/* Charactestic length */
#define CHAR_LEN (DEFAULT_ATT_MTU - 3)
#endif

/* discovery procedure mode context */
typedef struct discoveryContext_s {
  uint8_t device_found_address_type;
  uint8_t device_found_address[6];
  uint16_t connection_handle; 
} discoveryContext_t;

/* Private variables ---------------------------------------------------------*/
static uint16_t char_handle;
static uint16_t service_handle;

static Service_UUID_t service_uuid;
static Char_UUID_t char_uuid;
  
static discoveryContext_t discovery[MAX_SLAVES_NUMBER];

static uint8_t discovery_counter = 0; 
static uint8_t actual_discovered_slave_devices = 0; 

static uint8_t slave_notification_length; 
static uint8_t slave_index; 
static uint16_t slave_value;

static uint8_t disc_procedure_is_ongoing = FALSE;
static uint8_t connection_slaves_counter = 0;
static uint8_t disc_to_be_checked = FALSE;
static uint16_t master_connection_handle[MASTER_SLAVE_MAX_NUM_MASTERS]; /* master_slave could be slave up of 2 devices */
static uint8_t connection_masters_counter = 0;

volatile int app_flags = 0; 

/* Slaves local names */
static uint8_t slaves_local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'s','l','a','v','e','m'}; 

/**
 * @brief Multiple connection parameters variable
 */
static Multiple_Connection_type MS_Connection_Parameters; 

/**
* @brief Input parameters x Multiple connection formula:
* Default values are used if not defined
*/
uint8_t num_masters = MASTER_SLAVE_NUM_MASTERS; 
uint8_t num_slaves = MASTER_SLAVE_NUM_SLAVES;
float scan_window = MASTER_SLAVE_SCAN_WINDOW;
float sleep_time = MASTER_SLAVE_SLEEP_TIME;

static uint16_t Rx_Mtu[2] = {DEFAULT_ATT_MTU,DEFAULT_ATT_MTU};

static uint16_t Max_Tx_Octets[2] = {DEFAULT_ATT_MTU + 4, DEFAULT_ATT_MTU + 4};

static uint8_t master_char_value[CHAR_LEN]; 

/******************************************************************************
 * Function Name  : Print_Anchor_Period.
 * Description    : Print Device Ancor Period
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/  
void Print_Anchor_Period(void)
{
  uint8_t Status; 
  uint32_t Anchor_Period;
  uint32_t Max_Free_Slot;
  
  Status =  aci_hal_get_anchor_period(&Anchor_Period,
                                      &Max_Free_Slot);
  if (Status == 0)
  {
      PRINTF("Anchor Period = %.3f ms, Max Free Slot = %.3f ms\r\n", Anchor_Period*0.625, Max_Free_Slot*0.625);
  }
}
          
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

  uint8_t master_slave_address[] = {0xAA,0xAA,0x00,0xE1,0x80,0x02};
  
  uint8_t device_name[] = {0x61,0x64,0x76,0x73,0x63,0x61,0x6E};

  PRINTF("************** Master_Slave device \r\n");
  
  //aci_hal_write_config_data
  //status = aci_hal_write_config_data(offset,length,value);
  status = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN,master_slave_address);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_hal_write_config_data() failed:0x%02x\r\n", status);
    APP_FLAG_SET(APP_ERROR);
    return;
  }else{
    PRINTF("aci_hal_write_config_data --> SUCCESS\r\n");
  }

  //aci_hal_set_tx_power_level
  //status = aci_hal_set_tx_power_level(en_high_power,pa_level);
  status = aci_hal_set_tx_power_level(0x01,0x04);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_hal_set_tx_power_level() failed:0x%02x\r\n", status);
    APP_FLAG_SET(APP_ERROR);
    return;
  }else{
    PRINTF("aci_hal_set_tx_power_level --> SUCCESS\r\n");
  }

  //aci_gatt_init
  //status = aci_gatt_init();
  status = aci_gatt_init();
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_init() failed:0x%02x\r\n", status);
    APP_FLAG_SET(APP_ERROR);
    return;
  }else{
    PRINTF("aci_gatt_init --> SUCCESS\r\n");
  }

  //aci_gap_init
  status = aci_gap_init(GAP_PERIPHERAL_ROLE|GAP_CENTRAL_ROLE,0x00,0x07, &gap_service_handle, &dev_name_char_handle, &appearance_char_handle);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_init() failed:0x%02x\r\n", status);
    APP_FLAG_SET(APP_ERROR);
    return;
  }else{
    PRINTF("aci_gap_init --> SUCCESS\r\n");
  }

  status = aci_gatt_update_char_value_ext(0,gap_service_handle,dev_name_char_handle,0,0x07,0x00,0x07,device_name); 
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_update_char_value_ext() failed:0x%02x\r\n", status);
    APP_FLAG_SET(APP_ERROR);
    return;
  }else{
    PRINTF("aci_gatt_update_char_value_ext() --> SUCCESS\r\n");
  }
  
  status = hci_le_set_scan_response_data(0x00,NULL);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("hci_le_set_scan_response_data() failed:0x%02x\r\n", status);
    APP_FLAG_SET(APP_ERROR);
  }else{
    PRINTF("hci_le_set_scan_response_data --> SUCCESS\r\n");
  }
  
   /* ********************** Define Master_Slave database ************************* */
if (num_masters >0)
  {
    //Add database
    set_database(); 
  }
  
  /* Extended data length x supporting  increased ATT_MTU size on a single LL PDU packet (BlueNRG-2, BLE stack v2.1 or later) */
#ifdef BLUENRG2_DEVICE  /* NOTE: SDK 3.0.0 or later includes BLE stack v2.1x which supports BlueNRG-2, extended data length */
    status = hci_le_write_suggested_default_data_length(TX_OCTECTS, TX_TIME);
    if (status != BLE_STATUS_SUCCESS) {
      PRINTF("hci_le_write_suggested_default_data_length() failed:0x%02x\r\n", status);
      APP_FLAG_SET(APP_ERROR);
    }else{
      PRINTF("hci_le_write_suggested_default_data_length() --> SUCCESS\r\n");
    }
#endif 
          
  /* **********************  Call Multiple connections parameters formula ***********************/
  status = GET_Master_Slave_device_connection_parameters(num_masters,
                                                      num_slaves,
                                                      scan_window,
                                                      sleep_time,
                                                      &MS_Connection_Parameters);
  if (status !=0)
  {
    PRINTF("GET_Master_Slave_device_connection_parameters() failure: %d\r\n", status);  
    return;
  }
  
#if DEBUG ==1 
  
  PRINTF("****** Input Connection Parameters *******************************\r\n");
  PRINTF("\r\n");
  PRINTF("Num of Masters: %d\r\n", num_masters);
  PRINTF("Num of Slaves: %d\r\n", num_slaves);
  PRINTF("Minimal Scan Window: %.2f ms\r\n", scan_window);
  PRINTF("Sleep time: %.2f ms\r\n", sleep_time);
  PRINTF("\r\n");
  
  PRINTF("****** Output Connection Parameters ******************************\r\n");
  PRINTF("\r\n");
  PRINTF("Anchor Period Length: %.2f ms\r\n", MS_Connection_Parameters.AnchorPeriodLength);
  PRINTF("\r\n");
  PRINTF("****** BLE APIs Connection Parameters: BLE time units/(ms)********\r\n");
  PRINTF("\r\n");
  PRINTF("Scan Window: %d (%.2f ms)\r\n", MS_Connection_Parameters.Scan_Window, MS_Connection_Parameters.Scan_Window *0.625);
  PRINTF("Connection Interval: %d (%.2f ms)\r\n", MS_Connection_Parameters.Connection_Interval,MS_Connection_Parameters.Connection_Interval * 1.25);
  PRINTF("Scan Interval: %d (%.2f ms)\r\n", MS_Connection_Parameters.Scan_Interval, MS_Connection_Parameters.Scan_Interval * 0.625);
  PRINTF("Advertising Interval: %d (%.2f ms)\r\n", MS_Connection_Parameters.Advertising_Interval, MS_Connection_Parameters.Advertising_Interval * 0.625);
  PRINTF("CE Event Length: %d (%.2f ms)\r\n", MS_Connection_Parameters.CE_Length, MS_Connection_Parameters.CE_Length * 0.625);
  
  //PRINTF("Connection bandwidth: %.2f \r\n", (1000*PACKETS_PER_CI*20*8)/Connection_Interval_ms);
  PRINTF("******************************************************************\r\n");
#endif 

  /* ********************** Start Simultaneously Scanning Advertising ************************* */
  
#if (SCAN_FIRST ==1)
  
  Print_Anchor_Period();
  /* Starts discovery procedure */
  status = device_scanning();
  Print_Anchor_Period();
  /* Enter in Discoverable Mode with Name = 'advscan' (Add prefix 0x09 to indicate the AD type Name) */
  if (num_masters > 0)
  {
    APP_FLAG_SET(SET_CONNECTABLE); 
    set_device_discoverable();
  }
#else
  Print_Anchor_Period();
  /* Enter in Discoverable Mode with Name = 'advscan' (Add prefix 0x09 to indicate the AD type Name) */
  if (num_masters > 0)
  {
    APP_SET_FLAG(SET_CONNECTABLE); 
    set_device_discoverable();
  }
  Print_Anchor_Period();
  /* Starts discovery procedure */
  status = device_scanning();
#endif 
  
  if (status == BLE_STATUS_SUCCESS) 
  {
    disc_procedure_is_ongoing = TRUE; 
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
  uint8_t status = BLE_STATUS_SUCCESS;

  uint8_t uuid[2];
  
  COPY_UUID_16(uuid, 0xA0,0x01);

  Osal_MemCpy(&service_uuid.Service_UUID_16, uuid, 2);

  //aci_gatt_add_service
  //status = aci_gatt_add_service(service_uuid_type,service_uuid_type,service_type,max_attribute_records, &service_handle);
  status = aci_gatt_add_service(UUID_TYPE_16,&service_uuid,PRIMARY_SERVICE,0x04, &service_handle);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_add_service() failed:0x%02x\r\n", status);
    APP_FLAG_SET(APP_ERROR);
  }else{
    PRINTF("aci_gatt_add_service --> SUCCESS\r\n");
  }

  COPY_UUID_16(uuid, 0xA0,0x02);

  Osal_MemCpy(&char_uuid.Char_UUID_16, uuid, 2);

  //aci_gatt_add_char
  //status = aci_gatt_add_char(service_handle,char_uuid_type,char_uuid_type,char_value_length,char_properties,security_permissions,gatt_evt_mask,enc_key_size,is_variable, &char_handle);
  status = aci_gatt_add_char(service_handle,UUID_TYPE_16,&char_uuid,CHAR_LEN,CHAR_PROP_NOTIFY,ATTR_PERMISSION_NONE,GATT_NOTIFY_ATTRIBUTE_WRITE,0x07,0x01, &char_handle);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_add_char() failed:0x%02x\r\n", status);
    APP_FLAG_SET(APP_ERROR);
  }else{
    PRINTF("aci_gatt_add_char --> SUCCESS\r\n");
  }
  
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
  uint8_t status;
  uint8_t Local_Name[] = {AD_TYPE_COMPLETE_LOCAL_NAME, 0x61, 0x64, 0x76, 0x73, 0x63, 0x61,0x6e}; //advscan {0x61,0x64,0x76,0x73,0x63,0x61,0x6E};

  Print_Anchor_Period(); 
  //aci_gap_set_discoverable
  //status = aci_gap_set_discoverable(advertising_type,advertising_interval_min,advertising_interval_max,own_address_type,advertising_filter_policy,local_name_length,local_name,service_uuid_length,service_uuid_length,slave_conn_interval_min,slave_conn_interval_max);
  status = aci_gap_set_discoverable(ADV_IND,
                                    MS_Connection_Parameters.Advertising_Interval,
                                    MS_Connection_Parameters.Advertising_Interval,
                                    0x00,
                                    NO_WHITE_LIST_USE,
                                    sizeof(Local_Name),//0x08
                                    Local_Name,
                                    0x00,
                                    NULL,
                                    0x0000,
                                    0x0000);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_set_discoverable() failed:0x%02x\r\n", status);
    APP_FLAG_SET(APP_ERROR);
  }else{
    PRINTF("aci_gap_set_discoverable --> SUCCESS\r\n");
  }
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
  status = aci_gap_start_general_discovery_proc(MS_Connection_Parameters.Scan_Interval,
                                                MS_Connection_Parameters.Scan_Window,
                                                0x00,
                                                0x00);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_start_general_discovery_proc() failed:0x%02x\r\n", status);
    APP_FLAG_SET(APP_ERROR);
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
  status = aci_gap_create_connection(MS_Connection_Parameters.Scan_Interval,
                                     MS_Connection_Parameters.Scan_Window,
                                     discovery[connection_slaves_counter].device_found_address_type,
                                     discovery[connection_slaves_counter].device_found_address,
                                     0x00,
                                     MS_Connection_Parameters.Connection_Interval,
                                     MS_Connection_Parameters.Connection_Interval,
                                     0x0000,
                                     (int) (MS_Connection_Parameters.Connection_Interval * 1.25), //SUPERVISION_TIMEOUT,
                                     MS_Connection_Parameters.CE_Length,
                                     MS_Connection_Parameters.CE_Length);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_create_connection() failed:0x%02x\r\n", status);
    APP_FLAG_SET(APP_ERROR);
  }else{
    APP_FLAG_SET(WAIT_SLAVE_CONNECTION); 
    PRINTF("aci_gap_create_connection --> SUCCESS\r\n");
  }
  return (status); 
}


/******************************************************************************
 * Function Name  : write_characteristic_.
 * Description    : Write Characteristics.
 * Input          : Connection Handle.
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
    APP_FLAG_SET(APP_ERROR);
    
  }else{
    PRINTF("aci_gatt_write_char_desc --> SUCCESS\r\n");
  }
  
  APP_FLAG_CLEAR(NOTIFICATIONS_ENABLED); //Clear notification flag 
}


/******************************************************************************
 * Function Name  : update_characterists.
 * Description    : Update Characteristics 
 * Input          : index:  slave index.
                  : char_len: received slave characteristic notification length. 
 * Output         : None.
 * Return         : None.
******************************************************************************/
void update_characterists(uint8_t index, uint8_t char_len)
{
  uint8_t status;
  uint8_t update_len;
	
  for (uint8_t i =0; i<connection_masters_counter; i++)
  {
      /* Master_Slave sends notification with length = Max Number of bytes covered from increased ATT_MTU size and which also fits on an single LL PDU packet*/
            update_len = MIN(char_len, Rx_Mtu[i]); 
      update_len = MIN(update_len, Max_Tx_Octets[i]); 
		
      status=aci_gatt_update_char_value_ext(master_connection_handle[i], service_handle,char_handle,1,update_len,0x00,update_len,&(master_char_value[0])); 
      if (status == BLE_STATUS_BUSY)
      {
        PRINTF("aci_gatt_update_char_value_ext() FAILED due to BLE_STATUS_BUSY status (0x%02x), (0x%02x, 0x%02x) !!!\r\n", status , service_handle, char_handle); 
        //APP_FLAG_SET(APP_ERROR);
      }
      else if (status == BLE_STATUS_SUCCESS)
      {
              PRINTF("aci_gatt_update_char_value_ext() of %d bytes\r\n", update_len); 
      }
  }
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
  
  /* User has to add his code */
  
  /* Check if Slaves discovery procedure has to be started */
  if ((disc_to_be_checked == TRUE) && (disc_procedure_is_ongoing == FALSE) && (connection_slaves_counter < num_slaves))
  {
    PRINTF("----------------------------RESTART DISCOVERY PROCEDURE\r\n");
    
    Print_Anchor_Period();
    status = device_scanning();
    if (status == BLE_STATUS_SUCCESS)
    {
      disc_to_be_checked = FALSE;
      Print_Anchor_Period();
    }
  }
  
  /* do connection with discovered slaves devices */
  if (APP_FLAG(DO_SLAVES_CONNECTION)) 
  {
    if ((connection_slaves_counter < actual_discovered_slave_devices) && !(APP_FLAG(WAIT_SLAVE_CONNECTION)))
    {
      /* Connect to one of the discovered devices */
      status = set_connection();
      if (status == 0)
      {
        Print_Anchor_Period();
      }
    }
    else if APP_FLAG(NOTIFICATIONS_ENABLED)
    {
      /* Enable slave device characteristic notification */
      write_characteristic_descriptor(discovery[connection_slaves_counter-1].connection_handle);
    }
    /* All the "up to now" discovered slaves devices have been connected */
    else if (connection_slaves_counter == actual_discovered_slave_devices)
    {
      APP_FLAG_CLEAR(DO_SLAVES_CONNECTION);
      
      if (actual_discovered_slave_devices <= num_slaves)
      {
        /* Discovery procedure should be redone to find missing slaves devices */
        disc_to_be_checked = TRUE; 
      }
      
    }
  }/* end if APP_FLAG(DO_SLAVES_CONNECTION) */
  else if APP_FLAG(GET_NOTIFICATION)
  {
    APP_FLAG_CLEAR(GET_NOTIFICATION); 
    
    PRINTF("Notification of %d bytes from Slave: %d, Counter : 0x%02x\r\n", slave_notification_length, slave_index, slave_value); 

    /* Read the value from Slave i and notify the slave_index component to the Master device to have evidence of which slave data belong  */   
    if ((num_masters>0) && !APP_FLAG(TX_BUFFER_FULL)) 
    {
      update_characterists(slave_index,slave_notification_length); 
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
      if (memcmp(&data_value[index+1], &slaves_local_name[0], SLAVES_LOCAL_NAME_LEN) == 0)
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


static uint8_t Is_New_Device(uint8_t *bdaddr)
{
  uint8_t i; 
  
  for (i=0; i < discovery_counter; i++)
  {
    if (memcmp(bdaddr, &discovery[i].device_found_address, 6) == 0)
      return (FALSE);
  }
  
  return (TRUE);
  
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
  
  /* Advertising_Report contains all the expected parameters */
  uint8_t evt_type = advertising_report[0].Event_Type ;
  uint8_t data_length = advertising_report[0].Length_Data;
  uint8_t bdaddr_type = advertising_report[0].Address_Type;
  uint8_t bdaddr[6];

  Osal_MemCpy(bdaddr, advertising_report[0].Address,6);
      
  /*  check current found device */
  if ((evt_type == ADV_IND) && Is_New_Device(bdaddr) && Find_DeviceName(data_length, advertising_report[0].Data) )
  {
     /* store first device found:  address type and address value */
      discovery[discovery_counter].device_found_address_type = bdaddr_type;
      Osal_MemCpy(discovery[discovery_counter].device_found_address, bdaddr, 6);
      discovery_counter +=1;
      actual_discovered_slave_devices +=1; 
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
    PRINTF("--- END OF MASTER CREATE CONNECTION PROCEDURE: number of connected slaves: %d\r\n", connection_slaves_counter);
  }
  else
  {
    /* aci_gap_proc_complete_event triggererd as end of start general discovery procedure */
    disc_procedure_is_ongoing = FALSE;

    PRINTF("--- END OF SLAVES DISCOVERY PROCEDURE: number of found slaves: %d\r\n", actual_discovered_slave_devices);
    if (connection_slaves_counter < num_slaves)
    {
      if (actual_discovered_slave_devices == 0)  
      {
          disc_procedure_is_ongoing = FALSE;  //no slaves have been found 
      }
      
      APP_FLAG_SET(DO_SLAVES_CONNECTION); /* set do slaves connections flag */
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
  uint8_t ret; 
  
  if (status==0x00)
  {
    if (APP_FLAG(WAIT_SLAVE_CONNECTION)) /* CONNECTED as Master with Slaves devices */
    {
      APP_FLAG_CLEAR(WAIT_SLAVE_CONNECTION); 
      PRINTF("--> CONNECTED as Master with one of the Discovered Slaves device: connection handle : 0x%2x\r\n",connection_handle);

      discovery[connection_slaves_counter].connection_handle  = connection_handle; 
             
      APP_FLAG_SET(NOTIFICATIONS_ENABLED);
      connection_slaves_counter +=1;         
      disc_procedure_is_ongoing = FALSE;
      
    }
    else if (APP_FLAG(SET_CONNECTABLE))/* CONNECTED as Slave with Master */ 
    {
      PRINTF("--> CONNECTED as Slave with Master: connection handle : 0x%2x\r\n",connection_handle);
      
      master_connection_handle[connection_masters_counter] = connection_handle;
             
      PRINTF("disc_procedure_is_ongoing: %d\r\n", disc_procedure_is_ongoing);
      connection_masters_counter +=1;
      APP_FLAG_CLEAR(SET_CONNECTABLE); 

#if (ENABLE_CONNECTION_UPDATE == 1)
      /* Connection update request to connected Master for aligning Master to the Master Slots connection intervals with connected slaves devices */
      ret= aci_l2cap_connection_parameter_update_req(connection_handle,
                                                     MS_Connection_Parameters.Connection_Interval, 
                                                     MS_Connection_Parameters.Connection_Interval + 16, //16 = 20ms for Iphone (TBR)
                                                     0,  //Slave_latency,
                                                     (int) (MS_Connection_Parameters.Connection_Interval * 1.25)); //Timeout_Multiplier) //TBR SUPERVISION_TIMEOUT
      if (ret!=0x00)
      {
        PRINTF("aci_l2cap_connection_parameter_update_req() error: 0x%02x",ret);
      }
      else
      {
        APP_FLAG_SET(CONN_PARAM_UPD_SENT);  
      }    
#else 
      
      /* Enter in discovery mode again */
      if ((connection_masters_counter == 1) && (num_masters == 2)) //Master-Slave is expected to connect to 2 Master devices 
      {
          PRINTF("-- ENTER In DISCOVERY MODE AGAIN for second Master\r\n ");
          APP_FLAG_SET(SET_CONNECTABLE);
          set_device_discoverable();
      }
      
#endif //#if (ENABLE_CONNECTION_UPDATE == 1)
    }/* end else */
                  
  } /* end if (Status == 0) */
  else
  {
     PRINTF("--> MASTER_SLAVE connection ERROR with Slave!!! \r\n");
     APP_FLAG_SET(APP_ERROR);
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
  /* Receive Notifications from Slaves and send them to the Master */

  /* value = 0x<slave_index><slave_value> */
  slave_index = attribute_value[2]; 
  slave_value  = (uint16_t)(((uint16_t) attribute_value[0])  | (((uint16_t) attribute_value[1])  << 8)); 
  
  if (num_masters>0)
  {
    slave_notification_length = attribute_value_length;
    Osal_MemCpy(&(master_char_value[0]), attribute_value, attribute_value_length);
  }
  
   APP_FLAG_SET(GET_NOTIFICATION); 
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
    APP_FLAG_CLEAR(CONN_PARAM_UPD_SENT);
       
    /* Enter in discovery mode again */
    /* Master-Slave is expected to connect to 2 Master devices */
    if ((connection_masters_counter == 1) && (num_masters == 2)) 
    {
        PRINTF("-- ENTER In DISCOVERY MODE AGAIN for second Master\r\n ");
        APP_FLAG_SET(SET_CONNECTABLE); 
        set_device_discoverable();
    }
  }
  else 
  {
    PRINTF("hci_le_connection_update_complete_event() issue, Status: 0x%02x\r\n", Status);
  }
#endif
}

void aci_att_exchange_mtu_resp_event(uint16_t Connection_Handle,
                                     uint16_t Att_MTU)
{
  printf("ATT mtu exchanged with value = %d, \n", Att_MTU);
  
  if APP_FLAG(GATT_EXCHANGE_CONFIG_SENT) /* CONNECTED as Slave with Master */ 
  {
    APP_FLAG_CLEAR(GATT_EXCHANGE_CONFIG_SENT);  
    
    /* Set new increaed Att_Mtu size to be used for communciation between master_slave and master device */
    Rx_Mtu[connection_masters_counter - 1] = Att_MTU;
    /* Enter in discovery mode again */
    if ((connection_masters_counter == 1) && (num_masters == 2)) //Master-Slave is expected to connect to 2 Master devices 
    {
        PRINTF("-- ENTER In DISCOVERY MODE AGAIN for second Master\r\n ");
        APP_FLAG_SET(SET_CONNECTABLE);
        set_device_discoverable();
    }
  } 
}

/**
 * @brief  The LE Data Length Change event notifies the Host of a change to either the maximum Payload length or the maximum transmission time of Data Channel PDUs in either direction. The values reported are the maximum that will actually be used on the connection following the change. 
 * @param  param See file bluenrg1_events.h.
 * @retval See file bluenrg1_events.h.
*/
void hci_le_data_length_change_event(uint16_t connection_handle,uint16_t maxtxoctets,uint16_t maxtxtime,uint16_t maxrxoctets,uint16_t maxrxtime)
{
  uint8_t ret; 
  //USER ACTION IS NEEDED
  PRINTF("hci_le_data_length_change_event() coming from connection to Master device--> EVENT: TX: %d, TXTime: %d, RX: %d , RXTime: %d \r\n",maxtxoctets,maxtxtime,maxrxoctets,maxrxtime);
	
  /* If HCI_LE_DATA_LENGTH_CHANGE_EVENT is raised on Master_Slave device (DLE is supported): 
     do ACI_GATT_EXCHANGE_CONFIG for increasing ATT_MTU size for communciation between Master_Slave and Master device */
  if (connection_masters_counter > 0) 
  {
    Max_Tx_Octets[connection_masters_counter - 1] = maxtxoctets;
    ret = aci_gatt_exchange_config(master_connection_handle[connection_masters_counter - 1]);
    if (ret !=0)
    {
      PRINTF("aci_gatt_exchange_config() error: 0x%02x",ret);
    }
    else
    {
      APP_FLAG_SET(GATT_EXCHANGE_CONFIG_SENT);  
    }        
  }
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
  APP_FLAG_CLEAR(TX_BUFFER_FULL);
} 


/** \endcond 
*/
