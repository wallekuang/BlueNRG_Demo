/**
 * @file    hid_peripheral.h
 * @author  AMS - VMA RF Application Team
 * @version V1.0.0
 * @date    October 5, 2015
 * @brief   This file provides all the low level API to manage the HID/HOGP BLE peripheral role.
 * @details
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
 *
 * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HID_PERIPHERAL_H
#define __HID_PERIPHERAL_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "hid_peripheral_config.h"

/* Defines -------------------------------------------------------------------*/  

/**
 * @name HID/HOGP Device discoverable mode configuration constants
 *@{
 */

/**
 * @brief Limited discovery procedure constant
 */
#define LIMITED_DISCOVERABLE_MODE 0x01

/**
 * @brief General discovery procedure constant
 */
#define GENERAL_DISCOVERABLE_MODE 0x02

//@} \\END HID/HOGP Device discoverable mode configuration constants

/**
 * @name HID/HOGP Device Protocol mode configuration constants
 *@{
 */

/**
 * @brief Default protocol mode of all HID devices
 */
#define REPORT_PROTOCOL_MODE 0x01

/**
 * @brief A HID device shall enter in Boot Protocol
 * Mode after this value has been written
 */
#define BOOT_PROTOCOL_MODE   0x00

//@} \\END HID/HOGP Device Protocol mode configuration constants

/**
 * @name HID/HOGP Device Suspend constants
 *@{
 */

/**
 * @brief The HID Host is entering the suspend state.
 */
#define HID_SUSPEND_STATE      0x00

/**
 * @brief The HID Host is exiting the suspend state.
 */
#define HID_EXIT_SUSPEND_STATE 0x01

//@} \\END HID/HOGP Device Suspend constants

/**
 * @name HID/HOGP Report Type constants
 *@{
 */


#ifdef HID_KEYBOARD
#define REPORT_ID  0x00
#else
#define REPORT_ID  0x01 
#endif


/**
 * @brief Input report type
 */
#define INPUT_REPORT                    0x01

/**
 * @brief Output report type
 */
#define OUTPUT_REPORT                   0x02

/**
 * @brief Feature report type
 */
#define FEATURE_REPORT                  0x03

/**
 * @brief Boot keyboard input report type
 */
#define BOOT_KEYBOARD_INPUT_REPORT      0x04

/**
 * @brief Boot keyboard output report type
 */
#define BOOT_KEYBOARD_OUTPUT_REPORT     0x05

/**
 * @brief Boot mouse input report type
 */
#define BOOT_MOUSE_INPUT_REPORT         0x06

//@} \\END HID/HOGP Report Type

/**
 * @name HID/HOGP Device Status constants
 *@{
 */

/**
 * @brief The device is in unknown status.
 * This status should not appear ???[FC]
 */
#define HID_DEVICE_UNKNOWN_STATUS         0x00

/**
 * @brief The device is busy because it is executing
 * internal operation
 */
#define HID_DEVICE_BUSY                   0x01

/**
 * @brief The device has ended all internal operation 
 * and it is in idle state, with not notification pending.
 * In this status the HID device can enter in deep sleep to
 * save the battery life.
 */
#define HID_DEVICE_READY_TO_DEEP_SLEEP    0x02

/**
 * @brief The device is ready to send Notification report to
 * the HID Host
 */
#define HID_DEVICE_READY_TO_NOTIFY        0x04

/**
 * @brief The device is Bonded and Not Connected
 */
#define HID_DEVICE_BONDED                 0x08

/**
 * @brief The device is only Connected
 */
#define HID_DEVICE_CONNECTED              0x10

/**
 * @brief The device is not connected and not bonded
 */
#define HID_DEVICE_NOT_CONNECTED          0x20

//@} \\END HID/HOGP Device Status constants

/* Typedef -------------------------------------------------------------------*/

/**
 * @brief Connection parameter to request after the bonding and service discovery.
 * The HID Device requests to change to its preferred connection parameters which best
 * suit its use case
 */
