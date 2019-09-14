/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
* File Name          : gatt_db.c
* Author             : AMG RF Application Team
* Version            : V1.0.0
* Date               : 12-December-2016
* Description        : Functions to build GATT DB and handle GATT events.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/


#include "hal_types.h"
#include <string.h>
#include "ble_const.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "SDK_EVAL_Config.h"
#include "bluenrg1_stack.h"
#include "master_basic_profile.h"
#include "master_config.h" 
#include "gatt_db.h"
#include "sleep.h"

#ifndef DEBUG
#define DEBUG 1
#endif

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/*
  UUIDs:
  D973F2E0-B19E-11E2-9E96-0800200C9A66
  D973F2E1-B19E-11E2-9E96-0800200C9A66
  D973F2E2-B19E-11E2-9E96-0800200C9A66
  */
  
/* Primary Service UUID expected from  peripheral */
uint8_t GENERIC_ACCESS_PROFILE_UUID[]={0x00, 0x18};
uint8_t GENERIC_ATTRIBUTE_PROFILE_UUID[]={0x01, 0x18};
uint8_t SERVICE_CHANGED_CHAR_UUID[] = {0x05, 0x2A};
uint8_t PERIPHERAL_SERVICE_UUID[]={0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe0,0xf2,0x73,0xd9};
/* Characteristic UUID expected from  peripheral */
uint8_t PERIPHERAL_RX_CHAR_UUID[]={0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe2,0xf2,0x73,0xd9};
uint8_t PERIPHERAL_TX_CHAR_UUID[]={0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe1,0xf2,0x73,0xd9};

masterRoleContextType masterContext;

securitySetType sec_param;

static uint32_t pass_key = 0; 

static volatile uint8_t pass_key_digit_counter = 0; 
static volatile uint8_t index_counter = 5; 
static volatile uint8_t confirm_pass_key = 0;  
  
static uint32_t timeRXChar, currentTime; 


/* utility function for returning the 10^num value: only for temporary test (TBR)*/
static uint32_t exp_util(uint8_t num)
{
  uint32_t exp_value = 1; 
  
  for (uint8_t i =0; i < num; i++)
  {
    exp_value = exp_value *10;
  }
  return exp_value;
  
}

/* State Machine API for handling the pass key insertion and confirmation on Central device through Keyboard:
   the pass key is the one displayed on Peripheral device hyper terminal and it must be inserted digit by digit and
   then confirmed by pressing y (any other character for repeating the insertion  procedure) 
*/

void enter_pass_key_SM(void)
{
  uint8_t input = 200;
  int key_digit; 
   
  if (pass_key_digit_counter < 6) 
  {
    if (__io_getcharNonBlocking(&input))
    {
        if (isdigit(input))
        {
          key_digit = (uint8_t) (input - '0');
          pass_key = pass_key + key_digit * exp_util(index_counter);
          PRINTF("**** Inserted Pass Key Digit(%d): %d\r\n",index_counter, key_digit);
    
          pass_key_digit_counter += 1; 
          index_counter = index_counter -1; 
        }
    }
    
  }
  else if ((pass_key_digit_counter ==6) && (confirm_pass_key == 0))
  {
    PRINTF("**** Inserted Complete Pass Key : %d\r\n", (int)pass_key);
    PRINTF("\r\n --------> DO YOU CONFIRM PASS KEY? (PRESS 'y' key for confirming! ANY OTHER key for not confirming!)\r\n");
    confirm_pass_key =1; 
  }
  else if ((pass_key_digit_counter ==6) && (confirm_pass_key == 1))
  {
    if (__io_getcharNonBlocking(&input))
    {
        if (input == 'y')
        {
          /* pass key is OK */
          masterContext.start_insert_passkey = 0; 
          masterContext.end_insert_passkey = 1; 
          confirm_pass_key =0; 
        }
        else
        {
          /* re-enter pass key */
          PRINTF("**** Re-Enter Pass Key\r\n");
          pass_key = 0;
          pass_key_digit_counter = 0;
          index_counter = 5; 
          confirm_pass_key = 0; 
        }
    } 
  }
}

