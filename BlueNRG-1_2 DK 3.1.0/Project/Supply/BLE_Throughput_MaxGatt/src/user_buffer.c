/*****************************************************************************
File Name:    user_buffer.h
Description:   
Note: 
History:	
Date                Author                   Description
2018-12-06         Lucien                     Create
****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "user_buffer.h"
#include "SDK_EVAL_Config.h"
/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

#define DISABLE_IRQ
#ifdef DISABLE_IRQ
#define disable_irqs()  NVIC_DisableIRQ(UART_IRQn)//GLOBAL_INT_DISABLE()
#define enable_irqs()   NVIC_EnableIRQ(UART_IRQn)//GLOBAL_INT_RESTORE()
#else
#define disable_irqs() {}
#define enable_irqs() {}
#endif
/*
* GLOBAL VARIABLE DEFINITIONS
****************************************************************************************
*/
#define PRINTF		printf

 /*
 * LOCAL FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief       function used to declare a new buffer
 *
 * @param[in]   buffer_size     (how many items should the buffer store)
 * @param[in]   item_size       (how big should each item be)
 * @param[in]   paddr           (memory address of buffer(it is a arr, The size of arr is (buffer size + 1) ))
 *
 * @return      RingBuffer pointer
 ****************************************************************************************
 */
void user_buffer_create(RingBuffer* buffer, int buffer_size, uint8_t *paddr)
{
    //initialise circular buffer
    buffer->buffer_size = buffer_size + 1;
    buffer->readIdx     = 0;
    buffer->writeIdx    = 0;
    buffer->endIdx      = buffer_size;
    
    //memory allocation of circular buffer
    buffer->data_ptr = paddr;//malloc((long)(buffer->buffer_size)*sizeof(uint8_t)); 
    

    return;
}

/**
 ****************************************************************************************
 * @brief       function used to check if buffer is initialized
 *
 * @param[in]   buffer      (buffer to check)
 *
 * @return      TRUE(1) or FALSE(0)
 ****************************************************************************************
 */
uint8_t user_buffer_initialized(RingBuffer *buffer)
{
    if(buffer != NULL &&
        buffer->data_ptr != NULL)
    {
        return true;
    }
    return false;
}

/**
 ****************************************************************************************
 * @brief       function used to count the amount of items in buffer
 *
 * @param[in]   buffer      (buffer to check)
 *
 * @return      amount of items
 ****************************************************************************************
 */
int user_buffer_item_count(RingBuffer* buffer)
{
    int wr = buffer->writeIdx;
    int rd = buffer->readIdx;
    
    //count items in buffer
    if (rd > wr)
    {
        return buffer->endIdx - rd + 1 + wr;
    }
    else
    {
        return wr - rd;
    }
}
/**
 ****************************************************************************************
 * @brief       function used calculate the free space in buffer
 *
 * @param[in]   buffer  (buffer to check)
 *
 * @return      amount of items
 ****************************************************************************************
 */
 
uint32_t get_room_left( RingBuffer* buffer )
{
    
    int readIdx, writeIdx;
    uint32_t roomLeft;

    readIdx = buffer->readIdx;
    writeIdx = buffer->writeIdx;
    
    if (readIdx > writeIdx)
    {
        //buffer does not cycle, it can only reach read pointer - 1 
        roomLeft = readIdx - writeIdx - 1;
    }
    else
    {
        roomLeft = buffer->buffer_size - writeIdx + readIdx - 1;
    }   
    return roomLeft;
}
 

/**
****************************************************************************************
* @brief        function used to push multiple items to buffer
*
* @param[in]    buffer                  (buffer to push data)
* @param[in]    wrdata                  (pointer with data which should be pushed)
* @param[in]    write_amount            (amount of bytes which should be pushed to buffer)
*
* @return       none
****************************************************************************************
*/

