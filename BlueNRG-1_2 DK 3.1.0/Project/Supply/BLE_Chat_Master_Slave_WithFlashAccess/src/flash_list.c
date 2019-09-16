/*****************************************************************************
File Name:    flash_list.c
Description:  write and erase Flash  need to mutex all BLE event, when the BLE event is complex, 
	which need to use a list to manage it
Note: 
History:	
Date                Author                   Description
2019-09-02         Lucien                     Create
****************************************************************************/
#include "double_list.h"
#include "flash_list.h"
#include "BlueNRG1_flash.h"


enum{
		FLASH_OP_ERASE = 0,
		FLASH_OP_WRITE,
		
		FLASH_OP_MAX,
};

struct flash_op {
    struct dl_list node;
		uint8_t flash_op_mode;			// write or erase
		FLASH_DONE_CALLBACK cb;			// callback for when flash operate done
		void* param;								// param for callback
		union {
				// erase op
				struct{
						uint16_t pageNum;		// Flash erase page number
				}flash_erase_t;
				// write op
				struct{
						uint32_t Address;		// Flash been write address
						uint8_t* Data;			// Flash been write data point
						uint16_t length;		// Flash been write data length
				}flash_write_t;
		}info;
};

// memmory for flash operate list  ,ARRAY_LEN(__flash_memory) is the length of the list 
static struct flash_op __flash_memory[6];

static struct dl_list g_flash_list;
static struct dl_list g_flash_unuse;

/*  Init flash op list and unuse list */
void flash_list_init(void)
{
		dl_list_init(&g_flash_list);
		dl_list_init(&g_flash_unuse);

		for (int i = 0; i < ARRAY_LEN(__flash_memory); ++i) {
        dl_list_add(&g_flash_unuse, &__flash_memory[i].node);
    }
}

/* 
	erase flash page operate api
@input 
		pageNum: page number of wanting to erase
		cb:			 callback of when finish erase 
		param:	 param of when callback
*/
bool flash_list_erase_page(uint16_t pageNum, FLASH_DONE_CALLBACK cb, void *param)
{
		// check if it have free node for erase op
		struct flash_op *item = NULL;
		bool ret = false;
		if (!dl_list_empty(&g_flash_unuse)) {
			  // 
    		item = dl_list_last(&g_flash_unuse, struct flash_op, node);
    }
		else{
				printf("flash unuse list is empty\n");
		}

		if(item != NULL){
			  // remove node from unuse list
				dl_list_del(&item->node);
				item->cb = cb;
				item->flash_op_mode = FLASH_OP_ERASE;
				item->info.flash_erase_t.pageNum = pageNum;
				item->param = param;
				// add flash op node to tail
				dl_list_add_tail(&g_flash_list, &item->node);
				ret = true;
		}

		return ret;
	
}


/* 
	write flash page operate api
@input 
		Address: address for write to flash
						 To write address must be alignmented 4-byte
		Data:		 Data for write to flash
		length:	 length for write to flash bytes 
		cb:			 callback of when finish erase 
		param:	 param of when callback
@output
		bool:		 whether the operate is success add into list
								true: 	add into list
								false:  fail
*/

bool flash_list_program_write(uint32_t Address, uint8_t* Data, uint16_t length, 
															FLASH_DONE_CALLBACK cb, void *param)

{
		// check if it have free node for erase op
		struct flash_op *item = NULL;
		bool ret = false;
		if (!dl_list_empty(&g_flash_unuse)) {
			  // get the last node
    		item = dl_list_last(&g_flash_unuse, struct flash_op, node);
    }

		if(item != NULL){
			  // remove node from unuse list
				dl_list_del(&item->node);
				item->cb = cb;
				item->flash_op_mode = FLASH_OP_WRITE;
				item->info.flash_write_t.Address = Address;
				item->info.flash_write_t.Data = Data;
				item->info.flash_write_t.length = length;
				item->param = param;
				// add flash op node to tail
				dl_list_add_tail(&g_flash_list, &item->node);
				ret = true;
		}

		return ret;
}


int flash_list_empty(void)
{
		return dl_list_empty(&g_flash_list);
}


