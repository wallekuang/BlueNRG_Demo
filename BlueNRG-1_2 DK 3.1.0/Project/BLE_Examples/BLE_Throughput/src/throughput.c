/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
* File Name          : throughput.c
* Author             : AMS - VMA RF  Application team
* Version            : V1.0.0
* Date               : 08-February-2016
* Description        : This file handles bytes received from USB and the init
*                      function. 
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "gp_timer.h"
#include "ble_const.h" 
#include "bluenrg1_stack.h"
#include "app_state.h"
#include "throughput.h"
#include "osal.h"
#include "gatt_db.h"
#include "SDK_EVAL_Config.h"

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

#define ADV_INTERVAL_MIN        200     // ms
#define ADV_INTERVAL_MAX        300     // ms

#define CLIENT_ADDRESS 0xBB, 0x00, 0x03, 0xE1, 0x80, 0x02
#define SERVER_ADDRESS 0xBD, 0x00, 0x03, 0xE1, 0x80, 0x02

#define CMD_BUFF_SIZE 512

/* Private macros ------------------------------------------------------------*/ 

/* Private variables ---------------------------------------------------------*/

uint8_t connInfo[20];
volatile int app_flags = SET_CONNECTABLE;
volatile uint16_t connection_handle = 0;

/** 
  * @brief  Handle of TX,RX  Characteristics.
  */ 
#ifdef CLIENT
uint16_t tx_handle;
uint16_t rx_handle;
#endif 

/* UUIDs */
UUID_t UUID_Tx;
UUID_t UUID_Rx;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : THROUGHPUT_DeviceInit.
* Description    : Init the throughput test.
* Input          : none.
* Return         : Status.
*******************************************************************************/
uint8_t THROUGHPUT_DeviceInit(void)
{
  uint8_t ret;
  uint16_t service_handle;
  uint16_t dev_name_char_handle;
  uint16_t appearance_char_handle;
  uint8_t name[] = {'B', 'l', 'u', 'e', 'N', 'R', 'G', '1'};
  
#if SERVER
  uint8_t role = GAP_PERIPHERAL_ROLE;
  uint8_t bdaddr[] = {SERVER_ADDRESS};
#else
  uint8_t role = GAP_CENTRAL_ROLE;
  uint8_t bdaddr[] = {CLIENT_ADDRESS};
#endif 
  
  /* Configure Public address */
  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN, bdaddr);
  if(ret != BLE_STATUS_SUCCESS){
    printf("Setting BD_ADDR failed: 0x%02x\r\n", ret);
    return ret;
  }

  /* Set the TX power to -2 dBm */
  aci_hal_set_tx_power_level(1, 4);

  /* GATT Init */
  ret = aci_gatt_init();
  if (ret != BLE_STATUS_SUCCESS) {
    printf ("Error in aci_gatt_init(): 0x%02x\r\n", ret);
    return ret;
  } else {
    printf ("aci_gatt_init() --> SUCCESS\r\n");
  }
  
  /* GAP Init */
  ret = aci_gap_init(role, 0x00, 0x08, &service_handle, 
                     &dev_name_char_handle, &appearance_char_handle);
  if (ret != BLE_STATUS_SUCCESS) {
    printf ("Error in aci_gap_init() 0x%02x\r\n", ret);
    return ret;
  } else {
    printf ("aci_gap_init() --> SUCCESS\r\n");
  }

  /* Set the device name */
  ret = aci_gatt_update_char_value_ext(0, service_handle, dev_name_char_handle, 0,sizeof(name), 0, sizeof(name), name);
  if (ret != BLE_STATUS_SUCCESS) {
    printf ("Error in Gatt Update characteristic value 0x%02x\r\n", ret);
    return ret;
  } else {
    printf ("aci_gatt_update_char_value_ext() --> SUCCESS\r\n");
  }

