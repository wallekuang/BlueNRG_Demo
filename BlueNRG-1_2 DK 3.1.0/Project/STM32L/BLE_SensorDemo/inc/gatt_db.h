
#ifndef _GATT_DB_H_
#define _GATT_DB_H_

#include "ble_status.h"
#include "bluenrg1_types.h"

#define X_OFFSET 200
#define Y_OFFSET 50
#define Z_OFFSET 1000

/** 
 * @brief Structure containing acceleration value of each axis.
 */
typedef struct {
  int32_t AXIS_X;
  int32_t AXIS_Y;
  int32_t AXIS_Z;
} AxesRaw_t;

uint8_t GetAccAxesRaw (AxesRaw_t * emulated_acceleration_data);
void GetFreeFallStatus(void);

tBleStatus Add_Acc_Service(void);
void Read_Request_CB(uint16_t handle);
tBleStatus Free_Fall_Notify(void);
tBleStatus Acc_Update(AxesRaw_t *data);

tBleStatus Add_Environmental_Sensor_Service(void);

extern volatile uint8_t request_free_fall_notify;

#endif /* _GATT_DB_H_ */
