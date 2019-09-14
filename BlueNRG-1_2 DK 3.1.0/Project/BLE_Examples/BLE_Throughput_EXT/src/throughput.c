/******************** (C) COPYRIGHT 2018 STMicroelectronics ********************
 * File Name          : throughput.c
 * Author             : AMS - RF  Application team
 * Version            : V2.0.0
 * Date               : 09-February-2018
 * Description        : This file handles bytes received from USB and the init
 *                      function.
 ********************************************************************************
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
 * AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
 * CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
 * INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "gp_timer.h"
#include "ble_const.h"
#include "bluenrg1_stack.h"
#include "app_state.h"
#include "throughput.h"
#include "osal.h"
#include "gatt_db.h"
#include "SDK_EVAL_Config.h"

/*
 * External variables --------------------------------------------------------
 * Private typedef -----------------------------------------------------------
 * Private defines -----------------------------------------------------------
 */

#define COMMAND_SET(n)              (command |= (1 << n))
#define COMMAND_CLEAR(n)            (command &= ~(1 << n))
#define COMMAND_IS_SET(n)           (command & (1 << n))
#define COMMAND_RESET()             (command = 0)
#define SEND_NOTIFICATION           (0x01)
#define SEND_INDICATION             (0x02)
#if SERVER
#define DEVICE_NAME                 "SERVER"
#else
#define DEVICE_NAME                 "CLIENT"
#endif
#define PRINT_APP_FLAG_FORMAT(f)    (APP_FLAG(f) ? "ON" : ". ")
#define MAX_DESCRIPTION_LEN         (100)
#define CMD_TABLE_SIZE              (sizeof(command_table) / sizeof(command_table[0]))

#define TX_OCTETS_TO_TIME(OCTET)    ((OCTET + 14) * 8)

enum {
    CMD_DATA_LEN_UPDATE_REQ_LOW = 0,
    CMD_DATA_LEN_UPDATE_REQ_MID,
    CMD_DATA_LEN_UPDATE_REQ_HIGH,
    CMD_ATT_MTU,
    CMD_BIDIRECT,
    CMD_CONN_UPDATE,
    CMD_SEND_NOTIFICATION,
    CMD_SEND_INDICATION,
    CMD_PRINT_FLAGS,
    CMD_HELP,
    CMD_COUNT,
#if(SERVER)
    CMD_FLUSH,
#endif
};

typedef struct _cmd_t {
    char cmd;
    char description[MAX_DESCRIPTION_LEN];
    uint32_t command_flag;
} cmd_t;

typedef PACKED(struct) _header_t { 
    uint32_t sn;
    uint32_t crc;
} header_t;

typedef PACKED(struct) _pckt_t {
    header_t header;
    uint8_t data[];
} pckt_t;

static cmd_t command_table[] = {
    {
        .cmd = 'u',
        .description = "Send data len update request for 27 bytes",
        .command_flag = CMD_DATA_LEN_UPDATE_REQ_LOW,
    },
    {
        .cmd = 'm',
        .description = "Send data len update request for 100 bytes",
        .command_flag = CMD_DATA_LEN_UPDATE_REQ_MID,
    },
    {
        .cmd = 'U',
        .description = "Send data len update request for 251 bytes",
        .command_flag = CMD_DATA_LEN_UPDATE_REQ_HIGH,
    },
    {
        .cmd = 'a',
        .description = "Send ATT_MTU exchange",
        .command_flag = CMD_ATT_MTU,
    },
#if CLIENT
    {
        .cmd = 'b',
        .description = "Switch bidirectional test on-off",
        .command_flag = CMD_BIDIRECT
    },
    {
        .cmd = 'n',
        .description = "Send notifications",
        .command_flag = CMD_SEND_NOTIFICATION,
    },
    {
        .cmd = 'i',
        .description = "Send indication",
        .command_flag = CMD_SEND_INDICATION,
    },
#else
    {
        .cmd = 'c',
        .description = "Send connection parameter update request",
        .command_flag = CMD_CONN_UPDATE,
    },
#endif
#if (SERVER)
    {
        .cmd = 'f',
        .description = "Enable/disable flushable PDUs",
        .command_flag = CMD_FLUSH,
    },
#endif
    {
        .cmd = 'p',
        .description = "Print APP flags",
        .command_flag = CMD_PRINT_FLAGS,
    },
    {
        .cmd = '?',
        .description = "Print help",
        .command_flag = CMD_HELP,
    },
};

typedef struct _app_context_t {
    tClockTime start_time;
    tClockTime end_time;
    uint8_t not_first_packet;
    uint32_t packets;
    uint32_t sn;
    uint32_t data_size;
    uint32_t lost_pckts;
	uint32_t duplicated_pckts;
    uint32_t corrupted_pckts;
    uint32_t m_buffer_miss;
    float avg_val;
} app_context_t;

static app_context_t rx_app_context;
static app_context_t tx_app_context;

uint8_t connInfo[20];
volatile int app_flags = SET_CONNECTABLE;
volatile uint16_t connection_handle = 0;


extern uint8_t charUuidTX[16];
extern uint8_t charUuidRX[16];
/**
 * @brief  Handle of TX,RX  Characteristics.
 */
#ifdef CLIENT
uint16_t tx_handle;
uint16_t rx_handle;
#else
static uint8_t notifyType = 0x03;
#endif
static uint8_t notOrInd = SEND_NOTIFICATION;

