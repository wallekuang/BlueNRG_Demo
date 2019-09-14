/******************** (C) COPYRIGHT 2017 STMicroelectronics ********************
* File Name          : DoublePacketTx_main.c 
* Author             : RF Application Team
* Version            : V1.1.0
* Date               : April-2018
* Description        : This code sends two packets with full payload 31+1 bytes
*                      to 2 different connections. 
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "main_common.h"
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"


/** @addtogroup BlueNRG1_StdPeriph_Examples
  * @{
  */

/** @addtogroup RADIO_Examples RADIO Examples
  * @{
  */

/** @addtogroup RADIO_TxRxDoublePacket RADIO TxRxDoublePacket Example
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MAX_PKT_TEST            1000
#define MAX_RETRY_TEST          3

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t sendData1[34];
uint8_t sendData2[34];
uint8_t RxAck[MAX_PACKET_LENGTH];
ActionPacket actPacket[3];

uint16_t pkt_counter_test[2] = {0, 0};
uint16_t retry_counter_test = 0;
uint16_t retry_counter_test_max[2] = {0, 0};
uint16_t ack_counter_test[2] = {0, 0};

#define SCHEDULE_NEW_PACKET     2
#define SCHEDEULE_RETRY         1
#define IDLE                    0
uint8_t repeat_flag = IDLE;         /* Reschedule the sequence */

#define COMM_DEVICE_1           1
#define COMM_DEVICE_2           2
uint8_t comm_token = COMM_DEVICE_1; /* Communication token */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Condition routine.
  * @param  ActionPacket
  * @retval TRUE
  */
uint8_t conditionRoutine(ActionPacket* p)
{
  return TRUE;   
} 

/**
  * @brief  Data routine.
  * @param  ActionPacket: current
  * @param  ActionPacket: next
  * @retval TRUE
  */
uint8_t dataRoutine(ActionPacket* p,  ActionPacket* next)
{
  /* received a packet */
  if( (p->status & BIT_TX_MODE) == 0) {
    
    if((p->status & IRQ_RCV_OK) != 0) {        
      //        if(ack_counter_test==0)
      //          pkt_counter_test = 0;
      
      if(p->data[0] == 0xAE) {
        ack_counter_test[comm_token-1]++;
        pkt_counter_test[comm_token-1]++;
        retry_counter_test = 0;
        repeat_flag = SCHEDULE_NEW_PACKET;
      }
      else {
        retry_counter_test++;
        retry_counter_test_max[comm_token-1]++;
        repeat_flag = SCHEDEULE_RETRY;
      }
    }
    else if((p->status & IRQ_TIMEOUT) != 0) {
      retry_counter_test++;
      retry_counter_test_max[comm_token-1]++;
      repeat_flag = SCHEDEULE_RETRY;
    }
    else if((p->status & IRQ_CRC_ERR) != 0) {
      retry_counter_test++;
      retry_counter_test_max[comm_token-1]++;
      repeat_flag = SCHEDEULE_RETRY;
    }
    
    if( retry_counter_test == MAX_RETRY_TEST) {
      pkt_counter_test[comm_token-1]++;
      retry_counter_test = 0;
      repeat_flag = SCHEDULE_NEW_PACKET;
    }
    
  }
  /* Transmit complete */
  else {
  }
  return TRUE;  
}


/**
  * @brief  Main program code.
  * @param  None
  * @retval None
  */
