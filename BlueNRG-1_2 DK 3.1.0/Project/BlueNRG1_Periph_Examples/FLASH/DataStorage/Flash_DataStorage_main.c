/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : Flash/DataStorage/main.c 
* Author             : RF Application Team
* Version            : V1.0.0
* Date               : September-2015
* Description        : Code demostrating the Flash functionality
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

/** @addtogroup BlueNRG1_StdPeriph_Examples
* @{
*/

/** @addtogroup Flash_Examples
* @{
*/

/** @addtogroup Flash_DataStorage Flash Timer Example
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DATA_STORAGE_PAGE       (N_PAGES-8)
#define DATA_STORAGE_ADDR       (((N_PAGES-8)*N_BYTES_PAGE) + FLASH_START)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char message[] = "BlueNRG-1 Flash Example - DATA STORAGE";

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
* @brief  Main program code
* @param  None
* @retval None
*/
int main(void)
{
  uint32_t memory_word = 0;
  uint8_t message_length = sizeof(message);
  uint8_t number_of_words = ((message_length / N_BYTES_WORD) + 1) * N_BYTES_WORD;
    
  /* System initialization function */
  SystemInit();
  
  /* Identify BlueNRG1 platform */
  SdkEvalIdentification();
  
  /* UART initialization */
  SdkEvalComUartInit(UART_BAUDRATE);
  
  /* LEDs initialization function */
  SdkEvalLedInit(LED1);
  
  /* Flash - Data Storage example */  
  printf("******************************\r\n* Flash - Data Storage example\r\n******************************\r\n");
    
  /* Erase the DATA_STORAGE_PAGE before write operation */
  FLASH_ErasePage(DATA_STORAGE_PAGE);
  
  /* Wait for the end of erase operation */
  while(FLASH_GetFlagStatus(Flash_CMDDONE) != SET);
  
  /* Erase page success */
  printf("Page %d erased.\r\n", DATA_STORAGE_PAGE);
    
  /* Write and verify operation */
  printf("Write message: \"%s\"\r\n", message);  
  for(uint8_t i = 0; i < number_of_words; i++) {
    memory_word = 0;
    for(uint8_t j = 0; j < 4; j++) {
      if( ((i*4)+j) > message_length) {
        memory_word |= (uint32_t)(0xFF<<(8*j));
      }
      else {
        memory_word |= (uint32_t)(message[i+j]<<(8*j));
      }
    }
    /* Program the word */
    FLASH_ProgramWord(DATA_STORAGE_ADDR + (i*4), memory_word);
    
    /* Wait for the end of write operation */
    while(FLASH_GetFlagStatus(Flash_CMDDONE) != SET);
    printf("Write at address: 0x%08X. ", DATA_STORAGE_ADDR + (i*4));
    
    /* Verify operation */
    if( memory_word != FLASH_ReadWord(DATA_STORAGE_ADDR + (i*4)) ) {
      printf("Verification failed 0x%08X.\r\n", (int)FLASH_ReadWord(DATA_STORAGE_ADDR + (i*4)));
      while(1);
    }
    else {
      printf("Success.\r\n");
    }
  }
  SdkEvalLedOn(LED1);
  
  /* Infinite while loop */
  while (1);  
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
