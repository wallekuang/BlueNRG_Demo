/**
******************************************************************************
* @file    hci_parser.c 
* @author  RF Application Team - AMG
* @version V1.0.0
* @date    August, 2016
* @brief   HCI parser file
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
* <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
******************************************************************************
*/ 

/* Includes ------------------------------------------------------------------*/
#include "bluenrg1_types.h"
#include "osal.h"
#include "hci_parser.h"
#include "SDK_EVAL_Config.h"
#include "hci.h"


/* Added for UART version only */
#define HCI_PACKET_SIZE         512
static uint8_t hci_buffer[HCI_PACKET_SIZE];
uint8_t buffer_out[HCI_PACKET_SIZE];
static volatile uint16_t hci_pckt_len = 0;

static void packet_received(void);

void hci_input_event(uint8_t *buff, uint16_t len)
{
  static hci_state state = WAITING_TYPE;
    
  static uint16_t collected_payload_len = 0;
  static uint16_t payload_len;
  hci_acl_hdr *acl_hdr;
  uint8_t byte;
  uint8_t i = 0;
        
  if(state == WAITING_TYPE)
    hci_pckt_len = 0;
    
  while(hci_pckt_len < HCI_PACKET_SIZE && i++ < len){
        
    byte = *buff++;
    hci_buffer[hci_pckt_len++] = byte;        
        
    if(state == WAITING_TYPE){
      /* Only command or vendor packets are accepted. */
      if(byte == HCI_EVENT_PKT) { //HCI_COMMAND_PKT){
        state = WAITING_EVENT_CODE; //WAITING_OPCODE_1;
      }
      else if(byte == HCI_ACLDATA_PKT){
        state = WAITING_HANDLE;
      }
      else if(byte == HCI_VENDOR_PKT){
        state = WAITING_CMDCODE;
      }
      else{
        /* Incorrect type. Reset state machine. */
        state = WAITING_TYPE;
      }
    }
    else if(state == WAITING_EVENT_CODE) {
      state = WAITING_PARAM_LEN;
    }
    else if(state == WAITING_CMDCODE){
      state = WAITING_CMD_LEN1;
    }
    else if(state == WAITING_CMD_LEN1){
      payload_len = byte;
      state = WAITING_CMD_LEN2;
    }
    else if(state == WAITING_CMD_LEN2){
      payload_len += byte << 8;
      collected_payload_len = 0;
      if(payload_len == 0){
        state = WAITING_TYPE;
        packet_received();                
      }
      else {
        state = WAITING_PAYLOAD;
      }
    }
    else if(state == WAITING_PARAM_LEN){
      payload_len = byte;
      collected_payload_len = 0;
      if(payload_len == 0){
        state = WAITING_TYPE;
        packet_received();
      }
      else {
        state = WAITING_PAYLOAD;
      }
    }
    /*** State transitions for ACL packets ***/
    else if(state == WAITING_HANDLE){
      state = WAITING_HANDLE_FLAG;
    }
    else if(state == WAITING_HANDLE_FLAG){
      state = WAITING_DATA_LEN1;
    }
    else if(state == WAITING_DATA_LEN1){
      state = WAITING_DATA_LEN2;
    }
    else if(state == WAITING_DATA_LEN2){
      acl_hdr = (void *)&hci_buffer[1];
      payload_len = acl_hdr->dlen;
      collected_payload_len = 0;
      if(payload_len == 0){
        state = WAITING_TYPE;
        packet_received();
      }
      else{
        state = WAITING_PAYLOAD;
      }
    }
    /*****************************************/
    else if(state == WAITING_PAYLOAD){
      collected_payload_len += 1;
      if(collected_payload_len >= payload_len){
        /* Reset state machine. */
        state = WAITING_TYPE;                
        packet_received();
      }
    }
  }
}


void packet_received(void)
{ 
  tHciDataPacket * hciReadPacketParser = NULL;

  /* enqueueing a packet for read */
  list_remove_head (&hciReadPktPool, (tListNode **)&hciReadPacketParser);
  
  if(hci_pckt_len > 0){                    
    hciReadPacketParser->data_len = hci_pckt_len;
    Osal_MemCpy(hciReadPacketParser->dataBuff, hci_buffer, hci_pckt_len);
      
    if(HCI_verify(hciReadPacketParser) == 0)
      list_insert_tail(&hciReadPktRxQueue, (tListNode *)hciReadPacketParser);
    else
      list_insert_head(&hciReadPktPool, (tListNode *)hciReadPacketParser);          
  }
  
}

/******************* (C) COPYRIGHT 2016 STMicroelectronics *****END OF FILE****/