/* It allows to add the pass key using the Central library Master_Passkey_Response() API */
void add_pass_key(void)
{
  uint8_t ret; 
  
  ret = Master_Passkey_Response(masterContext.PassKey_connHandle, pass_key);
  if (ret == 0)
  {
    PRINTF("Set key OK  (connection handle: 0x%04x) ***************\r\n", masterContext.connHandle);
   
  }
  else
  {
   PRINTF("Set Passkey error = 0x%04x \r\n", ret);
  }
}

static void start_primary_service_discovery(void)
{
   uint8_t ret; 
   masterContext.getPrimaryService =TRUE;
   ret = Master_GetPrimaryServices(masterContext.connHandle, &masterContext.numPrimarySer,
                                   masterContext.primarySer, sizeof(masterContext.primarySer));
   if (ret != BLE_STATUS_SUCCESS) 
      PRINTF("\r\n****Master_GetPrimaryService() procedure fails with status 0x%02x\r\n", ret);
   else
      PRINTF("\r\n****Master_GetPrimaryService() OK \r\n");
}

/*******************************************************************************
* Function Name  : APP_Tick.
* Description    : Tick to run the application state machine.
* Input          : none.
* Return         : none.
*******************************************************************************/
void APP_Tick(void)
{   
    /* Discovery characteristics of service to enable the notifications/indications */
    if (masterContext.findCharacOfService) {
      findCharcOfService();
      masterContext.findCharacOfService = FALSE;
      timeRXChar = HAL_VTimerGetCurrentTime_sysT32();
    }

    /* Enable the notifications/indications */
    if (masterContext.enableNotif){
      enableSensorNotifications();
      masterContext.enableNotif = FALSE;
      timeRXChar = HAL_VTimerGetCurrentTime_sysT32();
    }
    
   if (masterContext.read_char ==TRUE)
   {
      /* Read the peripheral RX char every RX_CHAR_TIMEOUT sec */
      currentTime = HAL_VTimerGetCurrentTime_sysT32();  
      if ((HAL_VTimerDiff_ms_sysT32(currentTime, timeRXChar)) >= RX_CHAR_TIMEOUT) 
      {
        timeRXChar = HAL_VTimerGetCurrentTime_sysT32();
        if (masterContext.connHandle != 0xFFFF)
          readRXChar();
      }
   }
    
   if (masterContext.encryption_enabled_OK ==TRUE)
   {  
       masterContext.encryption_enabled_OK= FALSE; 
       /* Start service discovery procedure once security has been finalized with encryption event enabled */
       start_primary_service_discovery();
   }
   /* check if pass key has been provided by user */
   if(masterContext.end_insert_passkey==TRUE)
   {
      add_pass_key(); 
      masterContext.end_insert_passkey = 0; 
   }
    
    /* disconnect */
   if(masterContext.do_terminate_connection==TRUE)
   {
      masterContext.do_terminate_connection = 0;
      terminate_connection();
   }
    
   if(masterContext.start_insert_passkey==TRUE)
   {
      enter_pass_key_SM();
   }
  
   /* BLE Security v4.2 is supported: BLE stack FW version >= 2.x */
   if(masterContext.start_confirm_numeric_value==TRUE)
   {
      Confirm_Numeric_Value_SM(); 
   }
    
   /* Reconnection after user disconnection or slave device disconnection */
   if(masterContext.reconnection==TRUE)
   {
      masterContext.reconnection = 0; 
      deviceConnection(); 
   }
}


/*******************************************************************************
* Function Name  : App_SleepMode_Check
* Description    : It sets the sleep mode 
* Input          : None
* Return         : Current Sleep Mode
*******************************************************************************/
SleepModes App_SleepMode_Check(SleepModes sleepMode)
{  
  if (masterContext.start_insert_passkey || masterContext.do_terminate_connection || masterContext.start_confirm_numeric_value || SdkEvalComIOTxFifoNotEmpty())
  {
     /* In order to be able to insert the Pass key through keyboard sets sleep mode to 
        SLEEPMODE_RUNNING */ 
    return SLEEPMODE_RUNNING;
  }
 
  /* normal condition: lowest power mode */
  return SLEEPMODE_NOTIMER;
  
}