typedef struct connParamS {
  /** Minimum value for the connection event interval.
   *  connIntervalMin = Interval Min * 1.25ms
   */
  uint16_t interval_min;
  /** Maximum value for the connection event interval.
   * connIntervalMax = Interval Max * 1.25ms
   */
  uint16_t interval_max;
  /** Slave latency parameter */
  uint16_t slave_latency;
  /** Connection timeout parameter.
   *  The value is Timeout Multiplier * 10ms
   */
  uint16_t timeout_multiplier;
} connParam_Type;

/**
 * @brief Battery Service Specification
 */
typedef struct batteryServiceS {
  /** TRUE if the battery level is included in the report map.
   *  False otherwise
   */
  uint8_t inReportMap;
  /** Report ID if the battery level is included 
   *  in the report map
   */
  uint8_t reportID;
} batteryService_Type;

/**
 * @brief Device Information Service Specification
 */
typedef struct devInfServiceS {
  /** Manufacter Name */
  uint8_t manufacName[MANUFAC_NAME_LEN];
  /** Model Number */
  uint8_t modelNumber[MODEL_NUMB_LEN];
  /** FW Revision */
  uint8_t fwRevision[FW_REV_LEN];
  /** SW Revision */
  uint8_t swRevision[SW_REV_LEN];
  /** PNP ID */
  uint8_t pnpID[PNP_ID_LEN];
} devInfService_Type;

/**
 * @brief Included Service type definition
 */
typedef struct includeSerS {
  /** Service UUID to be included */
  uint16_t uuid;
  /** Start handle of the service to be included.
   *  If the service to include is BATTERY Service
   *  This data is not required
   */
  uint16_t start_handle;
  /** End handle of the service to be included.
   *  If the service to include is BATTERY Service
   *  This data is not required
   */
  uint16_t end_handle;
} includeSer_Type;

/**
 * @brief Report Type definition
 */
typedef struct reportS {
  /** Report ID */
  uint8_t ID;
  /** Report Type: 0x01 Input, 0x02 Output, 0x03 Feature */
  uint8_t type;
  /** Report Length */
  uint8_t length;
} report_Type;

/**
 * @brief HID Service Specification
 */
typedef struct hidServiceS {
  /** TRUE if the device supports the 
   *  boot protocol mode. FALSE otherwise
   */
  uint8_t bootSupport;
  /** Report Descriptor length */
  uint8_t reportDescLen;
  /** Report Descriptor */
  uint8_t *reportDesc;
  /** TRUE if the device supports the 
   *  Report characterisitic. FALSE otherwise
   */
  uint8_t reportSupport;
  /** Report Reference Descriptor */
  report_Type reportReferenceDesc[REPORT_NUMBER];
  /** TRUE if the HID device is a keyboard that 
   *  supports boot protocol mode.
   *  FALSE otherwise.
   */
  uint8_t isBootDevKeyboard;
  /** TRUE if the HID device is a mouse that supports
   *  boot protocol mode.
   *  FALSE otherwise.
   */
  uint8_t isBootDevMouse;
  /** TRUE if the HID Service has external report.
   *  FALSE otherwise.
   */
  uint8_t externalReportEnabled;
  /** External reports UUID 
   *  referenced in report Map
   */
  uint16_t externalReport[EXTERNAL_REPORT_NUMBER];
  /** TRUE if the HID service has included services.
   *  FALSE otherwise.
   */
  uint8_t includedServiceEnabled;
  /** Included services UUID */
  includeSer_Type includedService[INCLUDED_SERVICE_NUMBER];
  /** HID Information Characteristic value The format is:
   * - 2 bytes USB HID specification implemented by HID Device
   * - 1 byte Country Code
   * - 1 byte Bit0: RemoteWake, Bit1: Normally Connectable
   */
  uint8_t informationCharac[4];
} hidService_Type;

/* Function Prototypes--------------------------------------------------------*/

/**
 * @name HOGP Device Configuration Functions
 *@{
 */

