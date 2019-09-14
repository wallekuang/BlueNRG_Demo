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

#include "BlueNRG1_mft.h"

/* TnA pins for MFT1 and MFT2 respectively */
#define PWM0_PIN			GPIO_Pin_4 
#define PWM1_PIN			GPIO_Pin_5

/* GPIOs pins used to output a PWM signal */
#define PWM2_PIN                        GPIO_Pin_8
#define PWM3_PIN                        GPIO_Pin_2 
#define PWM4_PIN                        GPIO_Pin_3


#define TIME_PERIOD 4000

/* MFT1 Timer 1 Ton and T0ff (ticks number) PWM0*/
#define MFT1_TON_1  (1500 - 1)
#define MFT1_TOFF_1 (2500 - 1)

/* MFT1 Timer 2 Ton and T0ff (ticks number) PWM2*/
#define MFT1_TON_2 (2000 - 1)
#define MFT1_TOFF_2 (2000 -1)

/* MFT2 Timer 1 Ton and T0ff (ticks number) PWM1*/
#define MFT2_TON_1 (1000 - 1 )
#define MFT2_TOFF_1 (3000 - 1)

/* MFT2 Timer 2 Ton and T0ff (ticks number) PWM3*/
#define MFT2_TON_2 (500 - 1)
#define MFT2_TOFF_2 (3500 - 1)

/* MFT2 Timer 2 Ton and T0ff (ticks number) PWM4*/
#define MFT2_TON_3 (2500 - 1)
#define MFT2_TOFF_3 (1500 - 1)

void PWM_Init(void);
void PWM3_PWM4_handler(void);
void PWM2_handler(void);