/*******************************************************************************
* Function Name  : deviceInit
* Description    : Device init
* Input          : None
* Return         : Status of the call
*******************************************************************************/
uint8_t deviceInit(void)
{
  uint8_t ret;
  uint8_t bdaddr[] = MASTER_PUBLIC_ADDRESS;
  uint8_t devName[] = "MASTER_DEVICE_NAME";
  initDevType param;

  /* Master context init */
  masterContext.startDeviceDisc = FALSE;
  masterContext.findCharacOfService = FALSE;
  masterContext.genAttFlag = FALSE;
  masterContext.ServiceFlag = FALSE; 
  masterContext.getPrimaryService = FALSE;
  masterContext.enableNotif = FALSE; 
  
  masterContext.encryption_enabled_OK= FALSE; 
  masterContext.do_terminate_connection= FALSE; 
  masterContext.start_insert_passkey= FALSE;
  masterContext.end_insert_passkey= FALSE; 
  masterContext.read_char = FALSE;
  masterContext.isconnected = 0; 
  masterContext.reconnection = FALSE; 
  
  masterContext.connHandle = 0xFFFF;
  masterContext.PassKey_connHandle= 0xFFFF;
  /* BLE Security v4.2 is supported: BLE stack FW version >= 2.x */
  masterContext.start_confirm_numeric_value = FALSE; 

  /* Master device init */
  memcpy(param.public_addr, bdaddr, 6);
  param.device_name_len = MASTER_DEVICE_NAME_LEN;
  param.device_name = devName;
  param.txPowerLevel = 4;
  ret = Master_Init(&param);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Master_Init() status: 0x%02x\r\n", ret);
  }
  return ret;
}

uint8_t Device_Security(void)
{
  uint8_t ret; 

  ret = Master_ClearSecurityDatabase();
  
  sec_param.ioCapability = IO_CAPABILITY;
  sec_param.mitm_mode = MITM_MODE;
  
  sec_param.Min_Encryption_Key_Size = MIN_KEY_SIZE; 
  sec_param.Max_Encryption_Key_Size = MAX_KEY_SIZE;
  sec_param.bonding_mode = BONDING;
  sec_param.use_fixed_pin = FIXED_PIN_POLICY;
    
  sec_param.fixed_pin = MASTER_PASS_KEY; /* used only if use fixed pin policy is enabled */
  
  /* BLE Security v4.2 is supported: BLE stack FW version >= 2.x */
  sec_param.sc_support = SECURE_CONNECTION_SUPPORT; 
  sec_param.keypress_support = KEYPRESS_NOTIFICATION; 
  sec_param.identity_address_type = IDENTITY_ADDRESS; 
       
  ret = Master_SecuritySet(&sec_param);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Error in Master_SecuritySet() 0x%02x\r\n", ret);
  
  }
  return ret; 
 
}
  
/*******************************************************************************
* Function Name  : deviceDiscovery
* Description    : Starts the device discovery proceudre
* Input          : None
* Return         : Status of the call
*******************************************************************************/
uint8_t deviceDiscovery(void)
{
  uint8_t ret;
  devDiscoveryType devDiscParam;

  devDiscParam.procedure = GENERAL_DISCOVERY_PROCEDURE;
  devDiscParam.scanInterval = LIM_DISC_SCAN_INT;
  devDiscParam.scanWindow = LIM_DISC_SCAN_WIND;
  devDiscParam.own_address_type = PUBLIC_DEVICE_ADDRESS;
  ret = Master_DeviceDiscovery(&devDiscParam);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Master_DeviceDiscovery() status: 0x%02x\r\n", ret);
  } else {
    PRINTF("****Start Device Discovery Procedure\r\n");
  }
  return ret;
}

