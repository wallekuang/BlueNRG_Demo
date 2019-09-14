/******************** (C) COPYRIGHT 2018 STMicroelectronics ********************
* File Name          : mouse.c
* Author             : AMS - VMA RF Application Team
* Version            : V1.1.0
* Date               : 05-October-2018
* Description        : BlueNRG-1 mouse init and process data functions
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
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "BlueNRG1_conf.h"
#include "ble_const.h"
#include "bluenrg1_stack.h"
#include "hid_peripheral.h"
#include "hid_peripheral_config.h"
#include "sleep.h"
#include "LSM6DS3_hal.h"
#include "clock.h"

#include "mouse.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct {
  int32_t AXIS_X;
  int32_t AXIS_Y;
  int32_t AXIS_Z;
} AxesAccRaw_TypeDef, AxesGyrRaw_TypeDef;

/* Private define ------------------------------------------------------------*/

#define IDLE_CONNECTION_TIMEOUT (120*1000) // 2 min

#define MOUSE_NOTIFICATION_TIMER 0
#define WAKEUP_NOTIFY        10 // 10 ms

#define BUTTON_LEFT_PRESSED        1<<0
#define BUTTON_RIGHT_PRESSED       1<<1
#define BUTTON_STATUS_CHANGED_MASK 1<<2

#define STEADY_THRESHOLD 3 //8
#define X_THRESHOLD      5
#define Y_THRESHOLD      5


#define SENSOR_DEVIATION_THRESHOLD 25
#define NUMBER_GYRO_TO_SKIP        40
#define MAX_COORD_VALUE            2047
#define MIN_COORD_VALUE            -2047

/* The sensor calibration values are stored in the FLASH
 * starting from this address with the followng order:
 * - VALID_WORD = 0xFCECBCCC
 * - OFFSET_X
 * - OFFSET_Y
 * - OFFSET_Z
 */
#define FLASH_SENSOR_OFFSET_ADDR  0x10067FE8
#define VALID_SENSOR_CALIBRATION  0xFCECBCCC

/* Private macro -------------------------------------------------------------*/
#define DEBUG 1
#ifdef DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define abs(x) (x>0) ? x : -x
#define HOST_TO_LE_32(buf, val)    ( ((buf)[0] =  (uint8_t) (val)     ) , \
                                   ((buf)[1] =  (uint8_t) (val>>8)  ) , \
                                   ((buf)[2] =  (uint8_t) (val>>16) ) , \
                                   ((buf)[3] =  (uint8_t) (val>>24) ) ) 


/* Private variables ---------------------------------------------------------*/

hidService_Type hid_param;

uint8_t dev_name[]={'S', 'T', 'M', 'o', 'u', 's', 'e'}; 

// Mouse report descriptor
uint8_t reportDesc[] = {
  0x05, 0x01,        //USAGE_PAGE(Generic Desktop)
  0x09, 0x02,        //USAGE(Mouse)
  0xA1, 0x01,        //COLLECTION(Application)
  0x85, REPORT_ID,   // REPORT ID
  0x09, 0x01,        //USAGE(Pointer)
  0xA1, 0x00,        //COLLECTION(Physical)
  0x05, 0x09,        //USAGE_PAGE(Button)
  0x19, 0x01,        //USAGE_MINIMUM(Button 1)
  0x29, 0x03,        //USAGE_MAXIMUM(Button 3)
  0x15, 0x00,        //LOGICAL_MINIMUM(0)
  0x25, 0x01,        //LOGICAL_MAXIMUM(1)
  0x75, 0x01,        //REPORT_SIZE(1)
  0x95, 0x03,        //REPORT_COUNT(3)
  0x81, 0x02,        //INPUT(Data, Variable, Absolute) ; Button states
  0x75, 0x05,        //REPORT_SIZE(5)
  0x95, 0x01,        //REPORT_COUNT(1)
  0x81, 0x01,        //INPUT(Constant, Variable, Absolute) ; Reserved bits
  0x05, 0x01,        //USAGE_PAGE(Generic Desktop) 
  0x09, 0x30,        //USAGE(X)
  0x09, 0x31,        //USAGE(Y)
  0x16, 0x01, 0xF8,  //LOGICAL_MINIMUM(-2047)
  0x26, 0xFF, 0x07,  //LOGICAL_MAXIMUM(2047)
  0x75, 0x0C,        //REPORT_SIZE(12 bits)
  0x95, 0x02,        //REPORT_COUNT(2)
  0x81, 0x06,        //INPUT(Data, Variable, Relative) ; X & Y
  0x09, 0x38,        //USAGE(Z)
  0x15, 0x81,        //LOGICAL_MINIMUM(-127)
  0x25, 0x7F,        //LOGICAL_MAXIMUM(127)
  0x75, 0x08,        //REPORT_SIZE(8)
  0x95, 0x01,        //REPORT_COUNT(1)
  0x81, 0x06,        //INPUT(Data, Variable, Relative) ; Z
  0xC0,              //END_COLLECTION
  0xC0               //END_COLLECTION
};

