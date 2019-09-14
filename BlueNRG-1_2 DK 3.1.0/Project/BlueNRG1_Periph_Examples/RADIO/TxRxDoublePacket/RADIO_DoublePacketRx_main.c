/******************** (C) COPYRIGHT 2017 STMicroelectronics ********************
* File Name          : DoublePacketRx_main.c 
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
#include <stdio.h>
#include <string.h>
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

/* enable/disbale received data print */
#define DISPLAY_RECEIVED_DATA 0
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t sendData1[MAX_PACKET_LENGTH];
uint8_t sendData2[MAX_PACKET_LENGTH];
uint8_t sendData3[MAX_PACKET_LENGTH];

uint16_t sendDataCnt = 0;
ActionPacket actPacket[3]; 
uint8_t pckt_counter_test = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Condition routine.
  * @param  ActionPacket
  * @retval TRUE
  */
uint8_t conditionRoutine(ActionPacket* p)
{
  /* received a packet */
  if( (p->status & BIT_TX_MODE) == 0) {
    
    if((p->status & IRQ_RCV_OK) != 0) {
      pckt_counter_test++;
      return TRUE;
    }
    else if((p->status & IRQ_TIMEOUT) != 0) {
      pckt_counter_test = 0;
      return FALSE;
    }
    else if((p->status & IRQ_CRC_ERR) != 0) {
      pckt_counter_test = 0;
      return FALSE;
    }
  }
  /* Transmit complete */
  else {
    return TRUE;
  }
  return FALSE;
} 

/**
  * @brief  Data routine.
  * @param  ActionPacket: current
  * @param  ActionPacket: next
  * @retval TRUE
  */
uint8_t dataRoutine(ActionPacket* p,  ActionPacket* next)
{
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
  
  sendData3[0] = 0xAE;
  sendData3[1] = 0;
  
  /* Radio configuration - HS_STARTUP_TIME, external LS clock, NULL, whitening enabled */
#if LS_SOURCE==LS_SOURCE_INTERNAL_RO
  RADIO_Init(HS_STARTUP_TIME, 1, NULL, ENABLE);
#else
  RADIO_Init(HS_STARTUP_TIME, 0, NULL, ENABLE);
#endif
  
  /* Build Action Packets */
  actPacket[0].StateMachineNo = STATE_MACHINE_0;
  actPacket[0].ActionTag = RELATIVE | TIMER_WAKEUP | PLL_TRIG;
  actPacket[0].WakeupTime = RX_WAKEUP_TIME;                         /* 30 us minimum of internal delay before start action */
  actPacket[0].ReceiveWindowLength = RX_TIMEOUT_1ST_PART;            /* RX timeout in us */
  actPacket[0].data = (uint8_t *)&sendData1[0];         /* Buffer for reception */
  actPacket[0].next_true = &actPacket[1];               /* If condRoutine returns TRUE => Go to RX for the 2nd part of the payload */
  actPacket[0].next_false = &actPacket[0];              /* If condRoutine returns FALSE => Go to RX for the 1st part of the payload */   
  actPacket[0].condRoutine = conditionRoutine;          /* Condition routine */
  actPacket[0].dataRoutine = dataRoutine;               /* Data routine */
  
  actPacket[1].StateMachineNo = STATE_MACHINE_0;
  actPacket[1].ActionTag = RELATIVE | TIMER_WAKEUP ;
  actPacket[1].WakeupTime = RX_WAKEUP_TIME;                         /* 30 us minimum of internal delay before start action */
  actPacket[1].ReceiveWindowLength = RX_TIMEOUT_2ND_PART;              /* RX timeout in us */
  actPacket[1].data = (uint8_t *)&sendData2[0];         /* Buffer for reception */
  actPacket[1].next_true = &actPacket[2];               /* If condRoutine returns TRUE => Go to TX for sending ACK */
  actPacket[1].next_false = &actPacket[0];              /* If condRoutine returns FALSE => Go to RX for the 1st part of the payload */
  actPacket[1].condRoutine = conditionRoutine;          /* Condition routine */
  actPacket[1].dataRoutine = dataRoutine;               /* Data routine */
  
  actPacket[2].StateMachineNo = STATE_MACHINE_0;
  actPacket[2].ActionTag = RELATIVE | TIMER_WAKEUP | TXRX ;
  actPacket[2].WakeupTime = TX_WAKEUP_TIME;                         /* 40 us of internal delay before start action */
  actPacket[2].ReceiveWindowLength = 0;                 /* Not applied for TX */
  actPacket[2].data = (uint8_t *)&sendData3[0];         /* ACK buffer */
  actPacket[2].next_true = &actPacket[0];               /* If condRoutine returns TRUE  => Go to RX for the 1st part of the payload */   
  actPacket[2].next_false = &actPacket[0];              /* If condRoutine returns FALSE => Go to RX for the 1st part of the payload */   
  actPacket[2].condRoutine = conditionRoutine;          /* Condition routine */
  actPacket[2].dataRoutine = dataRoutine;               /* Data routine */
  
  /* Channel map configuration */
  uint8_t map[5]= {0xFF,0xFF,0xFF,0xFF,0xFF};
  RADIO_SetChannelMap(STATE_MACHINE_0, &map[0]);
  
  /* Setting of channel and the channel increment */
  RADIO_SetChannel(STATE_MACHINE_0, FREQUENCY_CHANNEL, 0);

  /* Sets of the NetworkID and the CRC. The last parameter SCA is not used */
  RADIO_SetTxAttributes(STATE_MACHINE_0, ACCESS_ADDRESS, 0x555555, 0);
  
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

    if(pckt_counter_test == 2) {
      pckt_counter_test = 0;
     
#if DISPLAY_RECEIVED_DATA ==1
      printf("%02X ", sendData1[0]);
      for(uint16_t i=0;i<sendData1[1];i++) {
        printf("%02X ", sendData1[2+i]);
      }
      printf("%02X ", sendData2[0]);
      for(uint16_t i=0;i<sendData2[1];i++) {
        printf("%02X ", sendData2[2+i]);
      }
      printf("\r\n");
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