uint16_t att_mtu;
static uint8_t ll_data_len;
uint8_t data[CHAR_SIZE];
static uint32_t command = 0;
static uint32_t g_seed;
static struct timer throughput_timer;
#if defined(SECURE_CONNECTION)
uint8_t force_rebond;
#endif

/* This is CRC-16-CCITT (non-reflected poly, non-inverted input/output).
 * crc16() is only used to bootstrap an initial 256-entry lookup table. */
#define POLY 0x1021
uint16_t crc16(const void *in_data, uint32_t len)
{
    const uint8_t *d = in_data;
    uint16_t crc = d[0];

    for (uint32_t i = 1; i < len; i++)
    {
        crc = crc ^ (d[i] << 8);
        for (int j = 0; j < 8; j++)
        {
            if(crc & 0x8000)
            {
                crc = (crc << 1) ^ POLY;
            }
            else
            {
                crc = (crc << 1);
            }
        }
    }

    return crc;
}

uint32_t fastrand()
{ 
    g_seed = (214013 * g_seed + 2531011);

    return (g_seed >> 16) & 0x7FFF;
}

void print_help()
{
    uint8_t j;

    printf("HELP:\n------------------------------------------\n");
    for (j = 0; j < CMD_TABLE_SIZE; j++)
    {
        printf("\tPress %c to %s\n", command_table[j].cmd, command_table[j].description);
    }
    printf("------------------------------------------\n");
}

void reset_context()
{
    if(APP_FLAG(TIMER_STARTED))
    {
        Timer_Reset(&throughput_timer);
    }
    memset(&rx_app_context, 0, sizeof(app_context_t));
    memset(&tx_app_context, 0, sizeof(app_context_t));
}

void print_app_flags()
{
    printf("\n---------------------------------------------\n");
#if CLIENT
    printf("CLIENT\n");
#else
    printf("SERVER\n");
#endif
    printf("Connection Handle = 0x%04x\n", connection_handle);
    printf("ATT MTU = %d bytes\n", att_mtu);
    printf("TX Link Layer Packet data length = %d bytes\n", ll_data_len);
    printf("TX average throughput = %-4.1f\n", tx_app_context.avg_val);
    printf("RX average throughput = %-4.1f\n", rx_app_context.avg_val);
    printf("Aggregated average throughput = %-4.1f\n", tx_app_context.avg_val + rx_app_context.avg_val);
#if SERVER
    printf("Notification type = %s\n", (notOrInd == SEND_INDICATION) ? "INDICATION" : "NOTIFICATION");
#endif
    printf("APP FLAGS : 0x%04x\n", app_flags);
    printf("Command buffer = 0x%04x\n", command);
    printf("\n---------------------------------------------\n");
    printf("\t %s = CONNECTED\n", PRINT_APP_FLAG_FORMAT(CONNECTED) );
    printf("\t %s = SET_CONNECTABLE\n", PRINT_APP_FLAG_FORMAT(SET_CONNECTABLE));
    printf("\t %s = NOTIFICATIONS_ENABLED\n", PRINT_APP_FLAG_FORMAT(NOTIFICATIONS_ENABLED));
    printf("\t %s = CONN_PARAM_UPD_SENT\n", PRINT_APP_FLAG_FORMAT(CONN_PARAM_UPD_SENT));
#if SERVER
    printf("\t %s = L2CAP_PARAM_UPD_SENT\n", PRINT_APP_FLAG_FORMAT(L2CAP_PARAM_UPD_SENT));
#endif
    printf("\t %s = TX_BUFFER_FULL\n", PRINT_APP_FLAG_FORMAT(TX_BUFFER_FULL));
    printf("\t %s = ATT_MTU_START_EXCHANGE\n", PRINT_APP_FLAG_FORMAT(ATT_MTU_START_EXCHANGE));
    printf("\t %s = ATT_MTU_EXCHANGED\n", PRINT_APP_FLAG_FORMAT(ATT_MTU_EXCHANGED));
    printf("\t %s = START_READ_TX_CHAR_HANDLE\n", PRINT_APP_FLAG_FORMAT(START_READ_TX_CHAR_HANDLE));
    printf("\t %s = END_READ_TX_CHAR_HANDLE\n", PRINT_APP_FLAG_FORMAT(END_READ_TX_CHAR_HANDLE));
    printf("\t %s = START_READ_RX_CHAR_HANDLE\n", PRINT_APP_FLAG_FORMAT(START_READ_RX_CHAR_HANDLE));
    printf("\t %s = END_READ_RX_CHAR_HANDLE\n", PRINT_APP_FLAG_FORMAT(END_READ_RX_CHAR_HANDLE));
    printf("\t %s = SEND_COMMAND\n", PRINT_APP_FLAG_FORMAT(SEND_COMMAND));
    printf("\t %s = INDICATION_PENDING\n", PRINT_APP_FLAG_FORMAT(INDICATION_PENDING));
    printf("\n---------------------------------------------\n");
}

