/**
  ******************************************************************************
  * @file    hci_parser.h 
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
#ifndef HCI_H_
#define HCI_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "compiler.h"
/* Exported macro ------------------------------------------------------------*/

/* HCI Packet types */
#define HCI_COMMAND_PKT		0x01
#define HCI_ACLDATA_PKT		0x02
#define HCI_SCODATA_PKT		0x03
#define HCI_EVENT_PKT		0x04
#define HCI_VENDOR_PKT		0xFF

#define HCI_TYPE_OFFSET                 0
#define HCI_VENDOR_CMDCODE_OFFSET       1
#define HCI_VENDOR_LEN_OFFSET           2
#define HCI_VENDOR_PARAM_OFFSET         4
/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

typedef enum {
  WAITING_TYPE,
  WAITING_OPCODE_1,
  WAITING_OPCODE_2,
  WAITING_CMDCODE,
  WAITING_CMD_LEN1,
  WAITING_CMD_LEN2,
  WAITING_PARAM_LEN,
  WAITING_HANDLE,
  WAITING_HANDLE_FLAG,
  WAITING_DATA_LEN1,
  WAITING_DATA_LEN2,
  WAITING_PAYLOAD
}hci_state;

/* Exported functions ------------------------------------------------------- */

/* HCI library functions. */
extern hci_state hci_input(uint8_t *buff, uint16_t len);

extern uint8_t buffer_out[];
extern uint16_t buffer_out_len;

#endif /* HCI_H_ */
