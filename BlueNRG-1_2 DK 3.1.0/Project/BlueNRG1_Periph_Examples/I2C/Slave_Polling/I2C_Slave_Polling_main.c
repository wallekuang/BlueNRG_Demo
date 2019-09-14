
/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : I2C/Slave_Polling/main.c 
* Author             : RF Application Team
* Version            : V1.0.0
* Date               : September-2015
* Description        : Code demostrating the I2C slave functionality
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

/** @addtogroup I2C_Examples I2C Examples
* @{
*/

/** @addtogroup I2C_Slave I2C Slave
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TOGGLE_DL1                         '1'
#define TOGGLE_DL2                         '2'
#define TOGGLE_DL3                         '3'

/* I2C device address */
#define DEV_ADDRESS             0xBE

/* I2C clock frequency */
#define SDK_EVAL_I2C_CLK_SPEED  (10000)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void processCommand(void);
void I2C_ConfigurationSlave(void);

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
  
  /* I2C initialization */
  I2C_ConfigurationSlave();
  
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
  uint8_t leds_state;
  uint8_t command;  
  
  /* Get the LEDS actual status */
  leds_state = ((uint8_t)SdkEvalLedGetState(LED3))<<2 | ((uint8_t)SdkEvalLedGetState(LED2))<<1 | ((uint8_t)SdkEvalLedGetState(LED1));
  
  /* Infinite loop */
  while(1) {
        
    /* Write-to-slave request received */
    if (I2C_GetITStatus((I2C_Type*)SDK_EVAL_I2C, I2C_IT_WTSR) == SET)
    {
      /* Clear write-to-slave flag */
      I2C_ClearITPendingBit((I2C_Type*)SDK_EVAL_I2C, I2C_IT_WTSR);
      
      /* Wait till I2C transaction is finished */
      while (I2C_GetITStatus((I2C_Type*)SDK_EVAL_I2C, I2C_IT_STD) == RESET);
      
      /* Length of the received data */
      command = ((I2C_Type*)SDK_EVAL_I2C)->SR_b.LENGTH;
      
      /* Get the register address to be used */
      command = I2C_ReceiveData((I2C_Type*)SDK_EVAL_I2C);
      
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
      
      /** Clear operation completed flag */
      I2C_ClearITPendingBit((I2C_Type*)SDK_EVAL_I2C, I2C_IT_STD);
      
    }
    
    /* Read-from-slave request received */
    else if (I2C_GetITStatus((I2C_Type*)SDK_EVAL_I2C, I2C_IT_RFSR) == SET)
    {
      /* Clear read-from-slave */
      I2C_ClearITPendingBit((I2C_Type*)SDK_EVAL_I2C, I2C_IT_RFSR);
      
      /* Fill the tx fifo for the response */
      I2C_FillTxFIFO((I2C_Type*)SDK_EVAL_I2C, leds_state);      
      
      
      /* Wait till the operation is finished */
      while (I2C_GetITStatus((I2C_Type*)SDK_EVAL_I2C, I2C_IT_STD) == RESET)
      {
        if(I2C_GetITStatus((I2C_Type*)SDK_EVAL_I2C, I2C_IT_RFSE ) == SET)
        {
          /* Fill the tx fifo for the response */
          I2C_FillTxFIFO((I2C_Type*)SDK_EVAL_I2C, leds_state);
        }
      }
      
      /* Get the LEDS actual status */
      leds_state = ((uint8_t)SdkEvalLedGetState(LED3))<<2 | ((uint8_t)SdkEvalLedGetState(LED2))<<1 | ((uint8_t)SdkEvalLedGetState(LED1));
      
      /* Slave transaction done - clear flag*/
      I2C_ClearITPendingBit((I2C_Type*)SDK_EVAL_I2C, I2C_IT_STD);
    }
     
  }
}



/**
  * @brief  I2C Slave initialization.
  * @param  None
  * @retval None
  */
void I2C_ConfigurationSlave(void)
{   
  GPIO_InitType GPIO_InitStructure;
  I2C_InitType I2C_InitStruct;
    
  /* Enable I2C and GPIO clocks */
  if((I2C_Type*)SDK_EVAL_I2C == I2C2) {
    SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_I2C2 | CLOCK_PERIPH_GPIO, ENABLE);
  }
  else {
    SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_I2C1 | CLOCK_PERIPH_GPIO, ENABLE);
  }
      
  /* Configure I2C pins */
  GPIO_InitStructure.GPIO_Pin = SDK_EVAL_I2C_CLK_PIN ;
  GPIO_InitStructure.GPIO_Mode = SDK_EVAL_I2C_DATA_MODE;
  GPIO_InitStructure.GPIO_Pull = ENABLE;
  GPIO_InitStructure.GPIO_HighPwr = DISABLE;
  GPIO_Init(&GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = SDK_EVAL_I2C_DATA_PIN;
  GPIO_InitStructure.GPIO_Mode = SDK_EVAL_I2C_DATA_MODE;  
  GPIO_Init(&GPIO_InitStructure);

  /* Configure I2C in slave mode */
  I2C_StructInit(&I2C_InitStruct);
  I2C_InitStruct.I2C_OperatingMode = I2C_OperatingMode_Slave;
  I2C_InitStruct.I2C_ClockSpeed = SDK_EVAL_I2C_CLK_SPEED;
  I2C_InitStruct.I2C_OwnAddress1 = DEV_ADDRESS;
  I2C_Init((I2C_Type*)(I2C_Type*)SDK_EVAL_I2C, &I2C_InitStruct);
  
  I2C_SetRxThreshold((I2C_Type*)SDK_EVAL_I2C, 1);
  I2C_SetTxThreshold((I2C_Type*)SDK_EVAL_I2C, 1);
  
  /** Clear all I2C pending interrupt */
  I2C_ClearITPendingBit((I2C_Type*)(I2C_Type*)SDK_EVAL_I2C, I2C_IT_MSK);
  
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

/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