void print_throughput()
{
    float tx_thr, rx_thr;
    tClockTime diff, time_now;

    time_now = Clock_Time();
    if(tx_app_context.data_size != 0)
    {
        diff = tx_app_context.end_time - tx_app_context.start_time;
        tx_thr = tx_app_context.data_size * 8.0 / diff;
        tx_app_context.start_time = tx_app_context.end_time = time_now;
        tx_app_context.data_size = 0;
    }
    else
    {
        tx_thr = 0;
    }
    tx_app_context.avg_val = (tx_app_context.avg_val > 0) ? ((0.95 * tx_app_context.avg_val) + (0.05 * tx_thr)) : tx_thr;

    if(rx_app_context.data_size != 0)
    {
        diff = rx_app_context.end_time - rx_app_context.start_time;
        rx_thr = rx_app_context.data_size * 8.0 / diff;
        rx_app_context.start_time = rx_app_context.end_time = time_now;
        rx_app_context.data_size = 0;
    }
    else
    {
        rx_thr = 0;
    }
    rx_app_context.avg_val = (rx_app_context.avg_val > 0) ? ((0.95 * rx_app_context.avg_val) + (0.05 * rx_thr)) : rx_thr;

    //printf("%-6d sec: TX = %-4.1f kbps. RX = %-4.1f kbps. Corrupted = %-2d Lost = %-2d\n", 
    //printf("%d sec: TX = %.1f kbps. RX = %.1f kbps. Corrupted = %d Lost = %d miss=%d\n",
    printf("%d sec: TX = %.1f kbps, RX = %.1f kbps, CRPT=%d, LOST=%d, DUP=%d\r\n", // MISS=%d\n",
                                    (int)(time_now / 1000),
                                    (float)tx_thr,
                                    (float)rx_thr,
                                    (int)rx_app_context.corrupted_pckts,
                                    (int)rx_app_context.lost_pckts,
				    (int)rx_app_context.duplicated_pckts);
				    //(int)tx_app_context.m_buffer_miss);

    tx_app_context.m_buffer_miss = 0;
}

uint8_t DeviceInit(void)
{
    uint8_t ret;
    uint16_t service_handle;
    uint16_t dev_name_char_handle;
    uint16_t appearance_char_handle;
    uint8_t name[] = {'B', 'l', 'u', 'e', 'N', 'R', 'G', '2'};

#if SERVER
    uint8_t role = GAP_PERIPHERAL_ROLE;
    uint8_t bdaddr[] = {SERVER_ADDRESS};
#else
    uint8_t role = GAP_CENTRAL_ROLE;
    uint8_t bdaddr[] = {CLIENT_ADDRESS};
#endif

    reset_context();
    /* Configure Public address */
    ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN, bdaddr);
    if(ret != BLE_STATUS_SUCCESS)
    {
        printf("%s Setting BD_ADDR failed: 0x%02x\r\n", DEVICE_NAME, ret);
        return ret;
    }

    /* Set the TX power to -2 dBm */
    aci_hal_set_tx_power_level(1, 4);

    /* GATT Init */
    ret = aci_gatt_init();
    if(ret != BLE_STATUS_SUCCESS)
    {
        printf ("%s Error in aci_gatt_init(): 0x%02x\r\n", DEVICE_NAME, ret);
        return ret;
    }
    else
    {
        printf ("%s aci_gatt_init() --> SUCCESS\r\n", DEVICE_NAME);
    }

    /* GAP Init */
    ret = aci_gap_init(role, 0x00, 0x08, &service_handle,
                       &dev_name_char_handle, &appearance_char_handle);
    if(ret != BLE_STATUS_SUCCESS)
    {
        printf ("%s Error in aci_gap_init() 0x%02x\r\n", DEVICE_NAME, ret);
        return ret;
    }
    else
    {
        printf ("%s aci_gap_init() --> SUCCESS\r\n", DEVICE_NAME);
    }

    /* Set the device name */
    ret = aci_gatt_update_char_value_ext(0, service_handle, dev_name_char_handle, 0,sizeof(name), 0, sizeof(name), name);
    if(ret != BLE_STATUS_SUCCESS)
    {
        printf ("%s Error in Gatt Update characteristic value 0x%02x\r\n", DEVICE_NAME, ret);
        return ret;
    }
    else
    {
        printf ("%s aci_gatt_update_char_value_ext() --> SUCCESS\r\n", DEVICE_NAME);
    }

