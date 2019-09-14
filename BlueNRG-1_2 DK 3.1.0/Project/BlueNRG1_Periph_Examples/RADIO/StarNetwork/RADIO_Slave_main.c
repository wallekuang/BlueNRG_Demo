/******************** (C) COPYRIGHT 2018 STMicroelectronics ********************
* File Name          : RADIO_Slave_main.c 
* Author             : RF Application Team
* Version            : V1.0.0
* Date               : March-2018
* Description        : The code defines a Slave device in a start network. 
*                               The Slave waits for a request of data from the Master. Once the request is received, 
*                               then the Slave sends the data packet and waits for an ACK packet from the Master. 
*                               The confirm the data reception of the Master. If the ACK packet is not received, 
*                               then the Slave sends again the data packet to the Master. The Slave can send up to 
*                               three packets of data to the Master if the ACK packet is not received.
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
#define BLE_NETWORK_ID          SLAVE_DEV1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t txDataBuffer[62];
uint8_t ackBuffer[2];
uint8_t receiveBuffer[MAX_PACKET_LENGTH];

ActionPacket waitRequestAction;
ActionPacket sendDataAction;
ActionPacket waitAckAction;

uint16_t pkt_counter_test = 0;
uint16_t retry_counter_test = 0;
uint8_t retry = 0;
uint16_t ack_counter_test = 0;


/* Private function prototypes -----------------------------------------------*/
void InitializationActionPackets(void);
uint8_t waitRequestCB(ActionPacket* p);
uint8_t sendDataCB(ActionPacket* p);
uint8_t waitAckCB(ActionPacket* p);
uint8_t dataRoutine(ActionPacket* p,  ActionPacket* next);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Condition routine.
  * @param  ActionPacket
  * @retval TRUE
  */
uint8_t waitRequestCB(ActionPacket* p)
{
  /* received a packet */
  if( (p->status & BIT_TX_MODE) == 0) {
    
    if((p->status & IRQ_RCV_OK) != 0) {
      return TRUE;
    }
    else if(((p->status & IRQ_TIMEOUT) != 0) || ((p->status & IRQ_CRC_ERR) != 0)) {
      return FALSE;
    }
  }
  return FALSE;
} 

uint8_t sendDataCB(ActionPacket* p)
{
  return TRUE;
} 

uint8_t waitAckCB(ActionPacket* p)
{
  /* received a packet */
  if( (p->status & BIT_TX_MODE) == 0) {
    
    if((p->status & IRQ_RCV_OK) != 0) {
      return TRUE;
    }
    else if(((p->status & IRQ_TIMEOUT) != 0) || ((p->status & IRQ_CRC_ERR) != 0)) {
      if(retry < MAX_RETRY_TEST) {
        retry++;
        return FALSE;
      }
      else {
        retry = 0;
        return TRUE;
      }
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
  
  /* Build data packet */
  txDataBuffer[0] = 0x0;  /* First byte */
  txDataBuffer[1] = 60;   /* Length position is fixed */
  for(uint8_t i=0; i != 60; i++) {
    txDataBuffer[i+2] = i;
  }
  
  /* Build ACK packet */
  ackBuffer[0] = 0xAE;
  ackBuffer[1] = 0;

  /* Initialize the routine for sending data and wait for the ACK */
  InitializationActionPackets();
  
  
  /* Call this function for the first action packet to be executed */
  RADIO_MakeActionPacketPending(&waitRequestAction);
  
  /* Infinite loop */
  while(1) {
    /* Perform calibration procedure */
    RADIO_CrystalCheck();
  }   
}



void InitializationActionPackets(void)
{
  /* Build Action Packets */
  waitRequestAction.StateMachineNo = STATE_MACHINE_0;
  waitRequestAction.ActionTag = RELATIVE | TIMER_WAKEUP | PLL_TRIG;
  waitRequestAction.WakeupTime = 30+WKP_OFFSET;           /* 30 us minimum of internal delay before start action */
  waitRequestAction.ReceiveWindowLength = RX_TIMEOUT_DATA;         /* Not applied for TX */
  waitRequestAction.data = receiveBuffer;                 /* Data to send */
  waitRequestAction.next_true = &sendDataAction;           /* If condRoutine returns TRUE => Go to next TX the 2nd part of the payload */
  waitRequestAction.next_false = &waitRequestAction;       /* Not Used */   
  waitRequestAction.condRoutine = waitRequestCB;          /* Condition routine */
  waitRequestAction.dataRoutine = dataRoutine;            /* Data routine */

  sendDataAction.StateMachineNo = STATE_MACHINE_0;
  sendDataAction.ActionTag = RELATIVE | TIMER_WAKEUP | TXRX;
  sendDataAction.WakeupTime = 40+WKP_OFFSET;              /* 40 us minimum of internal delay before start action */
  sendDataAction.ReceiveWindowLength = 0;                 /* RX timeout in us */
  sendDataAction.data = txDataBuffer;                     /* Buffer for ACK packet reception */
  sendDataAction.next_true = &waitAckAction;               /* Reschedule the RX */   
  sendDataAction.next_false = &waitAckAction;              /* Reschedule the RX */   
  sendDataAction.condRoutine = sendDataCB;                /* Condition routine */
  sendDataAction.dataRoutine = dataRoutine;               /* Data routine */
  
  waitAckAction.StateMachineNo = STATE_MACHINE_0;
  waitAckAction.ActionTag = RELATIVE | TIMER_WAKEUP;
  waitAckAction.WakeupTime = 30+WKP_OFFSET;              /* 30 us minimum of internal delay before start action */
  waitAckAction.ReceiveWindowLength = RX_TIMEOUT_ACK;            /* RX timeout in us */
  waitAckAction.data = receiveBuffer;                    /* Buffer for ACK packet reception */
  waitAckAction.next_true = &waitRequestAction;           /* Reschedule the RX */   
  waitAckAction.next_false = &sendDataAction;             /* Reschedule the RX */   
  waitAckAction.condRoutine = waitAckCB;                 /* Condition routine */
  waitAckAction.dataRoutine = dataRoutine;               /* Data routine */
  
  /* Channel map configuration */
  uint8_t map[5]= {0xFF,0xFF,0xFF,0xFF,0xFF};
  RADIO_SetChannelMap(STATE_MACHINE_0, &map[0]);
  
  /* Set the channel */
  RADIO_SetChannel(STATE_MACHINE_0, APP_CHANNEL, 0);

  /* Sets of the NetworkID and the CRC */
  RADIO_SetTxAttributes(STATE_MACHINE_0, BLE_NETWORK_ID, 0x555555, 0);
    
  /* Call these functions before execute the action packets */
  RADIO_SetReservedArea(&waitRequestAction);
  RADIO_SetReservedArea(&sendDataAction);
  RADIO_SetReservedArea(&waitAckAction);
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