int main(void)
{ 
  /* System Init */
  SystemInit();
  
  /* Identify BlueNRG-1 platform */
  SdkEvalIdentification();
  SdkEvalComUartInit(UART_BAUDRATE);
  
  /* Radio configuration - HS_STARTUP_TIME, external LS clock, NULL, whitening enabled */
#if LS_SOURCE==LS_SOURCE_INTERNAL_RO
  RADIO_Init(HS_STARTUP_TIME, 1, NULL, ENABLE);
#else
  RADIO_Init(HS_STARTUP_TIME, 0, NULL, ENABLE);
#endif
  
  /* Build packet 1 */
  sendData1[0] = 0x0;  /* First byte */
  sendData1[1] = 31;   /* Length position is fixed */
  
  for(uint8_t i=0;i<(31+1);i++) {
    sendData1[2+i] = i+1;
  }
  
  /* Build packet 2 */
  sendData2[0] = 32; 
  sendData2[1] = 31;   /* Length position is fixed */
  
  for(uint8_t i=0;i<(31+1);i++) {
    sendData2[2+i] = i+33;
  }
  
  /* Build Action Packets */
  actPacket[0].StateMachineNo = STATE_MACHINE_0;
  actPacket[0].ActionTag = RELATIVE | TIMER_WAKEUP | TXRX | PLL_TRIG;
  actPacket[0].WakeupTime = TX_WAKEUP_TIME;                         /* 30 us minimum of internal delay before start action */
  actPacket[0].ReceiveWindowLength = 0;                 /* Not applied for TX */
  actPacket[0].data = (uint8_t *)&sendData1[0];         /* Data to send */
  actPacket[0].next_true = &actPacket[1];               /* If condRoutine returns TRUE => Go to next TX the 2nd part of the payload */
  actPacket[0].next_false = NULL_0;                     /* Not Used */   
  actPacket[0].condRoutine = conditionRoutine;          /* Condition routine */
  actPacket[0].dataRoutine = dataRoutine;               /* Data routine */
  
  
  actPacket[1].StateMachineNo = STATE_MACHINE_0;
  actPacket[1].ActionTag = RELATIVE | TIMER_WAKEUP | TXRX;
  actPacket[1].WakeupTime = TX_WAKEUP_TIME;                         /* 40 us minimum of internal delay before start action */
  actPacket[1].ReceiveWindowLength = 0;                 /* Not applied for TX */
  actPacket[1].data = (uint8_t *)&sendData2[0];         /* Data to send */
  actPacket[1].next_true = &actPacket[2];               /* If condRoutine returns TRUE => Go to RX to receive the ACK packet */
  actPacket[1].next_false = NULL_0;                     /* Not Used */   
  actPacket[1].condRoutine = conditionRoutine;          /* Condition routine */
  actPacket[1].dataRoutine = dataRoutine;               /* Data routine */
  
  actPacket[2].StateMachineNo = STATE_MACHINE_0;
  actPacket[2].ActionTag = RELATIVE | TIMER_WAKEUP;
  actPacket[2].WakeupTime = RX_WAKEUP_TIME;                         /* 30 us minimum of internal delay before start action */
  actPacket[2].ReceiveWindowLength = RX_TIMEOUT_ACK;              /* RX timeout in us */
  actPacket[2].data = (uint8_t *)&RxAck[0];             /* Buffer for ACK packet reception */
  actPacket[2].next_true = NULL_0;                      /* Not Used */   
  actPacket[2].next_false = NULL_0;                     /* Not Used */   
  actPacket[2].condRoutine = conditionRoutine;          /* Condition routine */
  actPacket[2].dataRoutine = dataRoutine;               /* Data routine */
  
  /* Channel map configuration */
  uint8_t map[5]= {0xFF,0xFF,0xFF,0xFF,0xFF};
  RADIO_SetChannelMap(STATE_MACHINE_0, &map[0]);
  
  /* Setting of channel and the channel increment */
  RADIO_SetChannel(STATE_MACHINE_0, FREQUENCY_CHANNEL, 0);

  /* Sets of the NetworkID and the CRC. The last parameter SCA is not used */
  RADIO_SetTxAttributes(STATE_MACHINE_0, ADDRESS_DEV1, 0x555555, 0);
  
  /* Configures the transmit power level */
  RADIO_SetTxPower(MAX_OUTPUT_RF_POWER);
  
  /* Call this function before execute the action packets */
  RADIO_SetReservedArea(&actPacket[0]);
  RADIO_SetReservedArea(&actPacket[1]);
  RADIO_SetReservedArea(&actPacket[2]);
  
  /* Call this function for the first action packet to be executed */
  RADIO_MakeActionPacketPending(&actPacket[0]);
  
  /* Infinite loop */
  while(1) {
    /* Perform calibration procedure */    
    RADIO_CrystalCheck();  
    
    if(pkt_counter_test[comm_token-1] > (MAX_PKT_TEST-1)) {
      printf("PER = %.1f%%  PCKT sent %d, ACK received %d Retry %d\r\n",((float)(pkt_counter_test[comm_token-1] - ack_counter_test[comm_token-1]))/pkt_counter_test[comm_token-1]*100.0, pkt_counter_test[comm_token-1], ack_counter_test[comm_token-1], retry_counter_test_max[comm_token-1]);
      ack_counter_test[comm_token-1] = 0;
      pkt_counter_test[comm_token-1] = 0;
      retry_counter_test_max[comm_token-1] = 0;
    }
    if(pkt_counter_test[comm_token-1] > (MAX_PKT_TEST-1)) {
      printf("PER = %.1f%%  PCKT sent %d, ACK received %d Retry %d\r\n",((float)(pkt_counter_test[comm_token-1] - ack_counter_test[comm_token-1]))/pkt_counter_test[comm_token-1]*100.0, pkt_counter_test[comm_token-1], ack_counter_test[comm_token-1], retry_counter_test_max[comm_token-1]);
      ack_counter_test[comm_token-1] = 0;
      pkt_counter_test[comm_token-1] = 0;
      retry_counter_test_max[comm_token-1] = 0;
    }
    
    /* If need to schedule a new packet transmission */
    if(repeat_flag == SCHEDULE_NEW_PACKET) {
      
      /* Check the communication token */
      if(comm_token == COMM_DEVICE_1) {
        RADIO_SetTxAttributes(STATE_MACHINE_0, ADDRESS_DEV2, 0x555555, 0);
        comm_token = COMM_DEVICE_2;
      }
      else if(comm_token == COMM_DEVICE_2) {
        RADIO_SetTxAttributes(STATE_MACHINE_0, ADDRESS_DEV1, 0x555555, 0);
        comm_token = COMM_DEVICE_1;
      }
      repeat_flag = SCHEDEULE_RETRY;
    }
    
    /* Schedule the next communication */
    if(repeat_flag == SCHEDEULE_RETRY) {
      RADIO_MakeActionPacketPending(&actPacket[0]);
      repeat_flag = IDLE;
      
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
