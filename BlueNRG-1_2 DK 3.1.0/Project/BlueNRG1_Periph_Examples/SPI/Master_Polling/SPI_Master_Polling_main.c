/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : SPI/Master_Polling/main.c 
* Author             : RF Application Team
* Version            : V1.0.0
* Date               : September-2015
* Description        : Code demostrating the SPI master functionality
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
#include <stdio.h>
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"

/** @addtogroup BlueNRG1_StdPeriph_Examples BlueNRG1 Standard Peripheral Examples
* @{
*/

/** @addtogroup SPI_Examples SPI Examples
* @{
*/

/** @addtogroup SPI_Master_Polling SPI Master Polling
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TOGGLE_DL1                         '1'
#define TOGGLE_DL2                         '2'
#define TOGGLE_DL3                         '3'
#define TOGGLE_ALL_LED                     '4'

/* Chip Select pin used for the Master_Polling demo */
#define SPI_CS_MS_DEMO_PIN           GPIO_Pin_6


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void processCommand(void);
void helpMessage(void);
void SPI_Master_Configuration(void);
void SPI_SendReceiveData(uint8_t *command, uint8_t len);

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
  
  /* UART initialization */
  SdkEvalComUartInit(UART_BAUDRATE);
     
  /* SPI initialization */
  SPI_Master_Configuration();
  
  /* Infinite loop for process command */
  processCommand();
  
}

/**
* @brief  Help message code
* @param  None
* @retval None
*/
void helpMessage(void)
{
  printf("** SPI Master test program **\r\n(requires another board with SPI Slave test program connected by SPI\r\n\n");
  printf("%c: Toggle LED DL1\r\n", TOGGLE_DL1);
  printf("%c: Toggle LED DL2\r\n", TOGGLE_DL2);
  printf("%c: Toggle LED DL3\r\n", TOGGLE_DL3);
  printf("%c: Toggle ALL LED\r\n", TOGGLE_ALL_LED);
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
    while (UART_GetFlagStatus(UART_FLAG_RXFE) == SET);
    
    /* Store the received byte in RxBuffer */
    command = UART_ReceiveData();
    
    /* Parse the command */
    switch (command) {
    case TOGGLE_DL1:
      {
        /* SPI Master communication */
        SPI_SendReceiveData(&command, 1);
        printf("Toggle LED DL1\r\n");
      }
      break;
    case TOGGLE_DL2:      
      {
        /* SPI Master communication */
        SPI_SendReceiveData(&command, 1);
        printf("Toggle LED DL2\r\n");
      }
      break;     
    case TOGGLE_DL3:      
      {
        /* SPI Master communication */
        SPI_SendReceiveData(&command, 1);
        printf("Toggle LED DL3\r\n");
      }
      break;
    case TOGGLE_ALL_LED:      
      {
        uint8_t led[] = {TOGGLE_DL1, TOGGLE_DL2, TOGGLE_DL3};
        /* SPI Master communication */
        SPI_SendReceiveData(led, sizeof(led));
        printf("All LED Toggled\r\n");
      }
     break;
    default:
      helpMessage();
      break;
    }
  }
}


/**
  * @brief  SPI Master initialization.
  * @param  None
  * @retval None
  */
void SPI_Master_Configuration(void)
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
  
  GPIO_InitStructure.GPIO_Pin = SPI_CS_MS_DEMO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Output;
  GPIO_InitStructure.GPIO_HighPwr = ENABLE;
  GPIO_Init(&GPIO_InitStructure);
  GPIO_SetBits(SPI_CS_MS_DEMO_PIN);
  
  /* Configure SPI in master mode */
  SPI_StructInit(&SPI_InitStructure);
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b ;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_BaudRate = SPI_BAUDRATE;
  SPI_Init(&SPI_InitStructure);
  
  /* Clear RX and TX FIFO */
  SPI_ClearTXFIFO();
  SPI_ClearRXFIFO();
  
  /* Set null character */
  SPI_SetDummyCharacter(0xFF);
  
  /* Set communication mode */
  SPI_SetMasterCommunicationMode(SPI_FULL_DUPLEX_MODE);
  
  /* Enable SPI functionality */
  SPI_Cmd(ENABLE);  
  
}

/**
  * @brief  SPI Master communication.
  * @param  uint8_t command: value to send
  * @param  uint8_t pBuffer: value to get
  * @retval None
  */
void SPI_SendReceiveData(uint8_t *command, uint8_t len)
{  
  GPIO_ResetBits(SPI_CS_MS_DEMO_PIN);    
    
  for (uint8_t i=0; i< len; i++) {
    /* Write data to send to TX FIFO */
    while(RESET == SPI_GetFlagStatus(SPI_FLAG_TFE));
    SPI_SendData(command[i]);  
    while(RESET == SPI_GetFlagStatus(SPI_FLAG_RNE));
    SPI_ReceiveData();
    while (SET == SPI_GetFlagStatus(SPI_FLAG_BSY));
  }
  GPIO_SetBits(SPI_CS_MS_DEMO_PIN);
}


#ifdef USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
  ex: printff("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
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
