/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : RADIO_OTASeverFixedImage_main.c
* Author             : AMS - VMA division
* Version            : V1.1.0
* Date               : 15-January-2016
* Description        : BlueNRG-1 OTA Manager Main Application.
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

/** @addtogroup RADIO_OTA_Server_Fixed_Image RADIO OTA Server Fixed Image Example
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"
#include "hal_radio.h"
#include "main_common.h"

/* Private typedef -----------------------------------------------------------*/
extern const unsigned char image[];
extern uint32_t IMAGE_SIZE;

typedef  void (*pFunction)(void);
/* Private define ------------------------------------------------------------*/

#ifdef DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define RESET_WAKE_DEEPSLEEP_REASONS 0x05

#define BLE_OTA_SERVICE_MANAGER_VERSION_STRING "1.0.0" 

/* Private macro -------------------------------------------------------------*/

#define MAX_RETRY     (10)

/* Private variables ---------------------------------------------------------*/  
uint32_t image_size;
uint8_t rx_ota_buffer[MAX_PACKET_LENGTH], tx_buffer[MAX_PACKET_LENGTH];
static uint16_t seq_num = 0;
static uint16_t seq_num_max = 0;
uint8_t last_frame = 0;
ota_state_machine_t ota_state_machine_g = OTA_CONNECTION;
uint8_t image_tmp[BYTE_IN_FRAME+2+2];

static uint8_t bootloadingCompleted = 0;
static uint8_t bootloadingCompleted_end = 0; 

/* Private function prototypes -----------------------------------------------*/
uint8_t OTA_ConnectionCallback(ActionPacket* p, ActionPacket* next);
uint8_t OTA_SizeCallback(ActionPacket* p, ActionPacket* next);
uint8_t OTA_StartCallback(ActionPacket* p, ActionPacket* next);
uint8_t OTA_DataRequestCallback(ActionPacket* p, ActionPacket* next);
uint8_t OTA_SendDataCallback(ActionPacket* p, ActionPacket* next);

uint8_t OTA_Init(void);
uint8_t OTA_Tick(void);


int main(void)
{
  /* System Init */
  SystemInit();
  SdkEvalIdentification();
  SdkEvalLedInit(LED2);
  SdkEvalLedOn(LED2);
  
#ifdef DEBUG
  SdkEvalComUartInit(UART_BAUDRATE);
#endif
  
  OTA_Init();
  PRINTF("\r\nBlueNRG-1 BLE OTA Manager SERVER (version: %s)\r\n", BLE_OTA_SERVICE_MANAGER_VERSION_STRING); 
  
  while(1) {
    OTA_Tick();
  }
  
}


uint8_t OTA_ConnectionCallback(ActionPacket* p, ActionPacket* next)
{
  /* received a packet */
  if( (p->status & BIT_TX_MODE) == 0) {
    if((p->status & IRQ_RCV_OK) != 0) {
      if(p->data[0] == HEADER_CONNECTION) {
        ota_state_machine_g = OTA_SIZE;
      }
      else {
        PRINTF(">> OTA_ConnectionCallback != %02x\r\n",p->data[0]);
        ota_state_machine_g = OTA_CONNECTION;
      }
    }
    else if(((p->status & IRQ_TIMEOUT) != 0) || ((p->status & IRQ_CRC_ERR) != 0)) {
      ota_state_machine_g = OTA_CONNECTION;
    }
  }
  return TRUE;   
}


uint8_t OTA_SizeCallback(ActionPacket* p, ActionPacket* next)
{
  static uint8_t retry = 0;
  
  /* received a packet */
  if( (p->status & BIT_TX_MODE) == 0) {
    if((p->status & IRQ_RCV_OK) != 0) {
      if(p->data[0] == HEADER_SIZE) {
        ota_state_machine_g = OTA_START;
        retry = 0;
      }
      else {
        PRINTF(">> OTA_SizeCallback != %02x\r\n",p->data[0]);
        ota_state_machine_g = OTA_SIZE;
      }
    }
    else if(((p->status & IRQ_TIMEOUT) != 0) || ((p->status & IRQ_CRC_ERR) != 0)) {
      ota_state_machine_g = OTA_SIZE;
      retry++;      
    }
  }
  if(retry > MAX_RETRY) {
    PRINTF(">> OTA_SizeCallback MAX RETRY\r\n");
    ota_state_machine_g = OTA_CONNECTION;
    retry = 0;
  }
  return TRUE;   
}



