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

/** @addtogroup SPI_Slave_Polling SPI Slave Polling
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


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void processCommand(void);
void SPI_Slave_Configuration(void);

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
    
    /* Loop until the UART Receive Data Register is not empty */
    while (SPI_GetFlagStatus(SPI_FLAG_RNE) == RESET);
    
    /* Read command received from SPI master */
    command = SPI_ReceiveData();
        
    /* Parse the command */
    if(command == TOGGLE_DL1) {
      SdkEvalLedToggle(LED1);
    }
    else if(command == TOGGLE_DL2) {
      SdkEvalLedToggle(LED2);
    }
    else if(command == TOGGLE_DL3) {
      SdkEvalLedToggle(LED3);
    }
    
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
  
  /* Enable SPI functionality */
  SPI_Cmd(ENABLE);  
  
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
