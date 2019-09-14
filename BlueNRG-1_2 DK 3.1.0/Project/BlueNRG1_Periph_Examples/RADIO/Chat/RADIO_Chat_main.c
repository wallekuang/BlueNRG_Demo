/******************** (C) COPYRIGHT 2017 STMicroelectronics ********************
* File Name          : chat.c 
* Author             : RF Application Team
* Version            : V1.1.0
* Date               : April-2018
* Description        : This code generate a point to point communication generating a two ways chat.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"
#include "hal_radio.h"
#include "osal.h"
#include "fifo.h"

/** @addtogroup BlueNRG1_StdPeriph_Examples
* @{
*/

/** @addtogroup RADIO_Examples RADIO Examples
* @{
*/

/** @addtogroup RADIO_Chat RADIO Chat Example
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BLE_ADV_ACCESS_ADDRESS  (uint32_t)(0x8E89BED6)
#define STARTING_CHANNEL        (uint8_t)(24)    // RF channel 22
#define HS_STARTUP_TIME         (uint16_t)(1)  /* High Speed start up time min value */

#define CHAT_RECEIVE_RELATIVETIME       5000
#define CHAT_TRANSMIT_RELATIVETIME      5000
#define CHAT_RECEIVE_TIMEOUT            100000  /* 100 ms */

#define MAX_CHAT_PACKET_LEN 30
#define MAX_RETRIES 3

#define CHAT_FREQUENCYCHANNEL           STARTING_CHANNEL

#ifdef PKT_ENCRYPTION
#define	MIC_FIELD_LEN	MIC_FIELD_LENGTH
#else
#define	MIC_FIELD_LEN	0
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Blue FIFO */
circular_fifo_t blueRec_fifo;
uint8_t blueRec_buffer[MAX_PACKET_LENGTH*2];

uint32_t interval;
uint8_t uart_buffer[MAX_PACKET_LENGTH];
uint8_t sendData[MAX_PACKET_LENGTH];
uint8_t sendAckData[2] = {0xAE, 0}; /* 0xAE ACK value, length = 0 */
uint8_t receivedData[MAX_PACKET_LENGTH];
uint8_t receivedAckData[MAX_PACKET_LENGTH];
static uint8_t retries = 0;

uint8_t flag_SendingPacket = FALSE;
uint8_t SendingPacketFailed = FALSE;

/* Private function prototypes -----------------------------------------------*/
uint8_t TxCallback(ActionPacket* p, ActionPacket* next);
uint8_t RxCallback(ActionPacket* p, ActionPacket* next);

/* Private functions ---------------------------------------------------------*/

/**
* @brief  This routine is called when a receive event is complete. 
* @param  p: Current action packet which its transaction has been completed.
* @param  next: Next action packet which is going to be scheduled.
* @retval return value: TRUE
*/
uint8_t RxCallback(ActionPacket* p, ActionPacket* next)
{    
  /* received a packet */
  if( (p->status & BIT_TX_MODE) == 0) {
    
    if((p->status & IRQ_RCV_OK) != 0) {
      SdkEvalLedToggle(LED2);
      fifo_put(&blueRec_fifo, p->data[1]+2, &p->data[0]);
      
      if( (p->status & IRQ_ERR_ENC) != 0) {
        SdkEvalLedOn(LED2);
      }
      else {
        SdkEvalLedOff(LED2);
      }
    }
    else if((p->status & IRQ_TIMEOUT) != 0) {
      SdkEvalLedToggle(LED1);
      if(flag_SendingPacket == FALSE) {    
        HAL_RADIO_ReceivePacketWithAck(CHAT_FREQUENCYCHANNEL, CHAT_RECEIVE_RELATIVETIME, receivedData, sendAckData, CHAT_RECEIVE_TIMEOUT, RxCallback);
      }
      else {
        HAL_RADIO_SendPacketWithAck(CHAT_FREQUENCYCHANNEL, CHAT_TRANSMIT_RELATIVETIME, sendData, receivedAckData, CHAT_RECEIVE_TIMEOUT, TxCallback);
      }        
    }
    else if((p->status & IRQ_CRC_ERR) != 0) {
      if(flag_SendingPacket == FALSE) {    
        HAL_RADIO_ReceivePacketWithAck(CHAT_FREQUENCYCHANNEL, CHAT_RECEIVE_RELATIVETIME, receivedData, sendAckData, CHAT_RECEIVE_TIMEOUT, RxCallback);
      }
      else {
        HAL_RADIO_SendPacketWithAck(CHAT_FREQUENCYCHANNEL, CHAT_TRANSMIT_RELATIVETIME, sendData, receivedAckData, CHAT_RECEIVE_TIMEOUT, TxCallback);
      }   
    }      
  }
  
  /* Transmit complete */ 
  else {
    SdkEvalLedToggle(LED3);
    if(flag_SendingPacket == FALSE) { 
      HAL_RADIO_ReceivePacketWithAck(CHAT_FREQUENCYCHANNEL, CHAT_RECEIVE_RELATIVETIME, receivedData, sendAckData, CHAT_RECEIVE_TIMEOUT, RxCallback);
    }
    else {
      HAL_RADIO_SendPacketWithAck(CHAT_FREQUENCYCHANNEL,CHAT_TRANSMIT_RELATIVETIME, sendData, receivedAckData, CHAT_RECEIVE_TIMEOUT, TxCallback);
    }
  }
  
  return TRUE;   
}

