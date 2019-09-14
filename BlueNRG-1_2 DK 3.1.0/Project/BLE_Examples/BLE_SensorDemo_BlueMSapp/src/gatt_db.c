/******************** (C) COPYRIGHT 2018 STMicroelectronics ********************
* File Name          : gatt_db.c
* Author             : AMS - RF Application team
* Version            : V1.1.0
* Date               : 05-November-2018
* Description        : Functions to build GATT DB and handle GATT events.
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
#include <stdlib.h>
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h" 
#include "bluenrg1_stack.h"
#include "clock.h"
#include "gp_timer.h"
#include "gatt_db.h"
#include "osal.h"
#include "SDK_EVAL_Config.h"

#include "LPS25HB.h"
#include "lsm6ds3.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
do {\
    uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; uuid_struct[2] = uuid_2; uuid_struct[3] = uuid_3; \
        uuid_struct[4] = uuid_4; uuid_struct[5] = uuid_5; uuid_struct[6] = uuid_6; uuid_struct[7] = uuid_7; \
            uuid_struct[8] = uuid_8; uuid_struct[9] = uuid_9; uuid_struct[10] = uuid_10; uuid_struct[11] = uuid_11; \
                uuid_struct[12] = uuid_12; uuid_struct[13] = uuid_13; uuid_struct[14] = uuid_14; uuid_struct[15] = uuid_15; \
}while(0)


/* Hardware Characteristics Service */
#define COPY_HW_SENS_W2ST_SERVICE_UUID(uuid_struct)    COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x00,0x00,0x01,0x11,0xe1,0x9a,0xb4,0x00,0x02,0xa5,0xd5,0xc5,0x1b)
#define COPY_ENVIRONMENTAL_W2ST_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x00,0x00,0x01,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)
/* 0x00 c0 00 00  = 00 80 00 00 (acc) + 00 40 00 00 (gyro); [no + 00 20 00 00 (mag),  since magnetometer is not supported on BlueNRG-1/2 eval kits] */
#define COPY_ACC_GYRO_MAG_W2ST_CHAR_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0x00,0xC0,0x00,0x00,0x00,0x01,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

uint16_t HWServW2STHandle, EnvironmentalCharHandle, AccGyroMagCharHandle;

/* UUIDS */
Service_UUID_t service_uuid;
Char_UUID_t char_uuid;

extern uint16_t connection_handle;
extern PRESSURE_DrvTypeDef* xLPS25HBDrv;  
extern IMU_6AXES_DrvTypeDef *Imu6AxesDrv;
extern uint32_t start_time;

IMU_6AXES_StatusTypeDef GetAccAxesRaw(AxesRaw_t * acceleration_data, AxesRaw_t * gyro_data)
{
  IMU_6AXES_StatusTypeDef status = IMU_6AXES_OK;

  status = Imu6AxesDrv->Get_X_Axes((int32_t *)acceleration_data);
  status |= Imu6AxesDrv->Get_G_Axes((int32_t *)gyro_data);

  return status;
}

  
/*******************************************************************************
* Function Name  : Add_Chat_Service
* Description    : Add the 'Accelerometer' service.
* Input          : None
* Return         : Status.
*******************************************************************************/
tBleStatus Add_HWServW2ST_Service(void)
{
  tBleStatus ret;
  uint8_t uuid[16];
  
  /* ********************************* Add_HWServW2ST_Service */
  COPY_HW_SENS_W2ST_SERVICE_UUID(uuid);  
  Osal_MemCpy(&service_uuid.Service_UUID_128, uuid, 16);
  ret = aci_gatt_add_service(UUID_TYPE_128, &service_uuid, PRIMARY_SERVICE, 1+3*4, &HWServW2STHandle); 
  if (ret != BLE_STATUS_SUCCESS) 
    return BLE_STATUS_ERROR;    
  
  
  /* Fill the Environmental BLE Characteristc */
  COPY_ENVIRONMENTAL_W2ST_CHAR_UUID(uuid);
  uuid[14] |= 0x04; /* One Temperature value*/
  uuid[14] |= 0x10; /* Pressure value*/
  Osal_MemCpy(&char_uuid.Char_UUID_128, uuid, 16);  
  ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, &char_uuid, 2+2+4, 
                           CHAR_PROP_NOTIFY|CHAR_PROP_READ, 
                           ATTR_PERMISSION_NONE, 
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &EnvironmentalCharHandle);
  if (ret != BLE_STATUS_SUCCESS) 
    return BLE_STATUS_ERROR;
  
  
  /* Fill the AccGyroMag BLE Characteristc */
  COPY_ACC_GYRO_MAG_W2ST_CHAR_UUID(uuid);
  Osal_MemCpy(&char_uuid.Char_UUID_128, uuid, 16);  
  ret =  aci_gatt_add_char(HWServW2STHandle, UUID_TYPE_128, &char_uuid, 2+3*2*2, 
                           CHAR_PROP_NOTIFY, 
                           ATTR_PERMISSION_NONE, 
                           GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
                           16, 0, &AccGyroMagCharHandle);
  if (ret != BLE_STATUS_SUCCESS) 
    return BLE_STATUS_ERROR;
  
  return BLE_STATUS_SUCCESS;
}

