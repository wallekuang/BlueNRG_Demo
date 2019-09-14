/******************** (C) COPYRIGHT 2017 STMicroelectronics ********************
* File Name          : RADIO_AutomaticChMgm_main.c
* Author             : RF Application Team
* Version            : V1.1.0
* Date               : April-2018
* Description        : Transmission only example. The ActionTag INC_CHAN 
*                      is used to automatically change the channel.
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
#include <string.h>
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"

/** @addtogroup BlueNRG1_StdPeriph_Examples
* @{
*/

/** @addtogroup RADIO_Examples RADIO Examples
* @{
*/

/** @addtogroup RADIO_AutomaticChMgm RADIO Automatic Channel Management Example
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BLE_ADV_ACCESS_ADDRESS  (uint32_t)(0x8E89BED6)
#define STARTING_CHANNEL        (uint8_t)(24)    // RF channel 22
#define HS_STARTUP_TIME         (uint16_t)(1)  /* High Speed start up time min value */
#define WAKEUP_TIME             100000              /* Wake up time 100 ms */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t channel = STARTING_CHANNEL;   /* Start Channel */
uint8_t sendData[50];
ActionPacket actPacket[2]; 

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
  
  /* Radio configuration - HS_STARTUP_TIME, external LS clock, NULL, whitening enabled */
#if LS_SOURCE==LS_SOURCE_INTERNAL_RO
  RADIO_Init(HS_STARTUP_TIME, 1, NULL, ENABLE);
#else
  RADIO_Init(HS_STARTUP_TIME, 0, NULL, ENABLE);
#endif
  
  /* Build packet */
  sendData[0] = 0x02; 
  sendData[1] = 15;     /* Length position is fixed */
  
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
  
  /* Build Action Packet */
  actPacket[0].StateMachineNo = STATE_MACHINE_0;
  actPacket[0].ActionTag = INC_CHAN | RELATIVE | TIMER_WAKEUP | TXRX | PLL_TRIG;
  actPacket[0].WakeupTime = WAKEUP_TIME;    /* 100 ms before operation */
  actPacket[0].ReceiveWindowLength = 0;         /* not applied for TX */
  actPacket[0].data = (uint8_t *)&sendData[0]; 
  actPacket[0].next_true = &actPacket[0];       /* -> points to AP[0]  */
  actPacket[0].next_false = &actPacket[0];      /* -> points to AP[0]  */   
  actPacket[0].condRoutine = conditionRoutine;  /* Condition routine */
  actPacket[0].dataRoutine = dataRoutine;       /* Data routine */
  
  /* Channel map configuration */
  uint8_t map[5]= {0x00,0xFF,0xFF,0xFF,0xFF};
  RADIO_SetChannelMap(STATE_MACHINE_0, &map[0]);
  
  /* Setting of the channel and the channel increment */
  RADIO_SetChannel(STATE_MACHINE_0, STARTING_CHANNEL, 2);
  
  /* Sets of the NetworkID and the CRC.
   * The last parameter SCA is not used */
  RADIO_SetTxAttributes(STATE_MACHINE_0, BLE_ADV_ACCESS_ADDRESS, 0x555555, 0);
  
  /* Configures the transmit power level */
  RADIO_SetTxPower(MAX_OUTPUT_RF_POWER); 
    
  /* Call this function before execute the action packet */
  RADIO_SetReservedArea(&actPacket[0]);
  
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
