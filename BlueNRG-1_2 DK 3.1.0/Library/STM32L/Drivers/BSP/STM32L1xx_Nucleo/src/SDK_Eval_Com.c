/**
 * @file    SDK_EVAL_Io.c
 * @author  AMS VMA RF application team
 * @version V1.0.1
 * @date    March,17 2015
 * @brief   This file provides all the low level API to manage SDK I/O ports.
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
 * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
 */


/* Includes ------------------------------------------------------------------*/
#include "SDK_EVAL_Config.h"


#ifdef ENABLE_USB
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h" 
#include "usbd_cdc_interface.h"
#endif 

#include <stdio.h>
#include <yfuns.h>

/** @addtogroup SDK_EVAL_STM32L
 * @{
 */


/** @addtogroup SDK_EVAL_Io
 * @{
 */


/** @defgroup SDK_EVAL_Io_Private_TypesDefinitions             SDK EVAL IO Private Types Definitions
 * @{
 */

/**
 * @brief  IO_Receive_Data User callback 
 */
typedef struct _IO_ReceiveCallBack
{
  /**
   * callback to notify the applicatio with the received I/O data
   */ 
  IO_RECEIVE_DATA_USER_CALLBACK_TYPE IO_user_application;
} IO_ReceiveCallBack;
/**
 * @}
 */


/** @defgroup SDK_EVAL_Io_Private_Defines                      SDK EVAL IO Private Defines
 * @{
 */
#define READ_DATA_SIZE      1024
#define READ_BUFFER_IS_EMPTY() (Read_ptr_in == Read_ptr_out)

#ifdef ENABLE_USART
#define UART_TX_TIMEOUT 5 
#endif 

/**
 * @}
 */


/** @defgroup SDK_EVAL_Io_Private_Macros                       SDK EVAL IO Private Macros
 * @{
 */

/* IO User Callback used to handle received data from I/O channel*/
static IO_ReceiveCallBack IO_Receive_Data;

/**
 * @}
 */

/** @defgroup SDK_EVAL_Io_Private_Variables                    SDK EVAL IO Private Variables
 * @{
 */

static uint8_t  Read_Buffer [READ_DATA_SIZE]; 
static uint32_t Read_ptr_in = 0;
static uint32_t Read_ptr_out = 0;

UART_HandleTypeDef xUsartInit;
uint8_t read_data; 

#ifdef ENABLE_USB
void USB_Send_Data(uint8_t byte);
#endif

/**
 * @}
 */


/** @defgroup SDK_EVAL_Io_Private_FunctionPrototypes                   SDK EVAL IO Private Function Prototypes
 * @{
 */

/**
 * @}
 */


/** @defgroup SDK_EVAL_IO_Private_Functions                            SDK EVAL IO Private Functions
 * @{
 */

#ifdef ENABLE_USART

/**
 * @brief  Configures Uart port.
 * @param  xCom Specifies the COM port to be configured.
 * @retval None.
 */
static void SdkEvalComUart_Init(uint32_t baudrate)
{
//  GPIO_InitTypeDef GPIO_InitStructure;
  
  
  /* Enable GPIO clock */
  UART_CLOCK();
//  UART_TX_CLOCK();
//  UART_RX_CLOCK();
  xUsartInit.Instance      = UART_PORT;
  HAL_UART_DeInit(&xUsartInit);
  
  xUsartInit.Init.BaudRate   = baudrate;
  xUsartInit.Init.WordLength = UART_WORDLENGTH_8B;
  xUsartInit.Init.StopBits   = UART_STOPBITS_1;
  xUsartInit.Init.Parity     = UART_PARITY_NONE;
  xUsartInit.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  xUsartInit.Init.Mode       = UART_MODE_TX_RX;
  
//  /* Configure USART Tx as alternate function push-pull */
//  GPIO_InitStructure.Pin       = UART_TX_PIN;
//  GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
//  GPIO_InitStructure.Pull      = GPIO_PULLUP;
//  GPIO_InitStructure.Speed     = GPIO_SPEED_HIGH;
//  GPIO_InitStructure.Alternate = UART_TX_AF;
//  HAL_GPIO_Init(UART_TX_PORT, &GPIO_InitStructure);
//  
//  /* Configure USART Rx as input floating */
//  GPIO_InitStructure.Pin = UART_RX_PIN;
//  GPIO_InitStructure.Alternate = UART_RX_AF;
//  HAL_GPIO_Init(UART_RX_PORT, &GPIO_InitStructure);
  
//  /* Configure the NVIC for SPI */  
//  HAL_NVIC_SetPriority(UART_IRQ, 4, 0); //TBR Which priority < spi
//  HAL_NVIC_EnableIRQ(UART_IRQ);
  
  /* USART configuration */
  HAL_UART_Init(&xUsartInit);  
  
  /* Enable the USARTx Receive interrupt: this interrupt is generated when the
  USARTx receive data register is not empty */
  HAL_UART_Receive_IT(&xUsartInit, &read_data, 1);
}/* end SdkEvalUart_Init() */