#if  SERVER
  ret = Add_Throughput_Service();
  if (ret != BLE_STATUS_SUCCESS) {
    printf("Error in Add_Throughput_Service 0x%02x\r\n", ret);
    return ret;
  } else {
    printf("Add_Throughput_Service() --> SUCCESS\r\n");
  }
#endif
  
  return BLE_STATUS_SUCCESS;
}

/*******************************************************************************
* Function Name  : Make_Connection.
* Description    : If the device is a Client create the connection. Otherwise puts
*                  the device in discoverable mode.
* Input          : none.
* Return         : none.
*******************************************************************************/
void Make_Connection(void)
{  
  tBleStatus ret;
  
#if CLIENT
  tBDAddr bdaddr = {SERVER_ADDRESS}; 
  

  ret = aci_gap_create_connection(0x4000, 0x4000, PUBLIC_ADDR, bdaddr, PUBLIC_ADDR, 40, 40, 0, 60, 2000 , 2000); 
  if (ret != BLE_STATUS_SUCCESS)
  {
    printf("Error while starting connection: 0x%04x\r\n", ret);
    Clock_Wait(100);        
  }
  
#else
  
  uint8_t local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'B','l','u','e','N','R','G','1','_','C','h','a','t'};
  
  /* disable scan response */
  hci_le_set_scan_response_data(0,NULL);
  
  ret = aci_gap_set_discoverable(ADV_IND, ADV_INTERVAL_MIN, ADV_INTERVAL_MAX, PUBLIC_ADDR, NO_WHITE_LIST_USE,
                                 sizeof(local_name), local_name, 0, NULL, 0, 0);
  if (ret != BLE_STATUS_SUCCESS)
    printf ("Error in aci_gap_set_discoverable(): 0x%02x\r\n", ret);
  else
    printf ("aci_gap_set_discoverable() --> SUCCESS\r\n");
#endif
}

