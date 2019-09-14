
#ifndef _DEVICE_INIT_H_
#define _DEVICE_INIT_

/** 
  * @brief Peripheral security key (default value)
*/
#define PERIPHERAL_SECURITY_KEY 123456 //0x0001E240 

#define MIN_KEY_SIZE 0x07
   
#define MAX_KEY_SIZE 0x10

/* ----------- Security MODES -------------------------------------------------------*/

#define PASSKEY_RANDOM_PIN  0
#define PASSKEY_FIXED_PIN   1
#define JUST_WORKS          2
#define NUMERIC_COMPARISON  3

/* Default security mode */
#ifndef SECURITY_MODE
#define SECURITY_MODE PASSKEY_RANDOM_PIN
#endif 

/* ----------- JUST WORKs configuration --------------------------------------*/

#if (SECURITY_MODE==JUST_WORKS) 

#define SECURITY_MODE_NAME  "Just Works"

/** 
  * @brief Peripheral public address
*/
#define PERIPHERAL_PUBLIC_ADDRESS {0xA0, 0x12, 0x94, 0xE1, 0x80, 0x02}

/** 
  * @brief Peripheral device name
*/
#define PERIPHERAL_DEVICE_NAME 'S','l','a','v','e','S','e','c','_','A','0'

/** 
  * @brief MITM Mode
*/
#define MITM_MODE MITM_PROTECTION_NOT_REQUIRED /* MITM is not enabled */

/** 
  * @brief Fixed pin usage
*/
#define FIXED_PIN_POLICY  USE_FIXED_PIN_FOR_PAIRING 

/** 
* @brief aci_gap_slave_security_req(): bonding parameter
*/
#define SLAVE_BONDING_USAGE BONDING

/** 
  * @brief IO capability
*/
#define IO_CAPABILITY  IO_CAP_DISPLAY_ONLY /* Display only */

/** 
* @brief  Select random key mode: set to 1 to generate a random key 
*/
#define GENERATE_RANDOM_KEY 0

/**
*  @brief RX characteristic security permissions
*/
#define RX_CHAR_SECURITY_PERMISSIONS (ATTR_PERMISSION_ENCRY_READ)

/** 
  * @brief Secure Connections support level
*/
#define SECURE_CONNECTION_SUPPORT  SC_IS_SUPPORTED /* Supported &  optional:  Legacy Pairing may be accepted */
/** 
  * @brief KeyPress notification support
*/
#define KEYPRESS_NOTIFICATION  KEYPRESS_IS_NOT_SUPPORTED /* Not supported */

/** 
  * @brief Identity address tyoe
*/
#define IDENTITY_ADDRESS  0x00 /* Public address */

/* ----------- PASSKEY with fixed pin ----------------------------------------*/

#elif (SECURITY_MODE==PASSKEY_FIXED_PIN) 

#define SECURITY_MODE_NAME  "PassKey Fixed Pin"

/** 
  * @brief Peripheral public address
*/
#define PERIPHERAL_PUBLIC_ADDRESS {0xA1, 0x12, 0x94, 0xE1, 0x80, 0x02}

/** 
  * @brief Peripheral device name
*/
#define PERIPHERAL_DEVICE_NAME 'S','l','a','v','e','S','e','c','_','A','1'

/** 
  * @brief MITM Mode
*/
#define MITM_MODE MITM_PROTECTION_REQUIRED /* MITM is enabled */

/** 
  * @brief Fixed pin usage
*/
#define FIXED_PIN_POLICY  USE_FIXED_PIN_FOR_PAIRING /* use fixed pin */

/** 
* @brief aci_gap_slave_security_req(): bonding parameter
*/
#define SLAVE_BONDING_USAGE BONDING

/** 
  * @brief IO capability
*/
#define IO_CAPABILITY  IO_CAP_DISPLAY_ONLY /* Display only */


/** 
* @brief  Select random key mode: set to 1 to generate a random key 
*/
#define GENERATE_RANDOM_KEY FIXED_PIN_POLICY //0

/** 
  * @brief Secure Connections support level
*/
#define SECURE_CONNECTION_SUPPORT  SC_IS_SUPPORTED /* Supported &  optional:  Legacy Pairing may be accepted  */

/** 
  * @brief KeyPress notification support
*/
#define KEYPRESS_NOTIFICATION  KEYPRESS_IS_NOT_SUPPORTED /* Not supported */

/** 
  * @brief Identity address tyoe
*/
#define IDENTITY_ADDRESS  0x00 /* Public address */

/**
*  @brief RX characteristic security permissions
*/
#define RX_CHAR_SECURITY_PERMISSIONS (ATTR_PERMISSION_ENCRY_READ|ATTR_PERMISSION_AUTHEN_READ)

