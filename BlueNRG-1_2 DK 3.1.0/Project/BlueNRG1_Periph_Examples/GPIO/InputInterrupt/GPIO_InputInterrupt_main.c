
/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : GPIO/InputInterrupt/main.c 
* Author             : RF Application Team
* Version            : V1.1.0
* Date               : September-2015
* Description        : Code demostrating the GPIO interrupt functionality
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

/** @addtogroup BlueNRG1_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_Examples GPIO Examples
  * @{
  */

/** @addtogroup GPIO_InputInterrupt GPIO Input Interrupt Example
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program code.
  * @param  None
  * @retval None
  */
int main(void)
{
  /* System initialization function */
  SystemInit();
  
  /* Identify BlueNRG1 platform */
  SdkEvalIdentification();
  
  /* Enable the GPIO Clock */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_GPIO, ENABLE);
  
  /* Configure the LEDs */
  GPIO_InitType GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = Get_LedGpioPin(LED1) | Get_LedGpioPin(LED2);
  GPIO_InitStructure.GPIO_Mode = GPIO_Output;
  GPIO_InitStructure.GPIO_Pull = ENABLE;
  GPIO_InitStructure.GPIO_HighPwr = ENABLE;
  GPIO_Init(&GPIO_InitStructure);

  /* Put the LEDs off */
  GPIO_WriteBit(Get_LedGpioPin(LED1) | Get_LedGpioPin(LED2), Bit_RESET);
  
  /* Configure the push buttons */
  GPIO_InitStructure.GPIO_Pin = Get_ButtonGpioPin(BUTTON_1) | Get_ButtonGpioPin(BUTTON_2);
  GPIO_InitStructure.GPIO_Mode = GPIO_Input;
  GPIO_InitStructure.GPIO_Pull = DISABLE;
  GPIO_InitStructure.GPIO_HighPwr = DISABLE;
  GPIO_Init(&GPIO_InitStructure);
  
  /* Set the GPIO interrupt priority and enable it */
  NVIC_InitType NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = GPIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LOW_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Configures EXTI line for BUTTON_1 */
  GPIO_EXTIConfigType GPIO_EXTIStructure;
  GPIO_EXTIStructure.GPIO_Pin = Get_ButtonGpioPin(BUTTON_1);
  GPIO_EXTIStructure.GPIO_IrqSense = GPIO_IrqSense_Edge;
  GPIO_EXTIStructure.GPIO_Event = IRQ_ON_BOTH_EDGE;
  GPIO_EXTIConfig(&GPIO_EXTIStructure);

  /* Configures EXTI line for BUTTON_2 */
  GPIO_EXTIStructure.GPIO_Pin = Get_ButtonGpioPin(BUTTON_2);
  GPIO_EXTIStructure.GPIO_Event = IRQ_ON_RISING_EDGE;
  GPIO_EXTIConfig(&GPIO_EXTIStructure);
  
  /* Clear pending interrupt */
  GPIO_ClearITPendingBit(Get_ButtonGpioPin(BUTTON_1) | Get_ButtonGpioPin(BUTTON_2));
  
  /* Enable the interrupt */
  GPIO_EXTICmd(Get_ButtonGpioPin(BUTTON_1) | Get_ButtonGpioPin(BUTTON_2), ENABLE);

  /* Infinite loop */
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
