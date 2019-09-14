
/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : MTFX/Mode_1a/main.c 
* Author             : RF Application Team
* Version            : V1.0.0
* Date               : September-2015
* Description        : Code demostrating the MFT Mode 1a functionality
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

/** @addtogroup MTFX_Mode1a MTFX Mode 1a Example
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PWM1_PIN			GPIO_Pin_3

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);
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
  
  /* GPIO configuration */
  GPIO_Configuration();
  
  /* MFT configuration */
  MFT_Configuration();
    
  /* Connect PWM output from timer to TnA pin */
  MFT_TnXEN(MFT2, MFT_TnA, ENABLE);
  
  /* Select software trigger for pulse-train generation */
  MFT_PulseTrainTriggerSelect(MFT2, MFT_SOFTWARE_TRIGGER);
  
  /* Enable MFT2 interrupt A and B */
  MFT_EnableIT(MFT2, MFT_IT_TNA | MFT_IT_TNB, ENABLE);
  
  /* Software delay before start the timer pulses */
  for(volatile uint32_t i=0;i<0x2FFFF;i++);

  /* Start MFT2 timer */
  MFT_Cmd(MFT2, ENABLE);
  
  /* Trigger pulse-train generation */
  MFT_PulseTrainSoftwareTrigger(MFT2);

  /* Infinite loop */
  while(1);
}


/**
  * @brief  GPIO Configuration.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitType GPIO_InitStructure;

  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_GPIO, ENABLE);
  
  /** Configure PWM1 pin */
  GPIO_InitStructure.GPIO_Pin = PWM1_PIN;
  GPIO_InitStructure.GPIO_Mode = Serial1_Mode;
  GPIO_InitStructure.GPIO_Pull = DISABLE;
  GPIO_InitStructure.GPIO_HighPwr = DISABLE;
  GPIO_Init( &GPIO_InitStructure);
 
}

/**
  * @brief  MFT_Configuration.
  * @param  None
  * @retval None
  */
void MFT_Configuration()
{
  NVIC_InitType NVIC_InitStructure;
  MFT_InitType timer_init;
  
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_MTFX2, ENABLE);
  
  /* MFT2 configuration */
  timer_init.MFT_Mode = MFT_MODE_1a;
  timer_init.MFT_Prescaler = 160-1;      /* 10 us clock */
  
  timer_init.MFT_Clock1 = MFT_PRESCALED_CLK;
  timer_init.MFT_Clock2 = MFT_PRESCALED_CLK;
  timer_init.MFT_CRA = 50000 - 1;       /* 500 ms positive duration */
  timer_init.MFT_CRB = 25000 - 1;       /* 250 ms negative duration */
  MFT_Init(MFT2, &timer_init);
  
  /* Set the number of cycles */
  MFT_SetCounter(MFT2, 0, 30-1);      /* 30 pulse */

  /* Enable MFT Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = MFT2A_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LOW_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
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