void user_buffer_write_items(RingBuffer* buffer, uint8_t* wrdata, uint16_t write_amount)
{
    int readIdx, writeIdx;
    int overflow = 0;
    int roomLeft;
    // Critical section
    disable_irqs();

    readIdx = buffer->readIdx;
    writeIdx = buffer->writeIdx;

    // End of critical section
    enable_irqs();

    if (readIdx > writeIdx)
    {
        //buffer does not cycle, it can only reach read pointer - 1 
        roomLeft = readIdx - writeIdx - 1;
        if( write_amount > roomLeft )
        {
            buffer->nof_buffer_write_miss_bytes  +=     write_amount-roomLeft;  
            PRINTF("miss_bytes %i\r\n", (int)write_amount-roomLeft);
            write_amount = roomLeft;
        }   
    }
    else
    {
        roomLeft = buffer->buffer_size - writeIdx + readIdx - 1;
        if( write_amount > roomLeft ){
            buffer->nof_buffer_write_miss_bytes  +=     write_amount-roomLeft;  
            PRINTF("miss_bytes %i\r\n", (int)write_amount-roomLeft);
            write_amount = roomLeft;
        }   
        if (writeIdx + write_amount > buffer->buffer_size)
        {
            overflow = writeIdx + write_amount - buffer->buffer_size;
        }
        //app_write_items can use the entire buffer
        buffer->endIdx = buffer->buffer_size - 1;
    }
    memcpy(buffer->data_ptr + writeIdx, wrdata, write_amount - overflow);
    memcpy(buffer->data_ptr, wrdata + write_amount - overflow, overflow);
        
    buffer->nof_buffer_write_bytes += write_amount;

    writeIdx += write_amount;
    if(writeIdx >= buffer->buffer_size)
        writeIdx -= buffer->buffer_size;
    buffer->writeIdx = writeIdx;
        
}
 
 
/**
****************************************************************************************
* @brief        function used to calculate available items and return address of first item
*
* @param[in]    buffer              (buffer to pull from)
* @param[out]   rddata              (pulled data)
* @param[in]    read_amount         (maximum items to pull)
*
* @return       amount of available items
****************************************************************************************
*/

uint16_t user_buffer_read_address(RingBuffer* buffer, uint8_t** rddata, uint16_t read_amount)
{
    int itemsAvail;
    // Critical section
    disable_irqs();

    if (buffer->writeIdx >= buffer->readIdx)
    {
        //Buffer limit is set by the write pointer
        itemsAvail = buffer->writeIdx - buffer->readIdx;
    }
    else
    {
        //Buffer limit is set by the end pointer
        itemsAvail = buffer->endIdx + 1 - buffer->readIdx;
    }
    //Limit requested bytes to available
    read_amount = (read_amount > itemsAvail ? itemsAvail : read_amount);
    //Return read pointer
    *rddata = buffer->data_ptr + buffer->readIdx;
    //Return the amount of items pulled from buffer
    // End of critical section
    enable_irqs();
        
    buffer->nof_read_callback_bytes += read_amount;
    return read_amount;
}



/**
****************************************************************************************
* @brief        Function used release items that were previously pulled with user_buffer_read_address
*
* @param[in]    buffer              (buffer to pull from)
* @param[in]    read_amount         (items to pull as returned by user_buffer_read_address)
*
* @return       none
****************************************************************************************
*/
void user_buffer_release_items(RingBuffer* buffer, uint16_t read_amount)
{
    // Critical section
    disable_irqs();
    buffer->readIdx += read_amount;
    if(buffer->readIdx >= buffer->endIdx + 1 
            && buffer->readIdx > buffer->writeIdx) //endIdx has a proper value only when writeIdx is lower than readIdx
        buffer->readIdx -= (buffer->endIdx + 1);

    // End of critical section
    enable_irqs();
        
    buffer->nof_release_bytes += read_amount;
}

void user_buffer_release_all_items(RingBuffer* buffer)
{
	disable_irqs();
    buffer->readIdx     = 0;
    buffer->writeIdx    = 0;
	enable_irqs();
}


/**
 ****************************************************************************************
 * @brief           Check if required space is available and return the starting address
 *
 * @param[in]       buffer              (buffer to write to)
 * @param[out]      wrdata              (pointer of initial address)
 * @param[in]       max_write_amount    (required number of items to be written)
 *
 * @return          number of available items that can be written
 ****************************************************************************************
 */