/*******************************************************************************
* Function Name  : APP_Tick.
* Description    : Tick to run the application state machine.
* Input          : none.
* Return         : none.
*******************************************************************************/
void APP_Tick(void)
{
#if CLIENT
  tBleStatus ret;
#endif
  
  if(APP_FLAG(SET_CONNECTABLE))
  {
    Make_Connection();
    APP_FLAG_CLEAR(SET_CONNECTABLE);
  }

#if REQUEST_CONN_PARAM_UPDATE    
  if(APP_FLAG(CONNECTED) && !APP_FLAG(L2CAP_PARAM_UPD_SENT) && Timer_Expired(&l2cap_req_timer))
  {
    aci_l2cap_connection_parameter_update_req(connection_handle, 8, 16, 0, 600);
    APP_FLAG_SET(L2CAP_PARAM_UPD_SENT);
  }
#endif
  
#if CLIENT
  /* Start TX handle Characteristic discovery if not yet done */
  if (APP_FLAG(CONNECTED) && !APP_FLAG(END_READ_TX_CHAR_HANDLE))
  {
    if (!APP_FLAG(START_READ_TX_CHAR_HANDLE))
    {
      /* Discovery TX characteristic handle by UUID 128 bits */

      const uint8_t charUuid128_TX[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe1,0xf2,0x73,0xd9};

      Osal_MemCpy(&UUID_Tx.UUID_16, charUuid128_TX, 16);
      ret = aci_gatt_disc_char_by_uuid(connection_handle, 0x0001, 0xFFFF,UUID_TYPE_128,&UUID_Tx);
      if (ret != 0) 
        printf ("Error in aci_gatt_disc_char_by_uuid() for TX characteristic: 0x%04xr\n", ret);
      else
        printf ("aci_gatt_disc_char_by_uuid() for TX characteristic --> SUCCESS\r\n");
      APP_FLAG_SET(START_READ_TX_CHAR_HANDLE);
    }
  }
  /* Start RX handle Characteristic discovery if not yet done */
  else if (APP_FLAG(CONNECTED) && !APP_FLAG(END_READ_RX_CHAR_HANDLE))
  {
    /* Discovery RX characteristic handle by UUID 128 bits */
    if (!APP_FLAG(START_READ_RX_CHAR_HANDLE))
    {
      /* Discovery TX characteristic handle by UUID 128 bits */

      const uint8_t charUuid128_RX[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe2,0xf2,0x73,0xd9};
      
      Osal_MemCpy(&UUID_Rx.UUID_16, charUuid128_RX, 16);
      ret = aci_gatt_disc_char_by_uuid(connection_handle, 0x0001, 0xFFFF,UUID_TYPE_128,&UUID_Rx);
      if (ret != 0) 
        printf ("Error in aci_gatt_disc_char_by_uuid() for RX characteristic: 0x%04xr\n", ret);
      else
        printf ("aci_gatt_disc_char_by_uuid() for RX characteristic --> SUCCESS\r\n");
      
      APP_FLAG_SET(START_READ_RX_CHAR_HANDLE);
    }
  }
  
  if(APP_FLAG(CONNECTED) && APP_FLAG(END_READ_TX_CHAR_HANDLE) && APP_FLAG(END_READ_RX_CHAR_HANDLE) && !APP_FLAG(NOTIFICATIONS_ENABLED))
  {
    uint8_t client_char_conf_data[] = {0x01, 0x00}; // Enable notifications
    struct timer t;
    Timer_Set(&t, CLOCK_SECOND*10);
    
    while(aci_gatt_write_char_desc(connection_handle, tx_handle+2, 2, client_char_conf_data)==BLE_STATUS_NOT_ALLOWED){ //TX_HANDLE;
        // Radio is busy.
        if(Timer_Expired(&t)) break;
    }
    APP_FLAG_SET(NOTIFICATIONS_ENABLED);
  }
#endif
  
  
#if THROUGHPUT_TEST_SERVER && SERVER
  
  if(APP_FLAG(CONNECTED) && APP_FLAG(NOTIFICATIONS_ENABLED) && !APP_FLAG(TX_BUFFER_FULL)){
    
    uint8_t data[20] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F',0,0,0,0};
    
    static uint32_t packets = 0;
    static tClockTime time, time2; 
    
    HOST_TO_LE_32(data+16, packets);
    
    if(packets==0){
      printf("TX test start\n");
      time = Clock_Time(); 
    }
		
    if(aci_gatt_update_char_value_ext(connection_handle,  ServHandle, TXCharHandle, 1,20, 0, 20, data)==BLE_STATUS_INSUFFICIENT_RESOURCES)
    {
      // Radio is busy (buffer full).
      APP_FLAG_SET(TX_BUFFER_FULL);
    }
    else {
      packets++;
      if(packets != 0 && packets%NUM_PACKETS == 0){
        time2 = Clock_Time();
        tClockTime diff = time2-time;
        printf("%d TX packets. Elapsed time: %d ms. App throughput: %.1f kbps.\n", NUM_PACKETS, (int)diff, (float)NUM_PACKETS*20*8/diff);
        time = Clock_Time();
        
      }
      
    }
    
  }
#elif THROUGHPUT_TEST_CLIENT && CLIENT
  
  if(APP_FLAG(CONNECTED) && APP_FLAG(NOTIFICATIONS_ENABLED) && !APP_FLAG(TX_BUFFER_FULL)){
    
    uint8_t data[20] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F',0,0,0,0};
    
    static uint32_t packets = 0;
    //static tClockTime time, time2;
    
    HOST_TO_LE_32(data+16, packets);
    
    if(packets==0){
      printf("TX test start\n");
      //time = Clock_Time();
    }
    
    if(aci_gatt_write_without_resp(connection_handle, rx_handle+1, 20, data)==BLE_STATUS_INSUFFICIENT_RESOURCES) // It should be: == BLE_STATUS_INSUFFICIENT_RESOURCES
    {
      // Radio is busy (buffer full).
      APP_FLAG_SET(TX_BUFFER_FULL);
    }
    else {
      packets++;
      
#if 0
      if(packets != 0 && packets%NUM_PACKETS == 0){
        time2 = Clock_Time();
        tClockTime diff = time2-time;
        printf("%d TX packets. Elapsed time: %d ms. App throughput: %.1f kbps.\n", NUM_PACKETS, diff, (float)NUM_PACKETS*20*8/diff);
        time = Clock_Time();
        
      }
#endif
    }
    
  }
#endif
  
  
}/* end APP_Tick() */


