/******************** (C) COPYRIGHT 2018 STMicroelectronics ********************
* File Name          : rc.c
* Author             : AMS - VMA RF application team
* Version            : V1.0.0
* Date               : 09-April-2018
* Description        : Remote Control configuration function and state machines
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h" 
#include "bluenrg1_stack.h"
#include "app_state.h"
#include "gatt_db.h"
#ifndef SENSOR_EMULATION /* User Real sensor: LPS25HB (pressure and temperature) */
#include "LPS25HB.h"
#endif 

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

#define ADV_INTERVAL_MIN_MS     1000    
#define ADV_INTERVAL_MAX_MS     1000

#define SENSOR_TIMER_NUMBER      1
#define TEMPERATURE_UPDATE_RATE  1000 /* 1000ms */
#define DISCONNECTION_TIMEOUT    60000 /* 60 sec */

#define PUBLIC_ADDRESS 0x01, 0x00, 0x80, 0xE1, 0x80, 0x02

/* Enable debug printf */
#ifndef DEBUG
#define DEBUG 1
#endif

/* Private macros ------------------------------------------------------------*/
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define BLE_RC_VERSION_STRING "1.0.0" 

/* Private variables ---------------------------------------------------------*/
uint8_t user_timer_expired = FALSE;
volatile int app_flags = START_ADVERTISING;
volatile uint16_t connection_handle = 0;
              
/* Discoverable */
#define TEMP_OFFSET 8
uint8_t adv_data[] = {0x02,0x01,0x06,0x06,AD_TYPE_MANUFACTURER_SPECIFIC_DATA,0x30,0x00,0x05,0xFF,0xFF};

#ifndef SENSOR_EMULATION /* User Real sensor: LPS25HB (pressure and temperature) */
  /* LPS25HB initialization */
  PRESSURE_DrvTypeDef* xLPS25HBDrv = &LPS25HBDrv;  
#endif
  
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* Init Tempereture sensor */
void Init_Temperature_Sensor(void)
{
#ifndef SENSOR_EMULATION /* User Real sensor: LPS25HB (it has pressure and temperature sensors) */
  /* LPS25HB initialization */
  
  PRESSURE_InitTypeDef InitStructure;
  
  InitStructure.OutputDataRate = LPS25HB_ODR_1Hz;
  InitStructure.BlockDataUpdate = LPS25HB_BDU_READ; 
  InitStructure.DiffEnable = LPS25HB_DIFF_ENABLE;  
  InitStructure.SPIMode = LPS25HB_SPI_SIM_3W;  
  InitStructure.PressureResolution = LPS25HB_P_RES_AVG_32;
  InitStructure.TemperatureResolution = LPS25HB_T_RES_AVG_16;  
  xLPS25HBDrv->Init(&InitStructure);
#endif 
}

/* Update temperature data in advertising packets */
void Update_Temperature(void)
{
  float temperature_data;
  uint8_t status = 1;
  
#ifdef SENSOR_EMULATION /* User Emulated Data */
   temperature_data = 26 + ((uint64_t)rand()*15)/RAND_MAX;
#else
  /* Use temperature sensor */
   status = (xLPS25HBDrv->GetTemperature(&temperature_data) == 0) ? 1: 0; 
#endif
  if (status) {
    HOST_TO_LE_16(adv_data+TEMP_OFFSET, (int16_t)temperature_data);
    hci_le_set_advertising_data(sizeof(adv_data),adv_data);
    PRINTF("Updated temperature: %.2f C deg\n",temperature_data);
  }
}

/* Init remote control device */
uint8_t RC_DeviceInit(void)
{
  uint8_t bdaddr[] = {PUBLIC_ADDRESS};
  uint8_t device_name[]={'N', 'o', 'd', 'e'};
  uint8_t ret;
  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;

  /* Configure Public address */
  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN, bdaddr);
  if(ret != BLE_STATUS_SUCCESS) {
    PRINTF("aci_hal_write_config_data() failed: 0x%02x\r\n", ret);
    return ret;
  }
  
  /* Set the TX power to -2 dBm */
  aci_hal_set_tx_power_level(1, 4);
  
  /* GATT Init */
  ret = aci_gatt_init();    
  if(ret){
    PRINTF("aci_gatt_Init() failed: 0x%02x\r\n", ret);
    return ret;
  }
      
  /* GAP Init */
  ret = aci_gap_init(GAP_PERIPHERAL_ROLE, 0, 0x07, &service_handle, &dev_name_char_handle, &appearance_char_handle);
  if(ret){
    PRINTF("aci_gap_Init() failed: 0x%02x\r\n", ret);
    return ret;
  }
      
  /* Set the device name */
  ret = aci_gatt_update_char_value_ext(0,service_handle, dev_name_char_handle,0,sizeof(device_name),0, sizeof(device_name), device_name);
  if(ret){
    PRINTF("aci_gatt_update_char_value_ext() failed: 0x%02x\r\n", ret);
    return ret;
  }
  
  /* Set the IO capability */
  ret = aci_gap_set_io_capability(IO_CAP_DISPLAY_ONLY);
  if(ret){
    PRINTF("aci_gap_set_io_capability() failed: 0x%02x\r\n", ret);
    return ret;
  }

   ret = aci_gap_set_authentication_requirement(BONDING,
                                               MITM_PROTECTION_REQUIRED,
                                               SC_IS_SUPPORTED,
                                               KEYPRESS_IS_NOT_SUPPORTED,
                                               7, 
                                               16,
                                               USE_FIXED_PIN_FOR_PAIRING,
                                               123456,
                                               0x00);
  if(ret){
    PRINTF("aci_gap_set_io_capability failed: 0x%02x\r\n", ret);
    return ret;
  }
    
  /* Add the remoe control serivce and characteristics */
  ret = Add_RC_Service();
  if(ret != BLE_STATUS_SUCCESS) {
    PRINTF("ADD_RC_Service() failed: 0x%02x\r\n", ret);
    return ret;
  }
    
  /* Init temperature sensor */
  Init_Temperature_Sensor();
    
  return BLE_STATUS_SUCCESS;
}

