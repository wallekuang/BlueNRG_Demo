/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
 * File Name          : application_demo.c
 * Author             : AMG RF Application team
 * Version            : V1.0.0
 * Date               : 25-May-2016
 * Description        : Application demo to show ANCS application profile
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
#include "SDK_EVAL_Config.h"
#include "clock.h"
#include "sleep.h"
#include "osal.h"
#include "ancs.h"

/* Private define ------------------------------------------------------------*/

/* Advertising Interval */
#define  ADV_INTERVAL_MIN_MS  100
#define  ADV_INTERVAL_MAX_MS  120

/* Application Timer */
#define APPL_TIMER             1
#define PAIRING_TIMEOUT        2000  // 2 s
#define SHOW_NOTIF_TIMEOUT     500  // 500 ms
#define PERFORM_ACTION_TIMEOUT 5000 // 5 sec

/* MAX Number of entry in the context lists */
#define MAX_NMB_NOTIFY 50
#define MAX_DISPLAY_NAME_LEN 50
#define MAX_DATA_LIST_LEN    500

/* Invalid Notify Entry */
#define INVALID_NOTIFY_ENTRY 0xFFFF

/* State Machine */
#define APP_IDLE_STATE                         0x00
#define APP_CONNECTED_STATE                    0x01
#define APP_DISCONNECTED_STATE                 0x02
#define APP_BONDED_STATE                       0x03
#define APP_GET_NOTIF_ATTR_STATE               0x04
#define APP_GET_APP_ATTR_STATE                 0x05
#define APPL_SECURITY_REQ_STATE                0x06
#define APP_PERFORM_NOTIFICATION_ACTION_STATE  0x07
#define APP_WAIT_START_ENC                     0x08
#define APPL_CHECK_NOTIFICATION_STATE          0x09

/* Private typedef -----------------------------------------------------------*/
typedef struct notifyListS {
  uint8_t    used;
  EventID    evID;
  EventFlags evFlag;
  CategoryID catID;
  uint8_t    catCount;
  uint32_t   notifUID;  
} notifyList_type;
  
typedef struct app_contextS {
  uint8_t state;
  uint16_t conn_handle;
  uint8_t appDisplayName_len;
  uint8_t appDisplayName[MAX_DISPLAY_NAME_LEN];
  uint8_t list[MAX_DATA_LIST_LEN];
  notifyList_type notifyList[MAX_NMB_NOTIFY];
  uint16_t notifyEntry;
  uint8_t notifyShowed;
  uint32_t startTime;
  uint8_t genericFlag;
} app_context_type;

/* Private macro -------------------------------------------------------------*/

#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
do {\
    uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; uuid_struct[2] = uuid_2; uuid_struct[3] = uuid_3; \
        uuid_struct[4] = uuid_4; uuid_struct[5] = uuid_5; uuid_struct[6] = uuid_6; uuid_struct[7] = uuid_7; \
            uuid_struct[8] = uuid_8; uuid_struct[9] = uuid_9; uuid_struct[10] = uuid_10; uuid_struct[11] = uuid_11; \
                uuid_struct[12] = uuid_12; uuid_struct[13] = uuid_13; uuid_struct[14] = uuid_14; uuid_struct[15] = uuid_15; \
}while(0)

#define COPY_ANCS_SERVICE_UUID(uuid_struct)             COPY_UUID_128(uuid_struct, 0x79, 0x05, 0xF4, 0x31, 0xB5, 0xCE, 0x4E, 0x99, 0xA4, 0x0F, 0x4B, 0x1E, 0x12, 0x2D, 0x00, 0xD0)

/* Private variables ---------------------------------------------------------*/
app_context_type app_context;
extern uint32_t dyn_alloc_a[];

/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
extern uint8_t __io_getcharNonBlocking(uint8_t *data);

/* Private functions ---------------------------------------------------------*/

static void show_eventID(EventID evID)
{
  switch(evID) {
  case EventIDNotificationAdded:
    printf("\r\n** Event: Added\r\n");
    break;
  case EventIDNotificationModified:
    printf("\r\n** Event: Modified\r\n");
    break;
  case EventIDNotificationRemoved:
    printf("\r\n** Event: Removed\r\n");
    break;
  }
}

