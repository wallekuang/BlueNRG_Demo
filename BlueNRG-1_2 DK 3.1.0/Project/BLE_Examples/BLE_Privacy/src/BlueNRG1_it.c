
/* Includes----------------------------------------------------------------------*/
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h"
#include "bluenrg1_stack.h"
#include "SDK_EVAL_Com.h"
#include "clock.h"
#include "sleep.h"

#if SLAVE
#include "slave.h"
#elif MASTER
#include "master.h" 
#endif
/******************************************************************************/
/********************Cortex-M0 Processor Exceptions Handlers********************/
/******************************************************************************/


/******************************************************************************
 * Function Name  : nmi_handler.
 * Description    : This function handles NMI exception.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void NMI_Handler(void)
{
}


/******************************************************************************
 * Function Name  : hardfault_handler.
 * Description    : This function handles Hard Fault exception.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}


/******************************************************************************
 * Function Name  : svc_handler.
 * Description    : This function handles SVCall exception.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void SVC_Handler(void)
{
}


/******************************************************************************
 * Function Name  : pendsv_handler.
 * Description    : This function handles PendSV_Handler exception.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void PendSV_Handler(void)
{
}


/******************************************************************************
 * Function Name  : systick_handler.
 * Description    : This function handles SysTick Handler.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void SysTick_Handler(void)
{
  SysCount_Handler();
}


/******************************************************************************
 * Function Name  : gpio_handler.
 * Description    : This function handles GPIO Handler.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void GPIO_Handler(void)
{
   
  if(SdkEvalPushButtonGetITPendingBit(USER_BUTTON) == SET) 
  {
    SdkEvalPushButtonClearITPendingBit(USER_BUTTON);
  }
  
  /* Check If BUTTON is pressed: application action */
  if (SdkEvalPushButtonGetState(USER_BUTTON) == SET)
  {
    /* When connected BUTTON_1 allows to disconnects */
    if (APP_FLAG(CONNECTED))
    { 
       APP_FLAG_SET(DO_TERMINATE);
    }
  }
}
/******************************************************************************
*                 BlueNRG1LP Peripherals Interrupt Handlers                   *
*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the   *
*  available peripheral interrupt handler/'s name please refer to the startup *
* file (startup_BlueNRG1lp.s).
******************************************************************************/


/******************************************************************************
 * Function Name  : uart_handler.
 * Description    : This function handles UART interrupt request.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void UART_Handler(void)
{
}


/******************************************************************************
 * Function Name  : blue_handler.
 * Description    : This function handles Blue Handlers.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void Blue_Handler(void)
{
//  Call RAL_Isr
   RAL_Isr();
}
