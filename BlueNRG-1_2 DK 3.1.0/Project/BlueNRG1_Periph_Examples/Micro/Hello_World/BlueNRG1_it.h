/**
******************************************************************************
* @file    UART/Dma/BlueNRG1_it.h 
* @author  RF Application Team
* @version V1.0.0
* @date    September-2015
* @brief   This file contains the headers of the interrupt handlers.
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
* <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
******************************************************************************
*/ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BlueNRG1_IT_H
#define BlueNRG1_IT_H

/* Includes ------------------------------------------------------------------*/

/* Exported defines ----------------------------------------------------------*/
#define DMA_CH_UART_TX                  DMA_CH1
#define DMA_FLAG_TC_UART_TX               DMA_FLAG_TC1

#define DMA_CH_UART_RX                  DMA_CH0
#define DMA_FLAG_TC_UART_RX               DMA_FLAG_TC0

#define UART_BUFFER_SIZE        100

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* BlueNRG1_IT_H */

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