/**
  * @brief  Initializes the  HID Device with Peripheral Role and 
  * sets the public address.
  * @param IO_Capability Sets the device IO capabilities. Possible value are:
  * - IO_CAP_DISPLAY_ONLY
  * - IO_CAP_DISPLAY_YES_NO
  * - KEYBOARD_ONLY
  * - IO_CAP_NO_INPUT_NO_OUTPUT
  * - IO_CAP_KEYBOARD_DISPLAY
  * @param connParam Connection parameter to send to the
  * HID Host after bonding/service discovery
  * @param dev_name_len Device name length
  * @param dev_name Device name
  * @param addr Public device address
  * @retval Status of the call.
  */
uint8_t hidDevice_Init(uint8_t IO_Capability, connParam_Type connParam, 
                       uint8_t dev_name_len, uint8_t *dev_name, uint8_t *addr);

/**
 * @brief Sets the Idle Connection Timeout according the HID over GATT profile specification.
 * The HID device may perform the GAP Terminate Connection procedure if the connection is 
 * idle for a time period, which is implementation dependent.
 * After bonding the HID device should wait for this idle connection timeout to allow the HID 
 * Host to complete the configuration procedure.
 * Default value 500 ms.
 * @note If the timeout is zero the Power Save management is disabled
 * @param timeout Idle connection timeout expressed in number of millisecond.
 * @retval Nothing
 */
void hidSetIdleTimeout(uint32_t timeout);

//@} \\END HOGP Device Configuration Functions

/**
 * @name HOGP Security Functions
 *@{
 */

/**
  * @brief Configures the device for LE Security Mode 1 and either Security Level 2 or 3
  * To set the Security Level 2 the MITM mode param shall be equal to FALSE, all the other params are ignored.
  * To set the Security Level 3 the MITM mode param shall be equal to TRUE, the other params shall be used to
  * signal if the device uses a fixed PIN or not.
  * @param MITM_Mode MITM mode. TRUE to set the security level 3, FALSE to set the security level 2
  * @param fixedPinUsed TRUE to use a fixed PIN, FALSE otherwise. If the MITM_Mode param is FALSE, this param is ignored
  * @param fixedPinValue PIN value
  * @retval Status of the call.
  */

uint8_t hidSetDeviceSecurty(uint8_t MITM_Mode, uint8_t fixedPinUsed, uint32_t fixedPinValue);

/**
 * @brief Deletes all the security information of the devices bonded.
 * @retval Status of the call
 */
uint8_t hidDeleteBondedDevice(void);

/**
 * @brief This callback is called when the HID Host asks to insert a pass key to start
 * the Pairing procedure. To send the passkey inside the callback has to be called the
 * function hidSendPassKey()
 * @retval Nothing
 */
extern void hidPassKeyRequest_CB(void);

/**
 * @brief Sends the passk key to the HID Host
 * @param key Passkey to send
 * @retval Status of the call
 */
uint8_t hidSendPassKey(uint32_t key);

//@} \\END HOGP Security Functions

/**
 * @name HOGP GATT Database Configuration Functions
 *@{
 */

/**
 * @brief Adds the Primary service and the characteristics associated 
 * for the battery, device information and hid services.
 * @param battery Battery Service characteristics to add. 
 * See the tpyedef batteryService_Type for more details.
 * @param devInf Device Information Service characteristics to add. 
 * See the typedef devInfService_Type for more details.
 * @param hid HID Service characteristics to add. 
 * See the typedef HID Service for more details.
 * @retval Status of the call
 */
uint8_t hidAddServices(batteryService_Type* battery, devInfService_Type* devInf, hidService_Type* hid);

//@} \\END HOGP GATT Database Configuration Functions

/**
 * @name HOGP Discoverable Mode Functions
 *@{
 */

/**
  * @brief Starts the peripheral device discoverable mode. This mode is ended 
  * when either the upper layer issues a command to terminate the procedure 
  * using the command hidTerminateDiscoverableMode() or the   
  * timeout happens (timeout value is 180 s).
  * @param mode Discoverable mode to use:
  * - LIMITED_DISCOVERABLE_MODE
  * - GENERAL_DISCOVERABLE_MODE
  * @param nameLen Local name length. Max 20
  * @param name Local name value
  * @retval Status of the call.
  */
