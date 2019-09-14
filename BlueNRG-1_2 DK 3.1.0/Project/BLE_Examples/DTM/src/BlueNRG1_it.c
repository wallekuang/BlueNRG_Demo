/**
******************************************************************************
* @file    BlueNRG1_it.c 
* @author  VMA RF Application Team
* @version V1.1.0
* @date    27-March-2018
* @brief   Main Interrupt Service Routines.
*          This file provides template for all exceptions handler and
*          peripherals interrupt service routine.
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

/* Includes ------------------------------------------------------------------*/
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h"
#include "bluenrg1_stack.h"
#include "hci_parser.h"
#include "hw_config.h"
#include "transport_layer.h"
#include "miscutil.h" 

/** @addtogroup BlueNRG1_StdPeriph_Examples
* @{
*/

/** @addtogroup GPIO_Examples
* @{
*/ 

/** @addtogroup GPIO_IOToggle
* @{
*/ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  */
NOSTACK_FUNCTION(NORETURN_FUNCTION(void NMI_Handler(void)))
{
  HAL_CrashHandler(__get_MSP(), NMI_SIGNATURE);  
  /* Go to infinite loop when NMI exception occurs */
  while (1)
  {}
}

/**
* @brief  This function handles Hard Fault exception.
*/
NOSTACK_FUNCTION(NORETURN_FUNCTION(void HardFault_Handler(void)))
{
  HAL_CrashHandler(__get_MSP(), HARD_FAULT_SIGNATURE);  
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles SVCall exception.
  */
void SVC_Handler(void)
{
}


/**
  * @brief  This function handles PendSV_Handler exception.
  */
//void PendSV_Handler(void)
//{
//}

/**
* @brief  This function handles SysTick Handler.
*/
extern volatile uint32_t systick_counter;
void SysTick_Handler(void)
{
  systick_counter++;
}


/******************************************************************************/
/*                 BlueNRG-1 Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (system_bluenrg1.c).                                               */
/******************************************************************************/
/**
* @brief  This function handles UART interrupt request.
* @param  None
* @retval None
*/
uint8_t command_in_progress = 0;

#ifdef UART_INTERFACE
#ifdef UART_SLEEP
void GPIO_Handler(void)
{
  if(GPIO_GetITPendingBit(UART_CTS_PIN) == SET) {
    
    if(READ_BIT(GPIO->IS, UART_CTS_PIN)) {     /* if level sensitive */
      CLEAR_BIT(GPIO->IS, UART_CTS_PIN);       /* EDGE sensitive */
    }
    GPIO_ClearITPendingBit(UART_CTS_PIN);
    
    if(READ_BIT(GPIO->IEV, UART_CTS_PIN) != 0) {
      CLEAR_BIT(GPIO->IEV, UART_CTS_PIN);
      GPIO_RTS_Output();
    }
    else {
      SET_BIT(GPIO->IEV, UART_CTS_PIN); 
      GPIO_RTS_Uart();
      
    }
    
  }
}
#endif
void UART_Handler(void)
{  
  uint8_t data;
  
  while (UART_GetITStatus(UART_IT_RX) != RESET)
  {
    /* The Interrupt flag is cleared from the FIFO manager */
    data = UART_ReceiveData() & 0xFF;
    hci_input(&data, 1);
  }
  
  if (UART_GetITStatus(UART_IT_TX) != RESET)
  {
    /* Clear the interrupt */
    UART_ClearITPendingBit(UART_IT_TX);
  }
}

/**
* @brief  This function handles DMA Handler.
*/
void DMA_Handler(void)
{
  /* Check DMA_CH_UART_TX Transfer Complete interrupt */
  if(DMA_GetFlagStatus(DMA_FLAG_TC_UART_TX)) {
    DMA_ClearFlag(DMA_FLAG_TC_UART_TX);
    
    /* DMA1 finished the transfer of SrcBuffer */
    dma_state = DMA_IDLE;
    
    /* DMA_CH disable */
    DMA_CH_UART_TX->CCR_b.EN = RESET;
    DEBUG_NOTES(DMA_TC);
    
#ifdef UART_SLEEP
    while(UART_GetFlagStatus(UART_FLAG_TXFE) == RESET);
    while(UART_GetFlagStatus(UART_FLAG_BUSY) == SET);
    GPIO_SetBits(UART_RTS_PIN);     // high RTS
    GPIO_CTS_Input();
    GPIO_CTS_Irq(DISABLE);
    while(GPIO_ReadBit(UART_CTS_PIN) == Bit_RESET);    // wait for CTS high
    GPIO_CTS_Irq(ENABLE);
#endif
    advance_dma();
  }
}
#endif

/**
* @brief  This function handles GPIO interrupt request.
* @param  None
* @retval None
*/
#ifdef SPI_INTERFACE
extern uint16_t command_fifo_dma_len;
extern uint32_t systick_counter_prev;

void GPIO_Handler(void)
{
  if(GPIO_GetITPendingBit(SPI_CS_PIN) == SET) {
    
    /* Edge mode is the normal mode.
     * Level mode is for SLEEP mode because:
     * when in sleep mode, all the registers setting are lost
     * the wake up occurs when the SPI_CS_PIN is low and this trigger also the irq.
     * This irq will be lost if in edge sensitive mode, because
     * once the BlueNRG-1 is woken up, the restore of the registers setting is done,
     * and so the irq setting on edge detection, but the event is lost due to this delay.
     * So, before go in SLEEP state, the sensitive is changed to the level.
     * In this way, once the restore of the registers setting is done, the event is not lost.
     */
    if(READ_BIT(GPIO->IS, SPI_CS_PIN)) {     /* if level sensitive */
      CLEAR_BIT(GPIO->IS, SPI_CS_PIN);       /* EDGE sensitive */
      DEBUG_NOTES(EDGE_SENSITIVE);
    }
    GPIO_ClearITPendingBit(SPI_CS_PIN);
    
    /* CS pin rising edge - close SPI communication */
    if(READ_BIT(GPIO->IEV, SPI_CS_PIN) != 0) {
      DEBUG_NOTES(GPIO_CS_RISING);
      CLEAR_BIT(GPIO->IEV, SPI_CS_PIN);
      
      if(SPI_STATE_FROM(SPI_PROT_CONFIGURED_EVENT_PEND_STATE)) {
        GPIO_ResetBits(SPI_IRQ_PIN);
        systick_counter_prev = 0;
        SysTick_State(DISABLE);
        SPI_STATE_TRANSACTION(SPI_PROT_TRANS_COMPLETE_STATE);
        DEBUG_NOTES(SPI_PROT_TRANS_COMPLETE);
        
        /* Pass the number of data received in fifo_command */
        advance_spi_dma((command_fifo_dma_len - DMA_GetCurrDataCounter(DMA_CH_SPI_RX)));
      }
    }
    /* CS pin falling edge - start SPI communication */
    else {
      DEBUG_NOTES(GPIO_CS_FALLING);
      SET_BIT(GPIO->IEV, SPI_CS_PIN);
      
      if(SPI_STATE_CHECK(SPI_PROT_CONFIGURED_EVENT_PEND_STATE)) {
        SPI_STATE_TRANSACTION(SPI_PROT_WAITING_HEADER_STATE);
      }
      else if(SPI_STATE_CHECK(SPI_PROT_SLEEP_STATE) || SPI_STATE_CHECK(SPI_PROT_CONFIGURED_STATE)) {
        SPI_ClearTXFIFO();
        SPI_SendData(0xFF);
        SPI_STATE_TRANSACTION(SPI_PROT_CONFIGURED_HOST_REQ_STATE);
      }
    }
    
  }
  
}
#endif

/* irq_count used for the aci_hal_transmitter_test_packets_process() command implementation */
uint32_t irq_count = 0; 

void Blue_Handler(void)
{
  // Call RAL_Isr
  RAL_Isr();
  irq_count++;
}

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
