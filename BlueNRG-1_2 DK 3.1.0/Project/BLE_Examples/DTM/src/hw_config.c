/**
******************************************************************************
* @file    UART/Interrupt/main.c
* @author  VMA RF Application Team
* @version V1.1.0
* @date    27-March-2018
* @brief   HW configuration for the GUI application
******************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
******************************************************************************
*/ 

/* Includes ------------------------------------------------------------------*/
#include "BlueNRG_x_device.h"
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "hw_config.h"

/** @addtogroup BlueNRG1_StdPeriph_Examples
* @{
*/

/** @addtogroup UART Interrupt Example
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
* @brief  WDG configuration routine
* @param  None
* @retval None
*/
void WDG_Configuration(void)
{
#ifdef WATCHDOG  
  /* Enable watchdog clocks  */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_WDG, ENABLE);  
  
  /* Set watchdog reload period at 1 second */
  WDG_SetReload(RELOAD_TIME(WATCHDOG_TIME));
  
  /* Watchdog enable */
  WDG_Enable();
#endif
}

/**
* @brief  Configures the nested vectored interrupt controller.
* @param  None
* @retval None
*/
void NVIC_Configuration(void)
{
  NVIC_InitType NVIC_InitStructure;

#ifdef UART_INTERFACE 
  /* Enable the UART Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = UART_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = MED_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#ifdef UART_SLEEP
  /* Configures EXTI line */
  GPIO_EXTIConfigType GPIO_EXTIStructure;
  GPIO_EXTIStructure.GPIO_Pin = UART_CTS_PIN;
  GPIO_EXTIStructure.GPIO_IrqSense = GPIO_IrqSense_Edge;
  GPIO_EXTIStructure.GPIO_Event = GPIO_Event_Low;
  GPIO_EXTIConfig(&GPIO_EXTIStructure);
  
  
  /* Enable the SPI Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = GPIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LOW_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
#endif
#ifdef SPI_INTERFACE

  /* Configures EXTI line */
  GPIO_EXTIConfigType GPIO_EXTIStructure;
  GPIO_EXTIStructure.GPIO_Pin = SPI_CS_PIN;
  GPIO_EXTIStructure.GPIO_IrqSense = GPIO_IrqSense_Edge;
  GPIO_EXTIStructure.GPIO_Event = GPIO_Event_Low;
  GPIO_EXTIConfig(&GPIO_EXTIStructure);
  
  /* Clear pending interrupt */
  GPIO_ClearITPendingBit(SPI_CS_PIN);
  
  /* Enable the SPI Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = GPIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = MED_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
#endif
  
  /* Enable the Blue Controller Interrupt */
  NVIC_EnableRadioIrq();
}

void NVIC_DisableCSnIrq(void)
{
  NVIC_InitType NVIC_InitStructure;

  /* Enable the Blue Controller Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = GPIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = MED_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);

}
void NVIC_EnableCSnIrq(void)
{
  NVIC_InitType NVIC_InitStructure;

  /* Enable the Blue Controller Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = GPIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = MED_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void NVIC_DisableRadioIrq(void)
{
  NVIC_InitType NVIC_InitStructure;

  /* Enable the Blue Controller Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = BLUE_CTRL_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);

}
void NVIC_EnableRadioIrq(void)
{
  NVIC_InitType NVIC_InitStructure;

  /* Enable the Blue Controller Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = BLUE_CTRL_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void GPIO_RTS_Output(void)
{
  GPIO_InitType GPIO_InitStructure;
  
  /* Init Structure */
  GPIO_StructInit(&GPIO_InitStructure);
  
  /* Configure the RTS pin as output GPIO */
  GPIO_InitStructure.GPIO_Pin = UART_RTS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Output;
  
  /* Set high the RTS pin */
  GPIO_SetBits(UART_RTS_PIN);
  GPIO_Init(&GPIO_InitStructure);
  
}


