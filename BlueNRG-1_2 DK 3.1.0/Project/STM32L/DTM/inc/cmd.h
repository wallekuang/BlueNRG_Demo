#ifndef _CMD_H_
#define _CMD_H_

#include <stdint.h>

/* DTM mode codes */
#define DTM_MODE_DTM_SPI        0x02
#define DTM_MODE_DTM_UART       0x01
#define DTM_MODE_UART           0x00


/* Firmware version */
#define FW_VERSION_MAJOR    1
#define FW_VERSION_MINOR    5


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

  

uint16_t parse_cmd(uint8_t *hci_buffer, uint16_t hci_pckt_len, uint8_t *buffer_out);

#endif /* _CMD_H_ */