static void show_eventFlag(EventFlags evFlag)
{
  switch (evFlag) {
  case EventFlagSilent:
    printf("** Type: Silent\r\n");
    break;
  case EventFlagImportant:
    printf("** Type: Important\r\n");
    break;
  case EventFlagPreExisting:
    printf("** Type: Pre Existing\r\n");
    break;
  case EventFlagPositiveAction:
    printf("** Type: Positive Action\r\n");
    break;
  case EventFlagNegativeAction:
    printf("** Type: Negative Action\r\n");
    break;
  }
}

static void show_categoryID(CategoryID catID)
{
  switch (catID) {
  case CategoryIDOther:
    printf("** Category: Other\r\n");
    break;
  case CategoryIDIncomingCall:
    printf("** Category: Incoming Call\r\n");
    break;
  case CategoryIDMissedCall:
    printf("** Category: Missed Call\r\n");
    break;
  case CategoryIDVoicemail:
    printf("** Category: Voice Mail\r\n");
    break;
  case CategoryIDSocial:
    printf("** Category: Social\r\n");
    break;
  case CategoryIDSchedule:
    printf("** Category: Schedule\r\n");
    break;
  case CategoryIDEmail:
    printf("** Category: Email\r\n");
    break;
  case CategoryIDNews:
    printf("** Category: News\r\n");
    break;
  case CategoryIDHealthAndFitness:
    printf("** Category: Healt and Fitness\r\n");
    break;
  case CategoryIDBusinessAndFinance:
    printf("** Category: Business and Finance\r\n");
    break;
  case CategoryIDLocation:
    printf("** Category: Location\r\n");
    break;
  case CategoryIDEntertainment:
    printf("** Category: Entertainment\r\n");
    break;
  }
}

static void showAttr(NotificationAttributeID attrID)
{
  switch (attrID) {
  case NotificationAttributeIDAppIdentifier:
    printf("** App Identifier: ");
    break;
  case NotificationAttributeIDTitle:
    printf("** Title: ");
    break;
  case NotificationAttributeIDSubtitle:
    printf("** SubTitle: ");
    break;
  case NotificationAttributeIDMessage:
    printf("** Message: ");
    break;
  case NotificationAttributeIDMessageSize:
    printf("** Message Size: ");
    break;
  case NotificationAttributeIDDate:
    printf("** Date: ");
    break;
  case NotificationAttributeIDPositiveActionLabel:
    printf("** Positive ID: ");
    break;
  case NotificationAttributeIDNegativeActionLabel:
    printf("** Negative ID: ");
    break;
  }
}

static void showAppAttr(AppAttributeID appAttrID)
{
  switch (appAttrID) {
  case AppAttributeIDDisplayName:
    printf("** App Name: ");
    break;
  }
}

static void show_Notification(uint16_t index)
{
  printf("\r\n*************** Notification Received **********************\r\n");
  show_eventID((EventID)app_context.notifyList[index].evID);
  show_eventFlag((EventFlags)app_context.notifyList[index].evFlag);
  show_categoryID((CategoryID)app_context.notifyList[index].catID);
  printf("** Number of Active Notification: %d\r\n", app_context.notifyList[index].catCount);
  printf("** Notification UID: 0x%08x\r\n", (int)app_context.notifyList[index].notifUID);
}

SleepModes App_SleepMode_Check(SleepModes sleepMode)
{
  if (app_context.state == APP_PERFORM_NOTIFICATION_ACTION_STATE)
    return SLEEPMODE_RUNNING;
  else if(SdkEvalComIOTxFifoNotEmpty())
    return SLEEPMODE_RUNNING;
  else
    return SLEEPMODE_NOTIMER;
}

