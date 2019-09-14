/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : UART/Polling/main.c 
* Author             : RF Application Team
* Version            : V1.1.0
* Date               : September-2015
* Description        : Code demostrating the UART functionality
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


/** @addtogroup BlueNRG1_StdPeriph_Examples BlueNRG1 Standard Peripheral Examples
  * @{
  */


/** @addtogroup UART_Examples UART Examples
  * @{
  */

/** @addtogroup UART_Polling UART Polling Example
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
  uint8_t tmp;

  /* System initialization function */
  SystemInit();
  
  /* Identify BlueNRG1 platform */
  SdkEvalIdentification();
 
  /* Clock enable */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_UART | CLOCK_PERIPH_GPIO, ENABLE);
  
  /* Configure GPIO_Pin_8 and GPIO_Pin_11 as UART_TXD and UART_RXD*/
  GPIO_InitType GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = SDK_EVAL_UART_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = SDK_EVAL_UART_TX_MODE;
  GPIO_InitStructure.GPIO_Pull = DISABLE;
  GPIO_InitStructure.GPIO_HighPwr = DISABLE;
  GPIO_Init(&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = SDK_EVAL_UART_RX_PIN;
  GPIO_InitStructure.GPIO_Mode = SDK_EVAL_UART_RX_MODE;
  GPIO_Init(&GPIO_InitStructure);

  /* ------------ USART configuration -------------------
  - BaudRate = 115200 baud  
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */
  UART_InitType UART_InitStructure;
  UART_InitStructure.UART_BaudRate = (uint32_t)UART_BAUDRATE;
  UART_InitStructure.UART_WordLengthTransmit = UART_WordLength_8b;
  UART_InitStructure.UART_WordLengthReceive = UART_WordLength_8b;
  UART_InitStructure.UART_StopBits = UART_StopBits_1;
  UART_InitStructure.UART_Parity = UART_Parity_No;
  UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
  UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;
  UART_InitStructure.UART_FifoEnable = ENABLE;
  UART_Init(&UART_InitStructure);
  
  /* Interrupt as soon as data is received. */
  UART_RxFifoIrqLevelConfig(FIFO_LEV_1_64);

  /* Enable UART */
  UART_Cmd(ENABLE);
    
  /* Infinite loop */
  while(1) {
    /* Loop until the UART Receive Data Register is not empty */
    while (UART_GetFlagStatus(UART_FLAG_RXFE) == SET) {
      ;
    }
    
    /* Store the received byte in RxBuffer */
    tmp = (uint8_t)UART_ReceiveData();

    /* Loop until UART DR register is empty */
    while (UART_GetFlagStatus(UART_FLAG_TXFE) == RESET) {
      ;
    }
    
    /* Send byte through UART */
    UART_SendData((uint16_t)tmp);    
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

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
