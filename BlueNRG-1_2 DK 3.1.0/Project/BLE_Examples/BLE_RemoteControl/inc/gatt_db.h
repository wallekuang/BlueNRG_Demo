

#ifndef _GATT_DB_H_
#define _GATT_DB_H_

tBleStatus Add_RC_Service(void);
void Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data);

#endif /* _GATT_DB_H_ */
