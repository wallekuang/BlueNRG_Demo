/**
  ******************************************************************************
  * @file    transport_layer.h 
  * @author  VMA RF Application Team
  * @version V1.0.0
  * @date    July-2015
  * @brief   
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TRANSPORT_LAYER_H
#define TRANSPORT_LAYER_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "crash_handler.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define BLUE_FLAG_RAM_RESET             0x01010101

extern uint32_t __blueflag_RAM;
extern uint8_t dma_state;
extern uint8_t reset_pending; //TBR

typedef enum {
  SPI_PROT_INIT_STATE = 0,                  /* Initialization phase         */
  SPI_PROT_CONFIGURED_STATE,                /* Configuration phase          */
  SPI_PROT_SLEEP_STATE,                     /* Sleep phase                  */
  SPI_PROT_CONFIGURED_HOST_REQ_STATE,       /* Host request phase           */
  SPI_PROT_CONFIGURED_EVENT_PEND_STATE,     /* Event pending phase          */
  SPI_PROT_WAITING_HEADER_STATE,            /* Waiting header phase         */
  SPI_PROT_HEADER_RECEIVED_STATE,           /* Header received phase        */
  SPI_PROT_WAITING_DATA_STATE,              /* Waiting data phase           */
  SPI_PROT_TRANS_COMPLETE_STATE,            /* Transaction complete phase   */
} SpiProtoType;



extern SpiProtoType spi_proto_state;
#define SPI_STATE_TRANSACTION(NEWSTATE)        spi_proto_state = NEWSTATE
#define SPI_STATE_CHECK(STATE)                (spi_proto_state==STATE)
#define SPI_STATE_FROM(STATE)                 (spi_proto_state>=STATE)



#ifdef DEBUG_DTM
typedef enum {
  EMPTY = 0,                  /* Initialization phase         */       
  GPIO_IRQ = 1000,                  /* Initialization phase         */       
  GPIO_CS_FALLING,                /* Configuration phase          */
  GPIO_CS_RISING,           /* Event pending phase          */
  PARSE_HOST_REQ,             /* Host request phase           */
  PARSE_EVENT_PEND,                     /* Sleep phase                  */
  ADVANCE_DMA_WRITE,            /* Waiting header phase         */
  ADVANCE_DMA_READ,           /* Header received phase        */
  ADVANCE_DMA_EVENT_PEND,              /* Waiting data phase           */
  ADVANCE_DMA_RESTORE_0,
  ADVANCE_DMA_RESTORE_1,
  ADVANCE_DMA_RESTORE,
  RESTORE_SAVE,
  ADVANCE_DMA_DISCARD,
  ENQUEUE_EVENT,
  SEND_DATA,
  RECEIVE_DATA,
  BTLE_STACK_TICK_ON,
  BTLE_STACK_TICK_OFF,
  HEADER_RECEIVED,
  HEADER_NOT_RECEIVED,
  ENTER_SLEEP_FUNC,
  EXIT_SLEEP_FUNC,
  COMMAND_PROCESSED,
  CPU_HALT0,
  CPU_HALT1,
  CPU_HALT2,
  CPU_RUNNING0,
  CPU_RUNNING1,
  DEEP_SLEEP0,
  DEEP_SLEEP1,
  SPI_PROT_TRANS_COMPLETE,
  SPI_PROT_WAITING_DATA,
  EDGE_SENSITIVE,
  SLEEP_CHECK_PERFORMED,
  DISABLE_IRQ0,
  DISABLE_IRQ1,
  DMA_REARM,
  DMA_TC,
} DebugLabel;

#define DEBUG_ARRAY_LEN 1000
extern DebugLabel debug_buf[DEBUG_ARRAY_LEN];
extern uint32_t debug_cnt;
#define DEBUG_NOTES(NOTE)       {debug_buf[debug_cnt] = NOTE; debug_cnt = (debug_cnt+1)%DEBUG_ARRAY_LEN; }

#define DEBUG_SET_LED1()        GPIO_SetBits(GPIO_Pin_6)
#define DEBUG_SET_LED3()        GPIO_SetBits(GPIO_Pin_14)
#define DEBUG_RESET_LED1()      GPIO_ResetBits(GPIO_Pin_6)
#define DEBUG_RESET_LED3()      GPIO_ResetBits(GPIO_Pin_14)

#else

#define DEBUG_NOTES(NOTE)
#define DEBUG_SET_LED1()
#define DEBUG_SET_LED2()
#define DEBUG_SET_LED3()
#define DEBUG_RESET_LED1()
#define DEBUG_RESET_LED2()
#define DEBUG_RESET_LED3()

#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

extern void transport_layer_init(void);
extern void transport_layer_tick (void);
extern void send_command(uint8_t *cmd, uint16_t len);
extern void send_event_isr(uint8_t *buffer_out, uint16_t buffer_out_length, int8_t overflow_index);
extern void send_event(uint8_t *buffer_out, uint16_t buffer_out_length, int8_t overflow_index);
extern void advance_dma(void);
extern void advance_spi_dma(uint16_t rx_buffer_len);

#endif /* TRANSPORT_LAYER_H */