/**
* @brief  This routine is called when a transmit event is complete. 
* @param  p: Current action packet which its transaction has been completed.
* @param  next: Next action packet which is going to be scheduled.
* @retval return value: TRUE
*/
uint8_t TxCallback(ActionPacket* p, ActionPacket* next)
{   
  /* received a packet */
  if((p->status & BIT_TX_MODE) == 0) {
    
    if((p->status & IRQ_RCV_OK) != 0) {
      flag_SendingPacket = FALSE;
      HAL_RADIO_ReceivePacketWithAck(CHAT_FREQUENCYCHANNEL, CHAT_RECEIVE_RELATIVETIME, receivedData, sendAckData, CHAT_RECEIVE_TIMEOUT, RxCallback);
    }
    else if((p->status & IRQ_TIMEOUT) != 0) {
      if (retries > 0) {
        HAL_RADIO_SendPacketWithAck(CHAT_FREQUENCYCHANNEL,CHAT_TRANSMIT_RELATIVETIME, sendData, receivedAckData, CHAT_RECEIVE_TIMEOUT, TxCallback);
        retries--;
      }
      else {
        flag_SendingPacket = FALSE;
        SendingPacketFailed = TRUE;
        HAL_RADIO_ReceivePacketWithAck(CHAT_FREQUENCYCHANNEL, CHAT_RECEIVE_RELATIVETIME, receivedData, sendAckData, CHAT_RECEIVE_TIMEOUT, RxCallback);
      }
    }
    else if((p->status & IRQ_CRC_ERR) != 0) {
      if (retries > 0) {
        HAL_RADIO_SendPacketWithAck(CHAT_FREQUENCYCHANNEL,CHAT_TRANSMIT_RELATIVETIME, sendData, receivedAckData, CHAT_RECEIVE_TIMEOUT, TxCallback);
        retries--;
      }
      else {
        flag_SendingPacket = FALSE;
        SendingPacketFailed = TRUE;
        HAL_RADIO_ReceivePacketWithAck(CHAT_FREQUENCYCHANNEL, CHAT_RECEIVE_RELATIVETIME, receivedData, sendAckData, CHAT_RECEIVE_TIMEOUT, RxCallback);        
      }
    }
  }
  /* Transmit complete */
  else {
  }
  
  return TRUE;
}