uint16_t user_buffer_write_check (RingBuffer* buffer, uint8_t** wrdata, uint16_t max_write_amount)
{
    uint8_t write_amount;
    int roomLeft;
    // Critical section
    disable_irqs();

    if (max_write_amount == 0)
    {
        *wrdata = NULL;
        return 0;
    }
    if (buffer->readIdx > buffer->writeIdx)
    {
        //buffer does not cycle, it can only reach read pointer - 1
        roomLeft = buffer->readIdx - buffer->writeIdx - 1;
        *wrdata = buffer->data_ptr + buffer->writeIdx;
    }
    else
    {
        roomLeft = buffer->buffer_size - buffer->writeIdx - 1;

        if(max_write_amount > roomLeft)
        {
            //cycle to beggining of buffer and check if space available
            if(buffer->readIdx == 0)
            {
                roomLeft = 0;
                *wrdata = NULL;
            }
            else
            {
                roomLeft = buffer->readIdx - 1;
                *wrdata = buffer->data_ptr;
                buffer->endIdx = buffer->writeIdx - 1;
            }
        }
        else
        {
            *wrdata = buffer->data_ptr + buffer->writeIdx;
            buffer->endIdx = buffer->buffer_size - 1;
        }
    }
    write_amount = (max_write_amount > roomLeft ? roomLeft : max_write_amount);
    // End of critical section
    enable_irqs();
    return write_amount;
}

// peek fifo data but not pop
uint16_t user_buffer_peek(RingBuffer* buffer, uint8_t *rddata, uint16_t read_amount)
{
    int readIdx, writeIdx, endIdx;
    int overflow = 0;
    int itemsAvail;
    // Critical section
    disable_irqs();

    readIdx = buffer->readIdx;
    writeIdx = buffer->writeIdx;
    endIdx = buffer->endIdx;

    // End of critical section
    enable_irqs();

    if (writeIdx >= readIdx)
    {
        itemsAvail = writeIdx - readIdx;
        read_amount = (read_amount > itemsAvail ? itemsAvail : read_amount);
    }
    else
    {
        itemsAvail = endIdx - readIdx + 1 + writeIdx;
        read_amount = (read_amount > itemsAvail ? itemsAvail : read_amount);
        if (readIdx + read_amount > endIdx + 1)
        {
            overflow = read_amount + readIdx - endIdx - 1;
        }
    }
    memcpy(rddata, buffer->data_ptr + readIdx, read_amount - overflow);
    memcpy(rddata + read_amount - overflow, buffer->data_ptr, overflow);

    readIdx += read_amount;
    if(readIdx >= endIdx + 1 && readIdx > writeIdx) //endIdx has a proper value only when writeIdx is lower than readIdx
        readIdx -= (endIdx + 1);
    
    return read_amount; 
}

/**
 ****************************************************************************************
 * @brief       Confirm bytes written in the buffer. Always used with the user_buffer_write_check
 *
 * @param[in]   buffer                  (buffer to write to)
 * @param[in]   actual_write_amount     (actual number of items written)
 *
 * @return      none
 ****************************************************************************************
 */
#if 1
void user_buffer_cfm_write (RingBuffer* buffer, uint16_t actual_write_amount)
{
    // Critical section
    disable_irqs();

    buffer->writeIdx += actual_write_amount;
    if(buffer->writeIdx >= buffer->endIdx + 1)
        buffer->writeIdx -= (buffer->endIdx + 1);

    // End of critical section
    enable_irqs();
}
#else
void user_buffer_cfm_write (RingBuffer* buffer, uint16_t actual_write_amount)
{
    // Critical section
    disable_irqs();
    
    /*   Code without corner cases handling
    buffer->writeIdx += actual_write_amount;
    if(buffer->writeIdx >= buffer->endIdx + 1)
    buffer->writeIdx -= (buffer->endIdx + 1);  
    */  
    
    //---- Ring Buffer corner cases handling ---//
    //===========================================//
    int wrIndex = buffer->writeIdx;
    
    buffer->writeIdx += actual_write_amount;
    if(buffer->writeIdx >= buffer->endIdx + 1)
    {
        buffer->writeIdx -= (buffer->endIdx + 1);
            if( wrIndex == buffer->readIdx)
            buffer->readIdx = 0;
    }       
    //===========================================//
    
    // End of critical section    
    enable_irqs();
}


#endif






/// @} APP
