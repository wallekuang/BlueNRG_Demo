#include "bluenrg1_it_stub.h"
#include "bluenrg1_stack.h"
#include "clock.h"

extern const intvec_elem __vector_table[];
extern uint32_t *app_base;

typedef void (*EntryPoint)(void);
void NMI_Handler (void)
{
  if (app_base != (uint32_t *)__vector_table) {
     EntryPoint entryPoint = (EntryPoint)*((uint32_t *)((uint32_t)app_base + 0x08));
     entryPoint();
  } else {
    while(1);
  }
}
void HardFault_Handler (void)
{
  if (app_base != (uint32_t *)__vector_table) {
     EntryPoint entryPoint = (EntryPoint)*((uint32_t *)((uint32_t)app_base + 0x0c));
     entryPoint();
  } else {
    while(1);
  }
}
void SVC_Handler (void)
{
  if (app_base != (uint32_t *)__vector_table) {
     EntryPoint entryPoint = (EntryPoint)*((uint32_t *)((uint32_t)app_base + 0x2c));
     entryPoint();
  } else {
    while(1);
  }
}
void PendSV_Handler (void)
{
  if (app_base != (uint32_t *)__vector_table) {
     EntryPoint entryPoint = (EntryPoint)*((uint32_t *)((uint32_t)app_base + 0x38));
     entryPoint();
  } else {
    while(1);
  }
}
void SysTick_Handler (void)
{
  if (app_base != (uint32_t *)__vector_table) {
     EntryPoint entryPoint = (EntryPoint)*((uint32_t *)((uint32_t)app_base + 0x3c));
     entryPoint();
  } else {
#if ST_OTA_SERVICE_MANAGER_APPLICATION
    SysCount_Handler();
#else
    while(1);
#endif
  }
}
void GPIO_Handler (void)
{
  if (app_base != (uint32_t *)__vector_table) {
     EntryPoint entryPoint = (EntryPoint)*((uint32_t *)((uint32_t)app_base + 0x40));
     entryPoint();
  } else {
    while(1);
  }
}
void NVM_Handler (void)
{
  if (app_base != (uint32_t *)__vector_table) {
     EntryPoint entryPoint = (EntryPoint)*((uint32_t *)((uint32_t)app_base + 0x44));
     entryPoint();
  } else {
    while(1);
  }
}
void UART_Handler (void)
{
  if (app_base != (uint32_t *)__vector_table) {
     EntryPoint entryPoint = (EntryPoint)*((uint32_t *)((uint32_t)app_base + 0x50));
     entryPoint();
  } else {
    while(1);
  }
}
void SPI_Handler (void)
{
  if (app_base != (uint32_t *)__vector_table) {
     EntryPoint entryPoint = (EntryPoint)*((uint32_t *)((uint32_t)app_base + 0x54));
     entryPoint();
  } else {
    while(1);
  }
}
void Blue_Handler (void)
{
  if (app_base != (uint32_t *)__vector_table) {
     EntryPoint entryPoint = (EntryPoint)*((uint32_t *)((uint32_t)app_base + 0x58));
     entryPoint();
  } else {
#if ST_OTA_SERVICE_MANAGER_APPLICATION
    RAL_Isr();
#else
    while(1);
#endif
  }
}
void WDG_Handler (void)
{
  if (app_base != (uint32_t *)__vector_table) {
     EntryPoint entryPoint = (EntryPoint)*((uint32_t *)((uint32_t)app_base + 0x5c));
     entryPoint();
  } else {
    while(1);
  }
}
void ADC_Handler (void)
{
  if (app_base != (uint32_t *)__vector_table) {
     EntryPoint entryPoint = (EntryPoint)*((uint32_t *)((uint32_t)app_base + 0x74));
     entryPoint();
  } else {
    while(1);
  }
}
void I2C2_Handler (void)
{
  if (app_base != (uint32_t *)__vector_table) {
     EntryPoint entryPoint = (EntryPoint)*((uint32_t *)((uint32_t)app_base + 0x78));
     entryPoint();
  } else {
    while(1);
  }
}
void I2C1_Handler (void)
{
  if (app_base != (uint32_t *)__vector_table) {
     EntryPoint entryPoint = (EntryPoint)*((uint32_t *)((uint32_t)app_base + 0x7c));
     entryPoint();
  } else {
    while(1);
  }
}
void MFT1A_Handler (void)
{
  if (app_base != (uint32_t *)__vector_table) {
     EntryPoint entryPoint = (EntryPoint)*((uint32_t *)((uint32_t)app_base + 0x84));
     entryPoint();
  } else {
    while(1);
  }
}
void MFT1B_Handler (void)
{
  if (app_base != (uint32_t *)__vector_table) {
     EntryPoint entryPoint = (EntryPoint)*((uint32_t *)((uint32_t)app_base + 0x88));
     entryPoint();
  } else {
    while(1);
  }
}
void MFT2A_Handler (void)
{
  if (app_base != (uint32_t *)__vector_table) {
     EntryPoint entryPoint = (EntryPoint)*((uint32_t *)((uint32_t)app_base + 0x8c));
     entryPoint();
  } else {
    while(1);
  }
}
void MFT2B_Handler (void)
{
  if (app_base != (uint32_t *)__vector_table) {
     EntryPoint entryPoint = (EntryPoint)*((uint32_t *)((uint32_t)app_base + 0x90));
     entryPoint();
  } else {
    while(1);
  }
}
void RTC_Handler (void)
{
  if (app_base != (uint32_t *)__vector_table) {
     EntryPoint entryPoint = (EntryPoint)*((uint32_t *)((uint32_t)app_base + 0x94));
     entryPoint();
  } else {
    while(1);
  }
}
void DMA_Handler (void)
{
  if (app_base != (uint32_t *)__vector_table) {
     EntryPoint entryPoint = (EntryPoint)*((uint32_t *)((uint32_t)app_base + 0x9c));
     entryPoint();
  } else {
    while(1);
  }
}