/*******************************************************************************
* Function Name  : deviceConnection
* Description    : Starts the connection proceudre with the peripheral device
* Input          : None
* Return         : Status of the call
*******************************************************************************/
uint8_t deviceConnection(void)
{
  uint8_t ret;
  connDevType connParam;

  connParam.procedure = DIRECT_CONNECTION_PROCEDURE;
  connParam.fastScanDuration = FAST_SCAN_DURATION;
  connParam.fastScanInterval = FAST_SCAN_INTERVAL;  
  connParam.fastScanWindow = FAST_SCAN_WINDOW;
  connParam.reducedPowerScanInterval = REDUCED_POWER_SCAN_INTERVAL;
  connParam.reducedPowerScanWindow = REDUCED_POWER_SCAN_WINDOW;
  connParam.peer_addr_type = masterContext.peer_addr_type;
  memcpy(connParam.peer_addr, masterContext.peer_addr, 6);
  connParam.own_addr_type =0;
  connParam.conn_min_interval = FAST_MIN_CONNECTION_INTERVAL;
  connParam.conn_max_interval = FAST_MAX_CONNECTION_INTERVAL;
  connParam.conn_latency = FAST_CONNECTION_LATENCY;
  connParam.supervision_timeout = SUPERVISION_TIMEOUT;
  connParam.min_conn_length = 5;
  connParam.max_conn_length = 5;
  ret = Master_DeviceConnection(&connParam);
  if (ret != BLE_STATUS_SUCCESS) 
    PRINTF("Master_DeviceConnection() failed with status: 0x%02x\r\n", ret);
  else{
    PRINTF("Master_DeviceConnection() OK\r\n"); 
#if 0 //TBR
    PRINTF("peer_addr_type = 0x%0x\r\n", masterContext.peer_addr_type);
    PRINTF("peer_addr: "); 
      
    for (uint8_t j = 0; j< 6; j++)
    {
      PRINTF("0x%02x ", masterContext.peer_addr[j]);
    }
    PRINTF("\r\n");
#endif 
  }
  return ret;
}

/*******************************************************************************
* Function Name  : primaryServiceFound
* Description    : Function used to print the primary service found
* Input          : 
* Return         : 
*******************************************************************************/
void primaryServiceFound(void)
{
  uint8_t i, index, sizeElement;
  uint8_t uuid[16];

  PRINTF("\r\n************ PRIMAY SERVICE ****************************\r\n");
  PRINTF("Number Primary Service found = %d\r\n", masterContext.numPrimarySer); 
  index = 0;
  for (i=0; i<masterContext.numPrimarySer; i++) { 
    if (masterContext.primarySer[index] == 1)
      sizeElement = 6;
    else
      sizeElement = 20;
    index++;
    PRINTF("%d-- ", i+1);
    memcpy (uuid, &masterContext.primarySer[index+4], (sizeElement-4));
    if (memcmp(uuid, GENERIC_ACCESS_PROFILE_UUID, (sizeElement-4)) == 0) {
      PRINTF("Generic Access Profile\r\n");
    }
    if (memcmp (uuid, GENERIC_ATTRIBUTE_PROFILE_UUID, (sizeElement-4)) == 0) {
      PRINTF("Generic Attribute Profile\r\n");
      masterContext.genAttFlag = TRUE;
      masterContext.findCharacOfService = TRUE;
    }
    if (memcmp (uuid, PERIPHERAL_SERVICE_UUID, (sizeElement-4)) == 0) {
      PRINTF("Peripheral Service\r\n");
    }
   
    index += sizeElement;
  }
  masterContext.getPrimaryService = FALSE; 
  PRINTF("********************************************************\r\n");
}