/*******************************************************************************
* Function Name  : Acc_Update
* Description    : Update acceleration characteristic value
* Input          : AxesRaw_t structure containing acceleration value in mg.
* Return         : Status.
*******************************************************************************/
tBleStatus Acc_Update(AxesRaw_t *x_axes, AxesRaw_t *g_axes)
{  
  uint8_t buff[2+2*6];
  tBleStatus ret;
  uint32_t time = HAL_VTimerDiff_ms_sysT32(HAL_VTimerGetCurrentTime_sysT32(), start_time);
  
  HOST_TO_LE_16(buff,time);
  
  HOST_TO_LE_16(buff+2,-x_axes->AXIS_Y);
  HOST_TO_LE_16(buff+4, x_axes->AXIS_X);
  HOST_TO_LE_16(buff+6,-x_axes->AXIS_Z);
    
  HOST_TO_LE_16(buff+8,g_axes->AXIS_Y);
  HOST_TO_LE_16(buff+10,g_axes->AXIS_X);
  HOST_TO_LE_16(buff+12,g_axes->AXIS_Z);
  
  ret = aci_gatt_update_char_value_ext(connection_handle,HWServW2STHandle, AccGyroMagCharHandle, 1, 2+2*6, 0, 2+2*6, buff);
  if (ret != BLE_STATUS_SUCCESS){
    PRINTF("Error while updating Acceleration characteristic: 0x%02X\n",ret) ;
    return BLE_STATUS_ERROR ;
  }

  return BLE_STATUS_SUCCESS;    
}

/*******************************************************************************
* Function Name  : Read_Request_CB.
* Description    : Update the sensor valuse.
* Input          : Handle of the characteristic to update.
* Return         : None.
*******************************************************************************/
void Read_Request_CB(uint16_t handle)
{
  tBleStatus ret;
  
  if(handle == AccGyroMagCharHandle + 1)
  {
    AxesRaw_t x_axes, g_axes;
    if (GetAccAxesRaw(&x_axes, &g_axes) == IMU_6AXES_OK) {
      Acc_Update(&x_axes, &g_axes);
    }
  }
  else 
    if(handle == EnvironmentalCharHandle + 1)
    {
      float data_t, data_p;
      xLPS25HBDrv->GetPressure(&data_p);
      xLPS25HBDrv->GetTemperature(&data_t);
      BlueMS_Environmental_Update((int32_t)(data_p *100), (int16_t)(data_t * 10)); 
    }

  if(connection_handle !=0)
  { 
    ret = aci_gatt_allow_read(connection_handle);
    if (ret != BLE_STATUS_SUCCESS)
    {
      SdkEvalLedOn(LED3); 
    }
  }
}


tBleStatus BlueMS_Environmental_Update(int32_t press, int16_t temp)
{  
  tBleStatus ret;
  uint8_t buff[8];
  uint32_t time = HAL_VTimerDiff_ms_sysT32(HAL_VTimerGetCurrentTime_sysT32(), start_time);
  HOST_TO_LE_16(buff, time);
  
  HOST_TO_LE_32(buff+2,press);
  HOST_TO_LE_16(buff+6,temp);
  
  ret = aci_gatt_update_char_value_ext(connection_handle,HWServW2STHandle, EnvironmentalCharHandle, 1,8,0, 8, buff);
  if (ret != BLE_STATUS_SUCCESS){
          PRINTF("Error while updating TEMP characteristic: 0x%04X\n",ret) ;
          return BLE_STATUS_ERROR ;
  }
  
  return BLE_STATUS_SUCCESS;	
}

