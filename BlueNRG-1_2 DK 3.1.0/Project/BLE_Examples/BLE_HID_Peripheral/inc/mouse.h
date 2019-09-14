/**
  ******************************************************************************
  * @file    mouse.h 
  * @author  VMA RF Application Team
  * @version V1.0.0
  * @date    October-2015
  * @brief   Mouse init and process data
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
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MOUSE_H
#define MOUSE_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>


//#define PTS_AUTOMATING   1    //PTS Automating for Time Client
#ifdef PTS_AUTOMATING      
#define PTS_CODE        '@'  //to identy a message to parse for PTS automating tests
#define HIDS_PTS_CODE   'n'  //concatenated to PTS_CODE, this identifies an HIDS message to parse for PTS automating tests       
#else
#define PTS_CODE        ''     
#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/* Set HID Service params for demo */
void setDefaultHidParams(void);

/* Set HID Service params for PTS tests */
void setTestHidParams(void);
void setHidParams_ReportType(uint8_t reportType);

/* Init the HID peripheral and configures the GATT DB */
uint8_t Configure_HidPeripheral(void);

/* Power save management */
void DevicePowerSaveProcedure(void);

/* Runs the Application state machines */
void APP_Tick(void);

/* Sensor IRQ Handler */
void Sensor_IrqHandler(void);

#endif /* MOUSE_H */

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/

