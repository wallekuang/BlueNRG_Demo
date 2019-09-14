/******************** (C) COPYRIGHT 2017 STMicroelectronics ********************
* File Name          : RADIO_TX_main.c 
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
#define MAX_NUM_PACKET          100    /* Number of packets used for the test */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t packet_counter = 0;
uint16_t crc_error_counter = 0;
uint16_t timeout_error_counter = 0;
volatile extern uint32_t lSystickCounter;
uint8_t channel = FREQUENCY_CHANNEL;
uint8_t button_flag = 0;

uint8_t sendData[33];
uint8_t receivedData[MAX_PACKET_LENGTH];
uint8_t sendNewPacket = TRUE;

/* Private function prototypes -----------------------------------------------*/
uint8_t TxCallback(ActionPacket* p, ActionPacket* next);
void SdkDelayMs(volatile uint32_t lTimeMs);

/* Private functions ---------------------------------------------------------*/


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
    }
    else if((p->status & IRQ_TIMEOUT) != 0) {
      SdkEvalLedToggle(LED2);
      timeout_error_counter++;
    }
    else if((p->status & IRQ_CRC_ERR) != 0) {
      SdkEvalLedToggle(LED3);
      crc_error_counter++;
    }
    
    sendData[6] = (uint8_t)(++packet_counter);
    
    if(packet_counter != MAX_NUM_PACKET) {
      SdkEvalLedToggle(LED1);
      sendNewPacket = TRUE;
    }
  }
  /* Transmit complete */
  else {
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
  
  /* Configure I/O communication channel */
  SdkEvalComUartInit(UART_BAUDRATE);
  
  /* Configure the LEDs */
  SdkEvalLedInit(LED1);
  SdkEvalLedInit(LED2);
  SdkEvalLedInit(LED3);
  
  SdkEvalPushButtonInit(BUTTON_1);
  SdkEvalPushButtonIrq(BUTTON_1, IRQ_ON_RISING_EDGE);

#if ST_USE_OTA_RESET_MANAGER
  SdkEvalPushButtonInit(BUTTON_2);
#endif

  /* Configure SysTick to generate interrupt */
  SysTick_Config(SYST_CLOCK/1000 - 1);  
  
  /* Radio configuration - HS_STARTUP_TIME, external LS clock, NULL, whitening enabled */
#if LS_SOURCE==LS_SOURCE_INTERNAL_RO
  RADIO_Init(HS_STARTUP_TIME, 1, NULL, ENABLE);
#else
  RADIO_Init(HS_STARTUP_TIME, 0, NULL, ENABLE);
#endif
    
  /* Build packet */
  sendData[0] = 0x02;
  sendData[1] = 5;   /* Length position is fixed */
  sendData[2] = 0x01;
  sendData[3] = 0x02;
  sendData[4] = 0x03;
  sendData[5] = 0x04;
  sendData[6] = (uint8_t)packet_counter;
  
  /* Set the Network ID */
  HAL_RADIO_SetNetworkID(BLE_ADV_ACCESS_ADDRESS);
  
  /* Configures the transmit power level */
  RADIO_SetTxPower(MAX_OUTPUT_RF_POWER);
  
  /* Infinite loop */
  while(1) {
    /* Perform calibration procedure */    
    RADIO_CrystalCheck();  
    
    if(sendNewPacket == TRUE) {
      sendNewPacket = FALSE;
      SdkDelayMs(50);
      
      ret = HAL_RADIO_SendPacketWithAck(channel,TX_WAKEUP_TIME, sendData, receivedData, RX_TIMEOUT_ACK, TxCallback);
      if(ret != SUCCESS_0) {
        printf("ERROR %d (%d)\r\n",ret, packet_counter);
      }
    }
    
    if(packet_counter == MAX_NUM_PACKET) {
      printf("Channel = %d,Timeout errors = %d,CRC errors = %d,PER = %.2f %%\r\n",channel, timeout_error_counter, crc_error_counter, ((float)(timeout_error_counter + crc_error_counter))/packet_counter*100.0);
      packet_counter = 0;
      timeout_error_counter = 0;
      crc_error_counter = 0;
      sendData[6] = 0;
      sendNewPacket = TRUE;
      SdkDelayMs(100);
    }
    
    if(button_flag == 1) {
      button_flag = 0;
      printf("Channel %d\r\n",channel);
    }

#if ST_USE_OTA_RESET_MANAGER
    if (SdkEvalPushButtonGetState(BUTTON_2) == RESET)
    {
      while(sendNewPacket == FALSE);
      OTA_Jump_To_Reset_Manager();
    }
#endif

  }
}


/**
* @brief  Delay function
* @param  Delay in ms
* @retval None
*/
void SdkDelayMs(volatile uint32_t lTimeMs)
{
  uint32_t nWaitPeriod = ~lSystickCounter;
  
  if(nWaitPeriod<lTimeMs)
  {
    while( lSystickCounter != 0xFFFFFFFF);
    nWaitPeriod = lTimeMs-nWaitPeriod;
  }
  else
    nWaitPeriod = lTimeMs+ ~nWaitPeriod;
  
  while( lSystickCounter != nWaitPeriod ) ;

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
