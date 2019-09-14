/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
 * File Name          : sensor.c
 * Author             : AMS - VMA RF Application team
 * Version            : V1.0.0
 * Date               : 23-November-2015
 * Description        : Sensor init and sensor state machines
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
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h" 
#include "bluenrg1_stack.h"
#include "gp_timer.h"
#include "SDK_EVAL_Config.h"
#include "gatt_db.h"
#ifndef SENSOR_EMULATION
#include "LPS25HB.h"
#include "lsm6ds3.h"
#include "lsm6ds3_hal.h"
#endif
#include "OTA_btl.h"   
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define UPDATE_CONN_PARAM 0 // Can be set to 1 only when no low power mode is used 
#define  ADV_INTERVAL_MIN_MS  1000
#define  ADV_INTERVAL_MAX_MS  1200

#define BLE_SENSOR_VERSION_STRING "1.0.0" 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint8_t set_connectable = 1;
uint16_t connection_handle = 0;
uint8_t connInfo[20];

BOOL sensor_board = FALSE; // It is True if sensor boad has been detected

int connected = FALSE;
#if UPDATE_CONN_PARAM
int l2cap_request_sent = FALSE;
struct timer l2cap_req_timer;
#endif

#define SENSOR_TIMER 1
static uint16_t acceleration_update_rate = 200;
static uint8_t sensorTimer_expired = FALSE;

#ifndef SENSOR_EMULATION
PRESSURE_DrvTypeDef* xLPS25HBDrv = &LPS25HBDrv;  
IMU_6AXES_DrvTypeDef *Imu6AxesDrv = NULL;
LSM6DS3_DrvExtTypeDef *Imu6AxesDrvExt = NULL;
static AxesRaw_t acceleration_data; 
#endif 

volatile uint8_t request_free_fall_notify = FALSE; 
  
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifndef SENSOR_EMULATION
/*******************************************************************************
 * Function Name  : Init_Accelerometer.
 * Description    : Init LIS331DLH accelerometer.
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
void Init_Accelerometer(void)
{
  /* LSM6DS3 library setting */
  IMU_6AXES_InitTypeDef InitStructure;
  uint8_t tmp1 = 0x00;
  
  Imu6AxesDrv = &LSM6DS3Drv;
  Imu6AxesDrvExt = &LSM6DS3Drv_ext_internal;
  InitStructure.G_FullScale      = 125.0f;
  InitStructure.G_OutputDataRate = 13.0f;
  InitStructure.G_X_Axis         = 0; //1;
  InitStructure.G_Y_Axis         = 0;//1;
  InitStructure.G_Z_Axis         = 0; //1;
  InitStructure.X_FullScale      = 2.0f;
  InitStructure.X_OutputDataRate = 13.0f;
  InitStructure.X_X_Axis         = 1;
  InitStructure.X_Y_Axis         = 1;
  InitStructure.X_Z_Axis         = 1;  
  
  /* LSM6DS3 initiliazation */
  Imu6AxesDrv->Init(&InitStructure);
    
  /* Disable all mems IRQs in order to enable free fall detection */ //TBR
  LSM6DS3_IO_Write(&tmp1, LSM6DS3_XG_MEMS_ADDRESS, LSM6DS3_XG_INT1_CTRL, 1);
  
  /* Clear first previous data */
  Imu6AxesDrv->Get_X_Axes((int32_t *)&acceleration_data);
  
  /* Enable Free fall detection */
  Imu6AxesDrvExt->Enable_Free_Fall_Detection(); 
}

/*******************************************************************************
 * Function Name  : Init_Pressure_Temperature_Sensor.
 * Description    : Init LPS25HB pressure and temperature sensor.
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
void Init_Pressure_Temperature_Sensor(void)
{  
  /* LPS25HB initialization */

  PRESSURE_InitTypeDef InitStructure;
  InitStructure.OutputDataRate = LPS25HB_ODR_1Hz;
  InitStructure.BlockDataUpdate = LPS25HB_BDU_READ; //LPS25HB_BDU_READ LPS25HB_BDU_CONT
  InitStructure.DiffEnable = LPS25HB_DIFF_ENABLE;  // LPS25HB_DIFF_ENABLE
  InitStructure.SPIMode = LPS25HB_SPI_SIM_3W;  // LPS25HB_SPI_SIM_3W
  InitStructure.PressureResolution = LPS25HB_P_RES_AVG_32;
  InitStructure.TemperatureResolution = LPS25HB_T_RES_AVG_16;  
  xLPS25HBDrv->Init(&InitStructure);
}

