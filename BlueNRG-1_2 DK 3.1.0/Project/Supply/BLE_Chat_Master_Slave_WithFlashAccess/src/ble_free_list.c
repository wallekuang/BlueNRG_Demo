/*****************************************************************************
File Name:    ble_free_list.c
Description:  write and erase Flash  need to mutex all BLE event, when the BLE event is complex, 
which need to use a list to manage ble even time
Note: 
History:	
Date                Author                   Description
2019-08-30         Lucien                     Create
****************************************************************************/
#include <stdlib.h>
#include "double_list.h"
#include "ble_free_list.h"
#include "bluenrg1_stack.h"
#include "flash_list.h"


struct ble_free {
    struct dl_list node;
    uint32_t next_state_sysTime;
};


// this length of arry  can be modify up to application
static struct ble_free __ble_free_memory[12];

static struct dl_list g_free_list;
static struct dl_list g_free_unuse;

static void free_list_init(void)
{
		dl_list_init(&g_free_list);
		dl_list_init(&g_free_unuse);

		for (int i = 0; i < ARRAY_LEN(__ble_free_memory); ++i) {
        dl_list_add(&g_free_unuse, &__ble_free_memory[i].node);
    }
}

void flash_operate_mutex_BEL_event_init(void)
{
		flash_list_init();
		free_list_init();
}

void flash_operate_tick(void)
{
		uint32_t cur_tick = 0;
		uint32_t next_state_sys_time = 0;

		cur_tick = HAL_VTimerGetCurrentTime_sysT32();
		// check whether the node is expire ?
		struct ble_free *item = NULL;
		uint32_t gap_ticks = 0;
		item = dl_list_last(&g_free_list, struct ble_free, node);
		if(item != NULL){
				next_state_sys_time = item->next_state_sysTime;
			  //printf("cur_tick:%d  next_state_sys_time:%d \n",cur_tick, next_state_sys_time);
				if(cur_tick < next_state_sys_time){
						// Time overflow
						if(abs(cur_tick - next_state_sys_time) > (~0u /2)){
								gap_ticks = ~0u - next_state_sys_time + cur_tick;
						}
						else{
								gap_ticks = next_state_sys_time - cur_tick;
						}
						// printf("gap_ticks:%.2f ",gap_ticks*2.4414/1000);
						flash_list_tick(gap_ticks);
				}
				else{
						// node have expired , remove it
						dl_list_del(&item->node);
            dl_list_add(&g_free_unuse, &item->node);
						// Recursion call it.
						//printf("Recursion call it \n");
						flash_operate_tick();
					  
						return;
				}
				
		}
		else{
		}
		
}

void free_list_update(uint8_t Last_State,uint8_t Next_State, uint32_t Next_State_SysTime)
{
		/* look for same next state node */
		struct ble_free *item = NULL;
		
		if (!dl_list_empty(&g_free_unuse)) {
        item = dl_list_last(&g_free_unuse, struct ble_free, node);
    }
		else{
				// if the list is empty. Schedule flash_operate_tick for release the expire node to g_state_unuse
				flash_operate_tick();
				
				if (!dl_list_empty(&g_free_unuse)) {
        		item = dl_list_last(&g_free_unuse, struct ble_free, node);
    		}
				else{
						// cast abnormal
						printf("g_state_unuse list is empty\n\n");
				}
		}
				
		/* add the new node to list tail */
    if (item) {
				dl_list_del(&item->node);
				item->next_state_sysTime = Next_State_SysTime;
        dl_list_add_tail(&g_free_list, &item->node);
    }
}

void free_list_print(struct dl_list *list)
{
		struct ble_free *item;
		dl_list_for_each(item, list, struct ble_free, node){
				printf("next_state_sysTime: %x ", item->next_state_sysTime);
		}
}