uint8_t rightButtonPressed, leftButtonPressed;
static IMU_6AXES_DrvTypeDef *Imu6AxesDrv = NULL;
int32_t offset_x, offset_y, offset_z;

/* Private Functions ---------------------------------------------------------*/

static void storeCalibrationData(int32_t offset_x, int32_t offset_y, int32_t offset_z)
{
  FLASH_ProgramWord(FLASH_SENSOR_OFFSET_ADDR,    VALID_SENSOR_CALIBRATION);
  FLASH_ProgramWord(FLASH_SENSOR_OFFSET_ADDR+4,  offset_x);
  FLASH_ProgramWord(FLASH_SENSOR_OFFSET_ADDR+8,  offset_y);
  FLASH_ProgramWord(FLASH_SENSOR_OFFSET_ADDR+12, offset_z);
}

static uint8_t retrieveCalibrationData(int32_t *offset_x, int32_t *offset_y, int32_t *offset_z)
{
  volatile uint32_t* sensor_calib_value;

  sensor_calib_value = (volatile uint32_t*)FLASH_SENSOR_OFFSET_ADDR;

  if (*sensor_calib_value != VALID_SENSOR_CALIBRATION)
    return FALSE;

  sensor_calib_value++;
  *offset_x = *sensor_calib_value;
  sensor_calib_value++;
  *offset_y = *sensor_calib_value;
  sensor_calib_value++;
  *offset_z = *sensor_calib_value;

  return TRUE;
}

