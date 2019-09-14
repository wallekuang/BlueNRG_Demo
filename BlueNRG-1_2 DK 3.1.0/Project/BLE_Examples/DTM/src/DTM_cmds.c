
/**
  ******************************************************************************
  * @file    DTM_cmds.c
  * @author  AMS - RF Application team
  * @version V1.2.0
  * @date    21-May-2018
  * @brief   DTM specific commands to be implemented on DTM context 
  *          (not present on BLE stack) 
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2018 STMicroelectronics</center></h2>
  ******************************************************************************
  */
    
#include "BlueNRG_x_device.h"
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "BlueNRG1_api.h"
#include "DTM_cmd_db.h"
#include "ble_status.h"
#include "cmd.h"
#include "transport_layer.h"
#include "stack_user_cfg.h"

#ifdef WATCHDOG
#include "BlueNRG1_wdg.h"

#define RC32K_FREQ		32768
#define RELOAD_TIME(SEC)        ((SEC*RC32K_FREQ)-3) 
#endif

/** @brief BLE stack v2.1 stack modular configurations bitmap: bits [0-3] 
  *  CONTROLLER_PRIVACY_ENABLED: bit 0 selected (0x0001); 
  *  SECURE_CONNECTIONS_ENABLED: bit 1 selected (0x0002);
  *  CONTROLLER_MASTER_ENABLED:  bit 2 selected (0x0004);
  *  CONTROLLER_DATA_LENGTH_EXTENSION_ENABLED: bit 3 selected, valid only for BlueNRG-2 device (0x0008).
  */
#define CONTROLLER_PRIVACY_BIT                 ((uint16_t)0x0001)  /*!< Bit 0 selected */
#define SECURE_CONNECTIONS_BIT                 ((uint16_t)0x0002)  /*!< Bit 1 selected */
#define CONTROLLER_MASTER_BIT                  ((uint16_t)0x0004)  /*!< Bit 2 selected */
#define CONTROLLER_DATA_LENGTH_EXTENSION_BIT   ((uint16_t)0x0008)  /*!< Bit 3 selected */

/** @brief BLE stack v2.1 link layer only bitmap: bit 4 selected (0x0010)
  */
#define LINK_LAYER_ONLY_BIT ((uint16_t)0x0010)  /*!< Bit 4 selected */

/** @brief  Link Layer Enabled or not based on LL_ONLY preprocessor option */
#ifdef LL_ONLY
#define LINK_LAYER_ONLY_ENABLED (1U)
#else
#define LINK_LAYER_ONLY_ENABLED (0U)
#endif

/** @brief Define BLE stack configurations variant bitmap value based on enabled BLE stack options and associated bits (LSB 5 bits) */
#define BLE_STACK_CONFIGURATIONS_VARIANT (((uint16_t)(CONTROLLER_PRIVACY_ENABLED * CONTROLLER_PRIVACY_BIT)) | ((uint16_t)(SECURE_CONNECTIONS_ENABLED * SECURE_CONNECTIONS_BIT)) | \
                                          ((uint16_t) (CONTROLLER_MASTER_ENABLED * CONTROLLER_MASTER_BIT))  | ((uint16_t)(CONTROLLER_DATA_LENGTH_EXTENSION_ENABLED * CONTROLLER_DATA_LENGTH_EXTENSION_BIT)) | \
                                          ((uint16_t) (LINK_LAYER_ONLY_ENABLED   * LINK_LAYER_ONLY_BIT)))

tBleStatus aci_hal_updater_start(void)
{
   // For ACI_HAL_UPDATER_START, set flag to issue a updater start
   __blueflag_RAM = BLUE_FLAG_RAM_RESET;
   reset_pending = 1;
   return BLE_STATUS_SUCCESS;
}

tBleStatus aci_hal_get_firmware_details(uint8_t *DTM_version_major,
                                        uint8_t *DTM_version_minor,
                                        uint8_t *DTM_version_patch,
                                        uint8_t *DTM_variant,
                                        uint16_t *DTM_Build_Number,
                                        uint8_t *BTLE_Stack_version_major,
                                        uint8_t *BTLE_Stack_version_minor,
                                        uint8_t *BTLE_Stack_version_patch,
                                        uint8_t *BTLE_Stack_development,
                                        uint16_t *BTLE_Stack_variant,
                                        uint16_t *BTLE_Stack_Build_Number)
{

    aci_hal_get_fw_build_number(BTLE_Stack_Build_Number);
    uint8_t HCI_Version = 0;
    uint16_t HCI_Revision = 0;
    uint8_t LMP_PAL_Version = 0;
    uint16_t Manufacturer_Name = 0;
    uint16_t LMP_PAL_Subversion = 0;
    
    hci_read_local_version_information(&HCI_Version, &HCI_Revision, &LMP_PAL_Version,
                                       &Manufacturer_Name, &LMP_PAL_Subversion);
        
    *DTM_version_major  = DTM_FW_VERSION_MAJOR;
    *DTM_version_minor  = DTM_FW_VERSION_MINOR;
    *DTM_version_patch  = DTM_FW_VERSION_PATCH;
    *DTM_variant = DTM_VARIANT;
    *DTM_Build_Number = 0;
    *BTLE_Stack_version_major = HCI_Revision&0x0F;
    *BTLE_Stack_version_minor = (LMP_PAL_Subversion>>4)&0x0F;
    *BTLE_Stack_version_patch = LMP_PAL_Subversion&0x0F;
    *BTLE_Stack_development = (LMP_PAL_Subversion>>15)&0x01;
     
    /* Set the stack configurations variant bitmap value:
       first LSB 5 bits are reserved for BLE stack modular options + Link Layer only*/
    *BTLE_Stack_variant |= BLE_STACK_CONFIGURATIONS_VARIANT;
    
    return (BLE_STATUS_SUCCESS);
    
}

/**
* @brief  This API implements the hci le transmitter test with 
 *        the capability to set the number of packets to be sent. 
 * @param  TX_Frequency: TX frequency 
 * @param  Length_Of_Test_Data: lenght of test data
 * @param  Packet_Payload: packet payload 
 * @param  Number_Of_Packets: number pf packets to be sent on test
 * @retval status
*/
tBleStatus aci_hal_transmitter_test_packets(uint8_t TX_Frequency,
                                            uint8_t Length_Of_Test_Data,
                                            uint8_t Packet_Payload,
                                            uint16_t Number_Of_Packets)
{

  extern volatile uint32_t irq_count;
  tBleStatus status; 

  irq_count = 0;

  status = hci_le_transmitter_test(TX_Frequency /* 1 */,
                                    Length_Of_Test_Data /* 1 */,
                                    Packet_Payload /* 1 */);
  if (status == 0x00) {

    uint16_t LE_End_Number_Of_Packets;

#ifdef WATCHDOG
    uint16_t irq_count_last = 0;
#endif

    while (irq_count < Number_Of_Packets) {

#ifdef WATCHDOG
      if (irq_count_last != irq_count) {

        /* Clear watchdog pending bit, reload the timer and enable it */

        WDG_SetReload(RELOAD_TIME(WATCHDOG_TIME));

        irq_count_last = irq_count;
      }
#endif

    }
    status = hci_le_test_end(&LE_End_Number_Of_Packets);

  }

  return (status);
}



