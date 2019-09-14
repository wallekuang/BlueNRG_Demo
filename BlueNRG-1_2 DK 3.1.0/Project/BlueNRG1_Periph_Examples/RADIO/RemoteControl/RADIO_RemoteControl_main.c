/******************** (C) COPYRIGHT 2017 STMicroelectronics ********************
* File Name          : remoteControl.c 
* Author             : RF Application Team
* Version            : V1.1.0
* Date               : April-2018
* Description        : This code  shows a basic remote control scenario.
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
#include "BlueNRG1_conf.h"
#include "hal_radio.h"
#include "SDK_EVAL_Config.h"


/** @addtogroup BlueNRG1_StdPeriph_Examples
* @{
*/

/** @addtogroup RADIO_Examples RADIO Examples
* @{
*/

/** @addtogroup RADIO_RemoteControl RADIO Remote Control Example
* @{
*/


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define RX_RELATIVETIME        1000
#define TX_RELATIVETIME        2000

#define TX_RX_RELATIVETIME     5000

#define RX_RECEIVE_TIMEOUT         20000
#define TX_RECEIVE_TIMEOUT        100000

#define BLE_ADV_ACCESS_ADDRESS  (uint32_t)(0x8E89BED6)
#define FREQUENCY_CHANNEL       (uint8_t)(24)    // RF channel 22
#define HS_STARTUP_TIME         (uint16_t)(1)  /* High Speed start up time min value */
#define WAKEUP_TIME             100000         /* Wake up time 100 ms */


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

uint8_t flag_SendingPacket = FALSE;
uint8_t sendData[MAX_PACKET_LENGTH];
uint8_t receivedData[MAX_PACKET_LENGTH];

uint8_t LEDisON = 0;

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
      if(LEDisON) {
        SdkEvalLedOff(LED1);
        LEDisON = 0;
      }
      else {
        SdkEvalLedOn(LED1);
        LEDisON = 1;
      }
      
      if(flag_SendingPacket == FALSE) {
        HAL_RADIO_ReceivePacket(FREQUENCY_CHANNEL, RX_RELATIVETIME, receivedData, RX_RECEIVE_TIMEOUT, RxCallback);
      }
      else {
        HAL_RADIO_SendPacket(FREQUENCY_CHANNEL, TX_RELATIVETIME, sendData, TxCallback);
        flag_SendingPacket = FALSE;
      }
    }
    else if(((p->status & IRQ_TIMEOUT) != 0) || ((p->status & IRQ_CRC_ERR) != 0)) {        
      if(flag_SendingPacket == FALSE) {
        HAL_RADIO_ReceivePacket(FREQUENCY_CHANNEL, RX_RELATIVETIME, receivedData, RX_RECEIVE_TIMEOUT, RxCallback);
      }
      else {
        HAL_RADIO_SendPacket(FREQUENCY_CHANNEL, TX_RELATIVETIME, sendData, TxCallback);
        flag_SendingPacket = FALSE;
      }
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
  if( (p->status & IRQ_DONE) != 0) {
    
    /* received a packet */
    if( (p->status & BIT_TX_MODE) == 0) {
      
    }
    /* Transmit complete */
    else {
      HAL_RADIO_ReceivePacket(FREQUENCY_CHANNEL, TX_RX_RELATIVETIME, receivedData, TX_RECEIVE_TIMEOUT, RxCallback);
    }
  }
  return TRUE;
}


/**
* @brief  This main routine. 
*
*/
int main(void)
{
  /* System Init */
  SystemInit();
  
  /* Identify BlueNRG-1 platform */
  SdkEvalIdentification();
  
  /* Configure the USER_BUTTON */
  SdkEvalPushButtonInit(USER_BUTTON);
  
  /* Configure the LED1 */
  SdkEvalLedInit(LED1);
  
  /* Build ACK packet */
  sendData[0] = 0x02; 
  sendData[1] = 6;      /* Length position is fixed */
  sendData[2] = 0x01;
  sendData[3] = 0x02;
  sendData[4] = 0x03;
  sendData[5] = 0x04;
  sendData[6] = 0x05;
  sendData[7] = 0x06;
  
  /* Radio configuration - HS_STARTUP_TIME, external LS clock, NULL, whitening enabled */
#if LS_SOURCE==LS_SOURCE_INTERNAL_RO
  RADIO_Init(HS_STARTUP_TIME, 1, NULL, ENABLE);
#else
  RADIO_Init(HS_STARTUP_TIME, 0, NULL, ENABLE);
#endif
  
  /* Set the Network ID */
  HAL_RADIO_SetNetworkID(BLE_ADV_ACCESS_ADDRESS);
  
  /* Receives a packet. Then sends a packet as an acknowledgment. */
  HAL_RADIO_ReceivePacket(FREQUENCY_CHANNEL, TX_RX_RELATIVETIME, receivedData, TX_RECEIVE_TIMEOUT, RxCallback);
  
  /* Infinite loop */
  while(1) {
    /* Perform calibration procedure */
    RADIO_CrystalCheck();
    
    /* If USER_BUTTON is pressed a packet is sent */
    if(SdkEvalPushButtonGetState(USER_BUTTON) == RESET && flag_SendingPacket == FALSE) {
      flag_SendingPacket = TRUE;
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