/* Make the remote control device discoverable */
void Start_Advertising(void)
{  
  uint8_t ret;    
  uint8_t scan_resp_data[] = {0x05,AD_TYPE_COMPLETE_LOCAL_NAME,'N','o','d','e'};
    
  /*  Set the scan response data */
  ret = hci_le_set_scan_response_data(sizeof(scan_resp_data),scan_resp_data);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("hci_le_set_scan_response_data() failed: 0x%02x\r\n", ret);
  }
  
  /* Make the device discoverable */
  ret = aci_gap_set_discoverable(ADV_IND, (ADV_INTERVAL_MIN_MS*1000)/625, 
                                 (ADV_INTERVAL_MAX_MS*1000)/625, 
                                 PUBLIC_ADDR, NO_WHITE_LIST_USE,
                                 0, NULL, 0, NULL, 0, 0);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_set_discoverable() failed: 0x%02x\r\n", ret);
  }

  /* Set advertising data. Device will not be in discoverable mode anymore, but only in undirected connectable mode. */
  ret = hci_le_set_advertising_data(sizeof(adv_data),adv_data);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("hci_le_set_advertising_data() failed: 0x%02x\r\n", ret);
  }

  /* Update temperature */
  Update_Temperature();

  PRINTF("Start Undirected Connectable Mode\r\n");
}


/* Remote Control State machine */
void APP_Tick(void)
{
  if(APP_FLAG(START_ADVERTISING)){
    user_timer_expired = TRUE;
    Start_Advertising();
    APP_FLAG_CLEAR(START_ADVERTISING);
  }

  if(APP_FLAG(CONNECTED) && user_timer_expired){
    user_timer_expired = FALSE;
    aci_gap_terminate(connection_handle,0x13); /* 0x13: Remote User Terminated Connection */
    APP_FLAG_CLEAR(CONNECTED);
  }
    
  if(!APP_FLAG(CONNECTED) && user_timer_expired){ 
    user_timer_expired = FALSE;
    HAL_VTimerStart_ms(SENSOR_TIMER_NUMBER, TEMPERATURE_UPDATE_RATE);
    Update_Temperature();
  }    
}

/* ***************** BlueNRG-1 Stack Callbacks ********************************/

/* This function is called when there is a LE Connection Complete event. */
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
  APP_FLAG_SET(CONNECTED); 
  connection_handle = Connection_Handle;
    
  /* Start a timer to disconnect the link after a while. */    
  user_timer_expired = FALSE;
  HAL_VTimer_Stop(SENSOR_TIMER_NUMBER);
  HAL_VTimerStart_ms(SENSOR_TIMER_NUMBER, DISCONNECTION_TIMEOUT); 
    
}/* end hci_le_connection_complete_event() */

/* This function is called when the peer device get disconnected. */
void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason)
{
  APP_FLAG_CLEAR(CONNECTED);
  /* Make the device connectable again. */
  APP_FLAG_SET(START_ADVERTISING);

  /* Stop the timer when a disconnection event occurs: timer will be restarted 
     when entering in advertising with TEMPERATURE_UPDATE_RATE timeout */
  HAL_VTimer_Stop(SENSOR_TIMER_NUMBER);
}/* end hci_disconnection_complete_event() */


/* This function is called when a DB attribute is modified */
void aci_gatt_attribute_modified_event(uint16_t Connection_Handle,
                                       uint16_t Attr_Handle,
                                       uint16_t Offset,
                                       uint16_t Attr_Data_Length,
                                       uint8_t Attr_Data[])
{
  Attribute_Modified_CB(Attr_Handle, Attr_Data_Length, Attr_Data);      
}

/* This function is called when there is a L2CAP_CONN_UPDATE_RESP_Event vendor specific event. */ 
void aci_l2cap_connection_update_resp_event(uint16_t Connection_Handle,
                                            uint16_t Result)
{
  if(Result) {
    PRINTF("> Connection parameters rejected.\n");
  } else  {
    PRINTF("> Connection parameters accepted.\n");
  }
}

/* VTimer Callback */
void HAL_VTimerTimeoutCallback(uint8_t timerNum)
{
  if (timerNum == SENSOR_TIMER_NUMBER) {
    user_timer_expired = TRUE;
  }
}