uint8_t application_init(void)
{
  uint8_t bdaddr[] = {0xCC, 0xBC, 0x00, 0xE1, 0x80, 0x02};
  uint8_t ret;
  uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
  uint8_t device_name[] = {'A', 'N', 'C', 'S', 'd', 'e', 'm', 'o'};
  
  /* Set the device public address */
  ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN,
                                  bdaddr);  
  if(ret != BLE_STATUS_SUCCESS) {
    printf("aci_hal_write_config_data() failed: 0x%02x\r\n", ret);
    return ret;
  }
  
  /* Set the TX power -2 dBm */
  aci_hal_set_tx_power_level(1, 4);
  
  /* GATT Init */
  ret = aci_gatt_init();
  if (ret != BLE_STATUS_SUCCESS) {
    printf("aci_gatt_init() failed: 0x%02x\r\n", ret);
    return ret;
  }
  
  /* GAP Init */
  ret = aci_gap_init(GAP_PERIPHERAL_ROLE, 0, sizeof(device_name), &service_handle, &dev_name_char_handle, &appearance_char_handle);
  if (ret != BLE_STATUS_SUCCESS) {
    printf("aci_gap_init() failed: 0x%02x\r\n", ret);
    return ret;
  }
 
  /* Update device name */
  ret = aci_gatt_update_char_value_ext(0,service_handle, dev_name_char_handle,0,sizeof(device_name),0, sizeof(device_name), device_name);
  if(ret != BLE_STATUS_SUCCESS) {
    printf("aci_gatt_update_char_value_ext() failed: 0x%02x\r\n", ret);
    return ret;
  }

  /* Set IO capability */
  ret = aci_gap_set_io_capability(IO_CAP_NO_INPUT_NO_OUTPUT);
  if (ret != BLE_STATUS_SUCCESS) {
    printf("aci_gap_set_io_capability() failed: 0x%02x\r\n", ret);
    return ret;
  }
  /* Set Authentication requirements */
  
  ret = aci_gap_set_authentication_requirement(BONDING,
                                               MITM_PROTECTION_NOT_REQUIRED,
                                               SC_IS_NOT_SUPPORTED,
                                               KEYPRESS_IS_NOT_SUPPORTED,
                                               7, 
                                               16,
                                               DONOT_USE_FIXED_PIN_FOR_PAIRING,
                                               123456,
                                               STATIC_RANDOM_ADDR);
    if(ret != BLE_STATUS_SUCCESS) {
    printf("aci_gap_set_authentication_requirement()failed: 0x%02x\r\n", ret);
    return ret;
  }
  
  printf("BLE Stack Initialized with SUCCESS\n");

  app_context.state = APP_IDLE_STATE;
  
  return BLE_STATUS_SUCCESS;
}

uint8_t application_ADV_Start(void)
{  
  uint8_t ret= BLE_STATUS_SUCCESS;
  uint8_t local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'A','N','C','S','d','e','m','o'};
  uint8_t service_solicit[18];
  uint8_t *ptr;

  /* Add in scan response Service Solicit */
  service_solicit[0] = 0x11;
  service_solicit[1] = AD_TYPE_SERV_SOLICIT_128_BIT_UUID_LIST;
  ptr = &service_solicit[2];
  COPY_ANCS_SERVICE_UUID(ptr);
  
  ret = hci_le_set_scan_response_data(sizeof(service_solicit), service_solicit);
  if (ret != BLE_STATUS_SUCCESS) {
    printf("hci_le_set_scan_response_data() failed: 0x%02x\r\n",ret);
  }

  ret = aci_gap_set_discoverable(ADV_IND,
                                (ADV_INTERVAL_MIN_MS*1000)/625,(ADV_INTERVAL_MAX_MS*1000)/625,
                                 STATIC_RANDOM_ADDR, NO_WHITE_LIST_USE,
                                 sizeof(local_name), local_name, 0, NULL, 0, 0); 
  if(ret != BLE_STATUS_SUCCESS) {
    printf("aci_gap_set_discoverable() failed: 0x%02x\r\n",ret);
  } else {
    printf("Device in Advertising Mode...\r\n");
  }

  return ret;
}

