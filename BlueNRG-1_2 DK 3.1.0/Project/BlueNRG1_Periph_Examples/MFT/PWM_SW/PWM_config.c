/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : PWM/PWM_config.c
* Author             : SNM Application Team
* Version            : V1.0.0
* Date               : September-2015
* Description        : Configuration file for PWM
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#include "PWM_config.h"
#include "SDK_EVAL_Config.h"

/* 
 * Configure PWM sources and GPIO
 */
 
void PWM_Init() {
	
/* Prescaler for clock freqquency for MFT */	
#if (HS_SPEED_XTAL == HS_SPEED_XTAL_32MHZ)
	uint16_t prescaler = 16;
#elif (HS_SPEED_XTAL == HS_SPEED_XTAL_16MHZ)
	uint16_t prescaler = 8;
#endif
	
  /* GPIO Configuration */
  GPIO_InitType GPIO_InitStructure;
   
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_GPIO, ENABLE);

  /* Configure PWM pins */
  GPIO_InitStructure.GPIO_Pin = PWM0_PIN | PWM1_PIN;
  GPIO_InitStructure.GPIO_Mode = Serial2_Mode; //mode 1
  GPIO_InitStructure.GPIO_Pull = DISABLE;
  GPIO_InitStructure.GPIO_HighPwr = DISABLE;
  GPIO_Init( &GPIO_InitStructure);
  
  /* Configure GPIOs pin used to output PWM signal*/
  GPIO_InitStructure.GPIO_Pin = PWM2_PIN | PWM3_PIN | PWM4_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Output;
  GPIO_InitStructure.GPIO_Pull = ENABLE;
  GPIO_InitStructure.GPIO_HighPwr = ENABLE;
  GPIO_Init( &GPIO_InitStructure);
	
  /* MFT Configuration */
  NVIC_InitType NVIC_InitStructure;
  MFT_InitType timer_init;
  
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_MTFX1 | CLOCK_PERIPH_MTFX2, ENABLE);
  
  MFT_StructInit(&timer_init);
  
  timer_init.MFT_Mode = MFT_MODE_1;
  timer_init.MFT_Prescaler = prescaler - 1;      /* 2 MHz prescaled frequency*/
                                        
  /* MFT1 configuration */
  timer_init.MFT_Clock2 = MFT_PRESCALED_CLK;
  timer_init.MFT_Clock1 = MFT_PRESCALED_CLK;
  timer_init.MFT_CRA = MFT1_TON_1;       
  timer_init.MFT_CRB = MFT1_TOFF_1;       
  MFT_Init(MFT1, &timer_init);
  MFT_SetCounter2(MFT1, MFT1_TON_2);
  
  /* MFT2 configuration */
  timer_init.MFT_Clock2 = MFT_PRESCALED_CLK;
  timer_init.MFT_Clock1 = MFT_PRESCALED_CLK;
  timer_init.MFT_CRA = MFT2_TON_1;       
  timer_init.MFT_CRB = MFT2_TOFF_1;    
  MFT_Init(MFT2, &timer_init);
  MFT_SetCounter2(MFT2, MFT2_TON_2);
    
  /* Enable MFT2 Interrupt 1 */
  NVIC_InitStructure.NVIC_IRQChannel = MFT1B_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LOW_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable MFT2 Interrupt 2 */ 
  NVIC_InitStructure.NVIC_IRQChannel = MFT2B_IRQn;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the MFT interrupt */
  MFT_EnableIT(MFT1, MFT_IT_TND, ENABLE);
  MFT_EnableIT(MFT2, MFT_IT_TND, ENABLE);
  
  /* Connect PWM output from timers to TnA pins */
  MFT_TnXEN(MFT1, MFT_TnA, ENABLE);
  MFT_TnXEN(MFT2, MFT_TnA, ENABLE);
    
  /* Start MFT timers */
  MFT_Cmd(MFT1, ENABLE);
  MFT_Cmd(MFT2, ENABLE);
}
