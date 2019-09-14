/******************** (C) COPYRIGHT 2017 STMicroelectronics ********************
* File Name          : RADIO_RX_main.c 
* Author             : RF Application Team
* Version            : V1.1.0
* Date               : April-2018
* Description        : Code demostrating a simple TX/RX scenario
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
#include "main_common.h"
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"
#include "hal_radio.h"
#include "osal.h"
#include "fifo.h"
#if ST_USE_OTA_RESET_MANAGER
#include "radio_ota.h"
#endif

/** @addtogroup BlueNRG1_StdPeriph_Examples
* @{
*/

/** @addtogroup RADIO_Examples RADIO Examples
* @{
*/

/** @addtogroup RADIO_TxRx RADIO TxRx Example
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t packet_counter = 0;
uint8_t channel = FREQUENCY_CHANNEL;
uint8_t sendAckData[2] = {0xAE, 0}; /* 0xAE ACK value, length = 0 */
uint8_t receivedData[MAX_PACKET_LENGTH];


uint8_t rx_done = FALSE, button_flag = 0;

/* Private function prototypes -----------------------------------------------*/
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
  uint8_t ret;
  
  /* received a packet */
  if( (p->status & BIT_TX_MODE) == 0) {
    
    if((p->status & IRQ_RCV_OK) != 0) {
      SdkEvalLedToggle(LED1);
      rx_done = TRUE;
    }
    else if( ((p->status & IRQ_TIMEOUT) != 0) || ((p->status & IRQ_CRC_ERR) != 0) ){
      SdkEvalLedToggle(LED2);
      ret = HAL_RADIO_ReceivePacketWithAck(channel, RX_WAKEUP_TIME, receivedData, sendAckData, RX_TIMEOUT, RxCallback);
      if(ret != SUCCESS_0) {
        SdkEvalLedOn(LED1);
        SdkEvalLedOn(LED2);
        while(1);
      }
    }
  }
  
  /* Transmit complete */ 
  else {
    ret = HAL_RADIO_ReceivePacketWithAck(channel, RX_WAKEUP_TIME, receivedData, sendAckData, RX_TIMEOUT, RxCallback);
    if(ret != SUCCESS_0) {
      SdkEvalLedOn(LED1);
      SdkEvalLedOn(LED2);
      while(1);
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
  uint8_t ret;
  
  /* System Init */
  SystemInit();
  
  /* Identify BlueNRG-1 platform */
  SdkEvalIdentification();
  
  /* Configure the LEDs */
  SdkEvalLedInit(LED1);
  SdkEvalLedInit(LED2);
    
  /* Radio configuration - HS_STARTUP_TIME, external LS clock, NULL, whitening enabled */
#if LS_SOURCE==LS_SOURCE_INTERNAL_RO
  RADIO_Init(HS_STARTUP_TIME, 1, NULL, ENABLE);
#else
  RADIO_Init(HS_STARTUP_TIME, 0, NULL, ENABLE);
#endif
    
  /* Set the Network ID */
  HAL_RADIO_SetNetworkID(BLE_ADV_ACCESS_ADDRESS);
  
  /* Configures the transmit power level */
  RADIO_SetTxPower(MAX_OUTPUT_RF_POWER);
  
  ret = HAL_RADIO_ReceivePacketWithAck(channel, RX_WAKEUP_TIME, receivedData, sendAckData, RX_TIMEOUT, RxCallback);
  if(ret != SUCCESS_0) {
    SdkEvalLedOn(LED1);
    SdkEvalLedOn(LED2);
    while(1);
  }   
  
  /* Infinite loop */
  while(1) {
    /* Perform calibration procedure */    
    RADIO_CrystalCheck();  
    
    if(rx_done == TRUE) {
      rx_done = FALSE;
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
