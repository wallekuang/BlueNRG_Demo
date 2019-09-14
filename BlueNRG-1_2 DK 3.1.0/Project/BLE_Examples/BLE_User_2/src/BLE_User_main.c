/*
  ******************************************************************************
  * @file    BLE_User_main.c 
  * @author  AMG - RF Application Team
  * @version V1.0.0
  * @date    25 - 05 - 2019
  * @brief   User application initialization functions.
	This C file has been created automatically by BlueNRG GUI.
	This code has to be reviewed.
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
#include <stdint.h>
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h"
#include "bluenrg1_stack.h"
#include "SDK_EVAL_Config.h"
#include "sleep.h"
#include "user.h"
#include "osal.h"

#define BLE_STACK_V_1_X 0

#include "user_config.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

int main(void)
{
  int ret;
  uint8_t status;
  uint8_t hci_version;
  uint16_t hci_revision;
  uint8_t lmp_pal_version;
  uint16_t manufacturer_name;
  uint16_t lmp_pal_subversion;
  uint16_t length_21;
  uint16_t value_length_out_21;
  uint8_t value_21[255];
  uint16_t length_23;
  uint16_t value_length_out_23;
  uint8_t value_23[255];
  uint16_t length_25;
  uint16_t value_length_out_25;
  uint8_t value_25[255];
  uint16_t length_27;
  uint16_t value_length_out_27;
  uint8_t value_27[255];
  uint16_t length_29;
  uint16_t value_length_out_29;
  uint8_t value_29[255];
  uint16_t length_31;
  uint16_t value_length_out_31;
  uint8_t value_31[255];
  uint16_t length_33;
  uint16_t value_length_out_33;
  uint8_t value_33[255];
  uint16_t length_35;
  uint16_t value_length_out_35;
  uint8_t value_35[255];
  uint16_t length_37;
  uint16_t value_length_out_37;
  uint8_t value_37[255];
  uint16_t length_39;
  uint16_t value_length_out_39;
  uint8_t value_39[255];
  uint16_t length_41;
  uint16_t value_length_out_41;
  uint8_t value_41[255];
  uint16_t length_48;
  uint16_t value_length_out_48;
  uint8_t value_48[255];
  uint16_t length_52;
  uint16_t value_length_out_52;
  uint8_t value_52[255];


  /* System Init */
  SystemInit();

  /* Identify BlueNRG/BlueNRG-MS/BleNRG-1 or BlueNRG-2 platform */
  SdkEvalIdentification();

  /* Configure I/O communication channel */
  SdkEvalComUartInit(UART_BAUDRATE);


  /* BlueNRG-1 stack init */
  ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);

  if (ret != BLE_STATUS_SUCCESS) {
    printf("Error in BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
    while(1);
    }

  /* Application demo Led Init */
  SdkEvalLedInit(LED1);
  SdkEvalLedInit(LED3);
  SdkEvalLedOn(LED1);
  SdkEvalLedOn(LED3);


  /*NOTE: In order to generate an user-friendly C code and helping user on his coding post-build activities, 
          the generated BLE stack APIs are placed within some predefined C functions (device_initialization(), set_database(), device_scanning(), 
          set_device_discoverable(), set_connection(), set_connection_update(), discovery(), read_write_characteristics() and update_characterists()).
  
          Once C code has been generated, user can move and modify APIs sequence order, within and outside these default user functions, 
          according to his specific needs.
  */

  //hci_read_local_version_information
  //status = hci_read_local_version_information(&hci_version, &hci_revision, &lmp_pal_version, &manufacturer_name, &lmp_pal_subversion);
  status = hci_read_local_version_information(&hci_version, &hci_revision, &lmp_pal_version, &manufacturer_name, &lmp_pal_subversion);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("hci_read_local_version_information() failed:0x%02x\r\n", status);
  }else{
    PRINTF("hci_read_local_version_information --> SUCCESS\r\n");
  }

  //hci_reset
  //status = hci_reset();
  status = hci_reset();
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("hci_reset() failed:0x%02x\r\n", status);
  }else{
    PRINTF("hci_reset --> SUCCESS\r\n");
  }

  //device_initialization
  device_initialization();

  //aci_gatt_read_handle_value
  //status = aci_gatt_read_handle_value(attr_handle,offset,value_length_requested, &length_21, &value_length_out_21, value_21);
  status = aci_gatt_read_handle_value(0x0001,0x0000,0x00FF, &length_21, &value_length_out_21, value_21);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_read_handle_value() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_read_handle_value --> SUCCESS\r\n");
  }

  //aci_gatt_read_handle_value
  //status = aci_gatt_read_handle_value(attr_handle,offset,value_length_requested, &length_23, &value_length_out_23, value_23);
  status = aci_gatt_read_handle_value(0x0002,0x0000,0x00FF, &length_23, &value_length_out_23, value_23);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_read_handle_value() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_read_handle_value --> SUCCESS\r\n");
  }

  //aci_gatt_read_handle_value
  //status = aci_gatt_read_handle_value(attr_handle,offset,value_length_requested, &length_25, &value_length_out_25, value_25);
  status = aci_gatt_read_handle_value(0x0003,0x0000,0x00F4, &length_25, &value_length_out_25, value_25);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_read_handle_value() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_read_handle_value --> SUCCESS\r\n");
  }

  //aci_gatt_read_handle_value
  //status = aci_gatt_read_handle_value(attr_handle,offset,value_length_requested, &length_27, &value_length_out_27, value_27);
  status = aci_gatt_read_handle_value(0x0005,0x0000,0x00FF, &length_27, &value_length_out_27, value_27);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_read_handle_value() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_read_handle_value --> SUCCESS\r\n");
  }

  //aci_gatt_read_handle_value
  //status = aci_gatt_read_handle_value(attr_handle,offset,value_length_requested, &length_29, &value_length_out_29, value_29);
  status = aci_gatt_read_handle_value(0x0006,0x0000,0x00FF, &length_29, &value_length_out_29, value_29);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_read_handle_value() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_read_handle_value --> SUCCESS\r\n");
  }

  //aci_gatt_read_handle_value
  //status = aci_gatt_read_handle_value(attr_handle,offset,value_length_requested, &length_31, &value_length_out_31, value_31);
  status = aci_gatt_read_handle_value(0x0007,0x0000,0x00F4, &length_31, &value_length_out_31, value_31);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_read_handle_value() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_read_handle_value --> SUCCESS\r\n");
  }

  //aci_gatt_read_handle_value
  //status = aci_gatt_read_handle_value(attr_handle,offset,value_length_requested, &length_33, &value_length_out_33, value_33);
  status = aci_gatt_read_handle_value(0x0008,0x0000,0x00FF, &length_33, &value_length_out_33, value_33);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_read_handle_value() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_read_handle_value --> SUCCESS\r\n");
  }

  //aci_gatt_read_handle_value
  //status = aci_gatt_read_handle_value(attr_handle,offset,value_length_requested, &length_35, &value_length_out_35, value_35);
  status = aci_gatt_read_handle_value(0x0009,0x0000,0x00F4, &length_35, &value_length_out_35, value_35);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_read_handle_value() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_read_handle_value --> SUCCESS\r\n");
  }

  //aci_gatt_read_handle_value
  //status = aci_gatt_read_handle_value(attr_handle,offset,value_length_requested, &length_37, &value_length_out_37, value_37);
  status = aci_gatt_read_handle_value(0x000A,0x0000,0x00FF, &length_37, &value_length_out_37, value_37);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_read_handle_value() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_read_handle_value --> SUCCESS\r\n");
  }

  //aci_gatt_read_handle_value
  //status = aci_gatt_read_handle_value(attr_handle,offset,value_length_requested, &length_39, &value_length_out_39, value_39);
  status = aci_gatt_read_handle_value(0x000B,0x0000,0x00F4, &length_39, &value_length_out_39, value_39);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_read_handle_value() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_read_handle_value --> SUCCESS\r\n");
  }

  //aci_gatt_read_handle_value
  //status = aci_gatt_read_handle_value(attr_handle,offset,value_length_requested, &length_41, &value_length_out_41, value_41);
  status = aci_gatt_read_handle_value(0x000C,0x0000,0x00FF, &length_41, &value_length_out_41, value_41);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_read_handle_value() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_read_handle_value --> SUCCESS\r\n");
  }

  //set_database
  set_database();

  //aci_gatt_read_handle_value
  //status = aci_gatt_read_handle_value(attr_handle,offset,value_length_requested, &length_48, &value_length_out_48, value_48);
  status = aci_gatt_read_handle_value(0x000E,0x0000,0x00F4, &length_48, &value_length_out_48, value_48);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_read_handle_value() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_read_handle_value --> SUCCESS\r\n");
  }

  //aci_gatt_read_handle_value
  //status = aci_gatt_read_handle_value(attr_handle,offset,value_length_requested, &length_52, &value_length_out_52, value_52);
  status = aci_gatt_read_handle_value(0x0010,0x0000,0x00F4, &length_52, &value_length_out_52, value_52);
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_read_handle_value() failed:0x%02x\r\n", status);
  }else{
    PRINTF("aci_gatt_read_handle_value --> SUCCESS\r\n");
  }

  //set_device_discoverable
  set_device_discoverable();


  while(1) {
    /* BlueNRG-1 stack tick */
    BTLE_StackTick();


    /* Application Tick */
    APP_Tick();


    /* Power Save management */
#if BLE_STACK_V_1_X ==1
    BlueNRG_Sleep(SLEEPMODE_NOTIMER, 0, 0,0);
#else
    BlueNRG_Sleep(SLEEPMODE_NOTIMER, 0, 0);
#endif


  }
}

/****************** BlueNRG-1 Sleep Management Callback ********************************/

SleepModes App_SleepMode_Check(SleepModes sleepMode)
{
  if(SdkEvalComIOTxFifoNotEmpty())
    return SLEEPMODE_RUNNING;

  return SLEEPMODE_NOTIMER;
}
/***************************************************************************************/


#ifdef USE_FULL_ASSERT


/**
 * @brief  Reports the name of the source file and the source line number where the assert_param error has occurred
 * @param  file	pointer to the source file name.
 * @param  line	assert_param error line source number.
 * @retval None.
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/** \endcond 
*/
