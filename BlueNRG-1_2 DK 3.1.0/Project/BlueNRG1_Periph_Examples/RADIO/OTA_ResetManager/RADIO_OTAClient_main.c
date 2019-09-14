/******************** (C) COPYRIGHT 2017 STMicroelectronics ********************
* File Name          : OTA_Client_main.c
* Author             : RF Application Team
* Version            : V1.0.0
* Date               : 15-May-2018
* Description        : OTA Client Main Application.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/


/** @addtogroup BlueNRG1_StdPeriph_Examples
  * @{
  */

/** @addtogroup RADIO_Examples RADIO Examples
  * @{
  */

/** @addtogroup RADIO_OTA_Client RADIO OTA Client Example
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"
#include "bluenrg1_it_stub.h"
#include "hal_radio.h"
#include "main_common.h"

/* Private typedef -----------------------------------------------------------*/
typedef  void (*pFunction)(void);

/* Private define ------------------------------------------------------------*/
#ifdef DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
static uint32_t imageBase = APP_WITH_OTA_SERVICE_ADDRESS;
#else
#define PRINTF(...)
#endif

#define OTA_MANAGER_VERSION_STRING "1.0.0" 

#define MAX_RETRY       5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint16_t page_currently_written = 0;
uint8_t rx_buffer[MAX_PACKET_LENGTH], tx_buffer[MAX_PACKET_LENGTH];
uint32_t app_size = 0;

uint8_t page_buffer[2048] = {0,}; /* consider only 32 byte of data except the last one */
uint16_t page_size = 0;

uint8_t data_request = FALSE;
static uint16_t seq_num = 0;
static uint16_t seq_num_max = 0;

ota_state_machine_t ota_state_machine_g = OTA_CONNECTION;

static uint8_t bootloadingCompleted = 0;
static uint8_t bootloadingCompleted_end = 0; 


/* Private function prototypes -----------------------------------------------*/
static void OTA_Erase_Flash(uint16_t startNumber, uint16_t endNumber);
uint8_t OTA_ConnectionCallback(ActionPacket* p, ActionPacket* next);
uint8_t OTA_SizeCallback(ActionPacket* p, ActionPacket* next);
uint8_t OTA_StartCallback(ActionPacket* p, ActionPacket* next);
uint8_t OTA_NotStartCallback(ActionPacket* p, ActionPacket* next);
uint8_t OTA_DataRequestCallback(ActionPacket* p, ActionPacket* next);
uint8_t OTA_GetDataCallback(ActionPacket* p, ActionPacket* next);
void OTA_Jump_To_New_Application(void);
uint8_t OTA_Init(void);
uint8_t OTA_Tick(void);
static void OTA_Check_ServiceManager_Operation(void);
static uint32_t OTA_Check_Application_Tags_Value(void);
static void OTA_Set_Application_Tag_Value(uint32_t address,uint32_t Data);

/**
  * @brief  Main program code.
  * @param  None
  * @retval None
  */
int main(void)
{
  pFunction Jump_To_Application;
  static uint32_t JumpAddress, appAddress;
  
  /* Check Service manager RAM Location to verify if a jump to Service Manager has been set from the Application */
  OTA_Check_ServiceManager_Operation();
  
  /* Identifies the valid application where to jump based on the OTA application validity tags values placed on
  reserved vector table entry: OTA_TAG_VECTOR_TABLE_ENTRY_INDEX */
  appAddress = OTA_Check_Application_Tags_Value();
  
  /* Check if there is a valid application where to jump */
  if (appAddress == APP_WITH_OTA_SERVICE_ADDRESS) {
    /* Jump to user application */
    JumpAddress = *(__IO uint32_t*) (appAddress + 4);
    Jump_To_Application = (pFunction) JumpAddress;
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t*) appAddress);
    Jump_To_Application();
    
    /* Infinite loop */
    while (1);
  }
  
  /* Here, OTA Manager Application starts */
  
  /* System Init */
  SystemInit();
  SdkEvalIdentification();
  SdkEvalLedInit(LED3);
  SdkEvalLedOn(LED3);
    