/*******************************************************************************
 * Function Name  : Init_Humidity_Sensor.
 * Description    : Init HTS221 temperature sensor.
 * Input          : None.
 * Output         : None.
 * Return         : 0 if success, 1 if error.
 *******************************************************************************/
int Init_Humidity_Sensor(void)
{
  /* No humidity sensor available on BlueNRG-1 Application Board*/
  return 0;
}
#endif

/*******************************************************************************
 * Function Name  : Sensor_DeviceInit.
 * Description    : Init the device sensors.
 * Input          : None.
 * Return         : Status.
 *******************************************************************************/
uint8_t Sensor_DeviceInit()
{
  uint8_t bdaddr[] = {0x12, 0x34, 0x00, 0xE1, 0x80, 0x02};
  uint8_t ret;
  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
  uint8_t device_name[] = {'B', 'l', 'u', 'e', 'N', 'R', 'G'};

  /* Set the device public address */
  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN,
                                  bdaddr);  
  if(ret != BLE_STATUS_SUCCESS) {
    PRINTF("aci_hal_write_config_data() failed: 0x%02x\r\n", ret);
    return ret;
  }
  
  /* Set the TX power -2 dBm */
  aci_hal_set_tx_power_level(1, 4);
  
  /* GATT Init */
  ret = aci_gatt_init();
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_init() failed: 0x%02x\r\n", ret);
    return ret;
  }
  
  /* GAP Init */
  ret = aci_gap_init(GAP_PERIPHERAL_ROLE, 0, 0x07, &service_handle, &dev_name_char_handle, &appearance_char_handle);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_init() failed: 0x%02x\r\n", ret);
    return ret;
  }
 
  /* Update device name */
  ret = aci_gatt_update_char_value_ext(0, service_handle, dev_name_char_handle, 0, sizeof(device_name), 0, sizeof(device_name), device_name);
  if(ret != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gatt_update_char_value_ext() failed: 0x%02x\r\n", ret);
    return ret;
  }
  
  /* BLE Security v4.2 is supported: BLE stack FW version >= 2.x (new API prototype) */  
  ret = aci_gap_set_authentication_requirement(BONDING,
                                               MITM_PROTECTION_REQUIRED,
                                               SC_IS_SUPPORTED,
                                               KEYPRESS_IS_NOT_SUPPORTED,
                                               7, 
                                               16,
                                               USE_FIXED_PIN_FOR_PAIRING,
                                               123456,
                                               0x00);
  if(ret != BLE_STATUS_SUCCESS) {
    PRINTF("aci_gap_set_authentication_requirement()failed: 0x%02x\r\n", ret);
    return ret;
  } 
  
  PRINTF("BLE Stack Initialized with SUCCESS\n");

#ifndef SENSOR_EMULATION /* User Real sensors */
  Init_Accelerometer();
  Init_Pressure_Temperature_Sensor();
#endif   


  /* Add ACC service and Characteristics */
  ret = Add_Acc_Service();
  if(ret == BLE_STATUS_SUCCESS) {
    PRINTF("Acceleration service added successfully.\n");
  } else {
    PRINTF("Error while adding Acceleration service: 0x%02x\r\n", ret);
    return ret;
  }

  /* Add Environmental Sensor Service */
  ret = Add_Environmental_Sensor_Service();
  if(ret == BLE_STATUS_SUCCESS) {
    PRINTF("Environmental service added successfully.\n");
  } else {
    PRINTF("Error while adding Environmental service: 0x%04x\r\n", ret);
    return ret;
  }

#if ST_OTA_FIRMWARE_UPGRADE_SUPPORT     
  ret = OTA_Add_Btl_Service();
  if(ret == BLE_STATUS_SUCCESS)
    PRINTF("OTA service added successfully.\n");
  else
    PRINTF("Error while adding OTA service.\n");
#endif /* ST_OTA_FIRMWARE_UPGRADE_SUPPORT */ 
  
  /* Start the Sensor Timer */
  ret = HAL_VTimerStart_ms(SENSOR_TIMER, acceleration_update_rate);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("HAL_VTimerStart_ms() failed; 0x%02x\r\n", ret);
    return ret;
  } else {
    sensorTimer_expired = FALSE;
  }

  return BLE_STATUS_SUCCESS;
}

