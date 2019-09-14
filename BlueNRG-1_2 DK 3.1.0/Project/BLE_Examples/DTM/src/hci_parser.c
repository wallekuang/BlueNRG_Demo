/**
******************************************************************************
* @file    hci_parser.c 
* @author  VMA RF Application Team
  * @version V1.1.0
  * @date    April-2018
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

#include "BlueNRG1_conf.h"
#include "ble_const.h"
#include "bluenrg1_api.h"
#include "bluenrg1_events.h"
#include "bluenrg1_stack.h"
#include "hci_parser.h"
#include "hw_config.h"
#include "cmd.h"
#include "transport_layer.h"
#include "osal.h"


/* Private macro -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#ifndef DTM_DEBUG
/* Macro for debug purposes: not relevant x application */
#define DTM_DEBUG 0
#endif 

#define HCI_PACKET_SIZE 259 // Maximum size of HCI packets are 255 bytes + the HCI header (3 bytes) + 1 byte for transport layer.
#define HCI_ACL_HDR_SIZE 	4
#define HCI_HDR_SIZE 1

/* Private typedef -----------------------------------------------------------*/
typedef PACKED(struct) _hci_acl_hdr{
  uint16_t	handle;		/* Handle & Flags(PB, BC) */
  uint16_t	dlen;
} hci_acl_hdr;

/* Private variables ---------------------------------------------------------*/
static uint8_t hci_buffer[HCI_PACKET_SIZE];
static volatile uint16_t hci_pckt_len = 0;

uint8_t buffer_out[HCI_PACKET_SIZE];
uint16_t buffer_out_len=0;

/* Private function prototypes -----------------------------------------------*/
void packet_received(void);

hci_state hci_input(uint8_t *buff, uint16_t len)
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
      if(byte == HCI_COMMAND_PKT){
        state = WAITING_OPCODE_1;
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
    else if(state == WAITING_OPCODE_1){
      state = WAITING_OPCODE_2;
    }
    else if(state == WAITING_OPCODE_2){
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
      acl_hdr = (void *)&hci_buffer[HCI_HDR_SIZE];
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
  return state;
}

void packet_received(void)
{ 
  switch(hci_buffer[HCI_TYPE_OFFSET]) {
  case HCI_VENDOR_PKT: /* In SPI mode never gets HCI_VENDOR_PKT */
    buffer_out_len = parse_cmd(hci_buffer, hci_pckt_len, buffer_out);
    send_event(buffer_out, buffer_out_len, 1);
    break;
  case HCI_ACLDATA_PKT:
#if defined(LL_ONLY) || (DTM_DEBUG==1)
    {
      uint16_t connHandle;
      uint16_t dataLen;
      uint8_t* pduData;
      uint8_t  pb_flag;
      uint8_t  bc_flag;
      
      connHandle = ((hci_buffer[2] & 0x0F) << 8) + hci_buffer[1];
      dataLen = (hci_buffer[4] << 8) + hci_buffer[3];
      pduData = hci_buffer+5;
      pb_flag = (hci_buffer[2] >> 4) & 0x3;
      bc_flag = (hci_buffer[2] >> 6) & 0x3;
      hci_tx_acl_data(connHandle, pb_flag, bc_flag, dataLen, pduData);
    }
#endif
    break;
  case HCI_COMMAND_PKT:
    send_command(&hci_buffer[1], hci_pckt_len-1);
    break;
  default:
    // Error case not allowed ???
    break;
  }
}

#ifdef LL_ONLY
tBleStatus hci_rx_acl_data_event(uint16_t connHandle, uint8_t  pb_flag, uint8_t  bc_flag, uint16_t  dataLen, uint8_t*  pduData)
{
  uint8_t buffer_out[251+5];
  
  buffer_out[0] = 0x02;
  buffer_out[1] = connHandle & 0xFF;
  buffer_out[2] = (connHandle >> 8 & 0x0F) | (pb_flag << 4) | (bc_flag << 6) ;
  Osal_MemCpy(buffer_out+3,&dataLen, 2);
  Osal_MemCpy(buffer_out+5, pduData, dataLen);
  send_event_isr(buffer_out, dataLen+2+2+1, -1);
  return 0; // ???
}
#endif