/* ***************** BlueNRG-1 Stack Callbacks ********************************/

/*******************************************************************************
 * Function Name  : hci_le_connection_complete_event.
 * Description    : This event indicates that a new connection has been created.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void hci_le_connection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Role,
                                      uint8_t Peer_Address_Type,
                                      uint8_t Peer_Address[6],
                                      uint16_t Conn_Interval,
                                      uint16_t Conn_Latency,
                                      uint16_t Supervision_Timeout,
                                      uint8_t Master_Clock_Accuracy)

{ 
  connection_handle = Connection_Handle;
  
  APP_FLAG_SET(CONNECTED);
  
#if REQUEST_CONN_PARAM_UPDATE
  APP_FLAG_CLEAR(L2CAP_PARAM_UPD_SENT);
  Timer_Set(&l2cap_req_timer, CLOCK_SECOND*2);
#endif
}/* end hci_le_connection_complete_event() */

/*******************************************************************************
 * Function Name  : hci_disconnection_complete_event.
 * Description    : This event occurs when a connection is terminated.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason)
{
   APP_FLAG_CLEAR(CONNECTED);
  /* Make the device connectable again. */
  APP_FLAG_SET(SET_CONNECTABLE);
  APP_FLAG_CLEAR(NOTIFICATIONS_ENABLED);
  APP_FLAG_CLEAR(TX_BUFFER_FULL);

  APP_FLAG_CLEAR(START_READ_TX_CHAR_HANDLE);
  APP_FLAG_CLEAR(END_READ_TX_CHAR_HANDLE);
  APP_FLAG_CLEAR(START_READ_RX_CHAR_HANDLE); 
  APP_FLAG_CLEAR(END_READ_RX_CHAR_HANDLE);

}/* end hci_disconnection_complete_event() */