#if defined(SECURE_CONNECTION)
    force_rebond = 0;
    ret = aci_gap_clear_security_db();
    if(ret != BLE_STATUS_SUCCESS)
    {
        printf ("%s Error in aci_gap_clear_security_db() 0x%02x\r\n", DEVICE_NAME, ret);
        return ret;
    }
    else
    {
        printf ("%s aci_gap_clear_security_db() --> SUCCESS\r\n", DEVICE_NAME);
    }

    uint8_t event_mask[8] ={0xDF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    ret = hci_le_set_event_mask(event_mask);
    if(ret != BLE_STATUS_SUCCESS)
    {
        printf ("%s Error in hci_le_set_event_mask() 0x%02x\r\n", DEVICE_NAME, ret);
        return ret;
    }
    else
    {
        printf ("%s hci_le_set_event_mask() --> SUCCESS\r\n", DEVICE_NAME);
    }

    ret = aci_gap_set_io_capability(0x02); //keyboard only
    if(ret != BLE_STATUS_SUCCESS)
    {
        printf ("%s Error in aci_gap_set_io_capability() 0x%02x\r\n", DEVICE_NAME, ret);
        return ret;
    }
    else
    {
        printf ("%s aci_gap_set_io_capability() --> SUCCESS\r\n", DEVICE_NAME);
    }

    uint8_t Bonding_Mode = 0x01;
    uint8_t MITM_Mode = 0x01; //It requires authentication

    /* LE Secure connections:
            0x00: Not supported
            0x01: Supported but optional (i.e. a Legacy Pairing may be accepted)
            0x02: Supported but mandatory (i.e. do not accept Legacy Pairing but only Secure Connections v.4.2 Pairing).
    */
    uint8_t SC_Support = 0x02;
    uint8_t KeyPress_Notification_Support = 0x00;  //keyPress Notification feature Not Supported
    uint8_t Min_Encryption_Key_Size = 0x07;
    uint8_t Max_Encryption_Key_Size = 0x10;
    uint8_t Use_Fixed_Pin = 0x00; //0 means fixed pin!
    uint32_t Fixed_Pin = 123456;
    uint8_t Identity_Address_Type = PUBLIC_ADDR;
    ret = aci_gap_set_authentication_requirement(Bonding_Mode,
                                                 MITM_Mode,
                                                 SC_Support,
                                                 KeyPress_Notification_Support,
                                                 Min_Encryption_Key_Size,
                                                 Max_Encryption_Key_Size,
                                                 Use_Fixed_Pin,
                                                 Fixed_Pin,
                                                 Identity_Address_Type);
    if(ret != BLE_STATUS_SUCCESS)
    {
        printf ("%s Error in aci_gap_set_authentication_requirement() 0x%02x\r\n", DEVICE_NAME, ret);
        return ret;
    }
    else
    {
        printf ("%s aci_gap_set_authentication_requirement() --> SUCCESS\r\n", DEVICE_NAME);
    }
#endif
#if  SERVER
    ret = Add_Throughput_Service();
    if(ret != BLE_STATUS_SUCCESS)
    {
        printf("%s Error in Add_Throughput_Service 0x%02x\r\n", DEVICE_NAME, ret);
        return ret;
    }
    else
    {
        printf("%s Add_Throughput_Service() --> SUCCESS\r\n", DEVICE_NAME);
    }
#endif

    att_mtu = ATT_DEF_SIZE;
    COMMAND_RESET();
    APP_FLAG_RESET();
    APP_FLAG_SET(SET_CONNECTABLE);

    return BLE_STATUS_SUCCESS;
}

tBleStatus Make_Connection(void)
{
    tBleStatus ret;

#if CLIENT
    tBDAddr bdaddr = {SERVER_ADDRESS};

    ret = aci_gap_create_connection(0x4000,
                                    0x4000,
                                    PUBLIC_ADDR,
                                    bdaddr,
                                    PUBLIC_ADDR,
                                    40,
                                    40,
                                    0, 60, 2000, 2000);
    if(ret != BLE_STATUS_SUCCESS)
    {
        printf("%s Error while starting connection: 0x%04x\r\n", DEVICE_NAME, ret);
        Clock_Wait(100);

        return ret;
    }
#else
    uint8_t local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'B','l','u','e','N','R','G','2'};

    /* disable scan response */
    hci_le_set_scan_response_data(0, NULL);
    ret = aci_gap_set_discoverable(ADV_IND,
                                   ADV_INTERVAL_MIN,
                                   ADV_INTERVAL_MAX,
                                   PUBLIC_ADDR,
                                   NO_WHITE_LIST_USE,
                                   sizeof(local_name),
                                   local_name, 0, NULL, 0, 0);
    if(ret != BLE_STATUS_SUCCESS)
    {
        printf ("%s Error in aci_gap_set_discoverable(): 0x%02x\r\n", DEVICE_NAME, ret);

        return ret;
    }
#endif

    return BLE_STATUS_SUCCESS;
}

#if CLIENT
tBleStatus set_notification_type(uint16_t connection_handle, uint16_t tx_handle, uint8_t notification_type)
{
    uint8_t client_char_conf_data[] = {0x00, 0x00};
    tBleStatus ret;

    APP_FLAG_CLEAR(NOTIFICATIONS_ENABLED);
    client_char_conf_data[0] = notification_type;
    ret = aci_gatt_write_char_desc(connection_handle,
                                              tx_handle + 2,
                                              2,
                                              client_char_conf_data);
    if(ret != BLE_STATUS_SUCCESS)
    {
        return ret;
    }
    APP_FLAG_SET(NOTIFICATIONS_ENABLED);
    if(!APP_FLAG(TIMER_STARTED))
    {
        Timer_Set(&throughput_timer, CLOCK_SECOND * THROUGHPUT_TIMER_VAL_SEC);
        APP_FLAG_SET(TIMER_STARTED);
    }

    printf("Subscribed to receive %s\n", (notification_type == SEND_NOTIFICATION) ? "NOTIFICATION" : "INDICATION");
    notOrInd = notification_type;

    return BLE_STATUS_SUCCESS;
}
#endif

