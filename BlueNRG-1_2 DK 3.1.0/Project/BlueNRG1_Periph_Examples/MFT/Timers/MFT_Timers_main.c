
/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : MTF/Timers/main.c 
* Author             : RF Application Team
* Version            : V1.0.0
* Date               : September-2016
* Description        : Code demostrating timers functionality
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


/** @addtogroup MTFX_Examples MTFX Examples
  * @{
  */

/** @addtogroup MTFX_Timers MTFX Timers Example
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void MFT_Configuration(void);

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
  
  /* LEDs initialization */
  SdkEvalLedInit(LED1);
  SdkEvalLedInit(LED2);
  SdkEvalLedInit(LED3);
  
  /* MFT configuration */
  MFT_Configuration();
  
  /* Configure SysTick to generate interrupt */
  SysTick_Config(SYST_CLOCK/2);
    
  /* Start MFT timers */
  MFT_Cmd(MFT1, ENABLE);
  MFT_Cmd(MFT2, ENABLE);
  
  /* Infinite loop */
  while(1);
}



/**
  * @brief  MFT_Configuration.
  * @param  None
  * @retval None
  */
void MFT_Configuration(void)
{
  NVIC_InitType NVIC_InitStructure;
  MFT_InitType timer_init;
  
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_MTFX1 | CLOCK_PERIPH_MTFX2, ENABLE);
  
  MFT_StructInit(&timer_init);
  
  timer_init.MFT_Mode = MFT_MODE_1;
  timer_init.MFT_Prescaler = 160-1;      /* 10 us clock */
  
  /* MFT1 configuration */
  timer_init.MFT_Clock2 = MFT_PRESCALED_CLK;
  MFT_Init(MFT1, &timer_init);
  /* Set the counter at 500 ms */
  MFT_SetCounter2(MFT1, 50000);
  
  /* MFT2 configuration */
  timer_init.MFT_Clock2 = MFT_PRESCALED_CLK;
  MFT_Init(MFT2, &timer_init);
  /* Set the counter at 250 ms */
  MFT_SetCounter2(MFT2, 25000);
    
  /* Enable MFT2 Interrupt 1 */
  NVIC_InitStructure.NVIC_IRQChannel = MFT1B_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LOW_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable MFT2 Interrupt 2 */ 
  NVIC_InitStructure.NVIC_IRQChannel = MFT2B_IRQn;
  NVIC_Init(&NVIC_InitStructure);
  
  /** Enable the MFT interrupt */
  MFT_EnableIT(MFT1, MFT_IT_TND, ENABLE);
  MFT_EnableIT(MFT2, MFT_IT_TND, ENABLE);
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
/******************* (C) COPYRIGHT 2016 STMicroelectronics *****END OF FILE****/
