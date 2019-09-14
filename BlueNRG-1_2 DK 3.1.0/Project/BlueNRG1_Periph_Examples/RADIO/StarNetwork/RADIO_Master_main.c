/******************** (C) COPYRIGHT 2018 STMicroelectronics ********************
* File Name          : RADIO_Master_main.c 
* Author             : RF Application Team
* Version            : V1.0.0
* Date               : March-2018
* Description        : The code defines a Master device of a start network. 
*                      The Master knows how many slaves are in the network and their slave address. 
*                      Then, periodically, asks for data the slaves and waits for their data. 
*                      If data are received, then the Master answers with an ACK packet to the selected Slave.
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
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"
#include "main_common.h"


/** @addtogroup BlueNRG1_StdPeriph_Examples
  * @{
  */

/** @addtogroup RADIO_Examples RADIO Examples
  * @{
  */

/** @addtogroup RADIO_StarNetwork RADIO StarNetwork Example
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
uint8_t comm_token = 1;         /* Communication token */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t sendmeBuffer[2];
uint8_t receiveBuffer[MAX_PACKET_LENGTH];
uint8_t ackBuffer[2];

uint32_t networkID[NSLAVES] = {SLAVE_DEV1, SLAVE_DEV2};

ActionPacket waitDataAction;
ActionPacket sendmeAction;
ActionPacket sendAckAction;

uint32_t request_counter_test[NSLAVES] = {0,};
uint32_t pkt_counter_test[NSLAVES] = {0,};
uint32_t pkt_lost_counter_test[NSLAVES] = {0,};
uint8_t retry = 0;
uint8_t printer = 0;

/* Private function prototypes -----------------------------------------------*/
void InitializationActionPackets(void);
uint8_t sendmeCB(ActionPacket* p);
uint8_t waitDataCB(ActionPacket* p);
uint8_t sendAckCB(ActionPacket* p);
uint8_t dataRoutine(ActionPacket* p,  ActionPacket* next);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Condition routine.
  * @param  ActionPacket
  * @retval TRUE
  */
uint8_t sendmeCB(ActionPacket* p)
{
  SdkEvalLedToggle(LED1);
  request_counter_test[comm_token-1]++;
  printer++;
  return TRUE;
} 


uint8_t waitDataCB(ActionPacket* p)
{
  /* received a packet */
  if( (p->status & BIT_TX_MODE) == 0) {
    
    if((p->status & IRQ_RCV_OK) != 0) {
      printer++;
      pkt_counter_test[comm_token-1]++;
      return TRUE;
    }
    else if(((p->status & IRQ_TIMEOUT) != 0) || ((p->status & IRQ_CRC_ERR) != 0)) {
      pkt_lost_counter_test[comm_token-1]++;
      printer = 0;
      
      comm_token++;
      if(comm_token > NSLAVES) {
        comm_token = 1;
      }
      RADIO_SetTxAttributes(STATE_MACHINE_0, networkID[comm_token-1], 0x555555, 0);
      
      return FALSE;      
    }
  }
  return TRUE;
}

/**
  * @brief  Data routine.
  * @param  ActionPacket: current
  * @param  ActionPacket: next
  * @retval TRUE
  */