uint8_t OTA_StartCallback(ActionPacket* p, ActionPacket* next)
{
  static uint8_t rx_valid = 0, retry = 0;
  
  /* received a packet */
  if( (p->status & BIT_TX_MODE) == 0) {
    if((p->status & IRQ_RCV_OK) != 0) {
      if(p->data[0] == HEADER_START) {
        rx_valid = 1;
      }
      else {
        PRINTF(">> OTA_StartCallback != %02x\r\n",p->data[0]);
      }
    }
    else if(((p->status & IRQ_TIMEOUT) != 0) || ((p->status & IRQ_CRC_ERR) != 0)) {
      ota_state_machine_g = OTA_START;
      retry++;
    }
  }
  else { /* Transmit complete */
    if(rx_valid) {
      rx_valid = 0;
      retry = 0;
      ota_state_machine_g = OTA_DATAREQ;
    }
    else {
      ota_state_machine_g = OTA_START;
    }
  }
  if(retry > MAX_RETRY) {
    PRINTF(">> OTA_StartCallback MAX RETRY\r\n");
    ota_state_machine_g = OTA_CONNECTION;
    retry = 0;
  }
  return TRUE;   
}


uint8_t OTA_DataRequestCallback(ActionPacket* p, ActionPacket* next)
{
  static uint8_t rx_valid = 0, retry = 0;
  
  /* received a packet */
  if( (p->status & BIT_TX_MODE) == 0) {
    if((p->status & IRQ_RCV_OK) != 0) {      
      if(p->data[0] == HEADER_DATAREQ) {
        if(p->data[1] == 2) {
          rx_valid = 1;
          seq_num = (((uint16_t)p->data[2])<<8) | ((uint16_t)p->data[3]);
        }
        else {
          PRINTF(">> OTA_DataRequestCallback != %d LENGTH WRONG\r\n", p->data[1]);
          SdkEvalLedOff(LED2);
          while(1);
        }
      }
      /* It have received the ACK packet from GETDATA state */
      else if(p->data[0] == HEADER_SENDATA) {
        PRINTF(">> OTA_DataRequestCallback get HEADER_SENDATA\r\n");
      }
      else {
        PRINTF(">> OTA_DataRequestCallback != %02x\r\n", p->data[0]);
        retry = MAX_RETRY+1;
      }
    }
    else if(((p->status & IRQ_TIMEOUT) != 0) || ((p->status & IRQ_CRC_ERR) != 0)) {
      ota_state_machine_g = OTA_DATAREQ;
      retry++;
    }
  }
  else { /* Transmit complete */
    if(rx_valid) {
      retry = 0;
      rx_valid = 0;
      ota_state_machine_g = OTA_SENDATA;
    }
    else { /* Received a HEADER_SENDATA */
      ota_state_machine_g = OTA_DATAREQ;
    }
  }
  if(retry > MAX_RETRY) { /* create a reset function */
    retry = 0;
    image_size = IMAGE_SIZE;
    seq_num = 0;
    PRINTF(">> OTA_DataRequestCallback MAX RETRY\r\n");
    ota_state_machine_g = OTA_CONNECTION;
  }
  return TRUE;   
}



uint8_t OTA_SendDataCallback(ActionPacket* p, ActionPacket* next)
{
  static uint8_t retry = 0;
  
  /* received a packet */
  if( (p->status & BIT_TX_MODE) == 0) {
    if((p->status & IRQ_RCV_OK) != 0) {
      retry = 0;
      if(p->data[0] == HEADER_SENDATA) {
        if(last_frame) {
          ota_state_machine_g = OTA_COMPLETE;
        }
        else {
          ota_state_machine_g = OTA_DATAREQ;
        }
      }
      else if(p->data[0] == HEADER_DATAREQ) {
        if(p->data[1] == 2) {
          seq_num = (((uint16_t)p->data[2])<<8) | ((uint16_t)p->data[3]);
          ota_state_machine_g = OTA_SENDATA;
        }
        else {
          PRINTF(">> OTA_DataRequestCallback != %d LENGTH WRONG\r\n", p->data[1]);
          SdkEvalLedOff(LED2);
          while(1);
        }
      }
      else {
        PRINTF(">> OTA_SendDataCallback != %02x\r\n", p->data[0]);
        ota_state_machine_g = OTA_CONNECTION;
      }
    }
    else if(((p->status & IRQ_TIMEOUT) != 0) || ((p->status & IRQ_CRC_ERR) != 0)) {
      ota_state_machine_g = OTA_SENDATA;
      retry++;   
    }
  }
  if(retry > MAX_RETRY) {
    PRINTF(">> OTA_SendDataCallback MAX RETRY\r\n");
    retry = 0;
    ota_state_machine_g = OTA_DATAREQ;
  }
  return TRUE;   
}



uint8_t OTA_Init(void)
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
  
  seq_num_max = IMAGE_SIZE / BYTE_IN_FRAME;
  if(IMAGE_SIZE%BYTE_IN_FRAME) {
    seq_num_max++;
  }
    
  ota_state_machine_g = OTA_CONNECTION;
  
  return 0;
}