uint8_t hidSetDeviceDiscoverable(uint8_t mode, uint8_t nameLen, uint8_t *name);

/**
  * @brief  Forces the end of the GAP Discoverable mode.
  * @retval Status of the call.
  */
uint8_t hidStopDiscoverableMode(void);

//@} \\END HOGP Discoverable Mode Functions

/**
 * @name HOGP HID Report Management Functions
 *@{
 */

/**
 * @brief Sends the report data to the HID Central device.
 * @note If the data report to send is a boot report the "ID" param shall not be 
 * considered.
 * @param id Report ID.
 * @param type Report Type: 
 * - Input Report: 0x01
 * - Boot Keyboard Input Report: 0x04
 * - Boot Mouse Input Report: 0x06
 * @param reportLen Lengt of the Report Data
 * @param reportData Data to transmit to the HID master 
 * @retval Status of the call
 */
uint8_t hidSendReport(uint8_t id, uint8_t type, uint8_t reportLen, uint8_t *reportData);

/**
 * @brief Verifies if the Input Report is ready/enabled to send data
 * @param index Position in the array report_Type used during the hid service and characteristic
 * initialization.
 * @retval TRUE if the Input Report is ready to send data. FALSE otherwise
 */
uint8_t hidInputReportReady(uint8_t index);

/**
 * @brief This callback is used to report to the application an update report 
 * data value from the HID Host already done. It maps the Set_Report() request in USB HID.
 * @note If the data report to change is a boot report the "ID" param shall not be 
 * considered.
 * @param ID Report id
 * @param type Report Type: 
 * - Input Report 0x01
 * - Output Report 0x02
 * - Feature Report 0x03
 * - Boot Keyboard Input Report 0x04
 * - Boot Keyboard Output Report 0x05
 * - Boot Mouse Input Report 0x06
 * @param len Length of the report data
 * @param data Report Data received
 * @retval Nothing
 */
extern void hidSetReport_CB(uint8_t ID, uint8_t type, uint8_t len, uint8_t *data);

/**
 * @brief This callback is used to report to the application a read request 
 * from the HID Host. It maps the Get_Report() request in USB HID.
 * To update and allow the Get_Report() the application needs to call
 * inside the hidGetReport_CB() the function hidUpdateReportValue().
 * @note If the data report to retrieve is a boot report the "ID" param shall not be 
 * considered.
 * @param ID Report id
 * @param type Report Type: 
 * - Input Report 0x01
 * - Output Report 0x02
 * - Feature Report 0x03
 * - Boot Keyboard Input Report 0x04
 * - Boot Keyboard Output Report 0x05
 * - Boot Mouse Input Report 0x06
 * @retval Nothing
 */
extern void hidGetReport_CB(uint8_t ID, uint8_t type);

/**
 * @brief This function updates the report data value as consequence of a Get_Report() request
 * from the HID Host. This function MUST be called inside the hidGetReport_CB(), 
 * to update the data value and to allow the read of the data.
 * @note If the data report to retrieve is a boot report the "ID" param shall not be 
 * considered.
 * @param ID Report ID
 * @param type Report type:
 * - Input Report 0x01
 * - Output Report 0x02
 * - Feature Report 0x03
 * - Boot Keyboard Input Report 0x04
 * - Boot Keyboard Output Report 0x05
 * - Boot Mouse Input Report 0x06
 * @param len Report data length
 * @param data Report data
 * @retval Status of the call
 */
uint8_t hidUpdateReportValue(uint8_t ID, uint8_t type, uint8_t len, uint8_t *data);

/**
 * @brief This callback reports the change on the protocol mode, driven from the
 * HID Host.
 * @param mode Protocol mode:
 * - BOOT_PROTOCOL_MODE
 * - REPORT_PROTOCOL_MODE
 * @retval Nothing
 */
extern void hidChangeProtocolMode_CB(uint8_t mode);

/**
 * @brief This callback reports the change on the HID Host state. This
 *  value is driven from the HID Host.
 * @param value Control Point value:
 * - HID_SUSPEND_STATE
 * - HID_EXIT_SUSPEND_STATE
 * @retval Nothing
 */