void APP_Tick(void)
{
    tBleStatus ret;

    if(APP_FLAG(SET_CONNECTABLE))
    {
        ret = Make_Connection();
        if (ret == BLE_STATUS_SUCCESS)
        {
            APP_FLAG_CLEAR(SET_CONNECTABLE);
        }
    }
#if defined(SECURE_CONNECTION)
#if CLIENT
    else if(APP_FLAG(DEV_PAIRING_REQ))
    {
        ret = aci_gap_send_pairing_req(connection_handle, force_rebond);
        if (ret == BLE_STATUS_SUCCESS)
        {
            printf("Sent aci_gap_send_pairing_req() connection_handle = 0x%04x, force_rebond = 0x%02x\n",
                                                connection_handle, force_rebond);
            APP_FLAG_CLEAR(DEV_PAIRING_REQ);
            force_rebond = 0;
        }
        else
        {
            printf("Fail to call aci_gap_send_pairing_req() ret = 0x%02x\n", ret);
        }
    }
#endif
    else if(APP_FLAG(CONNECTED) && APP_FLAG(DEV_PAIRED))
#else
    else if(APP_FLAG(CONNECTED))
#endif
    {
        if(APP_FLAG(TIMER_STARTED))
        {
            if(Timer_Expired(&throughput_timer))
            {
                Timer_Set(&throughput_timer, CLOCK_SECOND * THROUGHPUT_TIMER_VAL_SEC);
                print_throughput();
            }
        }

        if(APP_FLAG(INDICATION_PENDING))
        {
            ret = aci_gatt_confirm_indication(connection_handle);
            if(ret == BLE_STATUS_SUCCESS)
            {
                APP_FLAG_CLEAR(INDICATION_PENDING);
            }
        }

        if(COMMAND_IS_SET(CMD_PRINT_FLAGS))
        {
            print_app_flags();
            COMMAND_CLEAR(CMD_PRINT_FLAGS);
        }
        else if((COMMAND_IS_SET(CMD_DATA_LEN_UPDATE_REQ_LOW) || COMMAND_IS_SET(CMD_DATA_LEN_UPDATE_REQ_MID) ||
            COMMAND_IS_SET(CMD_DATA_LEN_UPDATE_REQ_HIGH)))
        {
            uint8_t data_len_req;
            if(COMMAND_IS_SET(CMD_DATA_LEN_UPDATE_REQ_LOW))
            {
                data_len_req = LL_DATA_LEN_LOW;
                COMMAND_CLEAR(CMD_DATA_LEN_UPDATE_REQ_LOW);
            }
            else if(COMMAND_IS_SET(CMD_DATA_LEN_UPDATE_REQ_MID))
            {
                data_len_req = LL_DATA_LEN_MID;
                COMMAND_CLEAR(CMD_DATA_LEN_UPDATE_REQ_MID);
            }
            else if(COMMAND_IS_SET(CMD_DATA_LEN_UPDATE_REQ_HIGH))
            {
                data_len_req = LL_DATA_LEN_HIGH;
                COMMAND_CLEAR(CMD_DATA_LEN_UPDATE_REQ_HIGH);
            }
            printf("%s Send data len update request with value %d\n", DEVICE_NAME, data_len_req);
            ret = hci_le_set_data_length(connection_handle, data_len_req, TX_OCTETS_TO_TIME(data_len_req));
            if(ret != BLE_STATUS_SUCCESS)
            {
                printf ("%s Error in hci_le_set_data_length()\n", DEVICE_NAME);
            }
        }
        else if(COMMAND_IS_SET(CMD_ATT_MTU))
        {
            if(!APP_FLAG(ATT_MTU_EXCHANGED) && !APP_FLAG(ATT_MTU_START_EXCHANGE))
            {
                ret = aci_gatt_exchange_config(connection_handle);
                if(ret != BLE_STATUS_SUCCESS)
                {
                    printf ("Error in aci_gatt_exchange_config() ret = 0x%02xr\n", ret);
                }
                else
                {
                    APP_FLAG_SET(ATT_MTU_START_EXCHANGE);
                    COMMAND_CLEAR(CMD_ATT_MTU);
                }
            }
            else
            {
                printf ("Error aci_gatt_exchange_config can be called just one time\n");
                COMMAND_CLEAR(CMD_ATT_MTU);
            }
        }
        else
#if CLIENT
        if(COMMAND_IS_SET(CMD_BIDIRECT))
        {
            if(APP_FLAG(SEND_COMMAND))
            {
                APP_FLAG_CLEAR(SEND_COMMAND);
                printf("Set unidirectional mode\n");
            }
            else
            {
                APP_FLAG_SET(SEND_COMMAND);
                printf("Set bidirectional mode\n");
                if(!APP_FLAG(TIMER_STARTED))
                {
                    Timer_Set(&throughput_timer, CLOCK_SECOND * THROUGHPUT_TIMER_VAL_SEC);
                    APP_FLAG_SET(TIMER_STARTED);
                }
            }
            COMMAND_CLEAR(CMD_BIDIRECT);
        }
        else if(COMMAND_IS_SET(CMD_SEND_INDICATION))
        {
            ret = set_notification_type(connection_handle, tx_handle, SEND_INDICATION);
            if(ret == BLE_STATUS_SUCCESS)
            {
                COMMAND_CLEAR(CMD_SEND_INDICATION);
                printf("Sent Indication subscription\n");
            }
        }
        else if(COMMAND_IS_SET(CMD_SEND_NOTIFICATION))
        {
            ret =  set_notification_type(connection_handle, tx_handle, SEND_NOTIFICATION);
            if(ret == BLE_STATUS_SUCCESS)
            {
                COMMAND_CLEAR(CMD_SEND_NOTIFICATION);
                printf("Sent Notification subscription\n");
            }
        }
        else if(!APP_FLAG(END_READ_TX_CHAR_HANDLE))
        {
            /* Start TX handle Characteristic discovery if not yet done */
            if(!APP_FLAG(START_READ_TX_CHAR_HANDLE))
            {
                /* Discovery TX characteristic handle by UUID 128 bits */
                UUID_t UUID_Tx;
                Osal_MemCpy(&UUID_Tx.UUID_128, charUuidTX, 16);
                ret = aci_gatt_disc_char_by_uuid(connection_handle,
                                                 0x01,
                                                 0xFFFF,
                                                 UUID_TYPE_128, &UUID_Tx);
                if(ret != BLE_STATUS_SUCCESS)
                {
                    printf ("Error in aci_gatt_disc_char_by_uuid() for TX characteristic: 0x%02xr\n", ret);
                }
                else
                {
                    APP_FLAG_SET(START_READ_TX_CHAR_HANDLE);
                }
            }
        }
        /* Start RX handle Characteristic discovery if not yet done */
        else if(!APP_FLAG(END_READ_RX_CHAR_HANDLE))
        {
            /* Discovery RX characteristic handle by UUID 128 bits */
            if(!APP_FLAG(START_READ_RX_CHAR_HANDLE))
            {
                /* Discovery TX characteristic handle by UUID 128 bits */
                UUID_t UUID_Rx;
                Osal_MemCpy(&UUID_Rx.UUID_128, charUuidRX, 16);
                ret = aci_gatt_disc_char_by_uuid(connection_handle,
                                                 0x01,
                                                 0xFFFF,
                                                 UUID_TYPE_128, &UUID_Rx);
                if(ret != BLE_STATUS_SUCCESS)
                {
                    printf ("Error in aci_gatt_disc_char_by_uuid() for RX characteristic: 0x%02xr\n", ret);
                }
                else
                {
                    APP_FLAG_SET(START_READ_RX_CHAR_HANDLE);
                }
            }
        }
        else if(!APP_FLAG(NOTIFICATIONS_ENABLED))
        {
            /**
             * Subscribe to receive notification
             */
            set_notification_type(connection_handle, tx_handle, SEND_NOTIFICATION);
        }
        else if(APP_FLAG(SEND_COMMAND))
        {
            if (!APP_FLAG(TX_BUFFER_FULL))
            {
                uint8_t i;
                uint16_t *v, data_len;
                pckt_t *pckt;

                pckt = (pckt_t *)data;
                pckt->header.sn = tx_app_context.packets;
                v = (uint16_t *)pckt->data;
                data_len = att_mtu - 3 - sizeof(header_t);
                for (i = 0; i < (data_len/ 2); i++)
                {
                    v[i] = (uint16_t)(fastrand() & 0xFFFF);
                    //v[i] = i;
                }
                pckt->header.crc = crc16(pckt->data, data_len);


                tBleStatus ret = aci_gatt_write_without_resp(connection_handle,
                                                             rx_handle + 1,
                                                             data_len + sizeof(header_t),
                                                             data);  
                if(ret == BLE_STATUS_SUCCESS)
                {
                  if(tx_app_context.data_size == 0)
                  {
                    tx_app_context.start_time = Clock_Time();
                  }
                  tx_app_context.packets++;
                  tx_app_context.data_size += (att_mtu - 3);
                  tx_app_context.end_time = Clock_Time();
                }                
                else if(ret == BLE_STATUS_INSUFFICIENT_RESOURCES)
                {
                  /* Radio is busy (buffer full). */
                  APP_FLAG_SET(TX_BUFFER_FULL);
                  tx_app_context.m_buffer_miss++;
                } 
                else
                {
                  printf("Error 0x%02X\r\n", ret);
                }
            }
            else
            {
              tx_app_context.m_buffer_miss++;
            }
        }
#else  //SERVER
        if(COMMAND_IS_SET(CMD_CONN_UPDATE) && !APP_FLAG(L2CAP_PARAM_UPD_SENT))
        {
            printf("Send connection parameter update\n");
            ret = aci_l2cap_connection_parameter_update_req(connection_handle,
                                                            8, 16, 0, 600);
            if(ret != BLE_STATUS_SUCCESS)
            {
                printf ("Error in aci_l2cap_connection_parameter_update_req(): 0x%02xr\n", ret);
            }
            else
            {
                APP_FLAG_SET(L2CAP_PARAM_UPD_SENT);
                COMMAND_CLEAR(CMD_CONN_UPDATE);
            }
        }
        else if(COMMAND_IS_SET(CMD_FLUSH))
        {
          if (notifyType == 0x03)
          {
            printf ("Enabling flushable notifications\n");
            notifyType = 0x07;
          }
            else
          {
            printf ("Disabling flushable notifications\n");
            notifyType = 0x03;
          }
          COMMAND_CLEAR(CMD_FLUSH);
        }
        else if(APP_FLAG(NOTIFICATIONS_ENABLED) && !((notOrInd == SEND_INDICATION) && APP_FLAG(INDICATION_PENDING)))
        {
            if (!APP_FLAG(TX_BUFFER_FULL))
            {
                uint8_t i;
                uint16_t *v, data_len;
                pckt_t *pckt;

                pckt = (pckt_t *)data;
                pckt->header.sn = tx_app_context.packets;
                v = (uint16_t *)pckt->data;
                data_len = att_mtu - 3 - sizeof(header_t);
                for (i = 0; i < (data_len / 2); i++)
                {
                    v[i] = (uint16_t)(fastrand() & 0xFFFF);
                    //v[i] = i;
                }
                pckt->header.crc = crc16(pckt->data, data_len);

                ret = aci_gatt_update_char_value_ext(connection_handle,
                                                     ServHandle,
                                                     TXCharHandle,
                                                     notifyType,
                                                     CHAR_SIZE,
                                                     0,
                                                     data_len + sizeof(header_t),
                                                     data);
                if(ret == BLE_STATUS_INSUFFICIENT_RESOURCES)
                {
                    /* Radio is busy (buffer full). */
                    APP_FLAG_SET(TX_BUFFER_FULL);
                    tx_app_context.m_buffer_miss++;
                }
                else if(ret == BLE_STATUS_SUCCESS)
                {
                    if(tx_app_context.data_size == 0)
                    {
                        tx_app_context.start_time = Clock_Time();
                    }
                    if(notOrInd == SEND_INDICATION)
                    {
                        APP_FLAG_SET(INDICATION_PENDING);
                    }
                    tx_app_context.data_size += (att_mtu - 3);
                    tx_app_context.end_time = Clock_Time();
                    tx_app_context.packets++;
                }
            }
            else
            {
                tx_app_context.m_buffer_miss++;
            }
        }
#endif
    }
}

