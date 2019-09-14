
#include <stdio.h>
#include <string.h>
#include "ble_const.h"
#include "bluenrg1_stack.h"
#include "osal.h"
#include "app_state.h"
#include "SDK_EVAL_Config.h"
#include "clock.h"
#include "throughput.h"

uint16_t ServHandle, TXCharHandle, RXCharHandle;

/*
 * UUIDs:
 * D973F2E0-B19E-11E2-9E96-0800200C9A66
 * D973F2E1-B19E-11E2-9E96-0800200C9A66
 * D973F2E2-B19E-11E2-9E96-0800200C9A66
 */

const uint8_t serv_uuid[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe0,0xf2,0x73,0xd9};
const uint8_t charUuidTX[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe1,0xf2,0x73,0xd9};
const uint8_t charUuidRX[16] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe2,0xf2,0x73,0xd9};

/* UUIDs */
Service_UUID_t service_uuid;
Char_UUID_t char_uuid;

/*******************************************************************************
 * Function Name  : Add_Throughput_Service
 * Description    : Add the 'Throughput' service.
 * Input          : None
 * Return         : Status.
 *******************************************************************************/
uint8_t Add_Throughput_Service(void)
{
    uint8_t ret;

    Osal_MemCpy(&service_uuid.Service_UUID_128, serv_uuid, 16);
    ret = aci_gatt_add_service(UUID_TYPE_128, &service_uuid, PRIMARY_SERVICE, 6, &ServHandle);
    if (ret != BLE_STATUS_SUCCESS)
    {
        goto fail;
    }

    Osal_MemCpy(&char_uuid.Char_UUID_128, charUuidTX, 16);
    ret = aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, CHAR_SIZE,
                            CHAR_PROP_NOTIFY | CHAR_PROP_INDICATE, ATTR_PERMISSION_NONE, 0,
                            16, 1, &TXCharHandle);
    if (ret != BLE_STATUS_SUCCESS)
    {
        goto fail;
    }

    Osal_MemCpy(&char_uuid.Char_UUID_128, charUuidRX, 16);
    ret = aci_gatt_add_char(ServHandle, UUID_TYPE_128, &char_uuid, CHAR_SIZE,
                            CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RESP,
                            ATTR_PERMISSION_NONE, GATT_NOTIFY_ATTRIBUTE_WRITE,
                            16, 1, &RXCharHandle);
    if (ret != BLE_STATUS_SUCCESS)
    {
        goto fail;
    }

    printf("Chat Service added.\nTX Char Handle %04X, RX Char Handle %04X\n", TXCharHandle, RXCharHandle);
    return BLE_STATUS_SUCCESS;

fail:
    printf("Error while adding Chat service.\n");
    return BLE_STATUS_ERROR;
}