#ifdef DEBUG
  SdkEvalComUartInit(UART_BAUDRATE);
#endif
  
  /* Erase the storage area from start page to end page */
  OTA_Erase_Flash(APP_WITH_OTA_SERVICE_PAGE_NUMBER_START, APP_WITH_OTA_SERVICE_PAGE_NUMBER_END);

  OTA_Init();
  PRINTF("\r\nBlueNRG-1 BLE OTA Manager CLIENT (version: %s)\r\nNext free address location is: 0x%00000000X\r\n", OTA_MANAGER_VERSION_STRING, APP_WITH_OTA_SERVICE_ADDRESS);
  
  while(1) {    
    if (OTA_Tick() == 1) {
      
      /* Jump to the new application */
      OTA_Jump_To_New_Application();
    }
  }
  
}


/**
* @brief  It erases destination flash erase before starting OTA upgrade session. 
* @param  None.
* @retval None.
*/
static void OTA_Erase_Flash(uint16_t startNumber, uint16_t endNumber)
{
  uint16_t k; 
  
  for(k=startNumber;k<=endNumber;k++) { 
    FLASH_ErasePage(k); 
  }
  
}

/**
* @brief  Callback used during CONNECTION state.
* @param  None.
* @retval None.
*/
uint8_t OTA_ConnectionCallback(ActionPacket* p, ActionPacket* next)
{
  static volatile uint8_t rx_valid = 0;
  
  /* received a packet */
  if( (p->status & BIT_TX_MODE) == 0) {
    if((p->status & IRQ_RCV_OK) != 0) {
      /* check the reception of a connection header otherwise do not go to app_size state */
      if(p->data[0] == HEADER_CONNECTION) {
        rx_valid = 1;
      }
    }
    else if(((p->status & IRQ_TIMEOUT) != 0) || ((p->status & IRQ_CRC_ERR) != 0)) {
      ota_state_machine_g = OTA_CONNECTION;
    }
  }
  else { /* Transmit complete */
    if(rx_valid) {
      rx_valid = 0;
      ota_state_machine_g = OTA_SIZE;
    }
    else {
      ota_state_machine_g = OTA_CONNECTION;
    }
  }
  return TRUE;   
}

/**
* @brief  Callback used during SIZE state.
* @param  None.
* @retval None.
*/
uint8_t OTA_SizeCallback(ActionPacket* p, ActionPacket* next)
{
  static volatile uint8_t rx_valid = 0, retry = 0;
  
  /* received a packet */
  if( (p->status & BIT_TX_MODE) == 0) {
    if((p->status & IRQ_RCV_OK) != 0) {
      if(p->data[0] == HEADER_SIZE) {
        if(p->data[1] == 4) {
          rx_valid = 1;
          app_size = 0;
          for(uint8_t i=0; i<4; i++) {
            app_size |= ((uint32_t)(p->data[2+i]))<<((3-i)*8);
          }
        }
      }
      else {
        retry++;
      }
    }
    else if(((p->status & IRQ_TIMEOUT) != 0) || ((p->status & IRQ_CRC_ERR) != 0)) {
      ota_state_machine_g = OTA_SIZE;
      retry++;
    }
  }
  else { /* Transmit complete */
    if(rx_valid) {
      retry = 0;
      rx_valid = 0;
      ota_state_machine_g = OTA_START;
    }
    else {
      ota_state_machine_g = OTA_SIZE;
    }    
  }
  if(retry > MAX_RETRY) {
    retry = 0;
    ota_state_machine_g = OTA_CONNECTION;
  }

  return TRUE; 
}


/**
* @brief  Callback used during START state.
* @param  None.
* @retval None.
*/
uint8_t OTA_StartCallback(ActionPacket* p, ActionPacket* next)
{  
  static volatile uint8_t retry = 0;
  /* received a packet */
  if( (p->status & BIT_TX_MODE) == 0) {
    if((p->status & IRQ_RCV_OK) != 0) {
      if(p->data[0] == HEADER_START) {
        retry = 0;
        ota_state_machine_g = OTA_DATAREQ;
      }
      else {
        ota_state_machine_g = OTA_START;
        retry++;
      }
    }
    else if(((p->status & IRQ_TIMEOUT) != 0) || ((p->status & IRQ_CRC_ERR) != 0)) {
      ota_state_machine_g = OTA_START;
      retry++;
    }
  }
  if(retry > MAX_RETRY) {
    retry = 0;
    ota_state_machine_g = OTA_CONNECTION;
  }
  return TRUE;   
}