void GPIO_RTS_Uart(void)
{
  GPIO_InitType GPIO_InitStructure;
  
  /* Init Structure */
  GPIO_StructInit(&GPIO_InitStructure);
  
  /* Configure RTS pin as UART_RTS */
  GPIO_InitStructure.GPIO_Pull = ENABLE;
  GPIO_InitStructure.GPIO_HighPwr = ENABLE;
  GPIO_InitStructure.GPIO_Pin = UART_RTS_PIN;
  GPIO_InitStructure.GPIO_Mode = Serial1_Mode;
  
  GPIO_Init(&GPIO_InitStructure);
}

void GPIO_CTS_Input(void)
{
  GPIO_InitType GPIO_InitStructure;
  
  /* Init Structure */
  GPIO_StructInit(&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = UART_CTS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Input;
  
  GPIO_Init(&GPIO_InitStructure);
}

void GPIO_CTS_Irq(FunctionalState NewState)
{
  if (NewState != DISABLE) {
    GPIO_ClearITPendingBit(UART_CTS_PIN);  
    GPIO_EXTICmd(UART_CTS_PIN, ENABLE);
  }
  else {
    GPIO_EXTICmd(UART_CTS_PIN, DISABLE);
  }
}

void GPIO_CTS_Uart(void)
{
  GPIO_InitType GPIO_InitStructure;
  
  /** Init Structure */
  GPIO_StructInit(&GPIO_InitStructure);
  
  /* Configure CTS pin as UART_CTS */
  GPIO_InitStructure.GPIO_Pin = UART_CTS_PIN;
  GPIO_InitStructure.GPIO_Mode = Serial1_Mode;
  
  GPIO_Init(&GPIO_InitStructure);
  
  GPIO_EXTICmd(UART_CTS_PIN, DISABLE);
  
}


/**
* @brief  GPIO Configuration.
*	  Configure outputs GPIO pins.
* @param  None
* @retval None
*/
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;
  
  /* GPIO Periph clock enable */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_GPIO, ENABLE);
  
  /* Init Structure */
  GPIO_StructInit(&GPIO_InitStructure);
  
#ifdef UART_INTERFACE 
  /* Configure GPIO_Pin_8 and GPIO_Pin_11 as UART_TXD and UART_RXD */
  GPIO_InitStructure.GPIO_Pin = UART_TX_PIN | UART_RX_PIN;
  GPIO_InitStructure.GPIO_Mode = Serial1_Mode;
  GPIO_InitStructure.GPIO_Pull = DISABLE;
  GPIO_InitStructure.GPIO_HighPwr = DISABLE;
  
  GPIO_Init(&GPIO_InitStructure);
#ifdef UART_SLEEP
  /* Configure GPIO_Pin_6 (UART_RTS) as GPIO output */
  GPIO_RTS_Output(); 
  
  /* Configure GPIO_Pin_13 (UART_CTS) as GPIO input */
  GPIO_CTS_Input();
#endif
#endif
  
#ifdef SPI_INTERFACE
  /** Configure GPIO pins for SPI.
  * GPIO_Pin_11 = CS
  * GPIO_Pin_0 = CLK
  * GPIO_Pin_2 = MOSI
  * GPIO_Pin_3 = MISO
  */
  GPIO_InitStructure.GPIO_Pin = SPI_CLCK_PIN | SPI_OUT_PIN | SPI_IN_PIN | SPI_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_MODE_SPI;
  GPIO_Init(&GPIO_InitStructure);
  
  /* GPIO_Pin_7 = IRQ */
  GPIO_InitStructure.GPIO_Pin = SPI_IRQ_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Output;
  GPIO_Init(&GPIO_InitStructure);
  GPIO_ResetBits(SPI_IRQ_PIN);
  
#endif
  
#ifdef DEBUG_DTM
  /* LED1 LED2 LED3 init */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Output;
  GPIO_InitStructure.GPIO_Pull = DISABLE;
  GPIO_InitStructure.GPIO_HighPwr = DISABLE;  
  GPIO_Init(&GPIO_InitStructure);
#endif
  
}