void Process_InputData(uint8_t* data_buffer, uint16_t Nb_bytes)
{
    uint8_t i, j;

    for (i = 0; i < Nb_bytes; i++)
    {
        for (j = 0; j < CMD_TABLE_SIZE; j++)
        {
            if(data_buffer[i] == command_table[j].cmd)
            {
                COMMAND_SET(command_table[j].command_flag);
                break;
            }
        }
        if((j == CMD_TABLE_SIZE) || COMMAND_IS_SET(CMD_HELP))
        {
            print_help();
            COMMAND_CLEAR(CMD_HELP);
        }
       // printf("CMD_TABLE_SIZE = %d received data_buffer[%d] = %c Nb_bytes = %d\n", CMD_TABLE_SIZE, i, data_buffer[i], Nb_bytes);
    }
}

void hci_le_connection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Role,
                                      uint8_t Peer_Address_Type,
                                      uint8_t Peer_Address[6],
                                      uint16_t Conn_Interval,
                                      uint16_t Conn_Latency,
                                      uint16_t Supervision_Timeout,
                                      uint8_t Master_Clock_Accuracy)

{
    connection_handle = Connection_Handle;
    att_mtu = ATT_DEF_SIZE;
    ll_data_len = LL_DEFAULT_DATA_LEN;
    printf("Device is connected with Connection_Handle = 0x%04x\n", Connection_Handle);
    APP_FLAG_SET(CONNECTED);
#if defined(SECURE_CONNECTION) && CLIENT
    APP_FLAG_SET(DEV_PAIRING_REQ);
#endif
    APP_FLAG_CLEAR(L2CAP_PARAM_UPD_SENT);
}