/**
* @brief  Callback used during NOT START state.
* @param  None.
* @retval None.
*/
uint8_t OTA_NotStartCallback(ActionPacket* p, ActionPacket* next)
{
  static volatile uint8_t retry = 0;
  /* received a packet */
  if( (p->status & BIT_TX_MODE) == 0) {
    if((p->status & IRQ_RCV_OK) != 0) {
      if(p->data[0] == HEADER_NOTSTART) {
        retry = 0;
        ota_state_machine_g = OTA_CONNECTION;
      }
      else {
        ota_state_machine_g = OTA_START;
        retry++;
      }
    }
    else if(((p->status & IRQ_TIMEOUT) != 0) || ((p->status & IRQ_CRC_ERR) != 0)) {
      ota_state_machine_g = OTA_START;
      retry++;      
    }
  }
  if(retry > MAX_RETRY) {
    retry = 0;
    ota_state_machine_g = OTA_CONNECTION;
  }
  return TRUE;   
}


/**
* @brief  Callback used during DATA REQUEST state.
* @param  None.
* @retval None.
*/
uint8_t OTA_DataRequestCallback(ActionPacket* p, ActionPacket* next)
{
  static volatile uint8_t retry = 0;
  
  /* received a packet */
  if( (p->status & BIT_TX_MODE) == 0) {
    if((p->status & IRQ_RCV_OK) != 0) {
      if(p->data[0] == HEADER_DATAREQ) {
        retry = 0;
        ota_state_machine_g = OTA_GETDATA;
      }
      else {
        ota_state_machine_g = OTA_DATAREQ;
        retry++;
      }
    }
    else if(((p->status & IRQ_TIMEOUT) != 0) || ((p->status & IRQ_CRC_ERR) != 0)) {
      ota_state_machine_g = OTA_DATAREQ;
      retry++;
    }
  }
  if(retry > 250) { /* retry used during "connection" it lasts ~ 300 ms, then disconnect */
    PRINTF(">> OTA_SendDataCallback MAX_RETRY\r\n\n");
//    ota_state_machine_g = OTA_CONNECTION;
    retry = 0;
  }
  return TRUE;
}


/**
* @brief  Callback used during GET DATA state.
* @param  None.
* @retval None.
*/
uint8_t OTA_GetDataCallback(ActionPacket* p, ActionPacket* next)
{
  static volatile uint8_t rx_valid = 0, retry = 0;
  volatile uint16_t sequence_number_received = 0;
  
  /* received a packet */
  if( (p->status & BIT_TX_MODE) == 0) {
    if((p->status & IRQ_RCV_OK) != 0) {
      if(p->data[0] == HEADER_GETDATA) {
        rx_valid = 1;
        if(p->data[1] > 2) {
          sequence_number_received = (((uint16_t)p->data[2])<<8) | ((uint16_t)p->data[3]);
          if(seq_num == sequence_number_received) {
            for(uint8_t i=0; i< (p->data[1]-2); i++) {
              page_buffer[page_size+i] = p->data[4+i];
            }
            page_size += p->data[1] - 2;
            seq_num++;
          }
        }
      }
    }
    else if(((p->status & IRQ_TIMEOUT) != 0) || ((p->status & IRQ_CRC_ERR) != 0)) {
      ota_state_machine_g = OTA_GETDATA;
      retry++;
    }
  }
  else { /* Transmit complete */
    if(rx_valid) {
      retry = 0;
      rx_valid = 0;
      ota_state_machine_g = OTA_FLASHDATA;
    }
    else {
      ota_state_machine_g = OTA_GETDATA;
    }
  }
  if(retry > MAX_RETRY) {
    retry = 0;
    ota_state_machine_g = OTA_DATAREQ;
  }
  return TRUE;   
}





