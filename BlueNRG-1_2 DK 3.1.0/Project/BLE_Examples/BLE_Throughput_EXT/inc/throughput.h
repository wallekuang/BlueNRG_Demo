
#ifndef _THROUGHPUT_H_
#define _THROUGHPUT_H_

#define CHAR_SIZE                       (512)
#define ATT_DEF_SIZE                    (23)
#define THROUGHPUT_TIMER_VAL_SEC        (2)
#define LL_DATA_LEN_LOW                 (27)
#define LL_DATA_LEN_MID                 (100)
#define LL_DATA_LEN_HIGH                (251)
#define LL_DEFAULT_DATA_LEN             (LL_DATA_LEN_LOW)
#define ADV_INTERVAL_MIN                (200)     /* ms */
#define ADV_INTERVAL_MAX                (300)     /* ms */
#define CLIENT_ADDRESS                  0xAA, 0x00, 0x03, 0xE1, 0x80, 0x02
#define SERVER_ADDRESS                  0xAD, 0x00, 0x03, 0xE1, 0x80, 0x02

uint8_t DeviceInit(void);
void APP_Tick(void);
void Process_InputData(uint8_t* data_buffer, uint16_t Nb_bytes);
void print_help(void);
#endif // _THROUGHPUT_H_
