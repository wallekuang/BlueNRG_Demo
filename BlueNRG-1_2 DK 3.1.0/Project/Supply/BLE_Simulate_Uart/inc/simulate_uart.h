/*****************************************************************************
File Name:    simulate_uart.h
Description:   
Note: 
History:	
Date                Author                   Description
2020-01-15         Lucien                     Create
****************************************************************************/
#ifndef __SIMULATE_UART_H__
#define __SIMULATE_UART_H__
#include <stdint.h>



void simulate_uart_init(void);



void simulate_uart_handler(void);


void simulate_uart_write_fifo(uint8_t *pdata, uint16_t len);

uint32_t simulate_uart_fifo_len(void);



#endif