/**
 * @brief  Configures Uart port.
 * @param  xCom Specifies the COM port to be configured.
 * @retval None.
 */
static void SdkEvalUart_Init(SdkEvalCom xCom)
{
  GPIO_InitTypeDef GPIO_InitStructure;


  /* Enable GPIO clock */
  if(xCom==COM1) {
    UART_CLOCK();
    UART_TX_CLOCK();
    UART_RX_CLOCK();
    xUsartInit.Instance      = UART_PORT;
    HAL_UART_DeInit(&xUsartInit);
  
    xUsartInit.Init.BaudRate   = UART_BAUDRATE;
    xUsartInit.Init.WordLength = UART_WORDLENGTH_8B;
    xUsartInit.Init.StopBits   = UART_STOPBITS_1;
    xUsartInit.Init.Parity     = UART_PARITY_NONE;
    xUsartInit.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    xUsartInit.Init.Mode       = UART_MODE_TX_RX;
  }
  
  /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStructure.Pin       = UART_TX_PIN;
  GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull      = GPIO_PULLUP;
  GPIO_InitStructure.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStructure.Alternate = UART_TX_AF;
  HAL_GPIO_Init(UART_TX_PORT, &GPIO_InitStructure);
  
  /* Configure USART Rx as input floating */
  GPIO_InitStructure.Pin = UART_RX_PIN;
  GPIO_InitStructure.Alternate = UART_RX_AF;
  HAL_GPIO_Init(UART_RX_PORT, &GPIO_InitStructure);
   
  /* Configure the NVIC for SPI */  
  HAL_NVIC_SetPriority(UART_IRQ, 2, 0); //TBR Which priority < spi
  HAL_NVIC_EnableIRQ(UART_IRQ);
  
   /* USART configuration */
  HAL_UART_Init(&xUsartInit);  
  
  /* Enable the USARTx Receive interrupt: this interrupt is generated when the
  USARTx receive data register is not empty */
  HAL_UART_Receive_IT(&xUsartInit, &read_data, 1);
}/* end SdkEvalUart_Init() */


/**
 * @brief  Wait for serial data tx end
 * @param  None.
 * @retval None.
 */
static void SdkEvalUart_Wait_Send_Data()  
{	
  while(!(__HAL_UART_GET_FLAG(&xUsartInit, UART_FLAG_TXE)));
}/* end SdkEvalUart_Wait_Send_Data() */




/**
 * @brief  Send a byte on Serial port
 * @param  tx_data byte to be sent
 * @retval None.
 */
static void SdkEvalUart_Send_Data(uint8_t tx_data)
{
  /* wait for the transmission end */
  SdkEvalUart_Wait_Send_Data();
  /* send the data */
  HAL_UART_Transmit(&xUsartInit, &tx_data, 1, UART_TX_TIMEOUT);
}/* end SdkEvalUart_Send_Data() */


/**
 * @brief  Serial IRQ handler body to be put on user USARTx_IRQHandler() (stm32l1xx_it.c)
 * @param  None
 * @retval None.
 */
void SdkEval_IO_Uart_Irq_Handler(void)
{
  HAL_UART_IRQHandler(&xUsartInit);
}/* SdkEval_IO_Uart_Irq_Handler() */

/**
 * @brief  Configure the selected I/O communication channel
 * @param  None.
 * @retval None.
 */
