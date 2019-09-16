/*****************************************************************************
File Name:    flash_list.h
Description:  write and erase Flash  need to mutex all BLE event, when the BLE event is complex, which need to use a list to manage it
Note: 
History:	
Date                Author                   Description
2019-09-02         Lucien                     Create
****************************************************************************/
#ifndef __FLASH_LIST_H__
#define __FLASH_LIST_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "hal_types.h"


typedef void *(*FLASH_DONE_CALLBACK)(ErrorStatus sta,void* param);




void flash_list_init(void);

/* 
	erase flash page operate api
@input 
		pageNum: page number of wanting to erase
		cb:			 callback of when finish erase 
		param:	 param of when callback
*/
bool flash_list_erase_page(uint16_t pageNum, FLASH_DONE_CALLBACK cb, void *param);

/* 
	write flash page operate api
@input 
		Address: address for write to flash
		Data:		 Data for write to flash
		length:	 length for write to flash
		cb:			 callback of when finish erase 
		param:	 param of when callback
*/
bool flash_list_program_write(uint32_t Address, uint8_t* Data, uint16_t length, 
															FLASH_DONE_CALLBACK cb, void *param);

int flash_list_empty(void);


void flash_list_tick(uint32_t gap_tick);


void test_flash_list1(void);


#endif




