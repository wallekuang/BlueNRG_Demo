
/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : UART/Dma/main.c 
* Author             : RF Application Team
* Version            : V1.1.0
* Date               : September-2015
* Description        : Code demostrating the UART functionality with DMA
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
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"

/** @addtogroup BlueNRG1_StdPeriph_Examples
  * @{
  */

/** @addtogroup UART_Examples UART Examples
  * @{
  */

/** @addtogroup UART_DMA UART DMA Example
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USART_DR_ADDRESS        (UART_BASE)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/ 
uint8_t buffer_uart[UART_BUFFER_SIZE];
uint8_t buffer_uart_tail = 0;
volatile uint8_t buffer_uart_used = 0;
FlagStatus uart_tx_dma_eot = SET;
 
/* Private function prototypes -----------------------------------------------*/
void DMA_Configuration(void);
void DMAUart_Sending(uint8_t size);

/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
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
  
  /* DMA initialization */
  DMA_Configuration();
  
  /* Configure SysTick to generate interrupt with 25ms period */
  SysTick_Config(SYST_CLOCK/40 - 1);  

  while(1) {
    if(buffer_uart_used>0) {
      DMAUart_Sending(buffer_uart_used);      
      buffer_uart_used = 0;      
    }
    
  }
  
}


/**
* @brief  DMA_Configuration.
* @param  None
* @retval None
*/
void DMA_Configuration(void)
{ 
  DMA_InitType DMA_InitStructure;
  NVIC_InitType NVIC_InitStructure;
  
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_DMA, ENABLE);
  
  /* DMA_CH_UART_TX Initialization */
  DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_ADDRESS;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffer_uart;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = (uint32_t)UART_BUFFER_SIZE;  
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
  DMA_Init(DMA_CH_UART_TX, &DMA_InitStructure);
  
  /* Enable DMA_CH_UART_TX Transfer Complete interrupt */
  DMA_FlagConfig(DMA_CH_UART_TX, DMA_FLAG_TC, ENABLE);
  
  /* DMA_CH_UART_RX Initialization */
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffer_uart;
  DMA_InitStructure.DMA_BufferSize = (uint32_t)UART_BUFFER_SIZE;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_Init(DMA_CH_UART_RX, &DMA_InitStructure);
  DMA_Cmd(DMA_CH_UART_RX, ENABLE);
  
  /* Enable USART_DMAReq_Tx */
  UART_DMACmd(UART_DMAReq_Tx | UART_DMAReq_Rx, ENABLE);
  
  /* Enable the DMA Interrupt request */
  NVIC_InitStructure.NVIC_IRQChannel = DMA_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = HIGH_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
  
}

void DMAUart_Sending(uint8_t size) 
{
  while(uart_tx_dma_eot==RESET);
  
  uart_tx_dma_eot = RESET;
  
  DMA_CH_UART_TX->CMAR = (uint32_t)&buffer_uart[buffer_uart_tail];
    
  DMA_CH_UART_TX->CNDTR = size;
  buffer_uart_tail = (buffer_uart_tail+buffer_uart_used)%UART_BUFFER_SIZE;
  
  /* DMA_CH_UART_TX enable */
  DMA_CH_UART_TX->CCR_b.EN = SET;
  
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
