
#ifndef _CMD_H_
#define _CMD_H_

#include <stdint.h>

#define UNPACK_2_BYTE_PARAMETER(ptr)  \
                (uint16_t)((uint16_t)(*((uint8_t *)ptr))) |   \
                (uint16_t)((((uint16_t)(*((uint8_t *)ptr + 1))) << 8))


#define UNPACK_3_BYTE_PARAMETER(ptr)  \
                (uint32_t)(((uint32)(*((uint8_t *)ptr))) << 16)    |   \
                (uint32_t)(((uint32)(*((uint8_t *)ptr + 1))) << 8) |   \
                (uint32_t)(((uint32)(*((uint8_t *)ptr + 2))))


#define UNPACK_4_BYTE_PARAMETER(ptr)  \
                (uint32_t)(((uint32)(*((uint8_t *)ptr))) << 24)     |   \
                (uint32_t)(((uint32)(*((uint8_t *)ptr + 1))) << 16) |   \
                (uint32_t)(((uint32)(*((uint8_t *)ptr + 2))) << 8)  |   \
                (uint32_t)(((uint32)(*((uint8_t *)ptr + 3))))


#define PACK_2_BYTE_PARAMETER(ptr, param)  do{\
                *((uint8_t *)ptr) = (uint8_t)(param);   \
                *((uint8_t *)ptr+1) = (uint8_t)(param)>>8; \
                }while(0)

#define PACK_3_BYTE_PARAMETER(ptr, param)  do{\
                *((uint8_t *)ptr) = (uint8_t)(param);   \
                *((uint8_t *)ptr+1) = (uint8_t)(param)>>8; \
                *((uint8_t *)ptr+2) = (uint8_t)(param)>>16; \
                }while(0)

#define PACK_4_BYTE_PARAMETER(ptr, param)  do{\
                *((uint8_t *)ptr) = (uint8_t)(param);   \
                *((uint8_t *)ptr+1) = (uint8_t)(param)>>8; \
                *((uint8_t *)ptr+2) = (uint8_t)(param)>>16; \
                *((uint8_t *)ptr+3) = (uint8_t)(param)>>24; \
                }while(0)

#define USB2SERIAL_FW_VERSION_MAJOR    1
#define USB2SERIAL_FW_VERSION_MINOR    6
                  
                  
#define DTM_FW_VERSION_MAJOR    3 //DK 3.0.0 --> 3.0
#define DTM_FW_VERSION_MINOR    1 //DK 3.1.0 --> 3.1
#define DTM_FW_VERSION_PATCH    0 

#ifdef UART_INTERFACE                  
#define DTM_VARIANT             1
#endif
#ifdef SPI_INTERFACE
#define DTM_VARIANT             2
#endif
                  
/* Commands */
#define READ_VERSION            0x01
#define READ_EEPROM             0x02
#define WRITE_EEPROM            0x03
#define BLUENRG_RESET           0x04
#define HW_BOOTLOADER           0x05

uint16_t parse_cmd(uint8_t *hci_buffer, uint16_t hci_pckt_len, uint8_t *buffer_out);

#endif /* _CMD_H_ */