/*******************************************************************************
* Function Name  : findCharacOfService
* Description    : Search the characteristics of a service
* Input          : 
* Return         : 
*******************************************************************************/
void findCharcOfService(void)
{
  uint8_t ret, i, index, sizeElement, uuid[16];
  uint8_t max_size = 0;
  uint16_t startHandle, endHandle;
  uint8_t *numCharac = NULL;
  uint8_t *charac = NULL;

  index = 0;
  for (i=0; i<masterContext.numPrimarySer; i++) { 
    if (masterContext.primarySer[index] == 1)
      sizeElement = 6;
    else
      sizeElement = 20;
    index++;
    memcpy (uuid, &masterContext.primarySer[index+4], (sizeElement-4));
    /* Discovery all the characteristics of the Generic Attribute Profile */
    if ((memcmp(uuid, GENERIC_ATTRIBUTE_PROFILE_UUID, (sizeElement-4)) == 0) && masterContext.genAttFlag) {
      memcpy((uint8_t*)&startHandle, &masterContext.primarySer[index], 2);
      memcpy((uint8_t*)&endHandle, &masterContext.primarySer[index+2], 2);
      numCharac = &masterContext.numCharacGenAttSer;
      charac = masterContext.characGenAttSer;
      max_size = sizeof(masterContext.characGenAttSer);
    }
    

    /* Discovery all the characteristics of the Peripheral Service */
    if ((memcmp(uuid, PERIPHERAL_SERVICE_UUID, (sizeElement-4)) == 0) && masterContext.ServiceFlag) {
      memcpy((uint8_t*)&startHandle, &masterContext.primarySer[index], 2);
      memcpy((uint8_t*)&endHandle, &masterContext.primarySer[index+2], 2);  
      numCharac = &masterContext.numCharacSer;
      charac = masterContext.characSer;
      max_size = sizeof(masterContext.characSer);
    }

    index += sizeElement;
  }

  ret = Master_GetCharacOfService(masterContext.connHandle, startHandle, endHandle,
  				  numCharac, charac, max_size);

  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Master_GetCharacOfService() failed with status 0x%02x\r\n", ret);
  }

}

/*******************************************************************************
* Function Name  : extractCharacInfo
* Description    : Extracts the handle information for all the characteristics found
* Input          : 
* Return         : 
*******************************************************************************/
void extractCharacInfo(void)
{
  uint8_t i;

  /* Store in the master context the characteristic handles for the  TX, RX */
  if (masterContext.ServiceFlag) {
    for (i=0; i<masterContext.numCharacSer; i++) {
      if (memcmp(&masterContext.characSer[6+(i*22)], PERIPHERAL_TX_CHAR_UUID, 16) == 0) {
	memcpy((uint8_t*)&masterContext.TXHandle, &masterContext.characSer[1+(i*22)], 2);
        PRINTF("TX Char Handle (notification property): 0x%04x\r\n", masterContext.TXHandle);
      }
      if (memcmp(&masterContext.characSer[6+(i*22)], PERIPHERAL_RX_CHAR_UUID, 16) == 0) {
	memcpy((uint8_t*)&masterContext.RXHandle, &masterContext.characSer[1+(i*22)], 2);
        PRINTF("RX Char Handle (read property): 0x%04x\r\n", masterContext.RXHandle);
      }
    }
  }
  
}

/*******************************************************************************
* Function Name  : Master_DeviceDiscovery_CB
* Description    : Device discovery callback from master basic profile library
* Input          : 
* Return         : 
*******************************************************************************/
void Master_DeviceDiscovery_CB(uint8_t *status, uint8_t *addr_type, uint8_t *addr, 
			       uint8_t *data_length, uint8_t *data,
			       uint8_t *RSSI)
{

  if (*status == DEVICE_DISCOVERED) {
    
     if (!memcmp(&data[5], PERIPHERAL_DEVICE_NAME, PERIPHERAL_DEVICE_NAME_LEN)) { 
      PRINTF("SLAVE_SECURITY device found\r\n");
      masterContext.peer_addr_type = *addr_type;
      memcpy(masterContext.peer_addr, addr, 6);
      if (deviceConnection() != BLE_STATUS_SUCCESS)
	PRINTF("Error during the device connection procedure\r\n");
    }
  }
  if (*status == DEVICE_DISCOVERY_PROCEDURE_ENDED)
    PRINTF("***Device Discovery Procedure ended from the application\r\n");

  if (*status == DEVICE_DISCOVERY_PROCEDURE_TIMEOUT) {
    PRINTF("***Device Discovery Procedure Timeout\r\n");
    if (deviceDiscovery() != BLE_STATUS_SUCCESS) {
      PRINTF("Error during the device discovery procedure\r\n");
    }
  }    
}

