
#ifndef _GATT_DB_H_
#define _GATT_DB_H_

#include "lsm6ds3.h"

#define X_OFFSET 200
#define Y_OFFSET 50
#define Z_OFFSET 1000

/** 
 * @brief Number of application services
 */
#define NUMBER_OF_APPLICATION_SERVICES (2)
   
/** 
 * @brief Structure containing acceleration value of each axis.
 */
typedef struct {
  int32_t AXIS_X;
  int32_t AXIS_Y;
  int32_t AXIS_Z;
} AxesRaw_t;

enum {
  ACCELERATION_SERVICE_INDEX = 0,  
  ENVIRONMENTAL_SERVICE_INDEX = 1   
};

IMU_6AXES_StatusTypeDef GetAccAxesRaw (AxesRaw_t * acceleration_data, AxesRaw_t * gyro_data);
tBleStatus Add_HWServW2ST_Service(void);
void Read_Request_CB(uint16_t handle);
tBleStatus Acc_Update(AxesRaw_t *x_axes, AxesRaw_t *g_axes);
tBleStatus BlueMS_Environmental_Update(int32_t press, int16_t temp);

extern uint8_t Services_Max_Attribute_Records[];




#endif /* _GATT_DB_H_ */