/**
* @brief  UART Configuration.
*	  Configure UART peripheral.
* @param  None
* @retval None
*/
void UART_Configuration(void)
{
  UART_InitType UART_InitStructure;
  
  /** Enable UART clock */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_UART, ENABLE);
  
  /** Init Structure */
  UART_StructInit(&UART_InitStructure);
  
  /** Configure UART */
  UART_InitStructure.UART_BaudRate = 115200;
  UART_InitStructure.UART_WordLengthTransmit = UART_WordLength_8b;
  UART_InitStructure.UART_WordLengthReceive = UART_WordLength_8b;
  UART_InitStructure.UART_StopBits = UART_StopBits_1;
  UART_InitStructure.UART_Parity = UART_Parity_No;
#ifdef UART_SLEEP
  UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_RTS_CTS;
#else
  UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
#endif
  UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;
  UART_InitStructure.UART_FifoEnable = DISABLE;
  UART_Init(&UART_InitStructure);

  /* Enable the RX FIFO and setup the trigger points for receive FIFO interrupt to every byte */
  UART->LCRH_RX_b.FEN_RX = ENABLE;
  UART->IFLS_b.RXIFLSEL = 0;
}

/**
* @brief  SPI configuration.
* @param  None
* @retval None
*/
void SPI_Slave_Configuration(void)
{
  SPI_InitType SPI_InitStructure;
  
  /* Enable SPI and GPIO clocks */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_SPI, ENABLE);
  
  /* Configure SPI in master mode */
  SPI_StructInit(&SPI_InitStructure);
  SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_Init(&SPI_InitStructure);
      
  /* Clear RX and TX FIFO */
  SPI_ClearTXFIFO();
  SPI_ClearRXFIFO();
}


/**
* @brief  DMA configuration.
* @param  None
* @retval None
*/
void DMA_Configuration(void)
{  
  DMA_InitType DMA_InitStructure;
  
  /* Configure DMA peripheral */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_DMA, ENABLE);
  
  /* Configure DMA SPI TX channel */
  DMA_StructInit(&DMA_InitStructure);
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;   
  
#ifdef SPI_INTERFACE
  DMA_InitStructure.DMA_PeripheralBaseAddr = (SPI_BASE + 0x08); 
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
  DMA_Init(DMA_CH_SPI_TX, &DMA_InitStructure);
    
  /* Configure DMA SPI RX channel */
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_Init(DMA_CH_SPI_RX, &DMA_InitStructure);

  /* Enable the SPI RX DMA channel */
  DMA_Cmd(DMA_CH_SPI_RX, ENABLE);
  
  /* Enable SPI_TX/SPI_RX DMA requests */
  SPI_DMACmd(SPI_DMAReq_Tx | SPI_DMAReq_Rx, ENABLE);
#endif
  
#ifdef UART_INTERFACE
  DMA_InitStructure.DMA_PeripheralBaseAddr = UART_DR_ADDRESS;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_Init(DMA_CH_UART_TX, &DMA_InitStructure);

  /* Enable DMA_CH_UART_TX Transfer Complete interrupt */
  DMA_FlagConfig(DMA_CH_UART_TX, DMA_FLAG_TC, ENABLE);
  
  /* DMA_CH_UART_RX Initialization */
  DMA_Cmd(DMA_CH_UART_RX, DISABLE);
  
  /* Enable UART_DMAReq_Tx */
  UART_DMACmd(UART_DMAReq_Tx, ENABLE);
  
  /* Enable the UARTx Interrupt */
  NVIC_InitType NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = DMA_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = HIGH_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);    
  
#endif
}

uint8_t DMA_Rearm(DMA_CH_Type* DMAy_Channelx, uint32_t buffer, uint32_t size)
{
  /* DMA_CH_SPI_TX reset */
  DMAy_Channelx->CCR_b.EN = RESET;
  
  /* Rearm the DMA_CH_SPI_TX */
  DMAy_Channelx->CMAR = buffer;
  DMAy_Channelx->CNDTR = size;
  
  /* DMA_CH_SPI_TX enable */
  DMAy_Channelx->CCR_b.EN = SET;
  
  return 0;
}