void SdkEvalComUartInit(uint32_t baudrate)
{
  /* Init Serial Uart */
  SdkEvalComUart_Init(baudrate);
  
}/* end SdkEval_IO_Config() */

#endif /* #ifdef ENABLE_USART */

/**
 * @brief  Configure the selected I/O communication channel
 * @param  None.
 * @retval None.
 */
void SdkEval_IO_Config(IO_RECEIVE_DATA_USER_CALLBACK_TYPE io_receive_function)
{
    IO_Receive_Data.IO_user_application = io_receive_function;
#if defined (ENABLE_USB)
   /* Init Device Library */
  USBD_Init(&USBD_Device, &VCP_Desc, 0);
  
  /* Add Supported Class */
  USBD_RegisterClass(&USBD_Device, USBD_CDC_CLASS);
  
  /* Add CDC Interface Class */
  USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);
  
  /* Start Device Process */
  USBD_Start(&USBD_Device);
  
#elif defined (ENABLE_USART)
  /* Init Serial Uart */
  SdkEvalUart_Init(COM1);
#endif 

  
}/* end SdkEval_IO_Config() */

/**
 * @brief  Send a byte on selected I/O port
 * @param  tx_data byte to be sent
 * @retval None.
 */
void SdkEval_IO_Send_Data(uint8_t tx_data)
{
#if defined (ENABLE_USB)
  /* Call USB send data API on hw_config.c */
  USB_Send_Data(tx_data); 
#elif defined (ENABLE_USART)
  SdkEvalUart_Send_Data(tx_data);
#endif
}/* end SdkEval_IO_Send_Data() */

/**
 * @brief  Receive data on selected I/O port
 * @param  rx_data pointer to buffer of received
 * @param  data_size number of received data 
 * @retval None.
 */
void SdkEval_IO_Receive_Data(uint8_t * rx_data, uint16_t data_size)
{
  if(IO_Receive_Data.IO_user_application)
    IO_Receive_Data.IO_user_application(rx_data,data_size);
  
}/* end SdkEval_IO_Receive_Data() */

void Read_Buffer_Push(uint8_t byte)
{
    Read_Buffer[Read_ptr_in] = byte;
    
    Read_ptr_in++;
    
    /* To avoid buffer overflow */
    if(Read_ptr_in == READ_DATA_SIZE)
    {
        Read_ptr_in = 0;
    }        
}
int Read_Buffer_Pop(uint8_t *byte)
{
    if(READ_BUFFER_IS_EMPTY())
        return 0;
    
    *byte = Read_Buffer[Read_ptr_out];
    
    Read_ptr_out++;
    
    if(Read_ptr_out == READ_DATA_SIZE)
        Read_ptr_out = 0;  
    
    return 1;
}


void SdkEval_IO_processInputData(uint8_t * rx_data, uint16_t data_size)
{
  for(int i = 0; i < data_size; i++){
      Read_Buffer_Push(rx_data[i]);
  }
}

#undef putchar

int putchar(int c)
{
  SdkEval_IO_Send_Data((uint8_t) c);
  return c;
}

void __io_putchar(char c)
{
  putchar(c);
}

size_t _write(int handle, const unsigned char * buffer, size_t size)
{
  size_t nChars = 0;

  if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR) {
    return _LLIO_ERROR;
  }

  if (buffer == 0) {
    // This means that we should flush internal buffers.
    //spin until TX complete (TX is idle)
#ifdef ENABLE_USB
    while (!USB_OUT_BUFFER_IS_EMPTY()) {}   
#endif // ENABLE_USB
    return 0;
  }

  while(size--) {
    __io_putchar(*buffer++);
    ++nChars;
  }

  return nChars;
}


size_t __read(int handle, unsigned char * buffer, size_t size)
{
  size_t bytes = 0;
  
  while(bytes < size && Read_Buffer_Pop(buffer+bytes)){
    bytes++;
  }
  return bytes;
}

uint8_t __io_getcharNonBlocking(uint8_t *data)
{
  if (__read(_LLIO_STDIN,data,1))
    return 1;
  else
    return 0;
}/* end serialReadByte() */

/**
 * @}
 */


/**
 * @}
 */


/**
 * @}
 */


/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
