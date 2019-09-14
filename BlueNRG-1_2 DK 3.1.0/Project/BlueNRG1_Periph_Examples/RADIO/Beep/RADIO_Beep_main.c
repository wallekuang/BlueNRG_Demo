/******************** (C) COPYRIGHT 2017 STMicroelectronics ********************
* File Name          : beep.c 
* Author             : RF Application Team
* Version            : V1.1.0
* Date               : April-2018
* Description        : Transmission only example. The device sends a packet 
*                      continuously in three different channels: 37, 38, 39
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


/** @addtogroup BlueNRG1_StdPeriph_Examples
  * @{
  */

/** @addtogroup RADIO_Examples RADIO Examples
  * @{
  */

/** @addtogroup RADIO_Beep RADIO Beep Example
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BLE_ADV_ACCESS_ADDRESS  (uint32_t)(0x8E89BED6)
#define STARTING_CHANNEL        (uint8_t)(24)    // RF channel 22
#define END_CHANNEL             (uint8_t)(26)
#define HS_STARTUP_TIME         (uint16_t)(1)  /* High Speed start up time min value */
#define WAKEUP_TIME             100000              /* Wake up time 100 ms */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t channel = STARTING_CHANNEL;   /* Start Channel */
uint8_t sendData[MAX_PACKET_LENGTH];
uint8_t data_val = 0;
uint8_t DataLen;
ActionPacket actPacket; 

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

/**
  * @brief  Condition routine.
  * @param  ActionPacket
  * @retval TRUE
  */
uint8_t conditionRoutine(ActionPacket* p)
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
      SdkEvalLedToggle(LED1);
      channel ++;
      if(channel == (END_CHANNEL+1)) {
        channel = STARTING_CHANNEL;
      }
      RADIO_SetChannel(STATE_MACHINE_0, channel, 0);
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
  for(uint8_t i = 0; i < (DataLen-15); i++) {
    sendData[i+17] = i + data_val;
  }
  data_val++;
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

  /* Radio configuration - HS_STARTUP_TIME, external LS clock, NULL, whitening enabled */
#if LS_SOURCE==LS_SOURCE_INTERNAL_RO
  RADIO_Init(HS_STARTUP_TIME, 1, NULL, ENABLE);
#else
  RADIO_Init(HS_STARTUP_TIME, 0, NULL, ENABLE);
#endif

  DataLen = 20;
  
  /* Build packet */
  sendData[0] = 0x02; 
  sendData[1] = DataLen + MIC_FIELD_LEN;   /* Length position is fixed */
  
  sendData[2] = 0x01; /* Advertising address */
  sendData[3] = 0x02;
  sendData[4] = 0x03;
  sendData[5] = 0x04;
  sendData[6] = 0x05;
  sendData[7] = 0x06;
  
  sendData[8] = 0x08; /* Length name */
  sendData[9] = 0x08; /* Shortened local name */
  
  sendData[10] = 'B'; /* Local name */
  sendData[11] = 'l';
  sendData[12] = 'u';
  sendData[13] = 'e';
  sendData[14] = 'N';
  sendData[15] = 'R';
  sendData[16] = 'G';
  
  for(uint8_t i = 0; i < (DataLen-15); i++) {
    sendData[i+17] = i + data_val;
  }
  data_val++;
  
  /* Build Action Packet */
  actPacket.StateMachineNo = STATE_MACHINE_0;
  actPacket.ActionTag = RELATIVE | TIMER_WAKEUP | TXRX | PLL_TRIG;
  actPacket.WakeupTime = WAKEUP_TIME;                /* 100 ms before operation */
  actPacket.ReceiveWindowLength = 0;                 /* Not applied for TX */
  actPacket.data = (uint8_t *)&sendData[0];          /* Data to send */
  actPacket.next_true = &actPacket;                  /* Pointer to the next Action Packet  */
  actPacket.next_false = NULL_0;                     /* Null */   
  actPacket.condRoutine = conditionRoutine;          /* Condition routine */
  actPacket.dataRoutine = dataRoutine;               /* Data routine */
  
  /* Channel map configuration */
  uint8_t map[5]= {0xFF,0xFF,0xFF,0xFF,0xFF};
  RADIO_SetChannelMap(STATE_MACHINE_0, &map[0]);
  
  /* Setting of channel (37) and the channel increment (0) */
  RADIO_SetChannel(STATE_MACHINE_0, STARTING_CHANNEL, 0);

  /* Sets of the NetworkID and the CRC.
   * The last parameter SCA is not used */
  RADIO_SetTxAttributes(STATE_MACHINE_0, BLE_ADV_ACCESS_ADDRESS, 0x555555, 0);
  
  /* Configures the transmit power level */
  RADIO_SetTxPower(MAX_OUTPUT_RF_POWER);
  
#ifdef PACKET_ENCRYPTION  
  /* Configures the encryption count */
  RADIO_SetEncryptionCount(STATE_MACHINE_0, count_tx, count_rcv);
  
  /* Encryption Key and Initial Vector */
  RADIO_SetEncryptionAttributes( STATE_MACHINE_0, enc_iv, enc_key);
  
  /* Configures the packets encryption. Not used in this application */
  RADIO_SetEncryptFlags(STATE_MACHINE_0, ENABLE, ENABLE);
#endif
  
  /* Call this function before execute the action packet */
  RADIO_SetReservedArea(&actPacket);
  
  /* Call this function for the first action packet to be executed */
  RADIO_MakeActionPacketPending(&actPacket);
 
  /* Infinite loop */
  while(1) {
    /* Perform calibration procedure */
    RADIO_CrystalCheck();
    
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