static FlagStatus xUartDataReady = RESET;
void IOProcessInputData(uint8_t* data_buffer, uint16_t Nb_bytes)
{
  static uint8_t cUartDataSize = 0;
  uint8_t i;
  
  for (i = 0; i < Nb_bytes; i++) {
    uart_buffer[cUartDataSize] = data_buffer[i];
    SdkEvalComIOSendData(data_buffer[i]);    
    cUartDataSize++;

    if((uart_buffer[cUartDataSize-1] == '\r') || (cUartDataSize == (MAX_CHAT_PACKET_LEN - MIC_FIELD_LEN))) {
      xUartDataReady = SET;
      Osal_MemCpy(&sendData[2], uart_buffer, cUartDataSize);
			sendData[1] = cUartDataSize + MIC_FIELD_LEN;
      cUartDataSize = 0;
    }
  }
}

/**
  * @brief  Main program code.
  * @param  None
  * @retval None
*/
int main(void)
{
  uint8_t temp[MAX_PACKET_LENGTH];
  uint8_t length;
  
  /* System Init */
  SystemInit();
  
  /* Identify BlueNRG-1 platform */
  SdkEvalIdentification();
  
  /* Configure I/O communication channel */
  SdkEvalComIOConfig(IOProcessInputData);  
  
  /* Configure the LEDs */
  SdkEvalLedInit(LED1);
  SdkEvalLedInit(LED2);
  SdkEvalLedInit(LED3);
  
  /* Radio configuration - HS_STARTUP_TIME 642 us, external LS clock, NULL, whitening enabled */
#if LS_SOURCE==LS_SOURCE_INTERNAL_RO
  RADIO_Init(HS_STARTUP_TIME, 1, NULL, ENABLE);
#else
  RADIO_Init(HS_STARTUP_TIME, 0, NULL, ENABLE);
#endif
  
  /* Create the blueRec FIFO */
  fifo_init(&blueRec_fifo, MAX_PACKET_LENGTH*2, blueRec_buffer, 1);
  
  /* Set the Network ID */
  HAL_RADIO_SetNetworkID(BLE_ADV_ACCESS_ADDRESS);
  
  /* Configures the transmit power level */
  RADIO_SetTxPower(MAX_OUTPUT_RF_POWER);

#ifdef PKT_ENCRYPTION
  /* Set the encryption parameter */
  uint8_t RxCounter[5]    = {0,0,0,0,0};
  uint8_t TxCounter[5]    = {0,0,0,0,0};
  uint8_t encKey[16]      = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xFF,0xFF};
  uint8_t encInitVector[8]= {0,0,0,0,0,0,0,0};
  
  RADIO_SetEncryptionCount(STATE_MACHINE_0, TxCounter, RxCounter);
  RADIO_SetEncryptionAttributes(STATE_MACHINE_0, encInitVector, encKey);
  RADIO_SetEncryptFlags(STATE_MACHINE_0, ENABLE, ENABLE);
#endif
  
  /* Receives a packet. Then sends a packet as an acknowledgment. */
  HAL_RADIO_ReceivePacketWithAck(CHAT_FREQUENCYCHANNEL, CHAT_RECEIVE_RELATIVETIME, receivedData, sendAckData, CHAT_RECEIVE_TIMEOUT, RxCallback); 
  
  /* Infinite loop */
  while(1) {
    /* Perform calibration procedure */
    RADIO_CrystalCheck();
    
    /* SendingPacket == FALSE */
    if(flag_SendingPacket == FALSE) {

      if(xUartDataReady == SET) {
        /* enable sending */
        sendData[0] = 0x02;
        xUartDataReady = RESET;
        retries = MAX_RETRIES;        
        flag_SendingPacket = TRUE; 
      }
    }
    
    if(fifo_size(&blueRec_fifo) !=0) {
      fifo_get(&blueRec_fifo, HEADER_LENGTH, temp);
      length = temp[1];
      fifo_get(&blueRec_fifo, length, &temp[2]);         
      
      /* -4 because the MIC field */
      for(uint8_t i= 2; i<(length+HEADER_LENGTH-MIC_FIELD_LEN); i++) {
        printf("%c", temp[i]);
      }
    }
    else if(SendingPacketFailed == TRUE) {
      SendingPacketFailed = FALSE;
      printf("\n\r NACK \r\n");
    }
  }
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
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

/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/