/*******************************************************************************
 * Function Name  : aci_gatt_attribute_modified_event.
 * Description    : This event occurs when an attribute is modified.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_attribute_modified_event(uint16_t Connection_Handle,
                                       uint16_t Attr_Handle,
                                       uint16_t Offset,
                                       uint16_t Attr_Data_Length,
                                       uint8_t Attr_Data[])
{
  Attribute_Modified_CB(Attr_Handle, Attr_Data_Length, Attr_Data);      
}

#if CLIENT

/*******************************************************************************
 * Function Name  : aci_gatt_notification_event.
 * Description    : This event occurs when a notification is received.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_notification_event(uint16_t Connection_Handle,
                                 uint16_t Attribute_Handle,
                                 uint8_t Attribute_Value_Length,
                                 uint8_t Attribute_Value[])
{ 
  uint16_t attr_handle;
  uint8_t * attr_value = Attribute_Value;
  
  attr_handle = Attribute_Handle;
  
#if THROUGHPUT_TEST_SERVER && CLIENT
  static tClockTime time, time2;
  static uint32_t packets=0;
  static uint32_t n_packet0, n_packet1, n_packet2, lost_packets = 0;
  static uint32_t lost1, lost2;
  
  if(attr_handle == tx_handle+1){
    
    const uint8_t data[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    if(memcmp(data,attr_value,sizeof(data)!=0)){
      printf("Unexpected data.\n");
      return;
    }
    
    if(packets==0){
      printf("RX Test start\n");
      time = Clock_Time();
      n_packet1 = LE_TO_HOST_32(attr_value+16) - 1;
    }
    
    n_packet2 = LE_TO_HOST_32(attr_value+16);
    if(n_packet2 == n_packet1){
      printf("Duplicated %d\n!!!",(int)n_packet2);
      return;
    }      
    else if(n_packet2 != n_packet1 + 1){
      if(n_packet2 <  n_packet1){
        printf("Wrong seq %d %d %d\n", (int)n_packet0, (int)n_packet1, (int)n_packet2);
      }          
      else {
        lost_packets += n_packet2-(n_packet1+1);
        lost1 = n_packet1;
        lost2 = n_packet2;
        printf("%d %d\n", (int)lost1, (int)lost2);
      }
    }
    n_packet0 = n_packet1;
    n_packet1 = n_packet2;
    
    packets++;
    
    if(packets != 0 && packets%NUM_PACKETS == 0){
      time2 = Clock_Time();
      tClockTime diff = time2-time;
      printf("%d RX packets. Elapsed time: %d ms. App throughput: %.1f kbps.\n", NUM_PACKETS, (int)diff, (float)NUM_PACKETS*20*8/diff);
      if(lost_packets){
        printf("%d lost packet(s) (%d)\n", (int)lost_packets, (int)lost2);
      }
      time = Clock_Time();
      lost_packets=0;
    }
    
  }
  else {
    printf("Unexpected handle: 0x%04d.\n",attr_handle);
  }
#elif CLIENT && !THROUGHPUT_TEST_SERVER
  if(attr_handle == tx_handle+1)
  {
    for(int i = 0; i < Attribute_Value_Length; i++) 
      printf("%c", Attribute_Value[i]);
  }
#endif
    
}

/*******************************************************************************
 * Function Name  : aci_gatt_disc_read_char_by_uuid_resp_event.
 * Description    : This event occurs when a discovery read characteristic by UUID response.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_disc_read_char_by_uuid_resp_event(uint16_t Connection_Handle,
                                                uint16_t Attribute_Handle,
                                                uint8_t Attribute_Value_Length,
                                                uint8_t Attribute_Value[])
{
  printf("aci_gatt_disc_read_char_by_uuid_resp_event, Connection Handle: 0x%04X\n", Connection_Handle);
  if (APP_FLAG(START_READ_TX_CHAR_HANDLE) && !APP_FLAG(END_READ_TX_CHAR_HANDLE))
  {
    tx_handle = Attribute_Handle;
    printf("TX Char Handle 0x%04X\n", tx_handle);
  }
  else if (APP_FLAG(START_READ_RX_CHAR_HANDLE) && !APP_FLAG(END_READ_RX_CHAR_HANDLE))
  {
    rx_handle = Attribute_Handle;
    printf("RX Char Handle 0x%04X\n", rx_handle);
  }
}

/*******************************************************************************
 * Function Name  : aci_gatt_proc_complete_event.
 * Description    : This event occurs when a GATT procedure complete is received.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_proc_complete_event(uint16_t Connection_Handle,
                                  uint8_t Error_Code)
{ 
  if (APP_FLAG(START_READ_TX_CHAR_HANDLE) && !APP_FLAG(END_READ_TX_CHAR_HANDLE))
  {
    printf("aci_GATT_PROCEDURE_COMPLETE_Event for START_READ_TX_CHAR_HANDLE \r\n");
    APP_FLAG_SET(END_READ_TX_CHAR_HANDLE);
  }
  else if (APP_FLAG(START_READ_RX_CHAR_HANDLE) && !APP_FLAG(END_READ_RX_CHAR_HANDLE))
  {
    printf("aci_GATT_PROCEDURE_COMPLETE_Event for START_READ_TX_CHAR_HANDLE \r\n");
    APP_FLAG_SET(END_READ_RX_CHAR_HANDLE);
  }
}

#endif /* CLIENT */

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
