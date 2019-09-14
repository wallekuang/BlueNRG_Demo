/**
******************************************************************************
* @file    tramsport_layer.c 
* @author  VMA RF Application Team
* @version V1.1.0
* @date    27-March-2018
* @brief   Transport layer file
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

/* Includes ------------------------------------------------------------------*/
#include "BlueNRG_x_device.h"
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "BlueNRG1_api.h"
#include "transport_layer.h"
#include "sleep.h"
#include "hw_config.h"
#include "hci_parser.h"
#include "DTM_cmd_db.h"
#include "osal.h"
#include "fifo.h"
#include "cmd.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define EVENT_BUFFER_SIZE    1024
#define COMMAND_BUFFER_SIZE  (259+5)  /* 255 length + 4 ble header +5 SPI protocol header */
#define FIFO_ALIGNMENT       4
#define FIFO_VAR_LEN_ITEM_MAX_SIZE 258

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t event_buffer[EVENT_BUFFER_SIZE + FIFO_VAR_LEN_ITEM_MAX_SIZE];
uint8_t command_buffer[COMMAND_BUFFER_SIZE];
uint8_t command_fifo_buffer_tmp[COMMAND_BUFFER_SIZE];
circular_fifo_t event_fifo, command_fifo;
uint8_t reset_pending = 0; 


typedef PACKED(struct) event_lost_register_s {
  uint8_t event_lost;
  uint8_t event_register[5];
  uint64_t event_lost_code;
} event_lost_register_t;
static event_lost_register_t event_lost_register;

uint8_t dma_state = DMA_IDLE;

#ifdef DEBUG_DTM
DebugLabel debug_buf[DEBUG_ARRAY_LEN] = {EMPTY,};
uint32_t debug_cnt = 0;
#endif

/* SPI protocol variables & definitions */
#ifdef SPI_INTERFACE
#define SPI_HEADER_LEN  (uint8_t)(4)    /* Indeed the header len is 5 due to load of dummy from FIFO */
#define SPI_CTRL_WRITE  (uint8_t)(0x0A)
#define SPI_CTRL_READ   (uint8_t)(0x0B)

SpiProtoType spi_proto_state = SPI_PROT_INIT_STATE;

/* Store first 4 bytes replaced with spi header during send event procedure */
uint8_t event_fifo_header_restore[4] = {0,0,0,0};
uint8_t restore_flag = 0;
#endif

/* Private function prototypes -----------------------------------------------*/
static void enqueue_event(circular_fifo_t *fifo, uint16_t len, uint8_t *evt, int8_t overflow_idx);
/* Private functions ---------------------------------------------------------*/

SleepModes App_SleepMode_Check(SleepModes sleepMode)
{
#ifndef SPI_INTERFACE
  if (( (dma_state == DMA_IDLE) && (fifo_size(&event_fifo) > 0) )|| (fifo_size(&command_fifo) > 0) || reset_pending) {
    return SLEEPMODE_RUNNING;
  } else {
#ifdef UART_SLEEP
    if((UART_GetFlagStatus(UART_FLAG_BUSY) == SET) || (READ_BIT(GPIO->DATA, UART_CTS_PIN) == 0)){
      return SLEEPMODE_RUNNING;
    }
    else {
      SET_BIT(GPIO->IS, UART_CTS_PIN);
      return SLEEPMODE_NOTIMER; //SLEEPMODE_CPU_HALT;
    }
#else
  return SLEEPMODE_CPU_HALT;
#endif
  }
#else  
  if(((SPI_STATE_CHECK(SPI_PROT_SLEEP_STATE) || SPI_STATE_CHECK(SPI_PROT_CONFIGURED_STATE) ) && (fifo_size(&event_fifo) > 0)) || reset_pending) {
    return SLEEPMODE_RUNNING;
  }
  
  else if(SPI_STATE_CHECK(SPI_PROT_CONFIGURED_EVENT_PEND_STATE) || SPI_STATE_CHECK(SPI_PROT_WAITING_DATA_STATE)) {    
    return SLEEPMODE_CPU_HALT;
  }
  
  else if ((SPI_STATE_CHECK(SPI_PROT_SLEEP_STATE) || SPI_STATE_CHECK(SPI_PROT_CONFIGURED_STATE) ) && (fifo_size(&event_fifo) == 0)) {
    SPI_STATE_TRANSACTION(SPI_PROT_SLEEP_STATE);
    
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
    SET_BIT(GPIO->IS, SPI_CS_PIN);
    return SLEEPMODE_NOTIMER;
  }
  
  else {
    return SLEEPMODE_RUNNING;
  }
#endif
}

