
#ifndef _GATT_DB_H_
#define _GATT_DB_H_


#define SENSOR_TIMER_NUMBER      1

#define READ_TEMPERATURE_TIMEOUT 3000 // 3 sec.

/* Master context */
typedef struct masterRoleContextS {
  uint8_t startDeviceDisc;
  uint16_t connHandle;
  uint8_t peer_addr_type;
  uint8_t peer_addr[6];
  uint8_t numPrimarySer;
  uint8_t primarySer[100];
  uint8_t findCharacOfService;
  uint8_t genAttFlag;
  uint8_t numCharacGenAttSer;
  uint8_t characGenAttSer[10];
  uint8_t accFlag;
  uint8_t numCharacAccSer;
  uint8_t characAccSer[100];
  uint8_t envFlag;
  uint8_t numCharacEnv;
  uint8_t characEnvSer[100];
  uint8_t enableNotif;
  uint16_t freeFallHandle;
  uint16_t accHandle;
  uint16_t tempHandle;
  uint16_t tempVal_length;
  uint8_t tempValue[2];
} masterRoleContextType;

extern masterRoleContextType masterContext;

uint8_t deviceInit(void);
uint8_t deviceDiscovery(void);
uint8_t deviceConnection(void);
void findCharcOfService(void);
void enableSensorNotifications(void);
void readTemperature(void);

#endif /* _GATT_DB_H_ */
