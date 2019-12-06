/*
  ******************************************************************************
  * @file    user.c 
  * @author  AMG - RF Application Team
  * @version V1.0.0
  * @date    06 - 11 - 2019
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
#include <stdlib.h>
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h"
#include "bluenrg1_stack.h"
#include "gp_timer.h"
#include "SDK_EVAL_Config.h"
#include "OTA_btl.h"
#include "user.h"
#include "osal.h"
#include "user_buffer.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define CHARACTERISTIC_LEN			247//USER_MAX_ATT_SIZE


const uint8_t service_uuid[2] = {0xff,0xf0};
const uint8_t uuidTX[2] = 			{0xff,0xf1};
const uint8_t uuidRX[2] = 			{0xff,0xf2};


uint16_t servHandle, TXHandle, RXHandle;
static volatile uint16_t connection_handle = 0;
static uint8_t ccc_eable = FALSE;

uint16_t notify_len = DEFAULT_ATT_MTU-3;

static RingBuffer s_uart_rx_buf;
static uint8_t  s_uart_rx_fifo[1024];


uint8_t Add_Throughtput_Service(void)
{
  uint8_t ret;
  UUID_t uuid;


	
  Osal_MemCpy(&uuid.UUID_16, service_uuid, 2);
  ret = aci_gatt_add_service(UUID_TYPE_16, (Service_UUID_t*)&uuid, PRIMARY_SERVICE, 6, &servHandle); 
  if (ret != BLE_STATUS_SUCCESS) goto fail;    

  Osal_MemCpy(&uuid.UUID_16, uuidTX, 2);
  ret =  aci_gatt_add_char(servHandle, UUID_TYPE_16, (Char_UUID_t *)&uuid, CHARACTERISTIC_LEN, CHAR_PROP_NOTIFY, ATTR_PERMISSION_NONE, 0,
                16, 1, &TXHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;
  
  Osal_MemCpy(&uuid.UUID_16, uuidRX, 2);
  ret =  aci_gatt_add_char(servHandle, UUID_TYPE_16, (Char_UUID_t *)&uuid, CHARACTERISTIC_LEN, CHAR_PROP_WRITE_WITHOUT_RESP, ATTR_PERMISSION_NONE, GATT_NOTIFY_ATTRIBUTE_WRITE,
                16, 1, &RXHandle);
  if (ret != BLE_STATUS_SUCCESS) goto fail;

  PRINTF("Chat Service added.\nTX Char Handle 0x%04X, RX Char Handle 0x%04X\n", TXHandle, RXHandle);
  return BLE_STATUS_SUCCESS; 

fail:
  PRINTF("Error while adding Chat service.\n");
  return BLE_STATUS_ERROR ;
}

static void app_set_discoverble(void)
{
	uint8_t local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'B','l','u','e','N','R','G','1'};
	uint8_t ret;
	ret = aci_gap_set_discoverable(ADV_IND, 0, 0, PUBLIC_ADDR, NO_WHITE_LIST_USE,
                                 sizeof(local_name), local_name, 0, NULL, 0, 0);
  if (ret != BLE_STATUS_SUCCESS)
    PRINTF ("Error in aci_gap_set_discoverable(): 0x%02x\r\n", ret);
  else
    PRINTF ("aci_gap_set_discoverable() --> SUCCESS\r\n");

}

static void app_init(void)
{
		// init rx fifo 
		if(s_uart_rx_buf.buffer_size == 0){
				user_buffer_create(&s_uart_rx_buf, sizeof(s_uart_rx_fifo)-1, s_uart_rx_fifo);
		}
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

  uint8_t value_8[] = {0xBB,0x41,0x00,0xE1,0x80,0x02};

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

	status = Add_Throughtput_Service();
	if (status != BLE_STATUS_SUCCESS) {
    PRINTF("Add_Throughtput_Service() failed:0x%02x\r\n", status);
  }else{
    PRINTF("Add_Throughtput_Service --> SUCCESS\r\n");
  }

	app_set_discoverble();

	app_init();
}

void app_check_alive(void)
{
	static uint32_t last_time = 0;
	uint32_t cur_time = HAL_VTimerGetCurrentTime_sysT32();
	uint32_t ms = abs(cur_time-last_time)*2.4414/1000;
	if(ms > 1000){
			last_time = cur_time;
			PRINTF("*");
	}
}

void notify_Tick(void)
{
		if(!ccc_eable)
			return;


		int length = user_buffer_item_count(&s_uart_rx_buf);
		if(length > 0){
				uint8_t temp[CHARACTERISTIC_LEN];
				uint16_t act_len = user_buffer_peek(&s_uart_rx_buf, temp, (length>notify_len?notify_len:length));
				
        tBleStatus state = aci_gatt_update_char_value_ext(connection_handle, servHandle, TXHandle, 1, \
                                   act_len, 0, act_len, temp);
				if(state == BLE_STATUS_SUCCESS){
						user_buffer_release_items(&s_uart_rx_buf, act_len);
				}
				else{
						//PRINTF("error:%x \n", state);
				}				
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
 app_check_alive();
	notify_Tick();
}

/* *************** BlueNRG-1 Stack Callbacks****************/


void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason)
{

		PRINTF("%s \r\n", __func__);
		PRINTF("Reason: %x\n", Reason);
		app_set_discoverble();
		connection_handle = 0;
		ccc_eable = FALSE;
}


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
		PRINTF("%s \r\n", __func__);
		PRINTF("Conn_Interval %d \n", Conn_Interval);
		aci_gatt_exchange_config(Connection_Handle);
		notify_len = DEFAULT_ATT_MTU-3;
		connection_handle = Connection_Handle;

		
}

void aci_att_exchange_mtu_resp_event(uint16_t Connection_Handle,
																		 uint16_t Server_RX_MTU)
{
		PRINTF("%s \r\n", __func__);
		PRINTF("Server_RX_MTU:%x \n", Server_RX_MTU);
		notify_len = Server_RX_MTU-4;
		aci_l2cap_connection_parameter_update_req(connection_handle,6,6,0,200);
}


void printf_shor_arry(uint16_t length,uint8_t *pdata)
{
		if(length > 8){
				for(int i=0;i<4;i++){
						PRINTF("%x ", pdata[i]);
				}
				PRINTF("...");
				for(int i=length-4;i<length;i++){
						PRINTF("%x ", pdata[i]);
				}
		}
		else{
				for(int i=0;i<length;i++){
						PRINTF("%x ", pdata[i]);
				}
		}
		
		PRINTF("\n");
}
																		 
void aci_gatt_attribute_modified_event(uint16_t Connection_Handle,
                                       uint16_t Attr_Handle,
                                       uint16_t Offset,
                                       uint16_t Attr_Data_Length,
                                       uint8_t Attr_Data[])																			
{
		PRINTF("%s \r\n", __func__);
		PRINTF("Attr_Handle:%x Attr_Data_Length:%x \n", Attr_Handle, Attr_Data_Length);
		if(Attr_Handle == TXHandle+2){
				if(Attr_Data[0] != 0)
					ccc_eable = Attr_Data[0];
				else
					ccc_eable = 0;
		}

		printf_shor_arry(Attr_Data_Length, Attr_Data);
}



void Process_InputData(uint8_t* data_buffer, uint16_t Nb_bytes)
{
		if(s_uart_rx_buf.buffer_size == 0){
				user_buffer_create(&s_uart_rx_buf, sizeof(s_uart_rx_fifo)-1, s_uart_rx_fifo);
		}
		user_buffer_write_items(&s_uart_rx_buf, data_buffer, Nb_bytes); 
}


																			 
/** \endcond 
*/
