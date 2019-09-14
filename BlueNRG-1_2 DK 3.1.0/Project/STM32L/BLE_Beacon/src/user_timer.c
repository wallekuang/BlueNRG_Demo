/**
  ******************************************************************************
  * @file    user_timer.c
  * @author  RF Application Team - AMG
  * @version V1.0.0
  * @date    24-June-2015
  * @brief   
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
    
#include "user_timer.h"

TIM_HandleTypeDef    TimHandle;
tClockTime user_timer_expired = FALSE;

/**
  * @brief  Initialize a timer for application usage.
  * @retval None
  */
void Init_User_Timer(void)
{ 
  /* TIMx Peripheral clock enable */
  TIMx_CLK_ENABLE();
  
  /* Set TIMx instance */
  TimHandle.Instance = TIMx;
  TimHandle.Init.Period            = USER_TIMER_PERIOD;
  TimHandle.Init.Prescaler         = USER_TIMER_PRESCALER;
  TimHandle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_DOWN;
  
  /* Configure the NVIC for TIMx */  
  HAL_NVIC_SetPriority(TIMx_IRQn, 4, 0); 
  /* Enable the TIMx global Interrupt */
  HAL_NVIC_EnableIRQ(TIMx_IRQn);
  
  HAL_TIM_Base_Init(&TimHandle);
  __HAL_TIM_CLEAR_FLAG(&TimHandle,TIM_FLAG_UPDATE);

}

/**
  * @brief  Start the user timer
  * @retval None
  */
void Start_User_Timer(void)
{
  __HAL_TIM_SET_COUNTER(&TimHandle,0);
  
  HAL_TIM_Base_Start_IT(&TimHandle);
  
}

/**
  * @brief   Stop the user timer
  * @retval None
  */
void Stop_User_Timer(void)
{
  HAL_TIM_Base_Stop_IT(&TimHandle);
}

/**
  * @brief  Adjust user  timer prescaler when entering sleep mode
  * @retval None
  */
void User_Timer_Enter_Sleep(void)
{
  uint16_t cnt;
  
  if((&TimHandle)->Instance->CR1 & TIM_CR1_CEN)
  {
    __HAL_TIM_DISABLE_IT(&TimHandle, TIM_IT_UPDATE);
    __HAL_TIM_SET_PRESCALER(&TimHandle,USER_TIMER_PRESCALER_SLEEP);
    cnt = __HAL_TIM_GET_COUNTER(&TimHandle);
    (&TimHandle)->Instance->EGR = TIM_EVENTSOURCE_UPDATE; //TBR
    __HAL_TIM_SET_COUNTER(&TimHandle,cnt);
    __HAL_TIM_CLEAR_FLAG(&TimHandle,TIM_FLAG_UPDATE);
    __HAL_TIM_ENABLE_IT(&TimHandle, TIM_IT_UPDATE);
  }
}


/**
  * @brief  Adjust user timer prescaler when exiting sleep mode
  * @retval None
  */
void User_Timer_Exit_Sleep(void)
{  
  uint16_t cnt;
  
  if((&TimHandle)->Instance->CR1 & TIM_CR1_CEN)
  {
    __HAL_TIM_DISABLE_IT(&TimHandle, TIM_IT_UPDATE);  
    __HAL_TIM_SET_PRESCALER(&TimHandle,USER_TIMER_PRESCALER);
    cnt = __HAL_TIM_GET_COUNTER(&TimHandle);
    (&TimHandle)->Instance->EGR = TIM_EVENTSOURCE_UPDATE; //TBR
    __HAL_TIM_SET_COUNTER(&TimHandle,cnt);
    __HAL_TIM_CLEAR_FLAG(&TimHandle,TIM_FLAG_UPDATE);
    __HAL_TIM_ENABLE_IT(&TimHandle, TIM_IT_UPDATE);
  }    
}

/*******************************************************************************
* Function Name  : TIMx_IRQHandler
* Description    : User Timer ISR
* Input          : None.
* Return         : None.
*******************************************************************************/
void TIMx_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TimHandle); //TBR
  
  user_timer_expired = TRUE;    
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
