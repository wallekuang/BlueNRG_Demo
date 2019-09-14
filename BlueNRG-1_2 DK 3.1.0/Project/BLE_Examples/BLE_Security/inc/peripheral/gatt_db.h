

#ifndef _GATT_DB_H_
#define _GATT_DB_H_

/* Store Value into a buffer in Little Endian Format */
#define STORE_VALUE(buf, val)    ( ((buf)[0] =  (uint8_t) (val>>8)    ) , \
                                   ((buf)[1] =  (uint8_t) (val) ) )

tBleStatus Add_Service(void);

extern uint16_t TXCharHandle, RXCharHandle;

uint8_t __io_getcharNonBlocking(uint8_t *data);

#endif /* _GATT_DB_H_ */
