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
#include "clock.h"
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
#define MAX_NUM_PACKET          1000    /* Number of packets used for the test */
#define DATA_PACKET_LEN         20 //20 //255 //31    /* Number of packets used for the test */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t packet_counter = 0;
uint16_t crc_error_counter = 0;
uint16_t timeout_error_counter = 0;
uint8_t channel = FREQUENCY_CHANNEL;
uint8_t button_flag = 0;

//static tClockTime time = 0, time2 = 0, diff = 0, time_cumulate = 0;

static uint32_t time2 = 0, diff = 0, time_cumulate = 0;
extern uint32_t timer_reload;
static uint32_t timer_reload_store = 0;

uint8_t sendData[MAX_PACKET_LENGTH];
uint8_t receivedData[MAX_PACKET_LENGTH];
uint8_t sendNewPacket = TRUE;

/* Private function prototypes -----------------------------------------------*/
uint8_t TxCallback(ActionPacket* p, ActionPacket* next);
void SdkDelayMs(volatile uint32_t lTimeMs);

/* Private functions ---------------------------------------------------------*/


/**
  * @brief  MFT_Configuration.
  * @param  None
  * @retval None
  */
void MFT_Configuration(void)
{
  NVIC_InitType NVIC_InitStructure;
  MFT_InitType timer_init;
  
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_MTFX1, ENABLE);
  
  MFT_StructInit(&timer_init);
  
  timer_init.MFT_Mode = MFT_MODE_1;
  
#ifdef BLUENRG2_DEVICE
  timer_init.MFT_Prescaler = 32-1;      /* 1 us clock */
#else
  timer_init.MFT_Prescaler = 16-1;      /* 1 us clock */
#endif
  
  /* MFT1 configuration */
  timer_init.MFT_Clock1 = MFT_PRESCALED_CLK;
  timer_init.MFT_Clock2 = MFT_NO_CLK;
  timer_init.MFT_CRA = 65000 - 1;
  timer_init.MFT_CRB = 65000 - 1;
  MFT_Init(MFT1, &timer_init);
  
  /* Enable MFT2 Interrupt 1 */
  NVIC_InitStructure.NVIC_IRQChannel = MFT1A_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LOW_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  MFT_EnableIT(MFT1, MFT_IT_TNA | MFT_IT_TNB, ENABLE);
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
      time2 = MFT1->TNCNT1;
      timer_reload_store = timer_reload;
      packet_counter++;
    }
    else if((p->status & IRQ_TIMEOUT) != 0) {
    }
    else if((p->status & IRQ_CRC_ERR) != 0) {
    }
    
    MFT1->TNCNT1 = 0xFFFF;
    if(packet_counter != MAX_NUM_PACKET) {
      sendNewPacket = TRUE;
    }
  }
  /* Transmit complete */
  else {
#ifdef UNIDIRECTIONAL_TEST
    time2 = MFT1->TNCNT1;
    timer_reload_store = timer_reload;
    MFT1->TNCNT1 = 0xFFFF;
    packet_counter++;
    if(packet_counter != MAX_NUM_PACKET) {
      sendNewPacket = TRUE;
    }
#endif
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
  SdkEvalLedInit(LED1);
  SdkEvalLedInit(LED2);
  
  /* Init Clock */
  MFT_Configuration();
  
  /* Configure I/O communication channel */
  SdkEvalComUartInit(UART_BAUDRATE);
  
  /* Radio configuration - HS_STARTUP_TIME, external LS clock, NULL, whitening enabled */
#if LS_SOURCE==LS_SOURCE_INTERNAL_RO
  RADIO_Init(HS_STARTUP_TIME, 1, NULL, ENABLE);
#else
  RADIO_Init(HS_STARTUP_TIME, 0, NULL, ENABLE);
#endif
    
  if(DATA_PACKET_LEN > (MAX_PACKET_LENGTH-HEADER_LENGTH)) {
    printf("DATA_PACKET_LEN too big %d\r\n", DATA_PACKET_LEN);
    while(1);
  }
     
  /* Build packet */
  sendData[0] = 0x02;
  sendData[1] = DATA_PACKET_LEN;   /* Length position is fixed */
  for(volatile uint16_t j=0; j<DATA_PACKET_LEN; j++) {
    sendData[2+j] = 0xAE;
  }
  
  /* Set the Network ID */
  HAL_RADIO_SetNetworkID(BLE_ADV_ACCESS_ADDRESS);
  
  /* Configures the transmit power level */
  RADIO_SetTxPower(MAX_OUTPUT_RF_POWER);
  
  printf("BlueNRG-1 Radio Driver Throughput Application\r\n");
    
  /* Start MFT timer */
  MFT_Cmd(MFT1, ENABLE);
  
  /* Infinite loop */
  while(1) {
    /* Perform calibration procedure */    
    RADIO_CrystalCheck();
    
    if(packet_counter == MAX_NUM_PACKET) {
      if(time_cumulate!=0) {
        printf("%d TX packets. %d PCKT LEN. Average time: %.1f ms. Data throughput: %.1f kbps.\r\n", packet_counter, DATA_PACKET_LEN, (((float)time_cumulate)/1000.0/packet_counter), (((float)packet_counter*DATA_PACKET_LEN*8)*1000.0)/time_cumulate);
      }
      packet_counter = 0;
      timeout_error_counter = 0;
      crc_error_counter = 0;
      sendData[6] = 0;
      sendNewPacket = TRUE;
      time_cumulate = 0;
      for(volatile uint32_t i = 0; i<0xFFFFF; i++);
    }
    
    
    if(sendNewPacket == TRUE) {
      sendNewPacket = FALSE;
      diff = (0xFFFF - time2) + timer_reload_store*65000;
      time_cumulate += diff;
#ifdef UNIDIRECTIONAL_TEST
      ret = HAL_RADIO_SendPacket(channel, TX_WAKEUP_TIME, sendData, TxCallback);
#else
      ret = HAL_RADIO_SendPacketWithAck(channel, TX_WAKEUP_TIME, sendData, receivedData, RX_TIMEOUT_ACK, TxCallback);
#endif
      if(ret != SUCCESS_0) {
        printf("ERROR %d (%d)\r\n",ret, packet_counter);
      }
      MFT1->TNCNT1 = 0xFFFF;
      timer_reload = 0;
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