/* Process Commands */
uint16_t process_command(uint8_t *buffer_in, uint16_t buffer_in_length, uint8_t *buffer_out, uint16_t buffer_out_max_length)
{
  uint32_t i;
  uint16_t ret_val, opCode;
  
  Osal_MemCpy(&opCode, buffer_in, 2);
  for (i = 0; i < (sizeof(hci_command_table)/sizeof(hci_command_table_type)); i++) {
    if (opCode == hci_command_table[i].opcode) {
      ret_val = hci_command_table[i].execute(buffer_in+2, buffer_in_length-2, buffer_out, buffer_out_max_length);
      if (opCode == 0x0c03) {
        // For HCI_RESET, set flag to issue a sys reset
        reset_pending = 1;
      }
      /* add get crash handler */
      return ret_val;
    }
  }
  

  // Unknown command length
  buffer_out[0] = 0x04;
  buffer_out[1] = 0x0F;
  buffer_out[2] = 0x04;
  buffer_out[3] = 0x01;
  buffer_out[4] = 0x01;
  Osal_MemCpy(&buffer_out[5], &opCode, 2);
  return 7;
  
}


/**
* @brief  Transport Layer Init.
*	  Init the transport layer.
* @param  None
* @retval None
*/
void transport_layer_init(void)
{  
  /* Configure SysTick to generate interrupt */
  SysTick_Config(SYST_CLOCK/2);
  SysTick_State(DISABLE);
  
  /* WDG configuration */
  WDG_Configuration();
  
  /** GPIO configuration */
  GPIO_Configuration();
  
  /** NVIC configuration */
  NVIC_Configuration();  
  
#ifdef UART_INTERFACE
#ifdef UART_SLEEP
  GPIO_CTS_Irq(ENABLE);
#endif
  /** UART configuration */
  UART_Configuration();
  
  /** Enable UART interrupts */
  UART_ITConfig(UART_IT_RX, ENABLE);
  
  /** Enable UART */
  UART_Cmd(ENABLE);
#ifndef NO_DMA
  DMA_Configuration();
#endif
#endif
  
#ifdef SPI_INTERFACE
  
  /** SPI Configuration */
  SPI_Slave_Configuration();
  
  /** Enable SPI interrupts */
  GPIO_EXTICmd(SPI_CS_PIN, ENABLE);
  
  /* Enable SPI */
  SPI_SendData(0xFF);
  SPI_Cmd(ENABLE);
  DMA_Configuration();
  SPI_STATE_TRANSACTION(SPI_PROT_CONFIGURED_STATE);
#endif

  /* Queue index init */
  fifo_init(&event_fifo, EVENT_BUFFER_SIZE, event_buffer, FIFO_ALIGNMENT);
  fifo_init(&command_fifo, COMMAND_BUFFER_SIZE, command_buffer, FIFO_ALIGNMENT);
  
  /* event_lost_register init */
  event_lost_register.event_lost = 0;
  event_lost_register.event_register[0] = 0x04;
  event_lost_register.event_register[1] = 0xFF;
  event_lost_register.event_register[2] = 0x0A;
  event_lost_register.event_register[3] = 0x02;
  event_lost_register.event_register[4] = 0x00;
  event_lost_register.event_lost_code = 0;
}



/**
* @brief  Send data via transport layer
* @param  None
* @retval Desired sleep level
*/
uint16_t command_fifo_dma_len;