/*******************************************************************************
* Function Name  : Master_Connection_CB
* Description    : Connection callback from master basic profile library
* Input          : 
* Return         : 
*******************************************************************************/
void Master_Connection_CB(uint8_t *connection_evt, uint8_t *status, uint16_t *connection_handle, connUpdateParamType *param)
{
  switch (*connection_evt) {
  case CONNECTION_ESTABLISHED_EVT:
    PRINTF("\r\n****Connection Established (handle: 0x%04x, status: 0x%04x)\r\n", *connection_handle, *status); 
 
    masterContext.isconnected = 1; 
    /* Start the discovery all primary services procedure */
    if (*status == BLE_STATUS_SUCCESS) {
      masterContext.connHandle = *connection_handle;
    }
    break;
  case CONNECTION_FAILED_EVT:
    PRINTF("****CONNECTION FAILED EVENT\r\n");
    break;
  case DISCONNECTION_EVT:
    PRINTF("****DISCONNECTION EVENT, status: 0x%04x\r\n",*status);
    /* Restart Discovery & Connection procedure */
    masterContext.isconnected = 0; 
    
    masterContext.connHandle = 0xFFFF;
    masterContext.PassKey_connHandle = 0xFFFF; 
    masterContext.read_char = FALSE; 
    pass_key_digit_counter = 0; 
    index_counter = 5; 
    confirm_pass_key = 0;  
    pass_key = 0; 
    
    /* Set state for performing  a direct connection to the slave device which enters 
       in undirected connectable mode with advertising filter policy with white list enabled */
    masterContext.reconnection = TRUE; 

    break;
  case STOP_CONNECTION_PROCEDURE_EVT:
    PRINTF("****STOP CONNECTION PROCEDURE EVENT\r\n");
    break;
  case CONNECTION_UPDATE_PARAMETER_REQUEST_EVT:
    PRINTF("****CONNECTION UPDATE PARAMETER REQUEST EVENT\r\n");
    break;
  }
}

void terminate_connection(void)
{
  uint8_t ret ;
  
  ret = Master_CloseConnection(masterContext.connHandle);
  if (ret != BLE_STATUS_SUCCESS) {
    PRINTF("Master_CloseConnection() failed:0x%02x\r\n", ret);
  }
  else
  {
    PRINTF("Master_CloseConnection() --> SUCCESS\r\n");
  }
}

/*******************************************************************************
* Function Name  : Master_ServiceCharacPeerDiscovery_CB
* Description    : Service and Characteristics discovery callback from master basic profile library
* Input          : 
* Return         : 
*******************************************************************************/
void Master_ServiceCharacPeerDiscovery_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle)
{
  switch(*procedure) {
  case PRIMARY_SERVICE_DISCOVERY:
    if (*status == BLE_STATUS_SUCCESS) {
      primaryServiceFound();
    }
    break;
  case GET_CHARACTERISTICS_OF_A_SERVICE:
    {
      if (*status == BLE_STATUS_SUCCESS) {
	/* Discovery peripheral service characteristics */
	if (masterContext.genAttFlag) {
          extractCharacInfo(); 
	  masterContext.genAttFlag = FALSE;
	  masterContext.ServiceFlag = TRUE;
	  masterContext.findCharacOfService = TRUE;
	} else {
	  
	   /* Start the enable indication procedure for the service changed charac */
           if (masterContext.ServiceFlag) {
	      extractCharacInfo();
              masterContext.ServiceFlag = FALSE; 
	      masterContext.genAttFlag = TRUE;
	      masterContext.enableNotif = TRUE;
            }
	}
      }
    }
    break;
  }
}