void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason)
{
    printf("Device disconnected from Connection_Handle = 0x%04x, Reason = 0x%02x\r\n", Connection_Handle, Reason);

    reset_context();
    APP_FLAG_RESET();
    COMMAND_RESET();

    /* Make the device connectable again. */
    APP_FLAG_SET(SET_CONNECTABLE);
}

void handle_value_update(uint16_t Attribute_Handle,
                         uint8_t Attribute_Value_Length,
                         uint8_t Attribute_Value[])
{
    uint16_t crc;
    pckt_t *pckt;
    uint32_t sn;
    tClockTime time_now;

    time_now = Clock_Time();
    pckt = (pckt_t *)&Attribute_Value[0];
    sn = pckt->header.sn;

    if(rx_app_context.not_first_packet == 0)
    {
        rx_app_context.start_time = time_now;
        rx_app_context.sn = (uint32_t)(sn - 1);
        rx_app_context.not_first_packet = 1;
    }


    if(sn == rx_app_context.sn)
    {
        printf("Duplicated 0x04%x !!!\n", sn);
        rx_app_context.duplicated_pckts++;
    }
    else if(sn != (rx_app_context.sn + 1))
    {
        if(sn <  rx_app_context.sn)
        {
            rx_app_context.duplicated_pckts++;
        }
        else
        {
            rx_app_context.lost_pckts += sn - (rx_app_context.sn + 1);
        }
    }

    crc = crc16(pckt->data, Attribute_Value_Length - sizeof(header_t));
    if(crc != pckt->header.crc)
    {
        rx_app_context.corrupted_pckts++;
    }
    rx_app_context.sn = sn;
    rx_app_context.data_size += Attribute_Value_Length;
    rx_app_context.end_time = Clock_Time();
}

void aci_gatt_attribute_modified_event(uint16_t Connection_Handle,
                                       uint16_t handle,
                                       uint16_t Offset,
                                       uint16_t data_length,
                                       uint8_t att_data[])
{
#if SERVER
    if(handle == RXCharHandle + 1)
    {
        handle_value_update(handle, data_length, att_data);
    }
    else if(handle == TXCharHandle + 2)
    {
        if(att_data[0] == 0x1)
        {
            APP_FLAG_SET(NOTIFICATIONS_ENABLED);
            notOrInd = SEND_NOTIFICATION;
            printf ("Client has request to receive NOTIFICATION\n");
        }
        else if(att_data[0] == 0x2)
        {
            APP_FLAG_SET(NOTIFICATIONS_ENABLED);
            notOrInd = SEND_INDICATION;
            printf ("Client has request to receive INDICATION\n");
        }
        if(!APP_FLAG(TIMER_STARTED))
        {
            Timer_Set(&throughput_timer, CLOCK_SECOND * THROUGHPUT_TIMER_VAL_SEC);
            APP_FLAG_SET(TIMER_STARTED);
        }
    }
    else
    {
        printf("Unexpected handle: 0x%04d.\n",handle);
    }
#else
    printf("Unexpected data for CLIENT!!!\n\n");
    for(int i = 0; i < data_length; i++)
    {
        printf("%c", att_data[i]);
    }
#endif
}