void APP_Tick(void)
{
  uint8_t ret, performAction, nmbBytes;
  ActionID action;
  uint16_t index;
  
  switch (app_context.state) {
  case APP_IDLE_STATE:
    /* ???? */
    break;
  case APP_CONNECTED_STATE:
    {
      ret = HAL_VTimerStart_ms(APPL_TIMER, PAIRING_TIMEOUT);
      if (ret != BLE_STATUS_SUCCESS) {
        printf("Error in HAL_VTimerStart_ms() 0x%02x\r\n", ret);
      }
      SdkEvalLedOn(LED3);
      app_context.state = APP_WAIT_START_ENC;
    }
    break;
  case APP_WAIT_START_ENC:
    /* Nothing to do */
    break;
  case APPL_SECURITY_REQ_STATE:
    {
      ret = aci_gap_slave_security_req(app_context.conn_handle); 
      if (ret != BLE_STATUS_SUCCESS) {
        printf ("Error in call aci_gap_slave_security_req() 0x%02x\r\n", ret);
      }
      app_context.state = APP_IDLE_STATE;
    }
    break;
  case APP_BONDED_STATE:
    HAL_VTimer_Stop(APPL_TIMER);
    ANCS_ConsumerSetup(app_context.conn_handle, sizeof(app_context.list), app_context.list);
    app_context.state = APP_IDLE_STATE;
    break;
  case APPL_CHECK_NOTIFICATION_STATE:
    {
      app_context.notifyEntry = INVALID_NOTIFY_ENTRY;
      for (index=0; index<MAX_NMB_NOTIFY; index++) {
        if (app_context.notifyList[index].used) {
          app_context.notifyEntry = index;
          app_context.notifyShowed = FALSE;
          app_context.state = APP_GET_NOTIF_ATTR_STATE;
          break;
        }
      }
      if (app_context.notifyEntry == INVALID_NOTIFY_ENTRY) {
        ret = HAL_VTimerStart_ms(APPL_TIMER, SHOW_NOTIF_TIMEOUT);
        if (ret != BLE_STATUS_SUCCESS) {
          printf("Error in HAL_VTimerStart_ms() 0x%02x\r\n", ret);
        }
        app_context.state = APP_IDLE_STATE;
      }
    }
    break;
  case APP_GET_NOTIF_ATTR_STATE:
    {
      notificationAttr_type attr;

      attr.UID = app_context.notifyList[app_context.notifyEntry].notifUID;
      attr.appID_flag = TRUE;
      attr.title_flag = TRUE;
      attr.title_max_size = 50;
      attr.subtitle_flag = TRUE;
      attr.subtitle_max_size = 30;
      attr.message_flag = TRUE;
      attr.message_max_size = 300;
      attr.messageSize_flag = TRUE;
      attr.date_flag = TRUE;
      attr.positiveAction_flag = TRUE;
      attr.negativeAction_flag = TRUE;
      ret = ANCS_GetNotificationAttr(attr);
      if (ret != BLE_STATUS_SUCCESS) {
        printf("Error in ANCS_GetNotificationAttr() 0x%02x\r\n", ret);
      }
      app_context.state = APP_IDLE_STATE;
    }
    break;
  case APP_GET_APP_ATTR_STATE:
    {
      ret = ANCS_GetAppAttr(app_context.appDisplayName_len,
                            app_context.appDisplayName);
      if (ret != BLE_STATUS_SUCCESS) {
        printf("Error in ANCS_GetAppAttr() 0x%02x\r\n", ret);
      }
      app_context.state = APP_IDLE_STATE;
    }
    break;
  case APP_PERFORM_NOTIFICATION_ACTION_STATE:
    {
      if (app_context.genericFlag) {
        printf("** Notification Action. Press on the keyboard: A=Accept or R=Reject\r\n");
        printf("** Waiting 5 sec....\r\n");
        app_context.genericFlag = FALSE;
      }
      nmbBytes = __io_getcharNonBlocking(&performAction);
      if ((nmbBytes == 1) ||  (HAL_VTimerDiff_ms_sysT32(HAL_VTimerGetCurrentTime_sysT32(), app_context.startTime) >= PERFORM_ACTION_TIMEOUT)) {
        if (nmbBytes == 1) {
          if ((performAction == 'A') || (performAction == 'a')) {
            action = ActionIDPositive;
          } else {
            action = ActionIDNegative;
          }
          ret = ANCS_PerformNotificationAction(app_context.notifyList[app_context.notifyEntry].notifUID, action);
          if (ret != BLE_STATUS_SUCCESS) 
            printf("Error in ANCS_PerformNotificationAction() 0x%02x\r\n", ret);
          else 
            printf("** Action Performed = %c\r\n", performAction); 
        } else {
          printf("** Action Timeout\r\n");
        }
        app_context.notifyList[app_context.notifyEntry].used = FALSE;
        ret = HAL_VTimerStart_ms(APPL_TIMER, SHOW_NOTIF_TIMEOUT);
        if (ret != BLE_STATUS_SUCCESS) {
          printf("Error in HAL_VTimerStart_ms() 0x%02x\r\n", ret);
        }
        app_context.state = APP_IDLE_STATE;
      }
    }
    break;
  case APP_DISCONNECTED_STATE:
    {
      /* Nothing to do */
      SdkEvalLedOff(LED3);
      /* Start Advertising */
      ret = application_ADV_Start();
      if (ret != BLE_STATUS_SUCCESS) {
        printf("Error in application_ADV_Start() 0x%02x\n", ret);
      }
      app_context.state = APP_IDLE_STATE;
    }
    break;
  }
  ANCS_Tick();
}

