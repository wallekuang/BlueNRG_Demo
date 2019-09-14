
/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : I2C/Master_w_Sensor/main.c 
* Author             : RF Application Team
* Version            : V1.0.0
* Date               : September-2015
* Description        : Code demostrating the I2C master functionality using the environmental sensor
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "BlueNRG_x_device.h"
#include <stdio.h>
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"
#include "LPS25HB_hal.h"

/** @addtogroup BlueNRG1_StdPeriph_Examples BlueNRG1 Standard Peripheral Examples
  * @{
  */

/** @addtogroup I2C_Examples I2C Examples
  * @{
  */

/** @addtogroup I2C_Master_w_Sensor I2C Master with Sensor
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint32_t lSystickCounter=0;

/* Private function prototypes -----------------------------------------------*/
void SdkDelayMs(volatile uint32_t lTimeMs);

/* Private functions ---------------------------------------------------------*/


/**
* @brief  Main program code
* @param  None
* @retval None
*/
int main(void)
{
  float pressure, temperature;
  uint8_t tmpreg;
  
  /* System initialization function */
  SystemInit();
  
  /* Identify BlueNRG1 platform */
  SdkEvalIdentification();
  
  /* UART initialization */
  SdkEvalComUartInit(UART_BAUDRATE);
  
  /* LED initialization */
  SdkEvalLedInit(LED1);
  
  /* LPS25HB initialization */
  PRESSURE_DrvTypeDef* xLPS25HBDrv = &LPS25HBDrv;  
  PRESSURE_InitTypeDef InitStructure;
  InitStructure.OutputDataRate = LPS25HB_ODR_1Hz;
  InitStructure.BlockDataUpdate = LPS25HB_BDU_READ;
  InitStructure.DiffEnable = LPS25HB_DIFF_ENABLE;
  InitStructure.SPIMode = LPS25HB_SPI_SIM_3W;  
  InitStructure.PressureResolution = LPS25HB_P_RES_AVG_32;
  InitStructure.TemperatureResolution = LPS25HB_T_RES_AVG_16;  
  xLPS25HBDrv->Init(&InitStructure);
    
  /* Configure SysTick to generate interrupt */
  SysTick_Config(SYST_CLOCK/1000 - 1);  

  /* Infinite loop */
  while(1) {
    
    /* Polling of IRQ_STATUS register */
    LPS25HB_IO_Read(&tmpreg, LPS25HB_ADDRESS_LOW, LPS25HB_STATUS_REG_ADDR, 1);
    
    if( (tmpreg&0x03) == 0x03) {
      /* Get pressure and temperature data */
      xLPS25HBDrv->GetPressure(&pressure);
      xLPS25HBDrv->GetTemperature(&temperature);
      printf("PRESSURE: %.3f mbar TEMP: %.2f 'C\r\n", pressure, temperature);
      SdkEvalLedToggle(LED1);
    }
    SdkDelayMs(200);
  }
}
  

/**
* @brief  Delay function
* @param  Delay in ms
* @retval None
*/
void SdkDelayMs(volatile uint32_t lTimeMs)
{
  uint32_t nWaitPeriod = ~lSystickCounter;
  
  if(nWaitPeriod<lTimeMs)
  {
    while( lSystickCounter != 0xFFFFFFFF);
    nWaitPeriod = lTimeMs-nWaitPeriod;
  }
  else
    nWaitPeriod = lTimeMs+ ~nWaitPeriod;
  
  while( lSystickCounter != nWaitPeriod ) ;

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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

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
