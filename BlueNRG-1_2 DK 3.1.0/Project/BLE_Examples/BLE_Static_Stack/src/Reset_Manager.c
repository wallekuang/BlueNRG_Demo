/**
******************************************************************************
* @file    Reset_Manager.c 
* @author  VMA RF Application Team
* @version V1.0.0
* @date    July-2015
* @brief   Reset manager main file. It jumps to the application.
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
* <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
******************************************************************************
*/ 
#include "bluenrg_x_device.h"
#include "Reset_Manager_Config.h"

int main(void)
{
  if(*(uint32_t *)BLUE_FLAG_FLASH_BASE_ADDRESS == BLUE_FLAG_SET){
    
    EntryPoint entryPoint = (EntryPoint)(*(volatile uint32_t *)(APP_ADDR + 4));
    __set_MSP(*(volatile uint32_t*) APP_ADDR);
    entryPoint();
  }  
  while(1);
}

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