static void sensorCalibration(int32_t *offset_x, int32_t *offset_y, int32_t *offset_z)
{
  AxesGyrRaw_TypeDef pGyrData;
  uint16_t nmb_elements, i;
  float oldx_m, newx_m, oldx_s, newx_s;
  float oldy_m, newy_m, oldy_s, newy_s;
  float oldz_m, newz_m, oldz_s, newz_s;
  uint32_t deviation_x, deviation_y, deviation_z;
  uint8_t sensorCalibrated;

  if (retrieveCalibrationData(offset_x, offset_y, offset_z)) {
    SdkEvalLedOff(LED1);
    return;
  }
  
  nmb_elements = 0;
  oldx_m=oldy_m=oldz_m=0;
  newx_m=newy_m=newz_m=0;
  oldx_s=oldy_s=oldz_s=0;
  newx_s=newy_s=newz_s=0;
  sensorCalibrated = FALSE;

  for (i=0; i<400; i++) {
    Imu6AxesDrv->Get_G_Axes((int32_t *)&pGyrData);
    Clock_Wait(1);
    if ((i%200) == 0)
      SdkEvalLedToggle(LED1);
  }
  
  /* Offset caluclation from standard deviation and average */
  while (!sensorCalibrated) {
    nmb_elements = 0;
    for (i=0; i<200; i++) {
      nmb_elements++;
      Imu6AxesDrv->Get_G_Axes((int32_t *)&pGyrData);
    
      if (nmb_elements == 1) {
        oldx_m = pGyrData.AXIS_X;
        oldy_m = pGyrData.AXIS_Y;
        oldz_m = pGyrData.AXIS_Z;
        oldx_s=oldy_s=oldz_s=0;
      } else {
        newx_m = oldx_m + (pGyrData.AXIS_X - oldx_m)/nmb_elements;
        newx_s = oldx_s + (pGyrData.AXIS_X - oldx_m)*(pGyrData.AXIS_X - newx_m);
        oldx_m = newx_m;
        oldx_s = newx_s;

        newy_m = oldy_m + (pGyrData.AXIS_Y - oldy_m)/nmb_elements;
        newy_s = oldy_s + (pGyrData.AXIS_Y - oldy_m)*(pGyrData.AXIS_Y - newy_m);
        oldy_m = newy_m;
        oldy_s = newy_s;

        newz_m = oldz_m + (pGyrData.AXIS_Z - oldz_m)/nmb_elements;
        newz_s = oldz_s + (pGyrData.AXIS_Z - oldz_m)*(pGyrData.AXIS_Z - newz_m);
        oldz_m = newz_m;
        oldz_s = newz_s;
      }

      Clock_Wait(2);
    }

    /* Offset when the mouse is stopped */
    *offset_x = (int32_t)newx_m;
    *offset_y = (int32_t)newy_m;
    *offset_z = (int32_t)newz_m;
    
    deviation_x = (uint32_t)sqrt(newx_s/(nmb_elements - 1));
    deviation_y = (uint32_t)sqrt(newy_s/(nmb_elements - 1));
    deviation_z = (uint32_t)sqrt(newz_s/(nmb_elements - 1));

    if ((deviation_x <= SENSOR_DEVIATION_THRESHOLD) &&
        (deviation_y <= SENSOR_DEVIATION_THRESHOLD) &&
        (deviation_z <= SENSOR_DEVIATION_THRESHOLD))
      sensorCalibrated = TRUE;

    SdkEvalLedToggle(LED1);
  }
  SdkEvalLedOff(LED1);

  storeCalibrationData(*offset_x, *offset_y, *offset_z);
}

static void sensorInit(void)
{
  AxesAccRaw_TypeDef pAccData;
  AxesGyrRaw_TypeDef pGyrData;

  /* LSM6DS3 library setting */
  IMU_6AXES_InitTypeDef InitStructure;
  Imu6AxesDrv = &LSM6DS3Drv;
  
  InitStructure.G_FullScale      = 125.0f;
  InitStructure.G_OutputDataRate = 13.0f;
  InitStructure.G_X_Axis         = 1;
  InitStructure.G_Y_Axis         = 1;
  InitStructure.G_Z_Axis         = 1;
  InitStructure.X_FullScale      = 2.0f;
  InitStructure.X_OutputDataRate = 13.0f;
  InitStructure.X_X_Axis         = 1;
  InitStructure.X_Y_Axis         = 1;
  InitStructure.X_Z_Axis         = 1;  
  
  /* LSM6DS3 initiliazation */
  Imu6AxesDrv->Init(&InitStructure);

  /* Disable the MEMS + GYRO interrupt. We read the data in polling every 10 ms */
  GPIO_EXTICmd(LSM6DS3_IRQ_PIN, DISABLE);
  GPIO_ClearITPendingBit(LSM6DS3_IRQ_PIN);
  
  /* Clear first previous data */
  Imu6AxesDrv->Get_X_Axes((int32_t *)&pAccData);
  Imu6AxesDrv->Get_G_Axes((int32_t *)&pGyrData); 

  /* Button Init */
  SdkEvalPushButtonInit(BUTTON_1);
  SdkEvalPushButtonInit(BUTTON_2);
  SdkEvalPushButtonIrq(BUTTON_1, IRQ_ON_FALLING_EDGE); 
  SdkEvalPushButtonIrq(BUTTON_2, IRQ_ON_FALLING_EDGE);

  /* Sensor Calibration */
  sensorCalibration(&offset_x, &offset_y, &offset_z);
}

