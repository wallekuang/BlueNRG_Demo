/******************** (C) COPYRIGHT 2017 STMicroelectronics ********************
* File Name          : DTM_Updater_boot.h
* Author             : AMS - VMA
* Version            : V1.0.0
* Date               : 19-May-2017
* Description        : BlueNRG Low Level Init function:DON'T MODIFY IT.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#include "BlueNRG_x_device.h"
#include "DTM_Updater_Config.h"

#define RESET_WAKE_DEEPSLEEP_REASONS 0x05
#define CRITICAL_PRIORITY 0
#if ST_OTA_SERVICE_MANAGER_APPLICATION
#define OTA_VALID_APP_TAG (0xAABBCCDD) /* OTA Service Manager has a special valid tag */
#else
#define OTA_VALID_APP_TAG (0xAA5555AA) 
#endif

#define BLUE_FLAG_TAG   (0x00000000)
 
NORETURN_FUNCTION(void NMI_Handler(void))
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x08));
  entryPoint();
  while (1);
}
NORETURN_FUNCTION(void HardFault_Handler(void))
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x0c));
  entryPoint();
  while (1);
}
void SVC_Handler(void)
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x2c));
  entryPoint();
}
void PendSV_Handler(void)
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x38));
  entryPoint();
}
void SysTick_Handler(void)
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x3c));
  entryPoint();
}
void GPIO_Handler(void)
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x40));
  entryPoint();
}
void Blue_Handler(void)
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x58));
  entryPoint();
}
void WDG_Handler(void)
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x5c));
  entryPoint();
}
void SPI_Handler(void)
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x54));
  entryPoint();
}
void UART_Handler(void)
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x50));
  entryPoint();
}

void NVM_Handler(void)
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x44));
  entryPoint();
}
void ADC_Handler(void)
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x74));
  entryPoint();
}
void I2C2_Handler(void)
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x78));
  entryPoint();
}
void I2C1_Handler(void)
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x7c));
  entryPoint();
}
void MFT1A_Handler(void)
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x84));
  entryPoint();
}
void MFT1B_Handler(void)
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x88));
  entryPoint();
}
void MFT2A_Handler(void)
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x8c));
  entryPoint();
}
void MFT2B_Handler(void)
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x90));
  entryPoint();
}
void RTC_Handler(void)
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x94));
  entryPoint();
}

void PKA_Handler(void)
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x98));
  entryPoint();
}

void DMA_Handler(void)
{
  EntryPoint entryPoint = (EntryPoint)*((uint32_t *)(DTM_APP_ADDR + 0x9c));
  entryPoint();
}


SECTION(".intvec")
REQUIRED(const intvec_elem __vector_table[]) = {
    {.__ptr = _INITIAL_SP},                   /* Stack address                      */
    {RESET_HANDLER},           		      /* Reset handler is C initialization. */
    {NMI_Handler},                            /* The NMI handler                    */ 
    {HardFault_Handler},                      /* The hard fault handler             */ 
    {(intfunc) OTA_VALID_APP_TAG},            /* OTA Application                    */
    {(intfunc) BLUE_FLAG_TAG},                /* Reserved for blue flag DTM updater */ 
    {0x00000000},                             /* Reserved                           */ 
    {0x00000000},                             /* Reserved                           */ 
    {0x00000000},                             /* Reserved                           */ 
    {0x00000000},                             /* Reserved                           */ 
    {0x00000000},                             /* Reserved                           */ 
    {SVC_Handler},                            /* SVCall                             */ 
    {0x00000000},                             /* Reserved                           */ 
    {0x00000000},                             /* Reserved                           */ 
    {PendSV_Handler},                         /* PendSV                             */ 
    {SysTick_Handler},                        /* SysTick_Handler                    */ 
    {GPIO_Handler},                           /* IRQ0:  GPIO                        */ 
    {NVM_Handler},                            /* IRQ1:  NVM                         */ 
    {0x00000000},                             /* IRQ2:                              */ 
    {0x00000000},                             /* IRQ3:                              */ 
    {UART_Handler},                           /* IRQ4:  UART                        */ 
    {SPI_Handler},                            /* IRQ5:  SPI                         */ 
    {Blue_Handler},                           /* IRQ6:  Blue                        */ 
    {WDG_Handler},                            /* IRQ7:  Watchdog                    */ 
    {0x00000000},                             /* IRQ8:                              */ 
    {0x00000000},                             /* IRQ9:                              */ 
    {0x00000000},                             /* IRQ10:                             */ 
    {0x00000000},                             /* IRQ11:                             */ 
    {0x00000000},                             /* IRQ12:                             */ 
    {ADC_Handler},                            /* IRQ13  ADC                         */ 
    {I2C2_Handler},                           /* IRQ14  I2C2                        */ 
    {I2C1_Handler},                           /* IRQ15  I2C1                        */ 
    {0x00000000},                             /* IRQ16                              */ 
    {MFT1A_Handler},                          /* IRQ17  MFT1 irq1                   */ 
    {MFT1B_Handler},                          /* IRQ18  MFT1 irq2                   */ 
    {MFT2A_Handler},                          /* IRQ19  MFT2 irq1                   */ 
    {MFT2B_Handler},                          /* IRQ20  MFT2 irq2                   */ 
    {RTC_Handler},                            /* IRQ21  RTC                         */ 
    {PKA_Handler},                            /* IRQ22                              */ 
    {DMA_Handler},                            /* IRQ23  DMA                         */ 
};

extern int main(void);
void __iar_program_start(void) @ "ENTRYPOINT"
{
  asm("CPSID i"); // Disable interrupts.
  main();
}

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