/*******************************************************************************
 * Function Name  : Set_DeviceConnectable.
 * Description    : Puts the device in connectable mode.
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
void Set_DeviceConnectable(void)
{  
  uint8_t ret;
  uint8_t local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'B','l','u','e','N','R','G'}; 

#if ST_OTA_FIRMWARE_UPGRADE_SUPPORT
  hci_le_set_scan_response_data(18,BTLServiceUUID4Scan); 
#else
  hci_le_set_scan_response_data(0,NULL);
#endif /* ST_OTA_FIRMWARE_UPGRADE_SUPPORT */ 
  PRINTF("Set General Discoverable Mode.\n");
  
  ret = aci_gap_set_discoverable(ADV_IND,
                                (ADV_INTERVAL_MIN_MS*1000)/625,(ADV_INTERVAL_MAX_MS*1000)/625,
                                 STATIC_RANDOM_ADDR, NO_WHITE_LIST_USE,
                                 sizeof(local_name), local_name, 0, NULL, 0, 0); 
  if(ret != BLE_STATUS_SUCCESS)
  {
    PRINTF("aci_gap_set_discoverable() failed: 0x%02x\r\n",ret);
    SdkEvalLedOn(LED3);  
  }
  else
    PRINTF("aci_gap_set_discoverable() --> SUCCESS\r\n");
}

/*******************************************************************************
 * Function Name  : APP_Tick.
 * Description    : Sensor Demo state machine.
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
void APP_Tick(void)
{
  /* Make the device discoverable */
  if(set_connectable) {
    Set_DeviceConnectable();
    set_connectable = FALSE;
  }

#if UPDATE_CONN_PARAM      
  /* Connection parameter update request */
  if(connected && !l2cap_request_sent && Timer_Expired(&l2cap_req_timer)){
    ret = aci_l2cap_connection_parameter_update_req(connection_handle, 9, 20, 0, 600); //24, 24
    l2cap_request_sent = TRUE;
  }
#endif
    
  /*  Update sensor value */
  if (sensorTimer_expired) {    
    sensorTimer_expired = FALSE;
    if (HAL_VTimerStart_ms(SENSOR_TIMER, acceleration_update_rate) != BLE_STATUS_SUCCESS)
      sensorTimer_expired = TRUE;
    if(connected) {
      AxesRaw_t acc_data;
      
      /* Activity Led */
      SdkEvalLedToggle(LED1);  

      /* Get Acceleration data */
      if (GetAccAxesRaw(&acc_data) == IMU_6AXES_OK) {
        Acc_Update(&acc_data);
      }
        
      /* Get free fall status */
      GetFreeFallStatus();
    }
  }

  /* Free fall notification */
  if(request_free_fall_notify == TRUE) {
    request_free_fall_notify = FALSE;
    Free_Fall_Notify();
  }
}

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
  
  connected = TRUE;
  connection_handle = Connection_Handle;
  
#if UPDATE_CONN_PARAM    
  l2cap_request_sent = FALSE;
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
  connected = FALSE;
  /* Make the device connectable again. */
  set_connectable = TRUE;
  connection_handle =0;
  
  SdkEvalLedOn(LED1);//activity led   
#if ST_OTA_FIRMWARE_UPGRADE_SUPPORT
  OTA_terminate_connection();
#endif 
}/* end hci_disconnection_complete_event() */


/*******************************************************************************
 * Function Name  : aci_gatt_read_permit_req_event.
 * Description    : This event is given when a read request is received
 *                  by the server from the client.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_read_permit_req_event(uint16_t Connection_Handle,
                                    uint16_t Attribute_Handle,
                                    uint16_t Offset)
{
  Read_Request_CB(Attribute_Handle);    
}

/*******************************************************************************
 * Function Name  : HAL_VTimerTimeoutCallback.
 * Description    : This function will be called on the expiry of 
 *                  a one-shot virtual timer.
 * Input          : See file bluenrg1_stack.h
 * Output         : See file bluenrg1_stack.h
 * Return         : See file bluenrg1_stack.h
 *******************************************************************************/
void HAL_VTimerTimeoutCallback(uint8_t timerNum)
{
  if (timerNum == SENSOR_TIMER) {
    sensorTimer_expired = TRUE;
  }
}

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
#if ST_OTA_FIRMWARE_UPGRADE_SUPPORT
  OTA_Write_Request_CB(Connection_Handle, Attr_Handle, Attr_Data_Length, Attr_Data);
#endif /* ST_OTA_FIRMWARE_UPGRADE_SUPPORT */ 
}


void aci_hal_end_of_radio_activity_event(uint8_t Last_State,
                                         uint8_t Next_State,
                                         uint32_t Next_State_SysTime)
{
#if ST_OTA_FIRMWARE_UPGRADE_SUPPORT
  if (Next_State == 0x02) /* 0x02: Connection event slave */
  {
    OTA_Radio_Activity(Next_State_SysTime);  
  }
#endif 
}