static void getButtonStatus(uint8_t *buttonMask)
{
  static uint8_t lastButtonStatus=0, timesToSkip=0;

  *buttonMask = 0;

  if ((SdkEvalPushButtonGetState(BUTTON_1) == RESET) || leftButtonPressed) {
    *buttonMask |= BUTTON_LEFT_PRESSED;
    leftButtonPressed = FALSE;
  }

  if ((SdkEvalPushButtonGetState(BUTTON_2) == RESET) || rightButtonPressed) {
    *buttonMask |= BUTTON_RIGHT_PRESSED;
    rightButtonPressed = FALSE;
  } 

  if (lastButtonStatus != *buttonMask) {
    lastButtonStatus = *buttonMask;
    *buttonMask |= BUTTON_STATUS_CHANGED_MASK;
    timesToSkip = NUMBER_GYRO_TO_SKIP;
  }

  if (timesToSkip != 0) {
    timesToSkip--;
    *buttonMask |= BUTTON_STATUS_CHANGED_MASK;
  }
}

static void newMouseValue(int16_t *x, int16_t *y)
{
  AxesGyrRaw_TypeDef pGyrData;
  AxesAccRaw_TypeDef pAccData;
  double gain;
  int32_t Z_correct, Y_correct;
  
  *x = 0;
  *y = 0;

  /* Read data from LSM6DS3 */
  Imu6AxesDrv->Get_X_Axes((int32_t *)&pAccData);
  Imu6AxesDrv->Get_G_Axes((int32_t *)&pGyrData);
#ifdef SENSOR_DEBUG
  PRINTF("Acc:%d,%d,%d\n",pAccData.AXIS_X, pAccData.AXIS_Y, pAccData.AXIS_Z);
  PRINTF("Gyro:%d,%d,%d\n", pGyrData.AXIS_X, pGyrData.AXIS_Y, pGyrData.AXIS_Z);
#endif
  
  gain = 0.3;
  Z_correct = (pGyrData.AXIS_Z - offset_z);
  Y_correct = (pGyrData.AXIS_Y - offset_y);
    
  *x = (int16_t)(-Z_correct*gain/1000);
  *y = (int16_t)(-Y_correct*gain/1000);

  if (*x > MAX_COORD_VALUE) {
    *x = MAX_COORD_VALUE;
  } else {
    if (*x < MIN_COORD_VALUE) {
      *x = MIN_COORD_VALUE;
    }
  }

  if (*y > MAX_COORD_VALUE) {
    *y = MAX_COORD_VALUE;
  } else {
    if (*y < MIN_COORD_VALUE) {
      *y = MIN_COORD_VALUE;
    }
  }
}

/* Public Functions ----------------------------------------------------------*/

void Sensor_IrqHandler(void)
{
  /* Check if GPIO pin 13 interrupt event occured */
  if(GPIO_GetITPendingBit(GPIO_Pin_13) == SET) {
    GPIO_ClearITPendingBit(GPIO_Pin_13);
    leftButtonPressed = TRUE;
    hidSetNotificationPending(TRUE);
  }
  if (GPIO_GetITPendingBit(GPIO_Pin_5) == SET) { 
    GPIO_ClearITPendingBit(GPIO_Pin_5);
    rightButtonPressed = TRUE;
    hidSetNotificationPending(TRUE);
  }    
}

void setDefaultHidParams(void)
{
  hid_param.bootSupport = FALSE;
  hid_param.reportSupport = TRUE;
  hid_param.reportReferenceDesc[0].ID = REPORT_ID;
  hid_param.reportReferenceDesc[0].type = INPUT_REPORT;
  hid_param.reportReferenceDesc[0].length = 5;
  hid_param.isBootDevKeyboard = FALSE;
  hid_param.isBootDevMouse = FALSE;
  hid_param.externalReportEnabled = 0;
  hid_param.includedServiceEnabled = FALSE;
  hid_param.informationCharac[0] = 0x01;
  hid_param.informationCharac[1] = 0x01;
  hid_param.informationCharac[2] = 0;
  hid_param.informationCharac[3] = 0x01;
}