/*******************************************************************************
* Function Name  : enableSensorNotifications
* Description    : Enables the sensor notifications and indications
* Input          : 
* Return         : 
*******************************************************************************/
void enableSensorNotifications(void)
{
  uint8_t ret;
  uint16_t handle;
  
  /* Enable Service changed notification */
  if (masterContext.genAttFlag) {
    memcpy ((uint8_t*)&handle, &masterContext.characGenAttSer[4], 2);
    ret = Master_NotifIndic_Status(masterContext.connHandle, (handle+1), FALSE, TRUE); 
    if (ret != BLE_STATUS_SUCCESS)
      PRINTF("Error during the Master_NotifIndic_Status(), Handle+1: 0x%02x,  Status: 0x%02x,  \r\n", handle+1, ret);
    else
      PRINTF("Master_NotifIndic_Status() OK, Handle: 0x%02x\r\n", handle+1);
  }

  /* Enable TS char notifications */
  if (masterContext.ServiceFlag) {
    handle = masterContext.TXHandle + 2; // TX char client charac conf desc handle
    ret = Master_NotifIndic_Status(masterContext.connHandle, handle, TRUE, FALSE);
    if (ret != BLE_STATUS_SUCCESS)
      PRINTF("Error during the Master_NotifIndic_Status(), Handle: 0x%02x,  Status: 0x%02x,  \r\n", handle, ret);
    else
      PRINTF("Master_NotifIndic_Status() OK, Handle: 0x%02x\r\n", handle);
  }
}

/*******************************************************************************
* Function Name  : readRXChar
* Description    : Read the RX char from the peer device
* Input          : 
* Return         : 
*******************************************************************************/
void readRXChar(void)
{
  uint8_t status;

  status = Master_Read_Value(masterContext.connHandle, masterContext.RXHandle+1,
			     &masterContext.tempVal_length, masterContext.tempValue, 
			     sizeof(masterContext.tempValue)); 
  if (status != BLE_STATUS_SUCCESS) {
    PRINTF("Error during the Master_Read_Value() function call (handle = 0x%02x) returned status=0x%02x\r\n",masterContext.RXHandle, status); 
  }
}


/*******************************************************************************
* Function Name  : Master_PeerDataExchange_CB
* Description    : Peer Device data exchange callback from master basic profile library
* Input          : 
* Return         : 
*******************************************************************************/
void Master_PeerDataExchange_CB(uint8_t *procedure, uint8_t *status, uint16_t *connection_handle, dataReceivedType *data)
{
  switch(*procedure) {
  case NOTIFICATION_INDICATION_CHANGE_STATUS:
    {
      PRINTF("\r\n****NOTIFICATION_INDICATION_CHANGE_STATUS: p: %x, s: %x\r\n",*procedure,*status  );
      if (masterContext.genAttFlag) {
	masterContext.genAttFlag = FALSE;
	masterContext.ServiceFlag = TRUE;
	masterContext.enableNotif = TRUE;
	PRINTF("\r\n****Service Changed indication enabled----\r\n");
      } 
      else 
      {
	if (masterContext.ServiceFlag) {
	    masterContext.ServiceFlag = FALSE;
	    PRINTF("\r\n****TX Notification enabled----\r\n\n");
            masterContext.enableNotif = FALSE; 
            masterContext.read_char = TRUE; 
	}
      }      
    }
    break;
  case NOTIFICATION_DATA_RECEIVED:
    {
      if ((masterContext.TXHandle+1) == data->attr_handle) {
        PRINTF("TX char notification: ");
        
        for (uint8_t i =0; i< data->data_length-2; i++)
        {
          PRINTF("%c", data->data_value[i]);
        }
        
#if 0//TBR
        for (uint8_t i =18; i< data->data_length; i++)
        {
          PRINTF("%d ", data->data_value[i]);
        }
#else       
        PRINTF("%d ", (data->data_value[18])*256 + data->data_value[19]);
#endif
        PRINTF("\r\n");
      }
    }
    break;
  case INDICATION_DATA_RECEIVED:
    {
      PRINTF("****Service Changed Indication Received\r\n");
    }
    break;
  case READ_VALUE_STATUS:
    { 
       PRINTF("RX char read: ");
       for (uint8_t i =0; i< masterContext.tempVal_length-2; i++)
       {
          PRINTF("%c", masterContext.tempValue[i]);
       }
#if 0//TBR     
        for (uint8_t i =18; i< masterContext.tempVal_length; i++)
        {
          PRINTF("%d ", masterContext.tempValue[i]);
        }
        
#else       
        PRINTF("%d ", (masterContext.tempValue[18])*256 + masterContext.tempValue[19]);
#endif
        PRINTF("\r\n");
    }
    break;
  }
}

