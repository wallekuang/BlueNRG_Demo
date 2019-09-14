
/******************** (C) COPYRIGHT 2015 STMicroelectronics ********************
* File Name          : ADC/DMA/main.c 
* Author             : RF Application Team
* Version            : V1.1.0
* Date               : September-2015
* Description        : Code demostrating the ADC functionality through DMA channel
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
#include "BlueNRG1_conf.h"
#include "SDK_EVAL_Config.h"
#include <stdio.h>
#include <stdlib.h>

/** @addtogroup BlueNRG1_StdPeriph_Examples
  * @{
  */

/** @addtogroup ADC_Examples ADC Examples
  * @{
  */

/** @addtogroup ADC_DMA ADC DMA Example
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC_OUT_ADDRESS         (ADC_BASE + 0x16)
#define ADC_DMA_CH0             (DMA_CH0)

#define ADC_DMA_BUFFER_LEN      (256)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/ 
volatile uint32_t lSystickCounter=0;
ADC_InitType xADC_InitType;
uint16_t buffer_adc[ADC_DMA_BUFFER_LEN];

/* Private function prototypes -----------------------------------------------*/
void SdkDelayMs(volatile uint32_t lTimeMs);
void ADC_Configuration(void);
void DMA_Configuration(void);

/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

int main(void)
{
  /* System initialization function */
  SystemInit();
  
  /* Identify BlueNRG1 platform */
  SdkEvalIdentification();
  
  /* LED initialization function */
  SdkEvalLedInit(LED1);
  
  /* UART initialization */  
  SdkEvalComUartInit(UART_BAUDRATE);
  
  /* SysTick initialization 1ms */  
  SysTick_Config(SYST_CLOCK/1000 - 1);  

  /* DMA Initialization */
  DMA_Configuration();
  
  /* ADC Initialization */
  ADC_Configuration();
  
  /* ADC Initialization */
  ADC_DmaCmd(ENABLE);
  
  /* Start new conversion */
  ADC_Cmd(ENABLE);
  
  /* Infinite loop */
  while(1) {
    
    
    /* Check DMA_CH_UART_TX Transfer Complete interrupt */
    if(DMA_GetFlagStatus(DMA_FLAG_TC0)) {      
      DMA_ClearFlag(DMA_FLAG_TC0);
      
      /* ADC_DMA_CH0 disable */
      DMA_Cmd(ADC_DMA_CH0, DISABLE);
      
      printf("Acquisition from DMA channel 0\r\n");
      for(uint16_t i=0;i<ADC_DMA_BUFFER_LEN;i++) {
        printf("ADC value: %.0f mV\r\n", ADC_ConvertDifferentialVoltage(buffer_adc[i], xADC_InitType.ADC_Attenuation)*1000.0);
      }
      
      /* Application delay before next measurement */
      SdkDelayMs(100);
      
      /* Reload DMA channel 0 */
      ADC_DMA_CH0->CMAR = (uint32_t)buffer_adc;        
      ADC_DMA_CH0->CNDTR = ADC_DMA_BUFFER_LEN;
      
      /* Enable DMA channel 0 */
      DMA_Cmd(ADC_DMA_CH0, ENABLE);
      
    }
    
  }
}

/**
* @brief  Delay function in ms.
* @param  lTimeMs time in ms
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


/**
* @brief  ADC_Configuration.
* @param  None
* @retval None
*/
void ADC_Configuration(void)
{ 
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_ADC, ENABLE);
  
  /* Configure ADC */
  /* ADC_Input_AdcPin1 == ADC1 */
  /* ADC_Input_AdcPin2 == ADC2 */
  /* ADC_Input_AdcPin12 == ADC1 - ADC2 */
  xADC_InitType.ADC_OSR = ADC_OSR_200;
    //ADC_Input_BattSensor; //ADC_Input_TempSensor;// ADC_Input_AdcPin1 // ADC_Input_AdcPin12 // ADC_Input_AdcPin2
  xADC_InitType.ADC_Input = ADC_Input_AdcPin12; //ADC_Input_AdcPin12;
  xADC_InitType.ADC_ConversionMode = ADC_ConversionMode_Continuous;
  xADC_InitType.ADC_ReferenceVoltage = ADC_ReferenceVoltage_0V6; //ADC_ReferenceVoltage_0V6;
  xADC_InitType.ADC_Attenuation = ADC_Attenuation_9dB54;
    
  ADC_Init(&xADC_InitType);
  
  /* Enable auto offset correction */
  ADC_AutoOffsetUpdate(ENABLE);
  ADC_Calibration(ENABLE);
}


/**
* @brief  DMA_Configuration.
* @param  None
* @retval None
*/
void DMA_Configuration(void)
{ 
  DMA_InitType DMA_InitStructure;
  
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_DMA, ENABLE);
  
  /* DMA_CH_UART_TX Initialization */
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC_OUT_ADDRESS;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffer_adc;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = (uint32_t)ADC_DMA_BUFFER_LEN;  
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; //DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
  DMA_Init(ADC_DMA_CH0, &DMA_InitStructure);
  
  /* Enable DMA ADC CHANNEL 0 Transfer Complete interrupt */
  DMA_FlagConfig(ADC_DMA_CH0, DMA_FLAG_TC, ENABLE);
  
  /* Select DMA ADC CHANNEL 0 */
  DMA_SelectAdcChannel(DMA_ADC_CHANNEL0, ENABLE);
    
  /* Enable DMA ADC CHANNEL 0 */
  DMA_Cmd(ADC_DMA_CH0, ENABLE);
}

#ifdef  USE_FULL_ASSERT

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