void setTestHidParams(void)
{
  hid_param.bootSupport = TRUE; 
  hid_param.reportSupport = TRUE;
  hid_param.reportReferenceDesc[0].ID = REPORT_ID;
  hid_param.reportReferenceDesc[0].length = 5;
  hid_param.isBootDevKeyboard = FALSE;
  hid_param.isBootDevMouse = TRUE; 
  hid_param.externalReportEnabled = 1;  
  hid_param.includedServiceEnabled = FALSE;
  hid_param.informationCharac[0] = 0x01;
  hid_param.informationCharac[1] = 0x01;
  hid_param.informationCharac[2] = 0;
  hid_param.informationCharac[3] = 0x01;
}

void setHidParams_ReportType(uint8_t reportType)
{
  hid_param.reportReferenceDesc[0].type = reportType;
} 

uint8_t Configure_HidPeripheral(void)
{
  uint8_t ret;
  batteryService_Type battery;
  devInfService_Type devInf;
  hidService_Type hid;
  connParam_Type connParam;
  uint8_t addr[] = PERIPHERAL_PUBLIC_ADDRESS;

  /* HID Peripheral Init */
  connParam.interval_min = 6;
  connParam.interval_max = 12;
  connParam.slave_latency = 0;
  connParam.timeout_multiplier = 300;
  ret = hidDevice_Init(IO_CAP_NO_INPUT_NO_OUTPUT /*IO_CAP_DISPLAY_ONLY*/, connParam, sizeof(dev_name), dev_name, addr);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error in hidDevice_Init() 0x%02x\n", ret);
    return ret;
  }
  /* Set the HID Peripheral Security */
  ret = hidSetDeviceSecurty(FALSE, FALSE, 123456);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error in hidSetDeviceSecurty() 0x%02x\n", ret);
    return ret;
  }

  /* Set the HID Idle Timeout */
  hidSetIdleTimeout(IDLE_CONNECTION_TIMEOUT);
  
  /* Set the TX Power */
  ret = hidSetTxPower(4);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error in hidSetTxPower() 0x%02x\n", ret);
    return ret;
  }

  /**** Setup the GATT Database ****/
  /* Battery Service */
  battery.inReportMap = FALSE;
  /* Device Information Service */
  memcpy(devInf.manufacName, "ST Micro", 8);
  memcpy(devInf.modelNumber, "0001", 4);
  memcpy(devInf.fwRevision, "0630", 4);
  memcpy(devInf.swRevision, "0001", 4);
  devInf.pnpID[0] = 0x01;
  devInf.pnpID[1] = 0x30;
  devInf.pnpID[2] = 0x00;
  devInf.pnpID[3] = 0xfc;
  devInf.pnpID[4] = 0x00;
  devInf.pnpID[5] = 0xec;
  devInf.pnpID[6] = 0x00;
 
  /* HID Service */
  hid = hid_param;  
  hid.reportDescLen = sizeof(reportDesc);
  hid.reportDesc = reportDesc;

  ret = hidAddServices(&battery, &devInf, &hid);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error in hidAddServices() 0x%02x\n", ret);
    return ret;
  }

  /* Init the Acc + Gyro sensor */
  rightButtonPressed = FALSE;
  leftButtonPressed = FALSE;
  sensorInit();
  
  /* Set the HID Peripheral device discoverable */
  ret = hidSetDeviceDiscoverable(LIMITED_DISCOVERABLE_MODE, sizeof(dev_name), dev_name);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error in hidSetDeviceDiscoverable() 0x%02x\n", ret);
    return ret;
  }
  
  PRINTF("HID Mouse Configured\n");

  return BLE_STATUS_SUCCESS;
}