#ifdef UART_INTERFACE
static void transport_layer_send_data(uint8_t *data, uint16_t data_length)
{
#ifdef NO_DMA
#ifdef UART_SLEEP
  GPIO_CTS_Uart();
  GPIO_ResetBits(UART_RTS_PIN);   // low RTS
#endif
  for (uint16_t i = 0; i < data_length; i++) {
    UART_SendData(data[i]);
    while(UART_GetFlagStatus(UART_FLAG_TXFE) == RESET) {}
    while(UART_GetFlagStatus(UART_FLAG_BUSY) == SET) {}
  }
#ifdef UART_SLEEP
  data_length = 0;
  GPIO_SetBits(UART_RTS_PIN);     // high RTS
  GPIO_CTS_Input();
  GPIO_CTS_Irq(DISABLE);
  while(GPIO_ReadBit(UART_CTS_PIN) == Bit_RESET);    // wait for CTS high
  GPIO_CTS_Irq(ENABLE);
#endif
  fifo_discard_var_len_item(&event_fifo);
#else
  if (dma_state == DMA_IDLE) {
    dma_state = DMA_IN_PROGRESS;
    DEBUG_NOTES(DMA_REARM);
#ifdef UART_SLEEP
    while(UART_GetFlagStatus(UART_FLAG_TXFE) == RESET);
    GPIO_CTS_Uart();
    GPIO_ResetBits(UART_RTS_PIN);   // low RTS
#endif
    DMA_Rearm(DMA_CH_UART_TX, (uint32_t) data, data_length);
  }
#endif
  
}
#else
#ifdef SPI_INTERFACE

static void transport_layer_receive_data(void)
{  
  static uint8_t data[4];
  
  restore_flag = 0;  
  command_fifo_dma_len = (command_fifo.max_size - fifo_size(&command_fifo));
  
  data[0] = (uint8_t)command_fifo_dma_len;
  data[1] = (uint8_t)(command_fifo_dma_len>>8);
  data[2] = 0;
  data[3] = 0;
  
  DEBUG_NOTES(RECEIVE_DATA);
  DMA_Rearm(DMA_CH_SPI_RX, (uint32_t)command_fifo_buffer_tmp, command_fifo_dma_len);
  DMA_Rearm(DMA_CH_SPI_TX, (uint32_t)data, SPI_HEADER_LEN);
  
}


static void transport_layer_send_data(uint8_t *data, uint16_t data_length)
{  
  restore_flag = 1;
  
  event_fifo_header_restore[0] = data[0];
  event_fifo_header_restore[1] = data[1];
  event_fifo_header_restore[2] = data[2];
  event_fifo_header_restore[3] = data[3];
  
  command_fifo_dma_len = (command_fifo.max_size - fifo_size(&command_fifo));
  
  data[0] = (uint8_t)command_fifo_dma_len;
  data[1] = (uint8_t)(command_fifo_dma_len>>8);
  data[2] = (uint8_t)data_length;
  data[3] = (uint8_t)(data_length>>8);
  
  /* Dummy byte is the 1st byte of the header
  * Necessary because once the host master selects (CS low) the BlueNRG-1
  * a data is loaded in the shift-register and
  * the application cannot control this value.
  * So, the dummy byte has been added to the header.
  */
  SPI_ClearTXFIFO();        /* clear the TX FIFO */
  SPI_SendData(0xFF);
  
  //    DMASpi_Rearm(DMA_CH_SPI_RX, (uint32_t)&(command_fifo.buffer[command_fifo.head]), spi_buffer_len);
  DEBUG_NOTES(SEND_DATA);
  DMA_Rearm(DMA_CH_SPI_RX, (uint32_t)command_fifo_buffer_tmp, command_fifo_dma_len);
  DMA_Rearm(DMA_CH_SPI_TX, (uint32_t)data, data_length+SPI_HEADER_LEN);
  
}

#endif
#endif


volatile uint32_t systick_counter = 0;
uint32_t systick_counter_prev = 0;

