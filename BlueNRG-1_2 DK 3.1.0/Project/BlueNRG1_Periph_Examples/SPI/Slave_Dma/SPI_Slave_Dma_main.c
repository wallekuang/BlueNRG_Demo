/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : SPI/Slave_Polling/main.c 
* Author             : RF Application Team
* Version            : V1.0.0
* Date               : September-2015
* Description        : Code demostrating the SPI slave functionality
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

/** @addtogroup SPI_Examples SPI Examples
* @{
*/

/** @addtogroup SPI_Slave_DMA SPI Slave DMA
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TOGGLE_DL1                         '1'
#define TOGGLE_DL2                         '2'
#define TOGGLE_DL3                         '3'

/* Chip Select pin used for the Slave_Polling demo */
#define SPI_CS_SLAVE_DEMO_PIN        GPIO_Pin_11
#define SPI_CS_SLAVE_DEMO_MODE       Serial0_Mode

/* Variables used for DMA operation */
extern volatile uint8_t spi_buffer_tx[SPI_BUFF_SIZE];
extern volatile uint8_t spi_buffer_rx[SPI_BUFF_SIZE];
extern uint8_t buffer_spi_tail;
extern volatile uint8_t buffer_spi_used;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void processCommand(void);
void SPI_Slave_Configuration(void);
void DMASpi_Sending(uint32_t buffer, uint32_t size);

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
  
  /* LEDS initialization */
  SdkEvalLedInit(LED1);
  SdkEvalLedInit(LED2);
  SdkEvalLedInit(LED3);
  
  /* Configure SysTick to generate interrupt with 25ms period */
  SysTick_Config(SYST_CLOCK/40 - 1);  
  
  /* SPI initialization */
  SPI_Slave_Configuration();
    
  /* Infinite loop for process command */
  processCommand();
    
}

/**
* @brief  Process command code
* @param  None
* @retval None
*/
void processCommand(void)
{
  uint8_t command;
  
  /* Infinite loop */
  while(1) {
    
    /* Get the LEDS actual status */
    spi_buffer_tx[0] =  ((uint8_t)SdkEvalLedGetState(LED3))<<2 | ((uint8_t)SdkEvalLedGetState(LED2))<<1 | ((uint8_t)SdkEvalLedGetState(LED1));

    /* Prepare buffer to sent through SPI DMA TX channel */
    DMASpi_Sending((uint32_t)&spi_buffer_tx[0], 1);
    
    while(buffer_spi_used==0);    
    
    /* Parse the command */
    command =  spi_buffer_rx[buffer_spi_tail];
    
    if(command == TOGGLE_DL1) {
      SdkEvalLedToggle(LED1);
    }
    else if(command == TOGGLE_DL2) {
      SdkEvalLedToggle(LED2);
    }
    else if(command == TOGGLE_DL3) {
      SdkEvalLedToggle(LED3);
    }
    
    buffer_spi_tail = (buffer_spi_tail+1)%SPI_BUFF_SIZE;    
    buffer_spi_used--;      
    
  }
}



/**
  * @brief  SPI Slave initialization.
  * @param  None
  * @retval None
  */
void SPI_Slave_Configuration(void)
{
  SPI_InitType SPI_InitStructure;
  GPIO_InitType GPIO_InitStructure;
  
  /* Enable SPI and GPIO clocks */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_GPIO | CLOCK_PERIPH_SPI, ENABLE);   
  
  /* Configure SPI pins */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = SDK_EVAL_SPI_PERIPH_OUT_PIN;
  GPIO_InitStructure.GPIO_Mode = SDK_EVAL_SPI_PERIPH_OUT_MODE;
  GPIO_InitStructure.GPIO_Pull = ENABLE;
  GPIO_InitStructure.GPIO_HighPwr = DISABLE;
  GPIO_Init(&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = SDK_EVAL_SPI_PERIPH_IN_PIN;
  GPIO_InitStructure.GPIO_Mode = SDK_EVAL_SPI_PERIPH_IN_MODE;
  GPIO_Init(&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = SDK_EVAL_SPI_PERIPH_SCLK_PIN;
  GPIO_InitStructure.GPIO_Mode = SDK_EVAL_SPI_PERIPH_SCLK_MODE;
  GPIO_Init(&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = SPI_CS_SLAVE_DEMO_PIN;
  GPIO_InitStructure.GPIO_Mode = SPI_CS_SLAVE_DEMO_MODE;
  GPIO_Init(&GPIO_InitStructure);
  
  /* Configure SPI in master mode */
  SPI_StructInit(&SPI_InitStructure);
  SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b ;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_BaudRate = SPI_BAUDRATE;
  SPI_Init(&SPI_InitStructure);

  /* Clear RX and TX FIFO */
  SPI_ClearTXFIFO();
  SPI_ClearRXFIFO();
  
  
  /* Configure DMA peripheral */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_DMA, ENABLE);
  
  DMA_InitType DMA_InitStructure;
  /* Configure DMA SPI TX channel */
  DMA_InitStructure.DMA_PeripheralBaseAddr = SPI_DR_BASE_ADDR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)spi_buffer_tx;  
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = (uint32_t)SPI_BUFF_SIZE;  
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;    
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
  DMA_Init(DMA_CH_SPI_TX, &DMA_InitStructure);
    
  /* Configure DMA SPI RX channel */
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)spi_buffer_rx;  
  DMA_InitStructure.DMA_BufferSize = (uint32_t)SPI_BUFF_SIZE;  
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_Init(DMA_CH_SPI_RX, &DMA_InitStructure);
  DMA_Cmd(DMA_CH_SPI_RX, ENABLE);
  
  /* Enable SPI_TX/SPI_RX DMA requests */
  SPI_DMACmd(SPI_DMAReq_Tx | SPI_DMAReq_Rx, ENABLE);
  
    
  /* Enable SPI functionality */
  SPI_Cmd(ENABLE);
  
}

void DMASpi_Sending(uint32_t buffer, uint32_t size) 
{
  /* DMA_CH_SPI_TX reset */
  DMA_CH_SPI_TX->CCR_b.EN = RESET;
  
  /* Rearm the DMA_CH_SPI_TX */
  DMA_CH_SPI_TX->CMAR = buffer;
  DMA_CH_SPI_TX->CNDTR = size;
  
  /* DMA_CH_SPI_TX enable */
  DMA_CH_SPI_TX->CCR_b.EN = SET;
  
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
