/******************** (C) COPYRIGHT 2017 STMicroelectronics ********************
* File Name          : beep.c 
* Author             : RF Application Team
* Version            : V1.0.0
* Date               : March-2018
* Description        : Transmission only example with multi state functionality. A certain 
*                               number of state machine are configured to transmit in different 
*                               configurations and are executed consecutively
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

/** @addtogroup RADIO_Beep_MultiState RADIO Beep MultiState Example
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
typedef struct{
  uint8_t activity_flag;
  uint32_t wakeup_time[STATEMACHINE_COUNT];
}multi_state_t;

/* Private define ------------------------------------------------------------*/
#define HS_STARTUP_TIME         (uint16_t)(1)  /* High Speed start up time min value */
#define DATA_LEN                (uint8_t)(26)
#define N_STATE_MACHINES        STATEMACHINE_COUNT /* The number of state machines */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

multi_state_t multi_state = {{0,}, {0,}};

uint8_t channel_map[5] = {0xFF,0xFF,0xFF,0xFF,0xFF};
uint8_t channel_values[STATEMACHINE_COUNT] = {21,22,23,24,25,26,27,28};
uint32_t period_usec_values[STATEMACHINE_COUNT] = {100000,100000,100000,100000,100000,100000,100000,100000};
uint32_t network_id_values[STATEMACHINE_COUNT] = {0x8E89BED6,0x8E89BED6,0x8E89BED6,0x8E89BED6,0x8E89BED6,0x8E89BED6,0x8E89BED6,0x8E89BED6};
uint8_t encryption_values[STATEMACHINE_COUNT] = {0,0,0,1,1,1,1,0};

uint8_t channel[N_STATE_MACHINES];
uint32_t period_usec[N_STATE_MACHINES];
uint32_t network_id[N_STATE_MACHINES];
uint8_t encryption[N_STATE_MACHINES];

uint8_t sendData[N_STATE_MACHINES][DATA_LEN+3];
ActionPacket actPacket[N_STATE_MACHINES];

uint8_t count_tx[5] = {0x00,0x00,0x00,0x00,0x00};
uint8_t count_rcv[5] = {0x00,0x00,0x00,0x00,0x00};
uint8_t enc_key[16] = {0xBF,0x01,0xFB,0x9D,0x4E,0xF3,0xBC,0x36,0xD8,0x74,0xF5,0x39,0x41,0x38,0x68,0x4C};
uint8_t enc_iv[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  multi_state_schedular routine.
  * @param  multi_state
  * @retval void
  */
void multi_state_schedular(multi_state_t * multi_state)
{
  uint32_t next_wakeup_time_min = multi_state->wakeup_time[0];
  uint8_t i = 0;
  uint8_t index = 0;
  
  /* Find the minimum wakeup time among the active state machine */
  for(i = 1;i<STATEMACHINE_COUNT;i++) {
    if((next_wakeup_time_min > multi_state->wakeup_time[i]) && ((multi_state->activity_flag >> i)&0x01)) {
      next_wakeup_time_min = multi_state->wakeup_time[i];
      index  = i;
    }
  }
  RADIO_SetChannel(actPacket[index].StateMachineNo, channel[index], 0);
  RADIO_MakeActionPacketPending(&actPacket[index]);
  multi_state->wakeup_time[index] +=(period_usec[index] >> 10);
}

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
  if(next == NULL) {
    SdkEvalLedToggle(LED1);
    multi_state_schedular(&multi_state);
  }
  return TRUE;
}


/**
  * @brief  Beep Initilize.
  * @param  State Machine No
  * @retval void
  */
void beep_init(uint8_t StateMachineNo)
{
  sendData[StateMachineNo][0] = 0x02; 
  sendData[StateMachineNo][1] = DATA_LEN;       /* Length position is fixed */
  sendData[StateMachineNo][2] = StateMachineNo;
  
  for(uint8_t i = 1; i < DATA_LEN; i++) {
    sendData[StateMachineNo][2+i] = i;
  }
  
  if(encryption[StateMachineNo]) {
    sendData[StateMachineNo][1] += MIC_FIELD_LENGTH;
  }
  
  actPacket[StateMachineNo].StateMachineNo = StateMachineNo;
  actPacket[StateMachineNo].ActionTag = RELATIVE | TIMER_WAKEUP | TXRX | PLL_TRIG ;
  actPacket[StateMachineNo].WakeupTime = period_usec[StateMachineNo];
  actPacket[StateMachineNo].ReceiveWindowLength = 0;/* not apllied for Tx*/
  actPacket[StateMachineNo].data = (uint8_t *)&sendData[StateMachineNo][0]; 
  actPacket[StateMachineNo].next_true = NULL;   /* -> points to AP[0]  */
  actPacket[StateMachineNo].next_false = NULL;  /* -> points to AP[0]  */   
  actPacket[StateMachineNo].condRoutine = conditionRoutine;
  actPacket[StateMachineNo].dataRoutine = dataRoutine;  
  multi_state.activity_flag = multi_state.activity_flag | 1<<StateMachineNo;
  multi_state.wakeup_time[StateMachineNo] = actPacket[StateMachineNo].WakeupTime;
  RADIO_SetChannelMap(StateMachineNo, &channel_map[0]);
  RADIO_SetChannel(StateMachineNo, channel[StateMachineNo],0);
  RADIO_SetTxAttributes(StateMachineNo, network_id[StateMachineNo] , 0x555555, 0);
  RADIO_SetTxPower(MAX_OUTPUT_RF_POWER);
  
  if(encryption[StateMachineNo]) {
    RADIO_SetEncryptionCount(StateMachineNo, &count_tx[0], &count_rcv[0]); 
    RADIO_SetEncryptionAttributes(StateMachineNo, enc_iv, enc_key);
    RADIO_SetEncryptFlags(StateMachineNo,(FunctionalState)encryption[StateMachineNo],(FunctionalState)encryption[StateMachineNo]);    
  }
  
  RADIO_SetReservedArea(&actPacket[StateMachineNo]); 
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

  /* Configure the LEDs */
  SdkEvalLedInit(LED1);
  SdkEvalLedInit(LED2);
  SdkEvalLedInit(LED3);
  
  /* Radio configuration - HS_STARTUP_TIME, external LS clock, NULL, whitening enabled */
#if LS_SOURCE==LS_SOURCE_INTERNAL_RO
  RADIO_Init(HS_STARTUP_TIME, 1, NULL, ENABLE);
#else
  RADIO_Init(HS_STARTUP_TIME, 0, NULL, ENABLE);
#endif
  
  /* Set the parameters and configure the state machiness */
  for(uint8_t i = 0; i < N_STATE_MACHINES; i++) {
    network_id[i] = network_id_values[i];
    channel[i] = channel_values[i];
    encryption[i] = encryption_values[i];
    period_usec[i] = period_usec_values[i];
    
    beep_init(i);
  }
  
  /* Call this function for the first action packet to be executed */
  RADIO_MakeActionPacketPending(&actPacket[0]);
 
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
