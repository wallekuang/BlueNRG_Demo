
#ifndef _GATT_DB_H_
#define _GATT_DB_H_

/** 
  * @brief Central device name
*/
#define MASTER_DEVICE_NAME 'M','A','S','T','E','R','_','S','E','C','U','R','I','T','Y'
#define MASTER_DEVICE_NAME_LEN 15

#define SENSOR_TIMER_NUMBER      1

#define RX_CHAR_TIMEOUT 3000 // 3 sec. 

/* Master context */
typedef struct masterRoleContextS {
  uint8_t startDeviceDisc;
  uint16_t connHandle;
  uint16_t PassKey_connHandle;
  uint8_t peer_addr_type;
  uint8_t peer_addr[6];
  uint8_t numPrimarySer;
  uint8_t primarySer[100];
  uint8_t findCharacOfService;
  uint8_t genAttFlag;
  uint8_t numCharacGenAttSer;
  uint8_t characGenAttSer[10];
  uint8_t start_insert_passkey; 
  uint8_t end_insert_passkey; 
  uint8_t do_terminate_connection;
  uint8_t getPrimaryService;
  uint8_t isconnected; 
  uint8_t read_char; 
  uint8_t ServiceFlag;
  uint8_t numCharacSer;
  uint8_t characSer[100];
  uint8_t enableNotif;
  uint8_t encryption_enabled_OK; 
  uint8_t reconnection; 
  /* BLE Security v4.2 is supported: BLE stack FW version >= 2.x */
  uint8_t start_confirm_numeric_value;
  uint16_t RXHandle;
  uint16_t TXHandle;
  uint16_t tempVal_length;
  uint8_t tempValue[20];
} masterRoleContextType;

extern masterRoleContextType masterContext;

uint8_t deviceInit(void);
uint8_t deviceDiscovery(void);
uint8_t deviceConnection(void);
uint8_t Device_Security(void);
void enableSensorNotifications(void);
void readRXChar(void);
void terminate_connection(void);
void findCharcOfService(void);
void enter_pass_key_SM(void); 
void add_pass_key(void);
uint8_t read_data_handler(void);
/* BLE Security v4.2 is supported: BLE stack FW version >= 2.x */
void Confirm_Numeric_Value_SM(void);
void APP_Tick(void); 
uint8_t __io_getcharNonBlocking(uint8_t *data);
#endif /* _GATT_DB_H_ */
