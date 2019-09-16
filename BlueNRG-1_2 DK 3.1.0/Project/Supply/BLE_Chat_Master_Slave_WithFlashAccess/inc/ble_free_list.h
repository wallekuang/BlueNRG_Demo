/*****************************************************************************
File Name:    ble_free_list.h
Description:  write and erase Flash  need to mutex all BLE event, when the BLE event is complex, which need to use a list to manage it
Note: 
History:	
Date                Author                   Description
2019-08-30         Lucien                     Create
****************************************************************************/
#ifndef __BLE_FREE_LIST_H__
#define __BLE_FREE_LIST_H__

#include <stdint.h>
#include <stdio.h>




void free_list_update(uint8_t Last_State,
													uint8_t Next_State, uint32_t Next_State_SysTime);


void flash_operate_tick(void);



void state_list_test(void);


void flash_operate_mutex_BEL_event_init(void);


#endif