/**
 * @brief  Verifies flash content.
 * @param  currentWriteAddress: beginning of written address
 *         pbuffer: target buffer address
 *         size: buffer size
 * @retval Status.
 *
 * @note The API code could be subject to change in future releases.
 */
static ErrorStatus FLASH_Verify(uint32_t currentWriteAddress,uint8_t * pbuffer,uint8_t size)
{
  uint8_t * psource = (uint8_t*) (currentWriteAddress);
  uint8_t * pdest   = (uint8_t*) pbuffer;
  
  for(;(size>0) && (*(psource++) == *(pdest++)) ;size--);
  
  if (size>0)
    return ERROR;
  else
    return SUCCESS;
}

static ErrorStatus FLASH_ProgramWordWrite(uint32_t Addr, uint8_t *data, uint16_t length)
{
	uint8_t verifyStatus;
	uint16_t pos;
	uint32_t cur_addr;
	
	verifyStatus = SUCCESS;

	
	pos = 0; // unit byte
	
	while(pos <length){
			if(length > pos + 4*4){
					uint32_t temp[4];
					cur_addr = Addr + pos; 
					memcpy(temp,data+pos,4*4);
					FLASH_ProgramWordBurst(cur_addr, temp);
					verifyStatus = FLASH_Verify(cur_addr, (uint8_t*)temp, 4*4);
					pos += 4*4;
			}
			else{
					uint32_t alignmentWord = ~0;
					cur_addr = Addr + pos;
					uint8_t writ_byte = (pos+4<length) ? 4:(length-pos);
					memcpy(&alignmentWord, data+pos, writ_byte);
					FLASH_ProgramWord(cur_addr, alignmentWord);
					verifyStatus = FLASH_Verify(cur_addr, data+pos, writ_byte);
					pos += 1*4;
			}

			if (verifyStatus != SUCCESS){
					printf("write flash error \n");
					return ERROR;
			}
	}
	
	return SUCCESS;
}

/*
Flash interface timing characteristics  from datasheet
	Page erase time 									21.5 ms
	Mass erase time										21.5 ms
	Program time WRITE								44 us
	Program time BURSTWRITE 1 words		44 us
	Program time BURSTWRITE 2 words		66 us
	Program time BURSTWRITE 3 words		86 us
	Program time BURSTWRITE 4 words		107 us
*/ 
/* Schedule for flash list write
			gap_tick: the time of without rf activity gap
			if it have any ble even ,and it need to operate(erase or write) flash, 
					It need call the function flash_list_tick as follows:
							flash_list_tick(~0u);
*/
void flash_list_tick(uint32_t gap_tick)
{
		struct flash_op *item = NULL;
		if (!dl_list_empty(&g_flash_list)) {
				item = dl_list_first(&g_flash_list, struct flash_op, node);
		}

		if(item != NULL){
				uint32_t gap_us = (uint32_t)(gap_tick *2.4414);
				uint8_t op_mode = item->flash_op_mode;
				uint32_t guard_time = 0;
				if(op_mode >= FLASH_OP_MAX)
					return;
				
				if(op_mode == FLASH_OP_ERASE)
					guard_time = 21500 + 200;
				else 
					guard_time = (item->info.flash_write_t.length/16) * 107 + 400;
				ErrorStatus sta = SUCCESS;
				
				if((gap_us > guard_time) && (FLASH->IRQRAW_b.CMDDONE == SET)){
						switch(op_mode){
								case FLASH_OP_ERASE:
										  FLASH_ErasePage(item->info.flash_erase_t.pageNum);
									break;
								case FLASH_OP_WRITE:
											sta = FLASH_ProgramWordWrite(item->info.flash_write_t.Address,item->info.flash_write_t.Data, item->info.flash_write_t.length);
									break;
								default:
									break;
						}
						
						if(item->cb != NULL)
								item->cb(sta, item->param);
						
						dl_list_del(&item->node);
            dl_list_add(&g_flash_unuse, &item->node);
				}
		}

}

void print_list(struct dl_list *list)
{
		struct flash_op *item;
		dl_list_for_each(item, list, struct flash_op, node){
				printf("flash_op_mode: %d ", item->flash_op_mode);
				if(item->cb != 	NULL)
					item->cb(SUCCESS, NULL);
		}
}







