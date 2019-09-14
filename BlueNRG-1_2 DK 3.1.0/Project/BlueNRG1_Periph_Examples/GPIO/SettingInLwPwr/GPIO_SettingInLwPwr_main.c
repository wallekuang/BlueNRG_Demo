/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : GPIO/SettingInLwrPwr/GPIO_SettingInLwrPwr_main.c 
* Author             : AMS - VMA RF Application Team
* Version            : V1.0.0
* Date               : 06-June-2018
* Description        : BlueNRG-2 main file for Setting In Low Power Mode
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "sleep.h"
#include "bluenrg1_stack.h"
#include "ble_status.h"
#include "SDK_EVAL_Led.h"
#include "osal.h"
#include "GPIO_SettingInLwPwr_config.h"

/** @addtogroup BlueNRG1_StdPeriph_Examples BlueNRG1 Standard Peripheral Examples
  * @{
  */

/** @addtogroup Micro_Examples Micro Examples
  * @{
  */

/** @addtogroup Micro_SleepTest  Micro Sleep Test Example
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
extern uint8_t __io_getcharNonBlocking(uint8_t *data);

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


void help(void)
{
  printf("\r\n");
  printf("s:   Standby - SLEEPMODE_NOTIMER: wake on uart/button PUSH1\r\n");
  printf("1:   Toggle the GPIO9\r\n");
  printf("2:   Toggle the GPIO10\r\n");
  printf("3:   Toggle the GPIO11\r\n");
  printf("r:   Reset the BlueNRG-1\r\n");
  printf("?:   Display this help menu\r\n");
  printf("\r\n> ");
}


/* Enable the Standby - SLEEPMODE_NOTIMER */
void sleep(void)
{
  uint8_t ret, wakeup_source, wakeup_level;

  wakeup_source = WAKEUP_IO13;
  wakeup_level = (WAKEUP_IOx_LOW << WAKEUP_IO13_SHIFT_MASK);

  printf("\r\nEnter to Standby - SLEEPMODE_NOTIMER\r\n");
  while(SdkEvalComUARTBusy() == SET);
  ret = BlueNRG_Sleep(SLEEPMODE_NOTIMER, wakeup_source, wakeup_level);
  if (ret != BLE_STATUS_SUCCESS) {
    printf("BlueNRG_Sleep() error 0x%02x\r\n", ret);
    while(1);
  }
  printf("Wake-up!!\r\n");
}


int main(void)
{
  uint8_t charRead, ret;
  
  /* System Init */
  SystemInit();
  
  /* Identify BlueNRG1 platform */
  SdkEvalIdentification();
  
  /* Enable UART */
  SdkEvalComIOConfig(SdkEvalComIOProcessInputData);
  
  /* Configure the BUTTON_1 button */
  SdkEvalPushButtonInit(BUTTON_1);
  /* IRQ_ON_FALLING_EDGE is set since wakeup level is low (mandatory setup for wakeup sources);  
     IRQ_ON_RISING_EDGE is the specific application level */ 
  SdkEvalPushButtonIrq(BUTTON_1, IRQ_ON_BOTH_EDGE); 

  /* Led DL1 configuration */
  SdkEvalLedInit(LED1);
  SdkEvalLedOn(LED1);

  /* GPIO setting in low power mode: output */
  GPIO_InitType GPIO_InitStructure;
  
  /* Configures Button pin as input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Output;
  GPIO_InitStructure.GPIO_Pull = DISABLE;
  GPIO_InitStructure.GPIO_HighPwr = DISABLE;
  
  /* GPIO9, GPIO10, GPIO11 initialization during low power modes for BlueNRG-2. */
  GPIO_InitLowPowerModes(&GPIO_InitStructure);
  /* Set the output state of the GIO9, GIO10, GIO11 during low power modes. BlueNRG-2 only */
  GPIO_WriteBitsLowPowerModes(GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11, Bit_RESET);
  
  /* BlufeNRG-1 stack init */
  ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);
  if (ret != BLE_STATUS_SUCCESS) {
    printf("Error during BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
    while(1);
  }
  
  printf("========================================================\r\n");
  printf("GPIOs setting during low power mode.\r\n");
  printf("The GPIO9, GPIO10 and GPIO11 can be set\r\n to retains their output state during low power mode.\r\n");
  printf("See the STEVAL-IDB008Vx schematic for the location of these GPIOs.\r\n");
  printf("Note that the GPIO9 and GPIO10 are the SWD pins.\r\n");
  printf("========================================================\r\n");
  printf("Enter ? for list of commands\r\n");

  printf("SYSTEM_CTRL->SLEEPIO_OEN %08X\r\n", SYSTEM_CTRL->SLEEPIO_OEN);
  printf("CKGEN_SOC->CLOCK_EN %08X\r\n", CKGEN_SOC->CLOCK_EN);
  while(1) {
    BTLE_StackTick();

    if (__io_getcharNonBlocking(&charRead)) {
      switch (charRead) {
      case 's':
        printf("Press PUSH1 button to wake-up the BlueNRG-2.\r\n");
        sleep();
        break;
      case '1':
        printf("Toggle GPIO9 low power output state\r\n");
        GPIO_ToggleBitsLowPowerModes(GPIO_Pin_9);
        printf("SYSTEM_CTRL->SLEEPIO_OUT %08X\r\n", SYSTEM_CTRL->SLEEPIO_OUT);
        break;
      case '2':
        printf("Toggle GPIO10 low power output state\r\n");
        GPIO_ToggleBitsLowPowerModes(GPIO_Pin_10);
        printf("SYSTEM_CTRL->SLEEPIO_OUT %08X\r\n", SYSTEM_CTRL->SLEEPIO_OUT);
        break;
      case '3':
        printf("Toggle GPIO11 low power output state\r\n");
        GPIO_ToggleBitsLowPowerModes(GPIO_Pin_11);
        printf("SYSTEM_CTRL->SLEEPIO_OUT %08X\r\n", SYSTEM_CTRL->SLEEPIO_OUT);
        break;        
      case '?':
        help();
        break;
      case 'r':
        NVIC_SystemReset();
        break;
      case '\r':
      case '\n':
        // ignored
        break;
      default:
        printf("Unknown Command\r\n");
      }
    }
  }
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