void DevicePowerSaveProcedure(void)
{
  uint8_t ret, wakeup_source, wakeup_level, timer_set_flag=FALSE;
  SdkEvalButton Button = USER_BUTTON;
  ret = BLE_STATUS_SUCCESS;

  /* If the device is Busy during the HID configuration state machines apply Sleep Mode with the wake timer enabled*/
  if ((hidDeviceStatus() & HID_DEVICE_BUSY) ||
      (hidDeviceStatus() & HID_DEVICE_READY_TO_NOTIFY)) {
        /* If the HID device is ready to notify we read the gyro+mems every 10 ms */
        if (hidDeviceStatus() & HID_DEVICE_READY_TO_NOTIFY) {
          HAL_VTimerStart_ms(MOUSE_NOTIFICATION_TIMER, WAKEUP_NOTIFY);
          BTLE_StackTick();
          timer_set_flag = TRUE;
        }
        ret = BlueNRG_Sleep(SLEEPMODE_NOTIMER, 0, 0);
        if (timer_set_flag){
          HAL_VTimer_Stop(MOUSE_NOTIFICATION_TIMER);
          timer_set_flag = FALSE;
        }
  }

  /* If the device is disconnected or something fails in the connection procedure or
     the device is in Idle state apply Sleep Mode with lowest power save */
  if ((hidDeviceStatus() & HID_DEVICE_READY_TO_DEEP_SLEEP) || 
      (hidDeviceStatus() & HID_DEVICE_NOT_CONNECTED)) {
         if (Button == BUTTON_1)
         {
             wakeup_source = WAKEUP_IO13;
             wakeup_level = (WAKEUP_IOx_LOW << WAKEUP_IO13_SHIFT_MASK);
         }
         ret = BlueNRG_Sleep(SLEEPMODE_NOTIMER, wakeup_source, wakeup_level);
  }

  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("BlueNRG_Sleep() error 0x%02x\r\n", ret);
  }
}

void DeviceInputData(void)
{
  int16_t x, y;
  uint8_t ret, buff[5], buttonMask;
  static uint8_t connBlinkLed = 0;

  if (hidDeviceStatus() & HID_DEVICE_READY_TO_NOTIFY) {

    if ((connBlinkLed > 200) && (connBlinkLed < 210)) {
      SdkEvalLedOn(LED3);
    } else {
      SdkEvalLedOff(LED3);
      if (connBlinkLed > 210)
        connBlinkLed = 0;
    }
    connBlinkLed++;
    
    buttonMask = 0;
    x = 0;
    y = 0;

    /* Read the button Status */
    getButtonStatus(&buttonMask);
    /* Read the Acc + Gyro status */
    if (!(buttonMask & BUTTON_STATUS_CHANGED_MASK))
      newMouseValue(&x, &y);

    if ((x != 0) || (y != 0) || (buttonMask != 0)) {
      buttonMask &= ~(uint8_t)(BUTTON_STATUS_CHANGED_MASK); // Flag to be cleared before to send the packet
      buff[0] = buttonMask;
      HOST_TO_LE_32(&buff[1], ((uint32_t)(x&0x0FFF)|(uint32_t)(y&0x0FFF)<<12));
      ret = hidSendReport(REPORT_ID, INPUT_REPORT, sizeof(buff), buff);

      if (ret == BLE_STATUS_SUCCESS)
        hidSetNotificationPending(TRUE);
      else
        PRINTF("Error while sending the hid report. (error = 0x%02x)\n", ret) ;
    }
  } else {
    SdkEvalLedOff(LED3);
  }
}

void APP_Tick(void)
{
  /* HID library Tick */
  hidDevice_Tick();
  
  /* Process Device Input Data */
  DeviceInputData();  
}

/**** HID/HOGP Callback functions ****/
void hidSetReport_CB(uint8_t ID, uint8_t type, uint8_t len, uint8_t *data)
{
  uint8_t i;

  PRINTF("Set Report ID = 0x%x, Type = 0x%x, len = 0x%x data:", ID, type, len);
  for (i=0; i<len; i++){
    PRINTF("%x - ", data[i]);
  }
  PRINTF("\n");
}

void hidGetReport_CB(uint8_t ID, uint8_t type)
{
  uint8_t ret, len;

  PRINTF("Get Report Callback ID = %d\n", ID);
  if (type == INPUT_REPORT)  
  {  
    uint8_t data[5]={0,0,0,0,0};
    len = 4;
    ret = hidUpdateReportValue(ID, type, len, data);
  }
  else if (type == BOOT_MOUSE_INPUT_REPORT)
  {  
    uint8_t data_BootMouse[3]={0,0,0}; 
    len = 2;
    ret = hidUpdateReportValue(ID, type, len, data_BootMouse);
  }  
  if (ret != BLE_STATUS_SUCCESS)
    PRINTF("Update report value Error during a hidGetReport_CB() procedure: 0x%02x\n", ret);
}

