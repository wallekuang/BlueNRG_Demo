/******************** (C) COPYRIGHT 2017 STMicroelectronics ********************
* File Name          : RADIO_Sniffer_main.c
* Author             : RF Application Team
* Version            : V1.1.0
* Date               : April-2018
* Description        : Sniffer application.
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

/** @addtogroup RADIO_Sniffer RADIO Sniffer Example
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BLE_ADV_ACCESS_ADDRESS  (uint32_t)(0x8E89BED6)
#define HS_STARTUP_TIME         (uint16_t)(1)  /* High Speed start up time min value */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Blue FIFO */
circular_fifo_t blueRec_fifo;
uint8_t blueRec_buffer[MAX_PACKET_LENGTH*2+MAX_PACKET_LENGTH];

circular_fifo_t uart_fifo;
uint8_t uart_buffer[MAX_PACKET_LENGTH+MAX_PACKET_LENGTH];

uint8_t sendData[MAX_PACKET_LENGTH];
uint8_t receivedData[MAX_PACKET_LENGTH];

uint32_t delay =1000;
uint8_t channel = 24;                    
uint32_t timeOut = 100000;
int32_t rssi_val = 0;
uint32_t timestamp = 0;

#ifdef PACKET_ENCRYPTION
#define MIC_FIELD_LEN                   MIC_FIELD_LENGTH
uint8_t count_tx[5]     = {0x00,0x00,0x00,0x00,0x00};
uint8_t count_rcv[5]    = {0x00,0x00,0x00,0x00,0x00};
uint8_t enc_key[16]     = {0xBF,0x01,0xFB,0x9D,0x4E,0xF3,0xBC,0x36,0xD8,0x74,0xF5,0x39,0x41,0x38,0x68,0x4C};
uint8_t enc_iv[8]       = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
#else

#define MIC_FIELD_LEN                   0
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

uint8_t nullfunction(ActionPacket* p,  ActionPacket* next)
{
  return TRUE;
}


uint8_t RxCallback(ActionPacket* p, ActionPacket* next)
{
  if( (p->status & IRQ_DONE) != 0) {
    
    /* received a packet */
    if( (p->status & BIT_TX_MODE) == 0) {
      
      if((p->status & IRQ_RCV_OK) != 0) {
          fifo_put(&blueRec_fifo, p->data[1]+2, p->data);
          fifo_put(&blueRec_fifo, 4, (uint8_t*)(&p->rssi));
          fifo_put(&blueRec_fifo, 4, (uint8_t*)(&p->timestamp_receive));
          
          HAL_RADIO_ReceivePacket(channel, delay, receivedData, timeOut, RxCallback);
      }
      else if((p->status & IRQ_TIMEOUT) != 0) {
        HAL_RADIO_ReceivePacket(channel,delay, receivedData, timeOut, RxCallback);
      }
      else if((p->status & IRQ_CRC_ERR) != 0) {
        HAL_RADIO_ReceivePacket(channel, delay, receivedData, timeOut, RxCallback);
      }
    }
    
    /* Transmit complete */ 
    else {
    }
  }
  return TRUE;
}


int main(void)
{
  uint8_t temp[MAX_PACKET_LENGTH+8];
  uint16_t length;
  
  /* System Init */
  SystemInit();
  
  /* Identify BlueNRG-1 platform */
  SdkEvalIdentification();
  
  /* Configure I/O communication channel */
  SdkEvalComUartInit(UART_BAUDRATE);
  
  /* Create the blueRec FIFO */
  fifo_init(&blueRec_fifo, MAX_PACKET_LENGTH*2, blueRec_buffer, 1);
  
  /* Create the blueRec FIFO */
  fifo_init(&uart_fifo, MAX_PACKET_LENGTH, uart_buffer, 1);
  
  /* Radio configuration - HS_STARTUP_TIME 642 us, external LS clock, NULL, whitening enabled */
#if LS_SOURCE==LS_SOURCE_INTERNAL_RO
  RADIO_Init(HS_STARTUP_TIME, 1, NULL, ENABLE);
#else
  RADIO_Init(HS_STARTUP_TIME, 0, NULL, ENABLE);
#endif
  
  /* Set the Network ID */
  HAL_RADIO_SetNetworkID(BLE_ADV_ACCESS_ADDRESS); /* 0x8E89BED6 */
    
  printf("\r\nSniffing channel: %d, Network_ID: 0x%08X\r\n", channel, BLE_ADV_ACCESS_ADDRESS);
  
#ifdef PACKET_ENCRYPTION
  /* Set Encryption key and Initial vector */
  RADIO_SetEncryptionAttributes(STATE_MACHINE_0, enc_iv, enc_key);
  
  RADIO_SetEncryptionCount(STATE_MACHINE_0, &count_tx[0], &count_rcv[0]); 
  
  /* Enable encryption:1 for Enable, 0 for disable */
  RADIO_SetEncryptFlags(STATE_MACHINE_0, ENABLE, ENABLE);
#endif
  
  /* Receives a packet on the desired channel. */
  HAL_RADIO_ReceivePacket(channel, delay, receivedData, timeOut, RxCallback);

  /* Infinite loop */
  while(1) {
    /* Perform calibration procedure */
    RADIO_CrystalCheck();
    
    /* Print the packets sniffed */
    if(fifo_size(&blueRec_fifo) !=0) {
      
      /* Get the length field */
      fifo_get(&blueRec_fifo, HEADER_LENGTH, &temp[0]);
      length = temp[1];
      
      /* Get the packet */
      fifo_get(&blueRec_fifo, length, &temp[2]);
      
      /* Get the RSSI information */
      fifo_get(&blueRec_fifo, 4, &temp[length+2]);
      rssi_val = temp[length+2];
      rssi_val |= temp[length+2+1]<<8;
      rssi_val |= temp[length+2+2]<<16;
      rssi_val |= temp[length+2+3]<<24;
      
      /* Get the timestamp */
      fifo_get(&blueRec_fifo, 4, &temp[length+2+4]);
      timestamp = temp[length+2+4];
      timestamp |= temp[length+2+4+1]<<8;
      timestamp |= temp[length+2+4+2]<<16;
      timestamp |= temp[length+2+4+3]<<24;
      
      printf("\r\nchannel: %d, RSSI: %.0f dBm\r\n", channel, (float)rssi_val);
      printf("Timestamp: %.3f ms\r\n", ((float)timestamp)/512.0);
      printf("Frame: ");
      for(uint16_t i= 0; i<(length+2-MIC_FIELD_LEN); i++) {
        printf("%02x:", temp[i]);
      }
#ifdef PACKET_ENCRYPTION
      printf("\r\nMIC: ");
      for(uint16_t i= (length+2-MIC_FIELD_LEN); i<(length+2); i++) {
        printf("%02x:", temp[i]);
      }
#endif
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