uint8_t sendAckCB(ActionPacket* p)
{
  retry = 0;
  
  comm_token++;
  if(comm_token > NSLAVES) {
    comm_token = 1;
  }
  RADIO_SetTxAttributes(STATE_MACHINE_0, networkID[comm_token-1], 0x555555, 0);
  
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
  if( (p->status & IRQ_DONE) != 0) {
    
    /* received a packet */
    if( (p->status & BIT_TX_MODE) == 0) {
      
      if((p->status & IRQ_RCV_OK) != 0) {
      }
      else if((p->status & IRQ_TIMEOUT) != 0) {
      }
      else if((p->status & IRQ_CRC_ERR) != 0) {
      }
    }
    /* Transmit complete */
    else {
      
    }
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
  SdkEvalLedInit(LED1);
  SdkEvalComUartInit(UART_BAUDRATE);
  
  /* Radio configuration - HS_STARTUP_TIME 642 us, external LS clock, NULL, whitening enabled */
  RADIO_Init(HS_STARTUP_TIME, 0, NULL, ENABLE);
  
  /* Configures the transmit power level */
  RADIO_SetTxPower(MAX_OUTPUT_RF_POWER);
    
  /* Build send me packet */
  sendmeBuffer[0] = 0xB4;
  sendmeBuffer[1] = 0;
  
  /* Build ACK packet */
  sendmeBuffer[0] = 0xAE;
  sendmeBuffer[1] = 0;
  
  /* Initialize the routine for sending data and wait for the ACK */
  InitializationActionPackets();
  
  
  /* Call this function for the first action packet to be executed */
  RADIO_MakeActionPacketPending(&sendmeAction);
  
  printf("\r\n\nRequest Sent,Packet received, Request lost\r\n");
  
  /* Infinite loop */
  while(1) {
    /* Perform calibration procedure */
    RADIO_CrystalCheck();
    
    /* Printout a summary about the communication in the network */
    if(printer==0) {
      for(uint8_t i = 0; i< NSLAVES; i++) {
        printf("%8d,%8d,%8d,",(int)request_counter_test[i], (int)pkt_counter_test[i], (int)(request_counter_test[i]-pkt_counter_test[i]));
      }      
      printf("\r");
    }
  }   
}


void InitializationActionPackets(void)
{
  /* Build Action Packets */
  sendmeAction.StateMachineNo = STATE_MACHINE_0;
  sendmeAction.ActionTag = RELATIVE | TIMER_WAKEUP | TXRX | PLL_TRIG;
  sendmeAction.WakeupTime = 30+WKP_OFFSET;           /* Internal delay before start action */
  sendmeAction.ReceiveWindowLength = 0;              /* Not applied for TX */
  sendmeAction.data = sendmeBuffer;                  /* Data to send */
  sendmeAction.next_true = &waitDataAction;          /* Wait for data */
  sendmeAction.next_false = &waitDataAction;         /* Wait for data */
  sendmeAction.condRoutine = sendmeCB;               /* Condition callback */
  sendmeAction.dataRoutine = dataRoutine;            /* Data callback */

  waitDataAction.StateMachineNo = STATE_MACHINE_0;
  waitDataAction.ActionTag = RELATIVE | TIMER_WAKEUP;
  waitDataAction.WakeupTime = 30+WKP_OFFSET;               /* Internal delay before start action */
  waitDataAction.ReceiveWindowLength = RX_TIMEOUT_DATA;    /* RX timeout in us */
  waitDataAction.data = receiveBuffer;                     /* Buffer for ACK packet reception */
  waitDataAction.next_true = &sendAckAction;               /* If TRUE: send the ACK */   
  waitDataAction.next_false = &sendmeAction;               /* If FALSE: send a new request */
  waitDataAction.condRoutine = waitDataCB;                 /* Condition callback */
  waitDataAction.dataRoutine = dataRoutine;                /* Data callback */
  
  sendAckAction.StateMachineNo = STATE_MACHINE_0;
  sendAckAction.ActionTag = RELATIVE | TIMER_WAKEUP | TXRX;
  sendAckAction.WakeupTime = 40+WKP_OFFSET;           /* Internal delay before start action */
  sendAckAction.ReceiveWindowLength = 0;              /* RX timeout in us */
  sendAckAction.data = ackBuffer;                     /* Buffer for ACK packet reception */
  sendAckAction.next_true = &sendmeAction;            /* send a new request */   
  sendAckAction.next_false = &sendmeAction;           /* send a new request */   
  sendAckAction.condRoutine = sendAckCB;              /* Condition callback */
  sendAckAction.dataRoutine = dataRoutine;            /* Data callback */
  
  /* Channel map configuration */
  uint8_t map[5]= {0xFF,0xFF,0xFF,0xFF,0xFF};
  RADIO_SetChannelMap(STATE_MACHINE_0, &map[0]);
  
  /* Set the channel */
  RADIO_SetChannel(STATE_MACHINE_0, APP_CHANNEL, 0);

  /* Sets of the NetworkID and the CRC */
  RADIO_SetTxAttributes(STATE_MACHINE_0, networkID[0], 0x555555, 0);
    
  /* Call these functions before execute the action packets */
  RADIO_SetReservedArea(&sendmeAction);
  RADIO_SetReservedArea(&waitDataAction);
  RADIO_SetReservedArea(&sendAckAction);
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