void hidChangeProtocolMode_CB(uint8_t mode)
{
  PRINTF("Protocol mode changed to %x\n", mode);
}

void hidControlPoint_CB(uint8_t value)
{
  if (value == 1)
#ifdef PTS_AUTOMATING 
    PRINTF("(%c%c1) HID Control Point Command: 1 (Exit Suspend)\n", PTS_CODE, HIDS_PTS_CODE);
#else
    PRINTF("Control Point value changed to 1. Host Exit Suspend state\n");
#endif
  else if (value == 0)
#ifdef PTS_AUTOMATING 
    PRINTF("(%c%c2) HID Control Point Command: 0 (Suspend)\n", PTS_CODE, HIDS_PTS_CODE);
#else
    PRINTF("Control Point value changed to 0. Host in Suspend state\n");
#endif   
}

/***************** BlueNRG-1 stack events **********************************/

void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason)
{
  HID_Lib_hci_disconnection_complete_event(Status, Connection_Handle, Reason);
  PRINTF("Disconnection Complet Event\n"); 
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
  HID_Lib_hci_le_connection_complete_event(Status, Connection_Handle,
                                           Role, Peer_Address_Type,
                                           Peer_Address, Conn_Interval,
                                           Conn_Latency, Supervision_Timeout,
                                           Master_Clock_Accuracy);
  PRINTF("Connection Complet Event\n");        
}

void aci_gap_limited_discoverable_event()
{
  HID_Lib_aci_gap_limited_discoverable_event();
}

void aci_gap_pass_key_req_event(uint16_t Connection_Handle)
{
  HID_Lib_aci_gap_pass_key_req_event(Connection_Handle);
}

void aci_gap_pairing_complete_event(uint16_t Connection_Handle,
                                    uint8_t Status,
                                    uint8_t Reason)
{
  HID_Lib_aci_gap_pairing_complete_event(Connection_Handle, Status);
}

void aci_gap_bond_lost_event()
{
  HID_Lib_aci_gap_bond_lost_event();
}

void aci_gatt_attribute_modified_event(uint16_t Connection_Handle,
                                       uint16_t Attr_Handle,
                                       uint16_t Offset,
                                       uint16_t Attr_Data_Length,
                                       uint8_t Attr_Data[])
{
  HID_Lib_aci_gatt_attribute_modified_event(Connection_Handle, Attr_Handle,
                                            Offset, Attr_Data_Length,
                                            Attr_Data);
#ifdef PTS_AUTOMATING   
  if ((Attr_Data_Length == 2) && (Attr_Data[1] == 0x00 && Attr_Data[0] == 0x01))
  {
    PRINTF("(%c%c3) Enabled Char to Notify\n\n", PTS_CODE, HIDS_PTS_CODE);
  }  
#endif  
}

void aci_gatt_read_permit_req_event(uint16_t Connection_Handle,
                                    uint16_t Attribute_Handle,
                                    uint16_t Offset)
{
  HID_Lib_aci_gatt_read_permit_req_event(Connection_Handle, Attribute_Handle,
                                         Offset);
}

void aci_l2cap_connection_update_resp_event(uint16_t Connection_Handle,
                                            uint16_t Result)
{
  HID_Lib_aci_l2cap_connection_update_resp_event(Connection_Handle, Result);
}

void aci_l2cap_proc_timeout_event(uint16_t Connection_Handle,
                                  uint8_t Data_Length,
                                  uint8_t Data[])
{
  HID_Lib_aci_l2cap_proc_timeout_event(Connection_Handle, Data_Length,
                                       Data);
}

void HAL_VTimerTimeoutCallback(uint8_t timerNum)
{
  HID_Lib_HAL_VTimerTimeoutCallback(timerNum);
}
