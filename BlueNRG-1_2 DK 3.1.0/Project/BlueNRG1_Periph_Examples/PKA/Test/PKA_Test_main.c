/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : PKA/PKA/main.c 
* Author             : RF Application Team
* Version            : V1.0.0
* Date               : September-2015
* Description        : Code demostrating the PKA functionality
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "BlueNRG_x_device.h"
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"
#include <stdio.h>

/** @addtogroup BlueNRG1_StdPeriph_Examples BlueNRG1 Standard Peripheral Examples
  * @{
  */


/** @addtogroup PKA_Examples PKA Examples
  * @{
  */

/** @addtogroup PKA_DataEncrypt PKA DataEncrypt
  * @{
  */
    
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void GenerateRandomK16Word(uint32_t* buffer)
{
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_RNG, ENABLE);
  
  /* Create a random key */
  for(uint8_t i=0; i<16; i++) {
    
    /* Loop until the RNG Data Ready is SET */
    while (RNG_GetFlagStatus() != SET);
    
    /* Generate a Random Number and load it into ecc_addr_k register */
    buffer[i] = RNG_GetValue();
    
    /* Loop until the RNG Data Ready is SET */
    while (RNG_GetFlagStatus() != SET);
    
    /* Generate a Random Number and load it into ecc_addr_k register */
    buffer[i] |= (((uint32_t)RNG_GetValue())<<16);
  }
  
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_RNG, DISABLE);
}

void PkaFailure(void)
{
  printf("PKA failed.\r\n");
  while(1);
}

/**
* @brief  Main program code
* @param  None
* @retval None
*/
int main(void)
{
  static uint32_t Public_Key_Out_A[16], Public_Key_Out_B[16];
    
  /* PKA requires a starting point necessary to start the processing */
  const uint32_t PKAStartPoint[16] = {
  INITIAL_START_POINT_X_W1, INITIAL_START_POINT_X_W2, INITIAL_START_POINT_X_W3, INITIAL_START_POINT_X_W4,
  INITIAL_START_POINT_X_W5, INITIAL_START_POINT_X_W6, INITIAL_START_POINT_X_W7, INITIAL_START_POINT_X_W8,
  INITIAL_START_POINT_Y_W1, INITIAL_START_POINT_Y_W2, INITIAL_START_POINT_Y_W3, INITIAL_START_POINT_Y_W4,
  INITIAL_START_POINT_Y_W5, INITIAL_START_POINT_Y_W6, INITIAL_START_POINT_Y_W7, INITIAL_START_POINT_Y_W8};
  ErrorStatus error;
  static uint32_t RandomKA[16], RandomKB[16];
    
  /* System initialization function */
  SystemInit();
  
  /* Identify BlueNRG1 platform */
  SdkEvalIdentification();
  
  /* UART initialization */
  SdkEvalComUartInit(UART_BAUDRATE);
  
  /* PKA enable clock */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_PKA, ENABLE);
  
  
  /* ***** PROCEDURE FOR A ***** */
  /* Generate the random K for point A */
  GenerateRandomK16Word(RandomKA);
  
  /* Insert the random K for point A */
  PKA_SetData(PKA_DATA_SK, RandomKA);
  
  /* Insert the initial starting point coordinates */
  PKA_SetData(PKA_DATA_PCX, (uint32_t *)&PKAStartPoint[0]);
  PKA_SetData(PKA_DATA_PCY, (uint32_t *)&PKAStartPoint[8]);
  
  /* Start PKA processing */
  PKA_StartProcessing();
  
  /* Wait for the end of PKA processing */
  PKA_WaitProcess();
  
  /* Verify if the PKA process is ended with success (valid point) */
  error = PKA_VerifyProcess();
  
  if(error != 0) {
    /* PKA error */
    PkaFailure();
  }
  
  /* Get the new calculated point A of the ellipse */
  PKA_GetData(PKA_DATA_PCX, (uint8_t *)&Public_Key_Out_A[0]);
  PKA_GetData(PKA_DATA_PCY, (uint8_t *)&Public_Key_Out_A[8]);
   
  
  /* ***** PROCEDURE FOR B ***** */
  /* Generate the random K for point B */
  GenerateRandomK16Word(RandomKB);
  
  /* Generate the random K for point B */
  PKA_SetData(PKA_DATA_SK, RandomKB);
  
  /* Insert the initial starting point coordinates */
  PKA_SetData(PKA_DATA_PCX, (uint32_t *)&PKAStartPoint[0]);
  PKA_SetData(PKA_DATA_PCY, (uint32_t *)&PKAStartPoint[8]);
  
  /* Start PKA processing */
  PKA_StartProcessing();
  
  /* Wait for the end of PKA processing */
  PKA_WaitProcess();
  
  /* Verify if the PKA process is ended with success (valid point) */
  error = PKA_VerifyProcess();
  
  if(error != 0) {
    /* PKA error */
    PkaFailure();
  }
  
  /* Get the new calculated point B of the ellipse */
  PKA_GetData(PKA_DATA_PCX, (uint8_t *)&Public_Key_Out_B[0]);
  PKA_GetData(PKA_DATA_PCY, (uint8_t *)&Public_Key_Out_B[8]);
  
  
  /* ***** CHECK ELLIPTIC ***** */
  /* Insert the random k used to calculate the point A */
  PKA_SetData(PKA_DATA_SK, RandomKA);
  
  /* Insert the secrete coordinate of the point B */
  PKA_SetData(PKA_DATA_PCX, (uint32_t *)&Public_Key_Out_B[0]);
  PKA_SetData(PKA_DATA_PCY, (uint32_t *)&Public_Key_Out_B[8]);
  
  /* Start PKA processing */
  PKA_StartProcessing();
  
  /* Wait for the end of PKA processing */
  PKA_WaitProcess();
  
  /* Verify if the PKA process is ended with success (valid point) */
  error = PKA_VerifyProcess();
  
  if(error != 0) {
    /* PKA error */
    PkaFailure();
  }
  else {
    printf("PKA verified.\r\n");
  }

  while(1);  
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

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