void ANCS_SetupComplete_event(uint8_t status)
{
  uint8_t ret;
  if (status == BLE_STATUS_SUCCESS) {
    printf("ANCS Service Configured with SUCCESS\r\n");
  } else {
    printf("ANCS Service Configuration Error (0x%02x)\r\n", status);
  }  
  ret = HAL_VTimerStart_ms(APPL_TIMER, SHOW_NOTIF_TIMEOUT);
  if (ret != BLE_STATUS_SUCCESS) {
    printf("Error in HAL_VTimerStart_ms() 0x%02x\r\n", ret);
  }
  app_context.state = APP_IDLE_STATE;
}

void ANCS_Notification_Source_Received_event(uint8_t status,
                                             EventID evID,
                                             EventFlags evFlag,
                                             CategoryID catID,
                                             uint8_t catCount,
                                             uint32_t notifUID)
{
  uint16_t i;
  
  if (status != BLE_STATUS_SUCCESS) {
    printf("Error in ANCS_Notification_Source_Received_Callback() status = 0x%02x\r\n", status);
    return;
  }

  if (evID == EventIDNotificationRemoved) {
    for (i=0; i < MAX_NMB_NOTIFY; i++) {
      if (app_context.notifyList[i].notifUID == notifUID) {
        app_context.notifyList[i].used = FALSE;
        break;
      }
    }
  } else {
    for (i=0; i < MAX_NMB_NOTIFY; i++) {
      if (!app_context.notifyList[i].used) {
        app_context.notifyList[i].used = TRUE;
        app_context.notifyList[i].evID = evID;
        app_context.notifyList[i].evFlag = evFlag;
        app_context.notifyList[i].catID = catID;
        app_context.notifyList[i].catCount = catCount;
        app_context.notifyList[i].notifUID = notifUID;
        break;
      }
    }
  }
}

void ANCS_GetAttr_event(uint8_t  status,
                        uint8_t  commandID,
                        uint16_t attrLen,
                        uint8_t  *attrList)
{
  uint8_t appId;
  uint16_t len, index;

  if (status != BLE_STATUS_SUCCESS) {
    app_context.notifyList[app_context.notifyEntry].used = FALSE;
    app_context.state = APPL_CHECK_NOTIFICATION_STATE;
    return;
  }

  index = 0;
  if ((commandID == CommandIDGetNotificationAttributes) ||
      (commandID == CommandIDGetAppAttributes)) {
    while (index < attrLen) {
      Osal_MemCpy(&len, &attrList[index+1], 2);
      if (len == 0) {
        index += 3;
      } else {
        if (commandID == CommandIDGetNotificationAttributes) {
          if (!app_context.notifyShowed) {
            show_Notification(app_context.notifyEntry);
            app_context.notifyShowed = TRUE;
          }
          showAttr((NotificationAttributeID)attrList[index]);
          app_context.state = APP_GET_APP_ATTR_STATE; 
        } else {
         showAppAttr((AppAttributeID)attrList[index]);
         if ((app_context.notifyList[app_context.notifyEntry].catID == CategoryIDIncomingCall) ||
             (app_context.notifyList[app_context.notifyEntry].catID == CategoryIDSchedule)) {
           app_context.genericFlag = TRUE;
           app_context.startTime = HAL_VTimerGetCurrentTime_sysT32();
           app_context.state = APP_PERFORM_NOTIFICATION_ACTION_STATE;
         } else {
           app_context.notifyList[app_context.notifyEntry].used = FALSE;
           app_context.state = APPL_CHECK_NOTIFICATION_STATE;
         }
        }
        if (attrList[index] == NotificationAttributeIDAppIdentifier) {
          appId = TRUE;
        } else {
          appId = FALSE;
        }
        index++;
        if (len <= sizeof(app_context.list)) {
          Osal_MemCpy(app_context.list, &attrList[index+2], len);
          app_context.list[len] = '\0';
        } else {
          Osal_MemCpy(app_context.list, &attrList[index+2], sizeof(app_context.list)-1);
          app_context.list[sizeof(app_context.list)-1] = '\0';
        }
        
        index += len + 2;
        if (appId) {
          if ((len+1) <= MAX_DISPLAY_NAME_LEN) {
            Osal_MemCpy(app_context.appDisplayName, app_context.list, len+1);
            app_context.appDisplayName_len = len + 1;
          } else {
            Osal_MemCpy(app_context.appDisplayName, app_context.list, MAX_DISPLAY_NAME_LEN);
            app_context.appDisplayName_len = MAX_DISPLAY_NAME_LEN;
          }
        }
        if (len > 0)
          printf(" %s\r\n", app_context.list);
      }
    }
  }
}