/**
* @brief  Advance transport layer state machine
* @param  None
* @retval Desired sleep level
*/
void transport_layer_tick(void)
{
  uint8_t buffer[259], buffer_out[258];
  uint16_t len;
  uint16_t size = 0;
  
#ifdef WATCHDOG
  /* Clear watchdog pending bit, reload the timer and enable it */
  WDG_SetReload(RELOAD_TIME(WATCHDOG_TIME));
#endif
  
  /* Check reset pending */
  if ((fifo_size(&event_fifo) == 0) && reset_pending) {
#ifdef UART_INTERFACE
    while(UART_GetFlagStatus(UART_FLAG_TXFE) == RESET);
    while(UART_GetFlagStatus(UART_FLAG_BUSY) == SET);
#endif
    NVIC_SystemReset();
  }
  
#ifdef SPI_INTERFACE
  if(SPI_STATE_CHECK(SPI_PROT_CONFIGURED_HOST_REQ_STATE)) {
    DEBUG_NOTES(PARSE_HOST_REQ);    
    transport_layer_receive_data();
    SPI_STATE_TRANSACTION(SPI_PROT_WAITING_HEADER_STATE);
    GPIO_SetBits(SPI_IRQ_PIN);       /* Issue the SPI communication request */
  }
  else
#endif
    
#ifdef UART_INTERFACE
    /* Event queue */
    if ((fifo_size(&event_fifo) > 0) && (dma_state == DMA_IDLE)) {
      uint8_t *ptr;
      DEBUG_NOTES(SEND_DATA);
      if (fifo_get_ptr_var_len_item(&event_fifo, &size, &ptr) == 0) {
        transport_layer_send_data(ptr+FIFO_ALIGNMENT, size);
      }
    }
#endif
  
#ifdef SPI_INTERFACE
  /* Event queue */
  
  if ((fifo_size(&event_fifo) > 0) && (SPI_STATE_CHECK(SPI_PROT_CONFIGURED_STATE) || SPI_STATE_CHECK(SPI_PROT_SLEEP_STATE))) {
    uint8_t *ptr;
    if (fifo_get_ptr_var_len_item(&event_fifo, &size, &ptr) == 0) {
      DEBUG_NOTES(PARSE_EVENT_PEND);
      SPI_STATE_TRANSACTION(SPI_PROT_CONFIGURED_EVENT_PEND_STATE);
      
      if(SPI_STATE_CHECK(SPI_PROT_CONFIGURED_EVENT_PEND_STATE) && GPIO_ReadBit(SPI_CS_PIN) == Bit_RESET)
        SPI_STATE_TRANSACTION(SPI_PROT_WAITING_HEADER_STATE);        
      
      transport_layer_send_data(ptr, size);
      GPIO_SetBits(SPI_IRQ_PIN);       /* Issue the SPI communication request */
    }
  }
  
  
  while(SPI_STATE_CHECK(SPI_PROT_WAITING_HEADER_STATE)) {
    if( (command_fifo_dma_len - DMA_GetCurrDataCounter(DMA_CH_SPI_RX)) > 4) {
      DEBUG_NOTES(HEADER_RECEIVED);
      //      SPI_STATE_TRANSACTION(SPI_PROT_HEADER_RECEIVED_STATE);
      SPI_STATE_TRANSACTION(SPI_PROT_WAITING_DATA_STATE);
      DEBUG_NOTES(SPI_PROT_WAITING_DATA);
      GPIO_ResetBits(SPI_IRQ_PIN);       /* Issue the SPI communication request */      
      
      break;
    }
    if(systick_counter_prev == 0) {
      SysTick_State(ENABLE);
      systick_counter_prev = systick_counter;
    }
    /* wait 1 second */
    if((systick_counter-systick_counter_prev)>2) {
      SysTick_State(DISABLE);
      systick_counter_prev = 0;
      DEBUG_NOTES(HEADER_NOT_RECEIVED);
      
      SPI_STATE_TRANSACTION(SPI_PROT_TRANS_COMPLETE_STATE);
      GPIO_ResetBits(SPI_IRQ_PIN);       /* Issue the SPI communication request */
      
      if(restore_flag) {
        DEBUG_NOTES(ADVANCE_DMA_RESTORE);
        event_fifo.buffer[event_fifo.head] = event_fifo_header_restore[0];
        event_fifo.buffer[event_fifo.head+1] = event_fifo_header_restore[1];
        event_fifo.buffer[event_fifo.head+2] = event_fifo_header_restore[2];
        event_fifo.buffer[event_fifo.head+3] = event_fifo_header_restore[3];
      }
      SPI_STATE_TRANSACTION(SPI_PROT_CONFIGURED_STATE);  
      
      break;
    }
  }
#endif
  
  /* Command FIFO */
  if ((fifo_size(&command_fifo) > 0) && (!reset_pending)) {
    fifo_get_var_len_item(&command_fifo, &size, buffer);
    /* Set user events to temporary queue */
    len=process_command(buffer, size, buffer_out, 255);
    DEBUG_NOTES(COMMAND_PROCESSED);
    /* Set user events back to normal queue */
    send_event(buffer_out, len, 1);
    fifo_flush(&command_fifo);
  }
  
  if(event_lost_register.event_lost==1) {
    if (fifo_put_var_len_item(&event_fifo, 13, event_lost_register.event_register) == 0) {
      event_lost_register.event_lost = 0;
      event_lost_register.event_lost_code = 0;
    }
  }
}