uint8_t OTA_Tick()
{
  uint8_t ret;
  
  if(ota_state_machine_g == OTA_CONNECTION) {
    PRINTF("OTA_CONNECTION\r\n");
    image_size = IMAGE_SIZE;
    last_frame = 0;
    seq_num = 0;
    tx_buffer[0] = HEADER_CONNECTION;
    tx_buffer[1] = 0;
    ota_state_machine_g = OTA_ONGOING;
    RADIO_SetBackToBackTime(150);
    ret = HAL_RADIO_SendPacketWithAck(OTA_CHANNEL, TX_WAKEUP_TIME, tx_buffer, rx_ota_buffer, RX_TIMEOUT_ACK, OTA_ConnectionCallback);
    if(ret != SUCCESS_0) {
      PRINTF("ERROR %d\r\n",ret);
    }
  }
  else if(ota_state_machine_g == OTA_SIZE) {
    PRINTF("OTA_SIZE\r\n");    
    
    /* frame setting */
    tx_buffer[0] = HEADER_SIZE;
    tx_buffer[1] = 4;
    tx_buffer[2] = (uint8_t)(image_size>>24);
    tx_buffer[3] = (uint8_t)(image_size>>16);
    tx_buffer[4] = (uint8_t)(image_size>>8);
    tx_buffer[5] = (uint8_t)image_size;
    
    ota_state_machine_g = OTA_ONGOING;
    RADIO_SetBackToBackTime(150);
    ret = HAL_RADIO_SendPacketWithAck(OTA_CHANNEL, TX_WAKEUP_TIME, tx_buffer, rx_ota_buffer, RX_TIMEOUT_ACK, OTA_SizeCallback);
    if(ret != SUCCESS_0) {
      PRINTF("ERROR %d\r\n",ret);
    }
  }
  
  else if(ota_state_machine_g == OTA_START) {
    PRINTF("OTA_START\r\n");
    
    /* frame setting */
    tx_buffer[0] = HEADER_START;
    tx_buffer[1] = 0;
    
    ota_state_machine_g = OTA_ONGOING;
    RADIO_SetBackToBackTime(300);
    ret = HAL_RADIO_ReceivePacketWithAck(OTA_CHANNEL, RX_WAKEUP_TIME, rx_ota_buffer, tx_buffer, RX_TIMEOUT_DATA, OTA_StartCallback);
    if(ret != SUCCESS_0) {
      PRINTF("ERROR %d\r\n",ret);
    }
  }
  
  else if(ota_state_machine_g == OTA_DATAREQ) {
    PRINTF("OTA_DATAREQ %d\r\n", seq_num);

    /* frame setting */
    tx_buffer[0] = HEADER_DATAREQ;
    tx_buffer[1] = 0;
    ota_state_machine_g = OTA_ONGOING;
    RADIO_SetBackToBackTime(300);
    ret = HAL_RADIO_ReceivePacketWithAck(OTA_CHANNEL, RX_WAKEUP_TIME, rx_ota_buffer, tx_buffer, RX_TIMEOUT_DATA, OTA_DataRequestCallback);
    if(ret != SUCCESS_0) {
      PRINTF("ERROR %d\r\n",ret);
    }
  }
  
  else if(ota_state_machine_g == OTA_SENDATA) {
    PRINTF("OTA_SENDATA %d\r\n", seq_num);
    
    if(seq_num == (seq_num_max - 1)) {
      last_frame = 1;
    }    
    
    /* Here the packet to send with data must be defined */
    image_tmp[0] = HEADER_SENDATA;
    image_tmp[2] = (uint8_t)(seq_num>>8);
    image_tmp[3] = (uint8_t)seq_num;
    
    if(last_frame) {
      image_tmp[1] = (image_size - BYTE_IN_FRAME*seq_num) + 2;
    }
    else {
      image_tmp[1] = (BYTE_IN_FRAME+2);
    }
    
    for(uint8_t i = 0; i < image_tmp[1]; i++) {
      image_tmp[4+i] = image[seq_num*BYTE_IN_FRAME + i];
    }
    
    ota_state_machine_g = OTA_ONGOING;
    RADIO_SetBackToBackTime(150);
    ret = HAL_RADIO_SendPacketWithAck(OTA_CHANNEL, TX_WAKEUP_TIME, image_tmp, rx_ota_buffer, RX_TIMEOUT_ACK, OTA_SendDataCallback);
    if(ret != SUCCESS_0) {
      PRINTF("ERROR %d\r\n",ret);
    }
  }
  
  else if(ota_state_machine_g == OTA_COMPLETE) {
    PRINTF("OTA_COMPLETE\r\n");
    PRINTF("image %d\r\n",image_size);    
    ota_state_machine_g = OTA_CONNECTION;
    image_size = IMAGE_SIZE;
    last_frame = 0;
    seq_num = 0;
  }
  
  
  if (bootloadingCompleted) 
  { 
    bootloadingCompleted = 0; 
  }
  
  return (bootloadingCompleted_end);
}



void Blue_Handler(void)
{
  RADIO_IRQHandler();
}

/****************** BlueNRG-1 Sleep Management Callback ********************************/

//SleepModes App_SleepMode_Check(SleepModes sleepMode)
//{
//  if(SdkEvalComIOTxFifoNotEmpty())
//    return SLEEPMODE_RUNNING;
//  
//  return SLEEPMODE_NOTIMER;
//}

/***************************************************************************************/

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
