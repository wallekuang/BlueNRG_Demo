/**
  ******************************************************************************
  * @file    PWM/PWM_handlers.c
* @author  SNM Application Team
* @version V1.0.0
* @date    September-2015
  * @brief   Handlers for PWM and other support functions.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  ******************************************************************************
  */
	
#include "PWM_handlers.h"

/* Global Variables */
uint8_t Duty_flag_A = 0;
/*Ton values in tick for each channel*/
uint16_t PWM0_on, 
	 PWM1_on, 
	 PWM2_on = MFT1_TON_2, 
	 PWM3_on = MFT2_TON_2, 
	 PWM4_on = MFT2_TON_3;

uint32_t channel = PWM3_PIN; /*it is the channel with initial lowest duty-cycle*/
uint32_t period = TIME_PERIOD; /*period in ticks of the PWM signal */
uint8_t channel_number = 0; /* index of channel managed by MFT2 */

uint32_t PWM_channels[] = {PWM3_PIN, PWM4_PIN}; /* channels managed by MFT2 
                                                   Timer 2 */

uint16_t Ton_values[] = {MFT2_TON_2,MFT2_TON_3}; /*sorted array containing duty-cycle values
                                                   respectively for PWM3_PIN and PWM4_PIN */

uint16_t delta_1 = MFT2_TON_3 - MFT2_TON_2; /*delta between the first two duty-cycles */

uint16_t delta = MFT2_TON_3 - MFT2_TON_2;

uint8_t number_of_channels = 2; /*number of channels managed by MFT2 Timer 2 */
uint16_t max_duty = MFT2_TON_3; /*biggest duty-cycle*/

/*
 * PWM2 handler
 */
void PWM2_handler() {
            
  SET_BIT(MFT1->TNICLR, MFT_IT_TND);
  if(Duty_flag_A == 1)
  {
    WRITE_REG(GPIO->DATS, PWM2_PIN);
    MFT1->TNCNT2 = PWM2_on;
    Duty_flag_A = 0;
    return;
  }else
    
  {
    WRITE_REG(GPIO->DATC, PWM2_PIN);
    MFT1->TNCNT2 = TIME_PERIOD - PWM2_on;
    Duty_flag_A = 1;
    return;
  }
}


/*
 * PWM3 and PWM4 handler
 */
void PWM3_PWM4_handler() {
  SET_BIT(MFT2->TNICLR, MFT_IT_TND);
  /*STATE 0
  if delta != 0, there is a channel with a duty lower than the other one.
  So the channel with the lower duty is cleared. Delta is period - max_duty
  (the duty of the other channel)
  The machine passes to STATE 1.
  
  if delta == 0, the two channel are cleared together.
  The machine passes to STATE 2
  
  The delta is computed for the next timer event*/
  
  if (channel_number < number_of_channels - 1){ 
    if (delta != 0){
      WRITE_REG(GPIO->DATC, channel);
      
    }
    else{
      WRITE_REG(GPIO->DATC, PWM3_PIN | PWM4_PIN);
      delta = period - max_duty;
      MFT2->TNCNT2 = delta - 1;        
      channel_number = number_of_channels;
      return;
    }
    MFT2->TNCNT2 = delta - 1;
    channel_number ++;
    delta = period - max_duty; /*computed delta for the next reset of the counter*/
    channel = PWM_channels[channel_number];
    return;
  }
  
  /*STATE 1
  Here the channel with the max duty-cycle is cleared. The delta is that one 
  computed in STATE 0. The machine passes to STATE 2 */
  if (channel_number == number_of_channels - 1){
    WRITE_REG(GPIO->DATC, channel);
    MFT2->TNCNT2 = delta - 1;
    channel_number ++;
    return;
  }     
  
  /*STATE 2
  All the channels have been cleared and now are set again. The machine passes to STATE 0 */
  if (channel_number > number_of_channels - 1){
    WRITE_REG(GPIO->DATS, PWM3_PIN | PWM4_PIN);
    MFT2->TNCNT2 = Ton_values[0] - 1;
    Ton_sorting();
    channel_number = 0;
    delta = Ton_values[1] - Ton_values[0];
    channel = PWM_channels[0];
    return;
  } 
}


/* Here the channel with the lower duty-cycle is established*/
void Ton_sorting(void){
  if (PWM3_on > PWM4_on){
    max_duty = PWM3_on; 
    Ton_values[1] = PWM3_on;
    Ton_values[0] = PWM4_on;
    PWM_channels[1] = PWM3_PIN;
    PWM_channels[0] = PWM4_PIN;
  }
  else{
    max_duty = PWM4_on;
    Ton_values[0] = PWM3_on;
    Ton_values[1] = PWM4_on;
    PWM_channels[0] = PWM3_PIN;
    PWM_channels[1] = PWM4_PIN;  
  }
}

/* Handle modifications in duty cycle */
void Modify_PWM(uint8_t PWM_ID, uint16_t duty_cycle) {
  switch (PWM_ID) {
  case 0: // PWM0 
    {
      MFT1->TNCRA = duty_cycle;
      MFT1->TNCRB = TIME_PERIOD - duty_cycle;
    }
    break;
  case 1: // PWM1 
    {
      MFT2->TNCRA = duty_cycle;
      MFT2->TNCRB = TIME_PERIOD - duty_cycle;
    }
    break;
  case 2: // PWM2 
    {
      PWM2_on = duty_cycle;
    }
    break;
  case 3: // PWM3 
    {
      PWM3_on = duty_cycle;
    }
    break;
  case 4: // PWM4 
    {
      PWM4_on = duty_cycle;
    }
    break;
  }
}
