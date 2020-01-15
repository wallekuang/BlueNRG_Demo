/*****************************************************************************
File Name:    simulate_uart.c
Description:   only support:		data bits 	8bit
																Parity 			None
																Stop Bits 	1
																Flow Type		None
Note: 
History:	
Date                Author                   Description
2020-01-15         Lucien                     Create
****************************************************************************/
#include <stdlib.h>
#include "SDK_EVAL_Config.h"
#include "user_buffer.h"
#include "simulate_uart.h"


#ifdef UART_BAUDRATE
#undef  UART_BAUDRATE
#endif

#define UART_BAUDRATE            (9600)
#define UART_TX_PIN 				(GPIO_Pin_6)



#define TX_PIN_OUT_HIGH()					(GPIO_SetBits(UART_TX_PIN))
#define TX_PIN_OUT_LOW()					(GPIO_ResetBits(UART_TX_PIN))


static uint8_t write_fifo[512+1];
static RingBuffer write_char_buf;


void simulate_uart_init(void)
{
	GPIO_InitType GPIO_InitStructure;
  
  /* Enable the GPIO Clock */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_GPIO, ENABLE);
  
  /* Configure the LED pin */
  GPIO_InitStructure.GPIO_Pin = UART_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Output;
  GPIO_InitStructure.GPIO_Pull = ENABLE;
  GPIO_InitStructure.GPIO_HighPwr = ENABLE;
  GPIO_Init(&GPIO_InitStructure);
	TX_PIN_OUT_HIGH();
	
	
	SdkEvalLedInit(LED1);
	user_buffer_create(&write_char_buf, sizeof(write_fifo)-1, write_fifo);
	
	
	SysTick_Config(SYST_CLOCK/UART_BAUDRATE);
	NVIC_SetPriority(SysTick_IRQn,   HIGH_PRIORITY);
	// 排空前面两个字符
	simulate_uart_write_fifo("  ",2);
}

void simulate_uart_write_fifo(uint8_t *pdata, uint16_t len)
{
	user_buffer_write_items(&write_char_buf,pdata,len);
}


uint32_t simulate_uart_fifo_len(void)
{
	return user_buffer_item_count(&write_char_buf);
}

void simulate_uart_handler(void)
{
	static uint32_t bit_cnt = 0;
	static uint8_t SENDING_CHAR = 0x15;
	
	if(user_buffer_item_count(&write_char_buf) == 0)
		return;

	if(bit_cnt == 0){
			TX_PIN_OUT_LOW();
			bit_cnt += 1;
			user_buffer_peek(&write_char_buf,&SENDING_CHAR,1);
	}
	else if( (bit_cnt>0) && (bit_cnt<=8) ){
			if(SENDING_CHAR & (1<<(bit_cnt-1)))
				TX_PIN_OUT_HIGH();
			else
				TX_PIN_OUT_LOW();
			bit_cnt += 1;
	}
	else if(bit_cnt == 9){
			TX_PIN_OUT_HIGH();
			bit_cnt += 1;
	}
	else if(bit_cnt == 10){
			TX_PIN_OUT_HIGH();
			bit_cnt += 1;
	}
	else if(bit_cnt >= 11){
			bit_cnt += 1;
	}
	
	if(bit_cnt >= 15){
		bit_cnt = 0;
		user_buffer_release_items(&write_char_buf,1);
	}
}

















