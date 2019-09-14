/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : UART/RxTimeout/main.c 
* Author             : RF Application Team
* Version            : V1.0.0
* Date               : September-2015
* Description        : Code demostrating the UART RX FIFO level and RX timeout 
*                      functionality. The demo will printout the data received
*                      if the RX timeout expires OR
*                      if the data received are >= of the RX FIFO threshold.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "bluenrg_x_device.h"
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"


/** @addtogroup BlueNRG1_StdPeriph_Examples BlueNRG1 Standard Peripheral Examples
  * @{
  */


/** @addtogroup UART_Examples UART Examples
  * @{
  */

/** @addtogroup UART_RxTimeout UART RxTimeout Example
  * @{
  */
    
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
* @brief  Main program code
* @param  None
* @retval None
*/
int main(void)
{
  /* System initialization function */
  SystemInit();
  
  /* Identify BlueNRG1 platform */
  SdkEvalIdentification();
  
  /* LED initialization */
  SdkEvalLedInit(LED1);
  SdkEvalLedInit(LED2);
  
  /* UART initialization */
  SdkEvalComUartInit(UART_BAUDRATE);
  
  /* NVIC configuration */
  NVIC_InitType NVIC_InitStructure;
  
  /* Enable the UART Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = UART_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LOW_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Set the reception at RX FIFO level >= 32 bytes */
  UART_RxFifoIrqLevelConfig(FIFO_LEV_1_2);
  
  /* Set receive timeout at 500 ms */
  UART_RXTimeoutConfig(500);
  
  /* Enable the interrupts */
  UART_ITConfig(UART_IT_RT | UART_IT_RX, ENABLE);
  
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