extern void hidControlPoint_CB(uint8_t value);

/**
 * @brief Sends the battery level. This function can be used if the Battery service
 * is not included in the HID service. Otherwise to send the battery level use the 
 * hidSendReport() function call with the corresponding Report "id".
 * @param level Battery level to send
 * @retval Status of the call
 */
uint8_t sendBatteryLevel(uint8_t level);

//@} \\END HOGP HID Report Management Functions

/**
 * @name HOGP Common Service Functions
 *@{
 */

/**
  * @brief  This function needs to be called inside a main loop to run the HID state machines
  * @retval Nothing
  */
void hidDevice_Tick(void);

/**
 * @brief Enable/Disable the value of the report notification pending.
 * If there is a notification pending the device can't enter in power save.
 * @retval Nothing
 */
void hidSetNotificationPending(uint8_t enabled);

/**
 * @brief Returns the device status
 * @retval The device status. Possible values are:
 * - HID_DEVICE_READY_TO_NOTIFY: device connected, bonded and paired
 * - HID_DEVICE_READY_TO_POWER_SAVE: device in idle and not notification pending
 * - HID_DEVICE_BONDED: device bonded and not connected
 * - HID_DEVICE_CONNECTED: device connected and not bonded
 * - HID_DEVICE_BUSY: device busy executing a state machine
 * - HID_DEVICE_NOT_CONNECTED: device not connected and not bonded
 */
uint8_t hidDeviceStatus(void);

/**
 * @brief Closes the BLE connection with the HID Host
 * @retval Status of the call 
 */
uint8_t hidCloseConnection(void);

/**
 * @brief Sets the TX power level used during the communication
 * Default value is -2.1 dBm
 * @param level Power Level, valid range is [0 - 7]. The following table determines 
 * the output power level (dBm):
 * - 0 = -14
 * - 1 = -11
 * - 2 = -8
 * - 3 = -5
 * - 4 = -2
 * - 5 = 2
 * - 6 = 4
 * - 7 = 8
 * @retval Status of the call
 */
uint8_t hidSetTxPower(uint8_t level);

//@} \\END HOGP Common Service Functions

/**
 * @name HID Library events
 *@{
 */

void HID_Lib_hci_disconnection_complete_event(uint8_t Status,
                                              uint16_t Connection_Handle,
                                              uint8_t Reason);

void HID_Lib_hci_le_connection_complete_event(uint8_t Status,
                                              uint16_t Connection_Handle,
                                              uint8_t Role,
                                              uint8_t Peer_Address_Type,
                                              uint8_t Peer_Address[6],
                                              uint16_t Conn_Interval,
                                              uint16_t Conn_Latency,
                                              uint16_t Supervision_Timeout,
                                              uint8_t Master_Clock_Accuracy);

void HID_Lib_aci_gap_limited_discoverable_event(void);

void HID_Lib_aci_gap_pass_key_req_event(uint16_t Connection_Handle);

void HID_Lib_aci_gap_pairing_complete_event(uint16_t Connection_Handle,
                                            uint8_t Status);

void HID_Lib_aci_gap_bond_lost_event(void);

void HID_Lib_aci_gatt_attribute_modified_event(uint16_t Connection_Handle,
                                               uint16_t Attr_Handle,
                                               uint16_t Offset,
                                               uint16_t Attr_Data_Length,
                                               uint8_t Attr_Data[]);

void HID_Lib_aci_gatt_read_permit_req_event(uint16_t Connection_Handle,
                                            uint16_t Attribute_Handle,
                                            uint16_t Offset);

void HID_Lib_aci_l2cap_connection_update_resp_event(uint16_t Connection_Handle,
                                                    uint16_t Result);

void HID_Lib_aci_l2cap_proc_timeout_event(uint16_t Connection_Handle,
                                          uint8_t Data_Length,
                                          uint8_t Data[]);

void HID_Lib_HAL_VTimerTimeoutCallback(uint8_t timerNum);

//@} \\END  HID Library events

#endif