void send_command(uint8_t *cmd, uint16_t len)
{
  fifo_put_var_len_item(&command_fifo, len, cmd);
}

void enqueue_event(circular_fifo_t *fifo, uint16_t len, uint8_t *evt, int8_t overflow_index)
{
  if (fifo_put_var_len_item(fifo,len,evt) != 0) {
    // Event queue overflow!!! TBD
    if ((overflow_index >=0) && (overflow_index < 64)) {
      event_lost_register.event_lost = 1;
      event_lost_register.event_lost_code |= (1 << overflow_index);
    } else {
      // assert 
    }
  }
}
void send_event_isr(uint8_t *buffer_out, uint16_t buffer_out_length, int8_t overflow_index)
{
  DEBUG_NOTES(ENQUEUE_EVENT);
  enqueue_event(&event_fifo, buffer_out_length, buffer_out, overflow_index);
}

void send_event(uint8_t *buffer_out, uint16_t buffer_out_length, int8_t overflow_index)
{
  //  NVIC_DisableRadioIrq();
  //  NVIC_DisableCSnIrq();
  send_event_isr(buffer_out, buffer_out_length, overflow_index);
  //  NVIC_EnableCSnIrq();
  //  NVIC_EnableRadioIrq();
}

#ifdef SPI_INTERFACE
void advance_spi_dma(uint16_t rx_buffer_len)
{
  uint8_t spi_command;
  
  if(restore_flag) {
    DEBUG_NOTES(ADVANCE_DMA_RESTORE);
    event_fifo.buffer[event_fifo.head] = event_fifo_header_restore[0];
    event_fifo.buffer[event_fifo.head+1] = event_fifo_header_restore[1];
    event_fifo.buffer[event_fifo.head+2] = event_fifo_header_restore[2];
    event_fifo.buffer[event_fifo.head+3] = event_fifo_header_restore[3];
  }
  if(rx_buffer_len>5) {
    /* get ctrl field from command buffer */  
    spi_command = command_fifo_buffer_tmp[0];
    
    if(spi_command == SPI_CTRL_WRITE) {
      DEBUG_NOTES(ADVANCE_DMA_WRITE);
      hci_input(&command_fifo_buffer_tmp[5], rx_buffer_len-5);
    }
    else if(spi_command == SPI_CTRL_READ) {
      DEBUG_NOTES(ADVANCE_DMA_READ);
      fifo_discard_var_len_item(&event_fifo);
      DEBUG_NOTES(ADVANCE_DMA_DISCARD);
    }
  }
  SPI_STATE_TRANSACTION(SPI_PROT_CONFIGURED_STATE);  
}

#endif

#ifdef UART_INTERFACE
void advance_dma(void)
{
  uint8_t *ptr;
  uint16_t size;
  fifo_discard_var_len_item(&event_fifo);
  
  if (fifo_size(&event_fifo) > 0) {
    if (fifo_get_ptr_var_len_item(&event_fifo, &size, &ptr) == 0) {
      transport_layer_send_data(ptr+FIFO_ALIGNMENT, size);
    }
  }
}
#endif
