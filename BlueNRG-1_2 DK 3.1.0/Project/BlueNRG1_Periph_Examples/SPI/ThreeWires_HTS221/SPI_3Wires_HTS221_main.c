/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : SPI/ThreeWires_HTS221/main.c 
* Author             : RF Application Team
* Version            : V1.0.0
* Date               : January-2017
* Description        : Code demostrating the SPI 3 wires functionality
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"
#include "HTS221_Driver.h"
#include "HTS221_HAL.h"

/** @addtogroup BlueNRG1_StdPeriph_Examples BlueNRG1 Standard Peripheral Examples
* @{
*/

/** @addtogroup SPI_Examples SPI Examples
* @{
*/

/** @addtogroup ThreeWires_HTS221 Three Wires HTS221
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
* @brief  Main program code
* @param  None
* @retval None
*/
int main(void)
{  
  HTS221_BitStatus_et HTS221_BitStatus;
  HTS221_Init_st HTS221_Init;
  int16_t temperature;
  uint16_t humidity;
  
  /* System initialization function */
  SystemInit();
  
  /* Identify BlueNRG1 platform */
  SdkEvalIdentification();
  
  /* UART initialization */
  SdkEvalComUartInit(UART_BAUDRATE);
  
  /* HTS221 SPI initialization */
  HTTS221_HAL_SPI_Init();
  
  /* HTS221 memory boot */
  HTS221_MemoryBoot();
  
  /* HTS221 initialization */
  HTS221_Init.avg_h = HTS221_AVGH_512;
  HTS221_Init.avg_t = HTS221_AVGT_256;
  HTS221_Init.odr = HTS221_ODR_ONE_SHOT;
  HTS221_Init.bdu_status = HTS221_ENABLE;
  HTS221_Init.heater_status = HTS221_DISABLE;
  HTS221_Init.irq_enable = HTS221_DISABLE;
  HTS221_Init.irq_level = HTS221_LOW_LVL;
  HTS221_Init.irq_output_type = HTS221_PUSHPULL;
  HTS221_Set_InitConfig(&HTS221_Init);
  
  while(1) {
    HTS221_StartOneShotMeasurement();
    HTS221_Set_PowerDownMode(HTS221_SET);
    
    do {
      HTS221_IsMeasurementCompleted(&HTS221_BitStatus);
    } while(HTS221_BitStatus == HTS221_RESET);
    
    HTS221_Get_Measurement(&humidity, &temperature);    
    printf("Humidity %.0f %% Temperature %.1f 'C\r\n", humidity/10.0, temperature/10.0);
  }  
  
}



#ifdef USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
  ex: printff("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