void hci_le_data_length_change_event(uint16_t Connection_Handle,
                                     uint16_t MaxTxOctets,
                                     uint16_t MaxTxTime,
                                     uint16_t MaxRxOctets,
                                     uint16_t MaxRxTime)
{
    printf("%s hci_le_data_length_change_event ", DEVICE_NAME);
    printf("\nconn_handle = 0x%02x\nMaxTxOctets = %d\nMaxTxTime = %d\nMaxRxOctets = %d\nMaxRxTime = %d\n",
           Connection_Handle, MaxTxOctets, MaxTxTime, MaxRxOctets, MaxRxTime);
    ll_data_len = MaxTxOctets;
}

#if CLIENT
void aci_gatt_notification_event(uint16_t Connection_Handle,
                                 uint16_t Attribute_Handle,
                                 uint8_t Attribute_Value_Length,
                                 uint8_t Attribute_Value[])
{
    if (notOrInd == SEND_NOTIFICATION)
    {
        handle_value_update(Attribute_Handle, Attribute_Value_Length, Attribute_Value);
    }
}

void aci_gatt_indication_event(uint16_t Connection_Handle,
                               uint16_t Attribute_Handle,
                               uint8_t Attribute_Value_Length,
                               uint8_t Attribute_Value[])
{
    tBleStatus ret;

    if (notOrInd == SEND_INDICATION)
    {
        handle_value_update(Attribute_Handle, Attribute_Value_Length, Attribute_Value);
    }
    ret = aci_gatt_confirm_indication(Connection_Handle);
    if(ret != BLE_STATUS_SUCCESS)
    {
        APP_FLAG_SET(INDICATION_PENDING);
    }
}

void aci_gatt_disc_read_char_by_uuid_resp_event(uint16_t Connection_Handle,
                                                uint16_t Attribute_Handle,
                                                uint8_t Attribute_Value_Length,
                                                uint8_t Attribute_Value[])
{
    if(APP_FLAG(START_READ_TX_CHAR_HANDLE) && !APP_FLAG(END_READ_TX_CHAR_HANDLE))
    {
        tx_handle = Attribute_Handle;
        printf("TX Char Handle 0x%04X\n", tx_handle);
    }
    else if(APP_FLAG(START_READ_RX_CHAR_HANDLE) && !APP_FLAG(END_READ_RX_CHAR_HANDLE))
    {
        rx_handle = Attribute_Handle;
        printf("RX Char Handle 0x%04X\n", rx_handle);
    }
}

void aci_gatt_proc_complete_event(uint16_t Connection_Handle,
                                  uint8_t Error_Code)
{
    if(APP_FLAG(START_READ_TX_CHAR_HANDLE) && !APP_FLAG(END_READ_TX_CHAR_HANDLE))
    {
        APP_FLAG_SET(END_READ_TX_CHAR_HANDLE);
    }
    else if(APP_FLAG(START_READ_RX_CHAR_HANDLE) && !APP_FLAG(END_READ_RX_CHAR_HANDLE))
    {
        APP_FLAG_SET(END_READ_RX_CHAR_HANDLE);
    }
}
#else
void aci_gatt_server_confirmation_event(uint16_t Connection_Handle)
{
    APP_FLAG_CLEAR(INDICATION_PENDING);
}

#endif

/*******************************************************************************
 * Function Name  : aci_gatt_tx_pool_available_event.
 * Description    : This event occurs when a TX pool available is received.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_tx_pool_available_event(uint16_t Connection_Handle,
                                      uint16_t Available_Buffers)
{
    /* It allows to notify when at least 2 GATT TX buffers are available */
    APP_FLAG_CLEAR(TX_BUFFER_FULL);
}

void aci_att_exchange_mtu_resp_event(uint16_t Connection_Handle,
                                     uint16_t Att_MTU)
{
    att_mtu = Att_MTU;
    APP_FLAG_SET(ATT_MTU_EXCHANGED);
    printf("ATT mtu exchanged with value = %d\n", Att_MTU);
}

#if defined(SECURE_CONNECTION)
void aci_gap_pairing_complete_event(uint16_t Connection_Handle,
                                    uint8_t Status,
                                    uint8_t Reason)
{
    printf("Received aci_gap_pairing_complete_event with Status = 0x%02X Reason = 0x%02X\n", Status, Reason);
    if (Status == BLE_STATUS_SUCCESS)
    {
        APP_FLAG_SET(DEV_PAIRED);
    }
    else
    {
#if CLIENT
        APP_FLAG_SET(DEV_PAIRING_REQ);
#endif
    }
}

void hci_encryption_change_event(uint8_t Status,
                                 uint16_t Connection_Handle,
                                 uint8_t Encryption_Enabled)
{
    if ((Status != 0) || (Encryption_Enabled == 0))
    {
        APP_FLAG_CLEAR(DEV_PAIRED);
        APP_FLAG_SET(DEV_PAIRING_REQ);
        force_rebond = 1;
        printf("hci_encryption_change_event with Status = 0x%02X, Encryption_Enabled = 0x%02X\n", Status, Encryption_Enabled);
    }
}
#endif