/* ***************** BlueNRG-1 Stack Callbacks ********************************/
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
  if (Status == BLE_STATUS_SUCCESS) {
    app_context.conn_handle = Connection_Handle;
    app_context.state = APP_CONNECTED_STATE;
  }
}

void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason)
{
   ((uint8_t*)(dyn_alloc_a))[0x44c] = 0;
  app_context.state = APP_DISCONNECTED_STATE;
}

void HAL_VTimerTimeoutCallback(uint8_t timerNum)
{
  if (timerNum == APPL_TIMER) {
    if (app_context.state == APP_WAIT_START_ENC)
      app_context.state = APPL_SECURITY_REQ_STATE;
    if (app_context.state == APP_IDLE_STATE)
      app_context.state = APPL_CHECK_NOTIFICATION_STATE;
  }
    
}

void aci_gatt_attribute_modified_event(uint16_t Connection_Handle,
                                       uint16_t Attr_Handle,
                                       uint16_t Offset,
                                       uint16_t Attr_Data_Length,
                                       uint8_t Attr_Data[])
{
}

void aci_att_find_by_type_value_resp_event(uint16_t Connection_Handle,
                                           uint8_t Num_of_Handle_Pair,
                                           Attribute_Group_Handle_Pair_t Attribute_Group_Handle_Pair[])
{
  ANCS_ServiceDiscovery_Callback(Connection_Handle, Num_of_Handle_Pair, Attribute_Group_Handle_Pair);
}

void aci_gatt_disc_read_char_by_uuid_resp_event(uint16_t Connection_Handle,
                                                uint16_t Attribute_Handle,
                                                uint8_t Attribute_Value_Length,
                                                uint8_t Attribute_Value[])
{
  ANCS_CharacDiscovery_Callback(Connection_Handle,
                                Attribute_Handle,
                                Attribute_Value_Length,
                                Attribute_Value);
}

void aci_att_find_info_resp_event(uint16_t Connection_Handle,
                                  uint8_t Format,
                                  uint8_t Event_Data_Length,
                                  uint8_t Handle_UUID_Pair[])
{
  ANCS_FindInfoExtendedProp_Callback(Connection_Handle,
                                     Format,
                                     Event_Data_Length,
                                     Handle_UUID_Pair);
}

void aci_att_read_resp_event(uint16_t Connection_Handle,
                             uint8_t Event_Data_Length,
                             uint8_t Attribute_Value[])
{
  ANCS_ReadResp_Callback(Connection_Handle,
                         Event_Data_Length,
                         Attribute_Value);
}

void aci_gatt_proc_complete_event(uint16_t Connection_Handle,
                                  uint8_t Error_Code)
{
  ANCS_DiscoveryComplete_Callback(Connection_Handle, Error_Code);
}

void aci_gatt_notification_event(uint16_t Connection_Handle,
                               uint16_t Attribute_Handle,
                               uint8_t Attribute_Value_Length,
                               uint8_t Attribute_Value[])
{
  ANCS_Data_Received(Connection_Handle,
                     Attribute_Handle,
                     Attribute_Value_Length,
                     Attribute_Value);
}

void aci_gap_pairing_complete_event(uint16_t Connection_Handle,
                                    uint8_t Status,
                                    uint8_t Reason)
{
  if (app_context.conn_handle != Connection_Handle)
    return;

  if (Status == BLE_STATUS_SUCCESS) {
    printf("Device Connected and Bonded with SUCCESS\r\n");
    app_context.state = APP_BONDED_STATE;
  } else {
    printf("Device Not Bonded Error=0x%02x Reason = 0x%02x\r\n", Status, Reason);
    app_context.state = APP_IDLE_STATE;
  }
}

void aci_gap_bond_lost_event(void)
{
  uint8_t ret;
  
  ret = aci_gap_allow_rebond(app_context.conn_handle);
  if (ret != BLE_STATUS_SUCCESS) {
    printf ("Errro in aci_gap_allow_rebond() 0x%02x\r\n", ret);
  }
}