/*  ----------- PASS KEY ENTRY configuration with random key (default configuration)*/

#elif (SECURITY_MODE==PASSKEY_RANDOM_PIN) 

#define SECURITY_MODE_NAME  "PassKey Random Pin"

/** 
  * @brief Peripheral public address
*/
#define PERIPHERAL_PUBLIC_ADDRESS {0xA2, 0x12, 0x94, 0xE1, 0x80, 0x02}

/** 
  * @brief Peripheral device name
*/
#define PERIPHERAL_DEVICE_NAME 'S','l','a','v','e','S','e','c','_','A','2'

/** 
  * @brief MITM Mode
*/
#define MITM_MODE MITM_PROTECTION_REQUIRED /* MITM is enabled */

/** 
  * @brief Fixed pin usage
*/
#define FIXED_PIN_POLICY  DONOT_USE_FIXED_PIN_FOR_PAIRING /* don't use fixed pin */

/** 
* @brief aci_gap_slave_security_req(): bonding parameter
*/
#define SLAVE_BONDING_USAGE BONDING

/** 
  * @brief IO capability
*/
#define IO_CAPABILITY  IO_CAP_DISPLAY_ONLY /* Display only */


/** 
  * @brief Secure Connections support level
*/
#define SECURE_CONNECTION_SUPPORT  SC_IS_SUPPORTED /* Supported &  optional:  Legacy Pairing may be accepted  */

/** 
  * @brief KeyPress notification support
*/
#define KEYPRESS_NOTIFICATION  KEYPRESS_IS_NOT_SUPPORTED /* Not supported */

/** 
  * @brief Identity address tyoe
*/
#define IDENTITY_ADDRESS  0x00 /* Public address */

/** 
* @brief  Select random key mode: set to 1 to generate a random key 
*/
#define GENERATE_RANDOM_KEY FIXED_PIN_POLICY //1

/**
*  @brief RX characteristic security permissions
*/
#define RX_CHAR_SECURITY_PERMISSIONS (ATTR_PERMISSION_ENCRY_READ|ATTR_PERMISSION_AUTHEN_READ)

/*  ----------- NUMERIC COMPARISON VALUE configuration (ONLY BLE 4.2 security: LE Secure connections) */

#elif (SECURITY_MODE==NUMERIC_COMPARISON) 

#define SECURITY_MODE_NAME  "Numeric Comparison"

/** 
  * @brief Peripheral public address
*/
#define PERIPHERAL_PUBLIC_ADDRESS {0xA3, 0x12, 0x94, 0xE1, 0x80, 0x02}

/** 
  * @brief Peripheral device name
*/
#define PERIPHERAL_DEVICE_NAME 'S','l','a','v','e','S','e','c','_','A','3'

/** 
  * @brief MITM Mode
*/
#define MITM_MODE MITM_PROTECTION_REQUIRED /* MITM is enabled */

/** 
  * @brief Fixed pin usage
*/
#define FIXED_PIN_POLICY  USE_FIXED_PIN_FOR_PAIRING /* use fixed pin */

/** 
* @brief aci_gap_slave_security_req(): bonding parameter
*/
#define SLAVE_BONDING_USAGE BONDING

/** 
  * @brief IO capability
*/
#define IO_CAPABILITY  IO_CAP_KEYBOARD_DISPLAY /* Keyboard Display */

/** 
  * @brief Secure Connections support level
*/
#define SECURE_CONNECTION_SUPPORT  SC_IS_SUPPORTED /* Supported &  optional:  Legacy Pairing may be accepted  */

/** 
  * @brief KeyPress notification support
*/
#define KEYPRESS_NOTIFICATION  KEYPRESS_IS_NOT_SUPPORTED /* Not supported */

/** 
  * @brief Identity address tyoe
*/
#define IDENTITY_ADDRESS  0x00 /* Public address */

/** 
* @brief  Select random key mode: set to 1 to generate a random key 
*/
#define GENERATE_RANDOM_KEY 0 
/**
*  @brief RX characteristic security permissions
*/
#define RX_CHAR_SECURITY_PERMISSIONS (ATTR_PERMISSION_ENCRY_READ|ATTR_PERMISSION_AUTHEN_READ)

#endif 

#define PERIPHERAL_TIMER 1

uint8_t DeviceInit(void);
void APP_Tick(void);
void Process_InputData(uint8_t* data_buffer, uint16_t Nb_bytes);
uint8_t notification_data_handler(void);
uint8_t read_data_handler(void);
void Clear_Security_Database(void);

/* BLE Security v4.2 is supported: BLE stack FW version >= 2.x */
void Confirm_Numeric_Value_SM(void); 

#endif // _DEVICE_INIT_H_
