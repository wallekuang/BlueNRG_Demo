/**
******************************************************************************
* @file    DTM_Updater_main.c 
* @author  VMA RF Application Team
* @version V1.0.0
* @date    July-2015
* @brief   DTM updater main. IMPORTANT: user must NOT modify the updater algorithm and code.
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

#include "DTM_Updater.h"
#include "DTM_Updater_Config.h"

#define ENTERED_REASON_ACI_CMD          2
#define ENTERED_REASON_BAD_BLUEFLAG     3
#define ENTERED_REASON_IRQ_PIN          4

#define RESET_WAKE_DEEPSLEEP_REASONS 0x05
#define GPIO_Pin_3                 (0x0008)  /*!< Pin 3 selected */

int main(void)
{
  if ((CKGEN_SOC->REASON_RST == 0) && (CKGEN_BLE->REASON_RST > RESET_WAKE_DEEPSLEEP_REASONS)) {
    EntryPoint entryPoint = (EntryPoint)(*(volatile uint32_t *)(DTM_APP_ADDR + 4));
    __set_MSP(*(volatile uint32_t*) DTM_APP_ADDR);
    entryPoint();
    
    while(1);    
  }
  /* if BLUE_FLAG_RESET => a previous programming operation went bad */
  if(*(uint32_t *)BLUE_FLAG_RAM_BASE_ADDRESS == BLUE_FLAG_RAM_RESET) {
    *(uint32_t *)BLUE_FLAG_RAM_BASE_ADDRESS = BLUE_FLAG_SET;
    updater_init();
    updater(ENTERED_REASON_ACI_CMD);
  }
  else if(*(uint32_t *)BLUE_FLAG_FLASH_BASE_ADDRESS != BLUE_FLAG_SET) {
    updater_init();
    updater(ENTERED_REASON_BAD_BLUEFLAG);
  }
  else if(READ_BIT(GPIO->DATA, GPIO_Pin_3) != 0) {  // IO3 -> SPI MOSI PIN
    updater_init();
    updater(ENTERED_REASON_IRQ_PIN);
  }
  else {
    EntryPoint entryPoint = (EntryPoint)(*(volatile uint32_t *)(DTM_APP_ADDR + 4));
    __set_MSP(*(volatile uint32_t*) DTM_APP_ADDR);
    entryPoint();
    
    while(1);
  }
}

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