/*******************************************************************************
* Function Name  : Master_Pairing_CB
* Description    : Pairing callback from master basic profile library
* Input          : 
* Return         : 
*******************************************************************************/
/* BLE Security v4.2 is supported: BLE stack FW version >= 2.x */
void Master_Pairing_CB(uint16_t *conn_handle, uint8_t *status , uint8_t *reason)
{
  PRINTF("Pairing CB conn_handle = 0x%04x with status = 0x%02x, reason = 0x%02x***************\n", *conn_handle, *status, *reason);
  
  if (*status == PASS_KEY_REQUEST) 
  {
    masterContext.start_insert_passkey= TRUE;
    PRINTF("\r\n --------> INSERT PASS KEY DISPLAYED ON PERIPHERAL DISPLAY (digit by digit): \r\n");
    
    masterContext.PassKey_connHandle = *conn_handle;
  }
}

void Master_library_encryption_change_event_CB(uint8_t Status,
                                               uint16_t Connection_Handle,
                                               uint8_t Encryption_Enabled)
{
  if (Encryption_Enabled ==1)
  {
    PRINTF("\r\n Encryption_Enabled\r\n");
    masterContext.encryption_enabled_OK= TRUE;
  }
}

/*******************************************************************************
* Function Name  : Master_PeerDataPermitExchange_CB
* Description    : Peer data read/write permit callback from master basic profile library
* Input          : 
* Return         : 
*******************************************************************************/
void Master_PeerDataPermitExchange_CB(uint8_t *procedure, uint16_t *connection_handle, dataPermitExchangeType *data)
{
  
}

/* BLE Security v4.2 is supported: BLE stack FW version >= 2.x */

static void Confirm_Numeric_Value(uint16_t Connection_Handle, uint8_t yes_no)
{
  uint8_t ret = 0;
  
  /* First simple case: we confirm the value assiming it is correct */
  ret = aci_gap_numeric_comparison_value_confirm_yesno(Connection_Handle, yes_no); 
        
  if (ret != BLE_STATUS_SUCCESS) 
  {
     PRINTF("\r\n****aci_gap_numeric_comparison_value_confirm_yesno(%d) procedure fails with status 0x%02x\r\n", yes_no, ret);
  }
  else
  {
     PRINTF("\r\n****aci_gap_numeric_comparison_value_confirm_yesno(%d) OK \r\n", yes_no);
  }
}

void Confirm_Numeric_Value_SM(void)
{
  uint8_t input = 200;
  
  if (__io_getcharNonBlocking(&input))
  {
    if (input == 'y')
    {
      /* Input digit is OK */
      PRINTF("\r\n Numeric Value is CONFIRMED!\r\n"); 
      Confirm_Numeric_Value(masterContext.connHandle, 0x01);
    }
    else
    {
      PRINTF("\r\n Numeric Value is NOT CONFIRMED!\r\n");
      Confirm_Numeric_Value(masterContext.connHandle, 0x00);
    }  
    masterContext.start_confirm_numeric_value = 0; 
  }
}


void Master_Pairing_NumericComparison_CB(uint16_t *Connection_Handle, uint32_t Numeric_Value)
{
  PRINTF("\r\n Numeric value = %d. Do you confirm? (y/n)\r\n", (int)Numeric_Value);
  /* Add logic  (State machine) for confirming the value by calling the 
   
    aci_gap_numeric_comparison_confirm_yesno(uint16_t Connection_Handle, uint8_t confirm_yes_no); 
   */
  masterContext.start_confirm_numeric_value = 1; 
}

void Master_Library_gatt_tx_pool_available_event_CB(uint16_t Connection_Handle,
                                                    uint16_t Available_Buffers)
{
  
}


