
/******************** (C) COPYRIGHT 2016 STMicroelectronics ********************
* File Name          : ADC/PDM/main.c 
* Author             : RF Application Team
* Version            : V1.0.1
* Date               : April-2018
* Description        : Code demostrating the ADC PDM functionality
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

/** @addtogroup BlueNRG1_StdPeriph_Examples
  * @{
  */

/** @addtogroup ADC_Examples ADC Examples
  * @{
  */

/** @addtogroup ADC_PDM ADC PDM Example
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FS 8000

#undef UART_BAUDRATE
#define UART_BAUDRATE            (512000)

#if (FS == 16000)
#define PCM_BUFFER_SIZE (16 * 2)
#elif (FS == 8000)
#define PCM_BUFFER_SIZE (16)
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/ 
ADC_InitType xADC_InitType;
int16_t PCM_Buffer[PCM_BUFFER_SIZE]; /* Bytes */

/* Private function prototypes -----------------------------------------------*/
void ADC_Configuration(void);
void GPIO_Configuration(void);
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
  
  /* UART initialization */  
  SdkEvalComUartInit(UART_BAUDRATE);
  
  /* GPIO Initialization */
  GPIO_Configuration();
  
  /* ADC Initialization */
  ADC_Configuration();  
  
  /* DMA Initialization */
  DMA_Configuration();
  
  /* Start new conversion */
  ADC_Cmd(ENABLE);
  
  /* Infinite loop */
  while(1) 
  {
    
  }
}


/**
* @brief  GPIO_Configuration.
* @param  None
* @retval None
*/
void GPIO_Configuration(void)
{ 
  GPIO_InitType GPIO_InitStructure;
  
  /* Enables the GPIO Clock */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_GPIO, ENABLE);
  
  /* Configure GPIO_Pin_1 and GPIO_Pin_2 as PDM_DATA and PDM_CLOCK */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = Serial2_Mode;
  GPIO_InitStructure.GPIO_Pull = DISABLE;
  GPIO_InitStructure.GPIO_HighPwr = DISABLE;
  GPIO_Init(&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = Serial2_Mode;
  GPIO_Init(&GPIO_InitStructure);
    
}

/**
* @brief  ADC_Configuration.
* @param  None
* @retval None
*/
void ADC_Configuration(void)
{ 
  /* Enable ADC clock */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_ADC, ENABLE);
  
#if (FS == 16000)
  xADC_InitType.ADC_OSR = ADC_OSR_100;
#elif (FS == 8000)
  xADC_InitType.ADC_OSR = ADC_OSR_200;
#endif
  
  xADC_InitType.ADC_Input = ADC_Input_Microphone;
  xADC_InitType.ADC_ConversionMode = ADC_ConversionMode_Continuous;
  xADC_InitType.ADC_ReferenceVoltage = ADC_ReferenceVoltage_0V6;
  xADC_InitType.ADC_Attenuation = ADC_Attenuation_0dB;
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
  NVIC_InitType NVIC_InitStructure;
  
  /* Configure DMA peripheral */
  SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_DMA, ENABLE);
    
  /* Configure DMA TX channel */
    if(ADC->CONF_b.SKIP == 0) {
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC->DATA_CONV_MSB);
  }
  /* Conversion done with filter not bypassed */
  else {
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC->DATA_CONV_LSB); // *1.08 TODO in the filter
  }
  
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)PCM_Buffer;  
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = (uint32_t)(PCM_BUFFER_SIZE);  
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;    
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    
  DMA_Init(DMA_CH0, &DMA_InitStructure);
  
  /* Enable DMA_CH_UART_TX Transfer Complete interrupt */
  DMA_FlagConfig(DMA_CH0, DMA_FLAG_TC | DMA_FLAG_HT, ENABLE);

  /* Select DMA ADC CHANNEL 0 */
  DMA_SelectAdcChannel(DMA_ADC_CHANNEL0, ENABLE);
  
  /* Enable ADC DMA requests */
  DMA_Cmd(DMA_CH0, ENABLE);
  ADC_DmaCmd(ENABLE); 
  
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = DMA_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = HIGH_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
  
}


volatile uint8_t AudioVolume = 10;

typedef struct 
{
  int32_t Z; 
  int32_t oldOut; 
  int32_t oldIn; 
}HP_FilterState_TypeDef;

HP_FilterState_TypeDef HP_Filter;

#define SaturaLH(N, L, H) (((N)<(L))?(L):(((N)>(H))?(H):(N)))

/**
* @brief  DMA Transfer Complete Callback.
* @param  None
* @retval None
*/
void TC_IT_Callback(void)
{
  uint16_t nBytes, nPCMs = 0;
  
  for ( nBytes = 0; nBytes< (FS/1000)*2; nBytes++)  /* FS/1000 PCM samples(16 bit) -> (FS/1000)*2 in Bytes */
  {
    if((nBytes%2)==0) {
      HP_Filter.Z = PCM_Buffer[nPCMs + PCM_BUFFER_SIZE/2] * AudioVolume;
      HP_Filter.oldOut = (0xFC * (HP_Filter.oldOut +  HP_Filter.Z - HP_Filter.oldIn)) / 256;
      HP_Filter.oldIn = HP_Filter.Z;
      PCM_Buffer[nPCMs + PCM_BUFFER_SIZE/2] = SaturaLH(HP_Filter.oldOut, -32760, 32760);
      nPCMs++;
    }
    /* Send the PDM data through UART */
    UART_SendData( ((uint8_t *)PCM_Buffer)[nBytes + (FS/1000)*2]); /* FS/1000 PCM samples(16 bit) -> (FS/1000)*2 in Bytes */
    while (UART_GetFlagStatus(UART_FLAG_TXFE) == RESET);
  }
    
}

/**
* @brief  DMA Half Transfer Callback.
* @param  None
* @retval None
*/
void HT_IT_Callback(void)
{
  uint16_t nBytes, nPCMs = 0;
  
  for ( nBytes = 0; nBytes<(FS/1000)*2; nBytes++)  /* FS/1000 PCM samples(16 bit) -> (FS/1000)*2 in Bytes */
  {
    if((nBytes%2)==0) {
      HP_Filter.Z = PCM_Buffer[nPCMs] * AudioVolume;
      HP_Filter.oldOut = (0xFC * (HP_Filter.oldOut +  HP_Filter.Z - HP_Filter.oldIn)) / 256;
      HP_Filter.oldIn = HP_Filter.Z;
      PCM_Buffer[nPCMs] = SaturaLH(HP_Filter.oldOut, -32760, 32760);
      nPCMs++;
    }
    /* Send the PDM data through UART */
    UART_SendData(((uint8_t *)PCM_Buffer)[nBytes]); /* FS/1000 PCM samples(16 bit) -> (FS/1000)*2 in Bytes */
    while (UART_GetFlagStatus(UART_FLAG_TXFE) == RESET);
  }
  
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