/**
* @brief  It jumps to the new upgraded application
* @param  None
* @retval None
*
* @note The API code could be subject to change in future releases.
*/

void OTA_Jump_To_New_Application(void)
{
  /* Reset manager will take care of running the new application */
  NVIC_SystemReset(); 
}


/**
* @brief  Initialize the OTA state machine
* @param  None.
* @retval uint8_t return value.
*/
uint8_t OTA_Init()
{
#if LS_SOURCE==LS_SOURCE_INTERNAL_RO
  RADIO_Init(HS_STARTUP_TIME, 1, NULL, ENABLE);
#else
  RADIO_Init(HS_STARTUP_TIME, 0, NULL, ENABLE);
#endif
  
  /* Set the Network ID */
  HAL_RADIO_SetNetworkID(OTA_ACCESS_ADDRESS);
  
  /* Configures the transmit power level */
  RADIO_SetTxPower(MAX_OUTPUT_RF_POWER);
  
  tx_buffer[0] = HEADER_CONNECTION;
  tx_buffer[1] = 0;
  
  ota_state_machine_g = OTA_CONNECTION;
  
  return 0;
}

/**
* @brief  State machine routine
* @param  None.
* @retval uint8_t return value.
*/
uint8_t OTA_Tick()
{
  uint8_t ret;
  
  if(ota_state_machine_g == OTA_CONNECTION) {
    PRINTF("OTA_CONNECTION\r\n");
    app_size = 0;
    page_size = 0;
    seq_num = 0;
    seq_num_max = 0;
    page_currently_written = 0;
    tx_buffer[0] = HEADER_CONNECTION;
    tx_buffer[1] = 0;
    ota_state_machine_g = OTA_ONGOING;
    RADIO_SetBackToBackTime(300);
    ret = HAL_RADIO_ReceivePacketWithAck(OTA_CHANNEL, RX_WAKEUP_TIME, rx_buffer, tx_buffer, RX_TIMEOUT_DATA, OTA_ConnectionCallback);
    if(ret != SUCCESS_0) {
      PRINTF("ERROR %d\r\n",ret);
    }
  }
  
  else if(ota_state_machine_g == OTA_SIZE) {
    PRINTF("OTA_SIZE\r\n");
    tx_buffer[0] = HEADER_SIZE;
    tx_buffer[1] = 0;
    ota_state_machine_g = OTA_ONGOING;
    RADIO_SetBackToBackTime(300);
    ret = HAL_RADIO_ReceivePacketWithAck(OTA_CHANNEL, RX_WAKEUP_TIME, rx_buffer, tx_buffer, RX_TIMEOUT_DATA, OTA_SizeCallback);
    if(ret != SUCCESS_0) {
      PRINTF("ERROR %d\r\n",ret);
    }
  }
  
  else if(ota_state_machine_g == OTA_START) {
    if(app_size<=SM_APP_SIZE) { /* Check if the new application fit the Flash memory */
      PRINTF("OK for 0x%08X %.2f KB max %.2f KB\r\n", app_size, ((float)app_size)/1024.0, ((float)SM_APP_SIZE)/1024.0);
      PRINTF("OTA_START\r\n");
      tx_buffer[0] = HEADER_START;
      tx_buffer[1] = 0;
      
      seq_num_max = app_size / BYTE_IN_FRAME;
      if(app_size%BYTE_IN_FRAME) {
        seq_num_max++;
      }
      
      ota_state_machine_g = OTA_ONGOING;
      RADIO_SetBackToBackTime(150);
      ret = HAL_RADIO_SendPacketWithAck(OTA_CHANNEL, TX_WAKEUP_TIME, tx_buffer, rx_buffer, RX_TIMEOUT_ACK, OTA_StartCallback);
      if(ret != SUCCESS_0) {
        PRINTF("ERROR %d\r\n",ret);
      }
    }
    /* TO BE DONE */
    else { /* If the new image does not fit the Flash memory */
      PRINTF("NOT ok for 0x%08X %.2f KB max %.2f KB\r\n", app_size, ((float)app_size)/1024.0, ((float)SM_APP_SIZE)/1024.0);
      PRINTF("OTA_NOTSTART\r\n");
      tx_buffer[0] = HEADER_NOTSTART;
      tx_buffer[1] = 0;
      ota_state_machine_g = OTA_ONGOING;
      RADIO_SetBackToBackTime(150);
      ret = HAL_RADIO_SendPacketWithAck(OTA_CHANNEL, TX_WAKEUP_TIME, tx_buffer, rx_buffer, RX_TIMEOUT_ACK, OTA_NotStartCallback); 
      if(ret != SUCCESS_0) {
        PRINTF("ERROR %d\r\n",ret);
      }
    }
  }
  
  else if(ota_state_machine_g == OTA_DATAREQ) {
    if(seq_num < seq_num_max) {
//      PRINTF("OTA_DATAREQ %d app_size %d\r\n", seq_num, app_size);
      tx_buffer[0] = HEADER_DATAREQ;
      tx_buffer[1] = 2;
      tx_buffer[2] = (uint8_t)(seq_num>>8);
      tx_buffer[3] = (uint8_t)seq_num;
      ota_state_machine_g = OTA_ONGOING;
      RADIO_SetBackToBackTime(150);
      ret = HAL_RADIO_SendPacketWithAck(OTA_CHANNEL, TX_WAKEUP_TIME, tx_buffer, rx_buffer, RX_TIMEOUT_ACK, OTA_DataRequestCallback);
      if(ret != SUCCESS_0) {
        PRINTF("ERROR %d\r\n",ret);
      }
    }
    else {
      ota_state_machine_g = OTA_FLASHDATA;
//      ota_state_machine_g = OTA_COMPLETE;
    }
  }
  
  else if(ota_state_machine_g == OTA_GETDATA) {
//    PRINTF("OTA_GETDATA %d page_size %d\r\n", seq_num, page_size);
    tx_buffer[0] = HEADER_GETDATA;
    tx_buffer[1] = 0;
    ota_state_machine_g = OTA_ONGOING;
    RADIO_SetBackToBackTime(300);
    ret = HAL_RADIO_ReceivePacketWithAck(OTA_CHANNEL, RX_WAKEUP_TIME, rx_buffer, tx_buffer, RX_TIMEOUT_DATA, OTA_GetDataCallback);
    if(ret != SUCCESS_0) {
      PRINTF("ERROR %d\r\n",ret);
    }
    
  }
  
  else if(ota_state_machine_g == OTA_FLASHDATA) {
//    PRINTF("OTA_FLASHDATA\r\n");
    
    /* New data available */
    if((page_size == _MEMORY_BYTES_PER_PAGE_) || ((app_size - page_size) == 0)) {
      
      /* FLASH LOAD _MEMORY_BYTES_PER_PAGE_ */
      
      /* in which address ? must be calculated my free address plus size after write op */
      
      int app_size_tmp = app_size - page_size;
      if(app_size_tmp < 0) {
        while(1);
      }
      
      uint16_t i;

      i=0;
      while(i<page_size) {
        
        if( (i+16) <= page_size) {
          if(page_currently_written==0 && i==16) {
            page_buffer[16] = 0xFF;
            page_buffer[17] = 0xFF;
            page_buffer[18] = 0xFF;
            page_buffer[19] = 0xFF;
          }
          FLASH_ProgramWordBurst(APP_WITH_OTA_SERVICE_ADDRESS + page_currently_written*2048 + i, (uint32_t *)&page_buffer[i]);
          i += 16;
        }
        else {
          FLASH_ProgramWord(APP_WITH_OTA_SERVICE_ADDRESS + page_currently_written*2048 + i, *(uint32_t *)&page_buffer[i]);
          i += 4;
        }
      }
      page_currently_written++;
      
      /* erase data buffer */
      for(uint16_t i=0; i<page_size; i++) {
        page_buffer[i] = 0;
      }
      
      if(app_size_tmp >= 0) {
        app_size -= page_size;
      }
      page_size = 0;
      PRINTF("app_size 0x%08X %.2f KB\r\n", app_size, ((float)app_size)/1024.0);
    }
    if(app_size) {
      ota_state_machine_g = OTA_DATAREQ;
    }
    else {
      ota_state_machine_g = OTA_COMPLETE;
      /* If the new image does not fit the Flash range */
      PRINTF("OTA_COMPLETE\r\n");
      
      bootloadingCompleted = 1;
    }
  }
  
  if (bootloadingCompleted) 
  {
    /* Set valid tag x the new application just successfully upgraded through OTA */
    OTA_Set_Application_Tag_Value(APP_WITH_OTA_SERVICE_ADDRESS, OTA_VALID_TAG);
    
    bootloadingCompleted = 0;
    bootloadingCompleted_end = 1; 
    PRINTF("** Over The Air BLE  FW upgrade completed with success! *****************\r\n");
    PRINTF("** Application is JUMPING to new base address: 0x%08X *********************\r\n",(unsigned int)imageBase);
  }
  
  return (bootloadingCompleted_end);
}


/**
* @brief  It checks the runtime operation type and set the related OTA tags 
*         for handling the proper jumping to the valid application. 
* @param  None
* @retval None
*
* @note The API code could be subject to change in future releases.
*/
static void OTA_Check_ServiceManager_Operation(void) 
{
  if (ota_sw_activation == OTA_APP_SWITCH_OP_CODE_GO_TO_OTA_SERVICE_MANAGER) //Go to OTA Service manager
  {
    /* Unlock the Flash */
    flash_sw_lock = FLASH_UNLOCK_WORD;
    
    /* Set Invalid valid tag x OTA Application with OTA Service Manager  for allowing jumping to OTA Service manager */
    FLASH_ProgramWord(APP_WITH_OTA_SERVICE_ADDRESS + OTA_TAG_VECTOR_TABLE_ENTRY_OFFSET, OTA_INVALID_OLD_TAG);
    
    /* Lock the Flash */
    flash_sw_lock = FLASH_LOCK_WORD;
    
    /* Reset Service Manager ram location */ 
    ota_sw_activation = OTA_INVALID_OLD_TAG; 
  }
}

/**
* @brief  It defines the valid application address where to jump
*         by checking the OTA application validity tags for the lower and
*         higher applications
* @param  None.
* @retval appaddress: the application base address where to jump
*
* @note The API code could be subject to change in future releases.
*/
uint32_t OTA_Check_Application_Tags_Value(void)
{
  uint32_t appAddress = 0;
  
  if (((TAG_VALUE(APP_OTA_SERVICE_ADDRESS) == OTA_SERVICE_MANAGER_TAG) && (TAG_VALUE(APP_WITH_OTA_SERVICE_ADDRESS) == OTA_IN_PROGRESS_TAG))|| /* 10 */
      ((TAG_VALUE(APP_OTA_SERVICE_ADDRESS) == OTA_SERVICE_MANAGER_TAG) && (TAG_VALUE(APP_WITH_OTA_SERVICE_ADDRESS) == OTA_INVALID_OLD_TAG))) /* 11 */ 
  {
    /* Jump to OTA Service Manager Application */
    appAddress = APP_OTA_SERVICE_ADDRESS;
  }
  else if ((TAG_VALUE(APP_OTA_SERVICE_ADDRESS) == OTA_SERVICE_MANAGER_TAG) && (TAG_VALUE(APP_WITH_OTA_SERVICE_ADDRESS) == OTA_VALID_TAG)) /* 12 */
  {
    /* Jump to Application using OTA Service Manager */
    appAddress = APP_WITH_OTA_SERVICE_ADDRESS;
  }  
  
  return appAddress;
}

void OTA_Set_Application_Tag_Value(uint32_t address,uint32_t Data)
{
  FLASH_ProgramWord(address + OTA_TAG_VECTOR_TABLE_ENTRY_OFFSET,Data);
}


//SleepModes App_SleepMode_Check(SleepModes sleepMode)
//{
//  if(SdkEvalComIOTxFifoNotEmpty())
//    return SLEEPMODE_RUNNING;
//  
//  return SLEEPMODE_NOTIMER;
//}



#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
