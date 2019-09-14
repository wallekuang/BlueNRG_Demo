/**
******************************************************************************
* @file    DTM_Updater.c
* @author  VMA RF Application Team
* @version V1.0.0
* @date    July-2015
* @brief   Transport layer file: DON'T MODIFY IT.
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

/* Includes ------------------------------------------------------------------*/
#include "BlueNRG_x_device.h"
#include "DTM_Updater.h"
#include "DTM_Updater_Config.h"
#include "ble_status.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum {
  WAITING_TYPE,
  WAITING_OPCODE_1,
  WAITING_OPCODE_2,
  WAITING_CMDCODE,
  WAITING_CMD_LEN1,
  WAITING_CMD_LEN2,
  WAITING_PARAM_LEN,
  WAITING_HANDLE,
  WAITING_HANDLE_FLAG,
  WAITING_DATA_LEN1,
  WAITING_DATA_LEN2,
  WAITING_PAYLOAD
}hci_state;

typedef PACKED(struct) _hci_acl_hdr{
  uint16_t	handle;		/* Handle & Flags(PB, BC) */
  uint16_t	dlen;
} hci_acl_hdr;

typedef  PACKED(struct) devConfigS  {
  uint8_t  HS_crystal;
  uint8_t  LS_source;
  uint8_t  SMPS_management;
  uint8_t  Reserved;
  uint16_t HS_startup_time;
  uint16_t SlaveSCA;
  uint8_t  MasterSCA;
  uint32_t max_conn_event_length;
  uint8_t  Test_mode;
} devConfig_t;

/* Private define ------------------------------------------------------------*/
#ifdef UART_INTERFACE
#define CMD_BUFF_OFFSET 1
#define EVT_BUFF_OFFSET 0
#endif
#ifdef SPI_INTERFACE
#define CMD_BUFF_OFFSET 6
#define EVT_BUFF_OFFSET 4
#endif

#define FLASH_PREMAP_MAIN    0x08

#define DMA_DIR_PeripheralDST              ((uint32_t)0x00000010)
#define DMA_Priority_Medium                ((uint32_t)0x00001000)
#define DMA_Mode_Circular                  ((uint32_t)0x00000020)
#define DMA_Priority_High                  ((uint32_t)0x00002000)
#define DMA_MemoryInc_Enable               ((uint32_t)0x00000080)

#define Serial1_Mode                ((uint8_t)1)  /*!< Serial1 mode selected */
#define Serial0_Mode                ((uint8_t)4)  /*!< Serial0 mode selected */

#define DMA_CH_SPI_TX           (DMA_CH5)       /* SPI TX DMA channels */
#define DMA_CH_SPI_RX           (DMA_CH4)       /* SPI RX DMA channels */

#define SPI_DMAReq_Tx           ((uint8_t)0x04)  /*!< DMA SPI TX request */
#define SPI_DMAReq_Rx           ((uint8_t)0x01)  /*!< DMA SPI RX request */

#define SPI_Mode_Slave          ((uint8_t)0x01)  /*!< Slave mode */
#define SPI_CPOL_Low            ((uint8_t)0)  /*!< Clock polarity low */
#define SPI_CPHA_2Edge          ((uint8_t)1)  /*!< Clock phase 2nd edge */
#define SPI_DataSize_8b         ((uint8_t)0x07)  /*!< 8-bit data size */
#define SPI_FLAG_RNE            ((uint8_t)0x04)  /*!< Rx FIFO not empty flag */
#define CCR_CLEAR_MASK           ((uint32_t)0xFFFF800F)  


#define GPIO_Pin_0               ((uint16_t)0x0001)  /*!< Pin 0 selected */
#define GPIO_Pin_2               ((uint16_t)0x0004)  /*!< Pin 2 selected */
#define GPIO_Pin_3               ((uint16_t)0x0008)  /*!< Pin 3 selected */
#define GPIO_Pin_7               ((uint16_t)0x0080)  /*!< Pin 7 selected */
#define GPIO_Pin_8               ((uint16_t)0x0100)  /*!< Pin 8 selected */
#define GPIO_Pin_11              ((uint16_t)0x0800)  /*!< Pin 11 selected */

#define SPI_CS_PIN              (GPIO_Pin_11)
#define SPI_MOSI_PIN            (GPIO_Pin_2)
#define SPI_MISO_PIN            (GPIO_Pin_3)
#define SPI_CLCK_PIN            (GPIO_Pin_0)
#define GPIO_MODE_SPI           (Serial0_Mode)
#define SPI_IRQ_PIN             (GPIO_Pin_7)

#define GPIO_Input               ((uint8_t)0)  /*!< GPIO input selected */

#define UART_RX_PIN             (GPIO_Pin_11)
#define UART_TX_PIN             (GPIO_Pin_8)
#define GPIO_MODE_UART          (Serial1_Mode)

#define UART_FLAG_TXFE           ((uint16_t)0x0080)  /*!< Transmit FIFO empty flag */
#define UART_FLAG_BUSY           ((uint16_t)0x0008)  /*!< Busy flag */
#define UART_IT_RX               ((uint16_t)0x0010)  /*!< Receive interrupt */

#define UART_WORDLENGTH_8B      ((uint8_t)3)

#define UART_DR_ADDRESS        (UART_BASE)

#define DMA_CH_UART_TX           (DMA_CH1)       /* UART TX DMA channels */

#define UART_DMAReq_Tx           ((uint8_t)0x02)  /*!< DMA UART TX request */

#define DMA_FLAG_TC_UART_TX      ((uint32_t)0x00000020)

#define ROM_FLASH_WRITE           ((uint32_t)0x100000FD)  /* Bootloader function prototype */
#define ROM_FLASH_ERASE           ((uint32_t)0x10000193)  /* Bootloader function prototype */


/* SPI protocol variables & definitions */
#define SPI_HEADER_LEN          (uint8_t)(4)
#define SPI_CTRL_WRITE          (uint8_t)(0x0A)
#define SPI_CTRL_READ           (uint8_t)(0x0B)


#define UPDATER_BUF_SIZE        ((uint16_t)64)
#define UPDATER_VERSION         ((uint8_t)6)
#define BLUEFLAG_FAILED_OP      ((uint8_t)0x4A)

#define CMD_UPDATER_REBOOT              ((uint8_t)0x21)
#define CMD_UPDATER_GET_VERSION         ((uint8_t)0x22)
#define CMD_UPDATER_GET_BUFSIZE         ((uint8_t)0x23)
#define CMD_UPDATER_ERASE_BLUEFLAG      ((uint8_t)0x24)
#define CMD_UPDATER_RESET_BLUEFLAG      ((uint8_t)0x25)
#define CMD_UPDATER_ERASE_SECTOR        ((uint8_t)0x26)
#define CMD_UPDATER_PROG_DATA_BLOCK     ((uint8_t)0x27)
#define CMD_UPDATER_READ_DATA_BLOCK     ((uint8_t)0x28)
#define CMD_UPDATER_CALC_CRC            ((uint8_t)0x29)
#define CMD_UPDATER_READ_HW_VERS        ((uint8_t)0x2A)

#define COMMAND_COMPLETE_EVENT          ((uint8_t)0x0E)
#define COMMAND_STATUS_EVENT            ((uint8_t)0x0F)

#define DMA_IDLE        0
#define DMA_IN_PROGRESS 1

/* HCI Packet types */
#define HCI_COMMAND_PKT		0x01
#define HCI_ACLDATA_PKT		0x02
#define HCI_SCODATA_PKT		0x03
#define HCI_EVENT_PKT		0x04
#define HCI_VENDOR_PKT		0xFF

#define HCI_TYPE_OFFSET                 0
#define HCI_VENDOR_CMDCODE_OFFSET       1
#define HCI_VENDOR_LEN_OFFSET           2
#define HCI_VENDOR_PARAM_OFFSET         4

#define HCI_HDR_SIZE 1

/* Private macro -------------------------------------------------------------*/
#define SPI_READ_CS()           READ_BIT(GPIO->DATA, SPI_CS_PIN)
#define SPI_LOW_IRQ()           CLEAR_BIT(GPIO->DATA, SPI_IRQ_PIN)
#define SPI_HIGH_IRQ()          SET_BIT(GPIO->DATA, SPI_IRQ_PIN)

#define SPI_SENDATA(VAL)        {SPI->DR = VAL;}

#define SPI_CLEAR_TXFIFO()      { uint32_t tmp; \
SET_BIT(SPI->ITCR, SET<<1); \
  while(0 == SPI->SR_b.TFE) tmp |= SPI->TDR; \
    CLEAR_BIT(SPI->ITCR, SET<<1);}


/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

static uint32_t updater_calc_crc(uint32_t address, uint32_t nr_of_sector);
static void DMA_Rearm(DMA_CH_Type* DMAy_Channelx, uint32_t buffer, uint32_t size);


/* Private functions ---------------------------------------------------------*/

// x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 +
// x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x^1 + x^0
#define CRC_POLY        0x04C11DB7      // the poly without the x^32

static uint32_t updater_calc_crc(uint32_t address, uint32_t nr_of_sector)
{
  uint32_t nr_of_bytes;
  uint32_t i, j, a1;
  uint32_t crc;
  
  nr_of_bytes = nr_of_sector * 0x800;
  crc = 0;
  for (i = 0; i < nr_of_bytes; i += 4) {
    crc = crc ^ (*((uint32_t *)(address + i)));
    for (j = 0; j < 32; j ++) {
      a1 = (crc >> 31) & 0x1;
      crc = (crc << 1) ^ (a1 * CRC_POLY);
    }
  }
  return crc;
}

static void DMA_Rearm(DMA_CH_Type* DMAy_Channelx, uint32_t buffer, uint32_t size)
{
  /* DMA_CH_SPI_TX reset */
  CLEAR_BIT(DMAy_Channelx->CCR, SET);
  
  /* Rearm the DMA_CH_SPI_TX */
  DMAy_Channelx->CMAR = buffer;
  DMAy_Channelx->CNDTR = size;
  
  /* DMA_CH_SPI_TX enable */
  SET_BIT(DMAy_Channelx->CCR, SET);
  
}

#ifdef UART_INTERFACE
static void DTM_SystemInit(void)
{
  uint32_t sysclk_config, fsm_status;
  uint8_t cnt_time;
  
#ifdef SYSCLOCK_32MHZ
  /* External crystal clock usage enabled */
  sysclk_config = 0x000443A02U;
#else
  /* External crystal clock usage enabled */
  sysclk_config = 0x000443A02U;
#endif
  *((volatile uint32_t *)0x4800000CU) = 0x10000U | (uint16_t)(((uint32_t)(&sysclk_config)) & 0x0000FFFFU);
  while((*((volatile uint32_t *)0x4800000CU) & 0x10000) != 0);
  
#ifdef SYSCLOCK_32MHZ
  /* Set 32MHz_SEL bit in the System controller register */
  SYSTEM_CTRL->CTRL = 0x01;
#endif 
  
  /* Switch to active2 FSM enabling the external crystal*/
  sysclk_config = 0x0001B3402U;
  *((volatile uint32_t *)0x4800000CU) = 0x10000U | (uint16_t)(((uint32_t)(&sysclk_config)) & 0x0000FFFFU);
  while((*((volatile uint32_t *)0x4800000CU) & 0x10000) != 0);
  
  /* Read FSM Status Register to see if the radio is in ACTIVE2 state*/
  cnt_time = 0;
  do {
    cnt_time++;
    fsm_status = 0x00004382U;
    *((volatile uint32_t *)0x4800000CU) = 0x10000U | (uint16_t)(((uint32_t)(&fsm_status)) & 0x0000FFFFU);
    while((*((volatile uint32_t *)0x4800000CU) & 0x10000) != 0);
  } while(((fsm_status>>16) != 0x05) && (cnt_time<100));
  
#ifdef SYSCLOCK_32MHZ  
  /* AHB up converter command register write*/
  AHBUPCONV->COMMAND = 0x15;
#endif
}
#endif


#define UART_115200

/**
* @brief  updater Init.
* @param  None
* @retval None
*/
void updater_init(void)
{  
  uint32_t tmpreg = 0;
  
  /* Remap the vector table */
  FLASH->CONFIG = FLASH_PREMAP_MAIN;
  
#ifdef SPI_INTERFACE
  
  CKGEN_SOC->CLOCK_EN = 0x1C073;
    
  /** Configure GPIO pins for SPI.
  * GPIO_Pin_11 = CS
  * GPIO_Pin_0 = CLK
  * GPIO_Pin_2 = MOSI
  * GPIO_Pin_3 = MISO
  */
  GPIO->MODE0 = (GPIO_MODE_SPI | GPIO_MODE_SPI<<8 | GPIO_MODE_SPI<<12);
  GPIO->MODE1 = (Serial1_Mode<<4 | Serial1_Mode<<8 | GPIO_MODE_SPI<<12);
  
  /* SPI PORT GPIO_Pull DISABLE */
  CLEAR_BIT(GPIO->DS, (SPI_CLCK_PIN | SPI_MOSI_PIN | SPI_MISO_PIN | SPI_CS_PIN | SPI_IRQ_PIN));
  
  /* GPIO_Pin_7 = IRQ Set GPIO mode OUTPUT */
  SET_BIT(GPIO->OEN, SPI_IRQ_PIN);
  SPI_LOW_IRQ();
  
  /* Set in SPI MODE */
  MODIFY_REG(SPI->CR0, 0x60ffdf, (SPI_CPHA_2Edge<<7 | SPI_CPOL_Low<<6 | SPI_DataSize_8b));
  SET_BIT(SPI->CR1, (SPI_Mode_Slave<<2) | (SET<<1));
    
  /* Configure DMA SPI TX channel */
  tmpreg = DMA_CH_SPI_TX->CCR;
  tmpreg &= CCR_CLEAR_MASK;
  tmpreg |= (DMA_DIR_PeripheralDST | DMA_MemoryInc_Enable |                 
             DMA_Priority_Medium );
  DMA_CH_SPI_TX->CCR = tmpreg;
  DMA_CH_SPI_TX->CPAR = (SPI_BASE + 0x08);
  
  /* Configure DMA SPI RX channel */
  tmpreg = DMA_CH_SPI_RX->CCR;
  tmpreg &= CCR_CLEAR_MASK;
  tmpreg |= (DMA_Mode_Circular | DMA_MemoryInc_Enable |
             DMA_Priority_High);
  DMA_CH_SPI_RX->CCR = tmpreg;
  DMA_CH_SPI_RX->CPAR = (SPI_BASE + 0x08);
  
  /* Enable the SPI RX DMA channel */
  SET_BIT(DMA_CH_SPI_RX->CCR, SET);
  
  /* Enable SPI_TX/SPI_RX DMA requests */
  SET_BIT(SPI->DMACR, (SPI_DMAReq_Tx | SPI_DMAReq_Rx));
#endif
  
#ifdef UART_INTERFACE
  DTM_SystemInit();
  CKGEN_SOC->CLOCK_EN = 0x1C06B;
  
  /** Configure GPIO pins for UART.
  * GPIO_Pin_11 = UART_RXD
  * GPIO_Pin_8 = UART_TXD
  */  
  MODIFY_REG(GPIO->MODE1, (0xF | 0xF<<12), (GPIO_MODE_UART | GPIO_MODE_UART<<12));
  
  /* UART PORT GPIO_Pull DISABLE */
  CLEAR_BIT(GPIO->DS, (UART_RX_PIN | UART_TX_PIN));
  
  /* Set in UART MODE */
#ifdef UART_115200
  // divider = ((16000000<<7) / (16 * 115200))
  UART->IBRD = 8;     // divider >> 7
  UART->FBRD = 44;   //  ((divider - ((divider >> 7) <<7) + 1) >> 1)
#else // UART_921600
  // divider = ((16000000<<7) / (16 * 921600))
  UART->IBRD = 1;     // divider >> 7
  UART->FBRD = 5;   //  ((divider - ((divider >> 7) <<7) + 1) >> 1)
#endif

  SET_BIT(UART->LCRH_TX, UART_WORDLENGTH_8B<<5);
  SET_BIT(UART->LCRH_RX, (UART_WORDLENGTH_8B<<5 | SET<<4));
  CLEAR_BIT(UART->IFLS, 0x07<<3);
  
  /* Enable UART */
  SET_BIT(UART->CR, SET);
    
  /* Configure DMA UART TX channel */
  tmpreg = DMA_CH_UART_TX->CCR;
  tmpreg &= CCR_CLEAR_MASK;
  tmpreg |= (DMA_DIR_PeripheralDST | DMA_MemoryInc_Enable |                 
             DMA_Priority_High );
  DMA_CH_UART_TX->CCR = tmpreg;
  DMA_CH_UART_TX->CPAR = UART_DR_ADDRESS;
  
  /* Enable SPI_TX/SPI_RX DMA requests */
  SET_BIT(UART->DMACR, UART_DMAReq_Tx);
#endif
}

#define FLASH_WORD_LEN       4
#define FLASH_INT_CMDDONE    0x01
#define FLASH_CMD_WRITE      0x33

#define FLASH_REG_IRQMASK ((volatile uint32_t *)0x4010000C)
#define FLASH_REG_IRQRAW  ((volatile uint32_t *)0x40100010)
#define FLASH_REG_ADDRESS ((volatile uint32_t *)0x40100018)
#define FLASH_REG_DATA    ((volatile uint32_t *)0x40100040)
#define FLASH_REG_COMMAND ((volatile uint32_t *)0x40100000)
#define FLASH_REG_COMMAND ((volatile uint32_t *)0x40100000)

static uint32_t flash_Write(uint32_t address, uint8_t *data, uint32_t writeLength)
{
  volatile uint32_t read_back;
  uint32_t index, flash_data, totalLen;
  uint8_t flash_word[4], i;

  // Start and end address must be in MFB.
//  if ((addressArea(address) != AREA_MFB) || 
//      (addressArea(address + writeLength - 1) != AREA_MFB)) {
//    return ERR_INVALID_ADDRESS;
//  }

  totalLen = writeLength;
  // Total length multiple of FLASH_WORD_LEN
  if (writeLength%FLASH_WORD_LEN)
    totalLen += FLASH_WORD_LEN - (writeLength%FLASH_WORD_LEN);

  *FLASH_REG_IRQMASK = 0;
  for (index=0; index<totalLen; index+=FLASH_WORD_LEN) {
    for (i=0; i<FLASH_WORD_LEN; i++) {
      if ((index+i) < writeLength)
        flash_word[i] = data[index+i];
      else
        flash_word[i] = 0xFF;
    }

    // Clear the IRQ flags
    *FLASH_REG_IRQRAW = 0x0000003F;
    // Load the flash address to write
    *FLASH_REG_ADDRESS = (uint16_t)((address + index) >> 2);
    // Prepare and load the data to flash
    flash_data = LE_TO_HOST_32(flash_word);
    *FLASH_REG_DATA = flash_data;
    // Flash write command
    *FLASH_REG_COMMAND = FLASH_CMD_WRITE;
    // Wait the end of the flash write command
    while ((*FLASH_REG_IRQRAW & FLASH_INT_CMDDONE) == 0);
    // Verify if the data is written in a correct way
    read_back = *((uint32_t *)(address+index));
    if (read_back != flash_data) {
      return ERROR;
    }
  }

  return SUCCESS;
}


#define FLASH_SIZE_REG 0x40100014

#define BYTE_0(n) (uint8_t)((n) & 0xFF)
#define MFB_MASS_ERASE 0x01
#define MFB_PAGE_ERASE 0x02
#define FLASH_CMD_ERASE_PAGE 0x11
#define FLASH_CMD_MASSERASE  0x22
#define MFB_BOTTOM          (0x10040000)
#define MFB_PAGE_SIZE_B     (2048)
#define MFB_PAGE_SIZE_W     (MFB_PAGE_SIZE_B/4)
#define MFB_SIZE_B          ((64+32*((*(uint32_t *) FLASH_SIZE_REG) & 0x3)) *1024)
#define MFB_SIZE_W          (MFB_SIZE_B/4)

static uint32_t flash_Erase(uint32_t eraseType, uint32_t address)
{
  uint8_t command;
  uint32_t length, i;
  volatile uint32_t *ptr;

  if (BYTE_0(eraseType) == MFB_MASS_ERASE) {
    command = FLASH_CMD_MASSERASE;
    address = MFB_BOTTOM;
    ptr = (volatile uint32_t *)MFB_BOTTOM;
    length = MFB_SIZE_W;
  } else if (BYTE_0(eraseType) == MFB_PAGE_ERASE) {
      command = FLASH_CMD_ERASE_PAGE;
      ptr = (volatile uint32_t *)address;
      length = MFB_PAGE_SIZE_W;
    } else {
      return ERROR;
  }
  
  // Clear the IRQ flags
  *FLASH_REG_IRQRAW = 0x0000003F;
  // Load the flash address to erase
  *FLASH_REG_ADDRESS = (uint16_t)(address >> 2);
  // Flash erase command
  *FLASH_REG_COMMAND = command;
  // Wait the end of the flash erase command
  while ((*FLASH_REG_IRQRAW & FLASH_INT_CMDDONE) == 0);
  // Verify if the flash is erased
  for (i=0; i<length; i++) {
    if (*ptr != 0xFFFFFFFF) {
      return ERROR;
    }
    ptr++;
  }

  return SUCCESS;
}

/**
* @brief  Advance transport layer state machine
* @param  None
* @retval Desired sleep level
*/
void updater(uint8_t reset_event)
{
  uint8_t event_buffer[BUFFER_SIZE];  
  uint8_t command_buffer[BUFFER_SIZE];
  volatile uint8_t command_pending = 0;
  volatile uint8_t event_pending = 0;
  volatile uint8_t reset_pending = 0;
  uint16_t event_buffer_len = 0;
  volatile uint16_t hci_pckt_len = 0;
  
#ifdef SPI_INTERFACE  
  uint8_t cs_prev = SET;
  SpiProtoType spi_proto_state = SPI_PROT_CONFIGURED_STATE;
  
  event_buffer[4] = 0x04;
  event_buffer[5] = 0xFF;
  event_buffer[6] = 0x03;
  event_buffer[7] = 0x01;
  event_buffer[8] = 0x00;
  event_buffer[9] = reset_event;
#endif
#ifdef UART_INTERFACE
  hci_state state = WAITING_TYPE;
  uint8_t dma_state = DMA_IDLE;  
  uint16_t collected_payload_len = 0;
  uint16_t payload_len;
//  hci_acl_hdr *acl_hdr;
  
  event_buffer[0] = 0x04;
  event_buffer[1] = 0xFF;
  event_buffer[2] = 0x03;
  event_buffer[3] = 0x01;
  event_buffer[4] = 0x00;
  event_buffer[5] = reset_event;
#endif
  event_buffer_len = 6;
  event_pending = 1;
  
  
  while(1) {
    /* Check reset pending */
#ifdef SPI_INTERFACE
    if (event_pending==0 && reset_pending) {
#endif
#ifdef UART_INTERFACE
    if (event_pending==0 && reset_pending && dma_state == DMA_IDLE) {
      while(UART->FR != 0x00000090); // UART_FLAG_BUSY = 0 UART_FLAG_TXFE = 1 UART_FLAG_RXFE = 1
#endif
      NVIC_SystemReset();
    }
    
    
#ifdef SPI_INTERFACE
    /* CS pin falling edge - start SPI communication */
    if((SPI_READ_CS() == 0) && (cs_prev==SET)) {
      cs_prev = RESET;
      
      /*if(SPI_STATE_CHECK(SPI_PROT_CONFIGURED_EVENT_PEND_STATE)) {
      SPI_STATE_TRANSACTION(SPI_PROT_WAITING_HEADER_STATE);
    }
      else */
      if(SPI_STATE_CHECK(SPI_PROT_CONFIGURED_STATE)) {
        SPI_CLEAR_TXFIFO();
        SPI_SENDATA(0xFF);
        SPI_STATE_TRANSACTION(SPI_PROT_CONFIGURED_HOST_REQ_STATE);
      }
    }
    /* CS pin rising edge - close SPI communication */
    else if((SPI_READ_CS() != 0) && (cs_prev==RESET)) {
      cs_prev = SET;
      
      if(SPI_STATE_FROM(SPI_PROT_CONFIGURED_EVENT_PEND_STATE)) {
        SPI_LOW_IRQ();
        //        SPI_STATE_TRANSACTION(SPI_PROT_TRANS_COMPLETE_STATE);
        
        /* Pass the number of data received in fifo_command */
        uint16_t rx_buffer_len = (BUFFER_SIZE - ((uint16_t)(DMA_CH_SPI_RX->CNDTR)));        
        if((BUFFER_SIZE - ((uint16_t)(DMA_CH_SPI_RX->CNDTR)))>5) {
          
          /* check ctrl field from command buffer */  
          if(command_buffer[0] == SPI_CTRL_WRITE) {
            command_pending = 1;
          }
          else if(command_buffer[0] == SPI_CTRL_READ) {
            event_pending = 0;
          }
        }
        SPI_STATE_TRANSACTION(SPI_PROT_CONFIGURED_STATE);  
      }
    }
    
    if(SPI_STATE_CHECK(SPI_PROT_CONFIGURED_HOST_REQ_STATE)) {      
      //      transport_layer_receive_data();
      uint8_t data[4] = {(uint8_t)BUFFER_SIZE, (uint8_t)(BUFFER_SIZE>>8), 0, 0};      
      DMA_Rearm(DMA_CH_SPI_RX, (uint32_t)command_buffer, BUFFER_SIZE);
      DMA_Rearm(DMA_CH_SPI_TX, (uint32_t)data, SPI_HEADER_LEN);
      
      SPI_STATE_TRANSACTION(SPI_PROT_WAITING_HEADER_STATE);
      SPI_HIGH_IRQ();
    }
    /* Event queue */
    else if (event_pending!=0 && (SPI_STATE_CHECK(SPI_PROT_CONFIGURED_STATE)) ) {
      SPI_STATE_TRANSACTION(SPI_PROT_WAITING_HEADER_STATE);
      //      transport_layer_send_data(event_buffer, event_buffer_len);
      event_buffer[0] = (uint8_t)BUFFER_SIZE;
      event_buffer[1] = (uint8_t)(BUFFER_SIZE>>8);
      event_buffer[2] = (uint8_t)event_buffer_len;
      event_buffer[3] = (uint8_t)(event_buffer_len>>8);
      
      SPI_CLEAR_TXFIFO();
      SPI_SENDATA(0xFF);
      
      DMA_Rearm(DMA_CH_SPI_RX, (uint32_t)command_buffer, BUFFER_SIZE);
      DMA_Rearm(DMA_CH_SPI_TX, (uint32_t)event_buffer, event_buffer_len+SPI_HEADER_LEN);
      
      SPI_HIGH_IRQ();
    }
    
    while(SPI_STATE_CHECK(SPI_PROT_WAITING_HEADER_STATE)) {
      if( (BUFFER_SIZE - ((uint16_t)(DMA_CH_SPI_RX->CNDTR))) >4) {
        SPI_LOW_IRQ();
        SPI_STATE_TRANSACTION(SPI_PROT_WAITING_DATA_STATE);
        break;
      }
    }
#endif
    
#ifdef UART_INTERFACE
    while ((UART->RIS & UART_IT_RX) != RESET) {
      /* The Interrupt flag is cleared from the FIFO manager */    
      uint8_t data = UART->DR;      
      {        
        if(state == WAITING_TYPE)
          hci_pckt_len = 0;    
        {
          command_buffer[hci_pckt_len++] = data;
          
          if(state == WAITING_TYPE){
            /* Only command or vendor packets are accepted. */
            if(data == HCI_COMMAND_PKT){
              state = WAITING_OPCODE_1;
            }
//            else if(data == HCI_ACLDATA_PKT){
//              state = WAITING_HANDLE;
//            }
            else if(data == HCI_VENDOR_PKT){
              state = WAITING_CMDCODE;
            }
            else{
              /* Incorrect type. Reset state machine. */
              state = WAITING_TYPE;
            }
          }
          else if(state == WAITING_OPCODE_1){
            state = WAITING_OPCODE_2;
          }
          else if(state == WAITING_OPCODE_2){
            state = WAITING_PARAM_LEN;
          }
          else if(state == WAITING_CMDCODE){
            state = WAITING_CMD_LEN1;
          }
          else if(state == WAITING_CMD_LEN1){
            payload_len = data;
            state = WAITING_CMD_LEN2;
          }
          else if(state == WAITING_CMD_LEN2){
            payload_len += data << 8;
            collected_payload_len = 0;
            if(payload_len == 0){
              state = WAITING_TYPE;
              //        packet_received();
              command_pending = 1;           
            }
            else {
              state = WAITING_PAYLOAD;
            }
          }
          else if(state == WAITING_PARAM_LEN){
            payload_len = data;
            collected_payload_len = 0;
            if(payload_len == 0){
              state = WAITING_TYPE;
              //        packet_received();
              command_pending = 1;
            }
            else {
              state = WAITING_PAYLOAD;
            }            
          }
          
          /*** State transitions for ACL packets ***/
//          else if(state == WAITING_HANDLE){
//            state = WAITING_HANDLE_FLAG;
//          }
//          else if(state == WAITING_HANDLE_FLAG){
//            state = WAITING_DATA_LEN1;
//          }
//          else if(state == WAITING_DATA_LEN1){
//            state = WAITING_DATA_LEN2;
//          }
//          else if(state == WAITING_DATA_LEN2){
//            acl_hdr = (void *)&command_buffer[HCI_HDR_SIZE];
//            payload_len = acl_hdr->dlen;
//            collected_payload_len = 0;
//            if(payload_len == 0){
//              state = WAITING_TYPE;
//              //        packet_received();
//              command_pending = 1;
//            }
//            else{
//              state = WAITING_PAYLOAD;
//            }
//          }
          /*****************************************/
          
          else if(state == WAITING_PAYLOAD){
            collected_payload_len += 1;
            if(collected_payload_len >= payload_len){
              /* Reset state machine. */
              state = WAITING_TYPE;
              //        packet_received();
              command_pending = 1;
            }
          }          
        }        
      }     
    }    
#endif
    
    /* Command FIFO */
    if (command_pending==1 && (!reset_pending)) {
      
      /* ======================================================================================= */
      /* Set user events to temporary queue */
#ifdef SPI_INTERFACE
      uint8_t * buff = command_buffer+CMD_BUFF_OFFSET;
#endif
#ifdef UART_INTERFACE
      uint8_t * buff = command_buffer+CMD_BUFF_OFFSET;
#endif      
      
      event_buffer[EVT_BUFF_OFFSET+0] = 0x04;     // HCI_EVENT
      event_buffer[EVT_BUFF_OFFSET+1] = COMMAND_COMPLETE_EVENT;
      event_buffer[EVT_BUFF_OFFSET+2] = 0x04;     // number of bytes in the event
      event_buffer[EVT_BUFF_OFFSET+3] = 0x01;     // 1 HCI_CMD_SLOT 
      event_buffer[EVT_BUFF_OFFSET+4] = buff[0];     // OPCODE_LB
      event_buffer[EVT_BUFF_OFFSET+5] = buff[1];     // OPCODE_HB
      event_buffer[EVT_BUFF_OFFSET+6] = 0x00;     // reason code: command successful
      
      event_pending = 1;
      
      if(buff[1] == 0xFC) {
        uint32_t word_tmp;
        
        if(buff[2]>=4) {
          /* Get the address */
          word_tmp  = ((uint32_t)buff[3]);
          word_tmp |= (((uint32_t)buff[4]) << 8);
          word_tmp |= (((uint32_t)buff[5]) << 16);
          word_tmp |= (((uint32_t)buff[6]) << 24);
        }
        
        switch(buff[0]) {
        case CMD_UPDATER_REBOOT:
          reset_pending = 1;
          break;
          
        case CMD_UPDATER_GET_VERSION:
          event_buffer[EVT_BUFF_OFFSET+2] = 0x05;             // overwrite return data length
          event_buffer[EVT_BUFF_OFFSET+7] = UPDATER_VERSION;
          break;
          
        case CMD_UPDATER_GET_BUFSIZE:
          event_buffer[EVT_BUFF_OFFSET+2] = 0x06;             // overwrite return data length
          event_buffer[EVT_BUFF_OFFSET+7] = (uint8_t)(UPDATER_BUF_SIZE);
          event_buffer[EVT_BUFF_OFFSET+8] = (uint8_t)(UPDATER_BUF_SIZE >> 8);
          break;
          
        case CMD_UPDATER_ERASE_BLUEFLAG:
          word_tmp = BLUE_FLAG_RESET;
          if(flash_Write(BLUE_FLAG_FLASH_BASE_ADDRESS, (uint8_t *)&word_tmp, 4) == 0) {
            event_buffer[EVT_BUFF_OFFSET+6] = BLE_STATUS_SUCCESS;
          }
          else {
            event_buffer[4+6] = BLUEFLAG_FAILED_OP;
          }
          break;
        case CMD_UPDATER_RESET_BLUEFLAG:
          /* Write the blue flag */
          word_tmp = BLUE_FLAG_SET;
          if(flash_Write(BLUE_FLAG_FLASH_BASE_ADDRESS, (uint8_t *)&word_tmp, 4) == 0) {
            event_buffer[EVT_BUFF_OFFSET+6] = BLE_STATUS_SUCCESS;
          }
          else {
            event_buffer[EVT_BUFF_OFFSET+6] = BLUEFLAG_FAILED_OP;
          }
          break;
          
        case CMD_UPDATER_READ_HW_VERS:
          event_buffer[EVT_BUFF_OFFSET+2] = 0x05;               // overwrite return data length
          event_buffer[EVT_BUFF_OFFSET+7] = ((CKGEN_SOC->DIE_ID)>>4)&0x0000000F;
          break;
          
        case CMD_UPDATER_ERASE_SECTOR:
          
          /* Verify the address is inside the Flash range and avoid the first page used by the updater itself */
          event_buffer[EVT_BUFF_OFFSET+6] = FLASH_ERASE_FAILED;
          /* Removed the check of _MEMORY_FLASH_END_ to be compatible with BlueNRG-2 (that has an increased flash size
           * Otherwise, the project should be splitted BlueNRG-1 / BlueNRG-2. So, 6 configuration.
           * 6 different c files will be generated, and the DTM should include the related Updater file according to the device used. */          
//          if(word_tmp>=DTM_APP_ADDR && word_tmp<=_MEMORY_FLASH_END_ && (word_tmp&0x3FF) == 0) {
          if(word_tmp>=DTM_APP_ADDR && (word_tmp&0x3FF) == 0) {
            
            if(flash_Erase(0x02, word_tmp)==0) {
              event_buffer[EVT_BUFF_OFFSET+6] = BLE_STATUS_SUCCESS;
            }          
          }          
          break;
          
        case CMD_UPDATER_PROG_DATA_BLOCK:
          /* Verify the address is inside the Flash range */
          event_buffer[EVT_BUFF_OFFSET+6] = FLASH_WRITE_FAILED;
          /* Removed the check of _MEMORY_FLASH_END_ to be compatible with BlueNRG-2 (that has an increased flash size
           * Otherwise, the project should be splitted BlueNRG-1 / BlueNRG-2. So, 6 configuration.
           * 6 different c files will be generated, and the DTM should include the related Updater file according to the device used. */
//          if(word_tmp>=DTM_APP_ADDR && word_tmp<=_MEMORY_FLASH_END_) {
          if(word_tmp>=DTM_APP_ADDR) {
            
            uint32_t length;            
            uint32_t length_a, length_b;
            length = (uint16_t)buff[7];
            length |= ((uint16_t)buff[8])<<8;
            
            uint8_t misalign_word;
            misalign_word = word_tmp & 3;
            
            if(misalign_word) {
              word_tmp -= misalign_word; // move back the address to align to a word
              length += misalign_word;   // increase the number of byte to write
              
              for(uint8_t i=0;i<misalign_word;i++) {
                buff[9-misalign_word+i] = *(uint8_t *)(word_tmp+i);
              }
            }            
            length_b = length & 0x3;
            length_a = length & ~0x3;
            
            if(length_a) {              
              if(flash_Write(word_tmp, buff + 9-misalign_word, length_a) == 0) {
                event_buffer[EVT_BUFF_OFFSET+6] = BLE_STATUS_SUCCESS;
                word_tmp += length_a;                
              }
              else {
                break;
              }
            }
            
            if(length_b) {
              /*read and write */
              for(uint8_t i=0;i<4;i++) {
                if(i>length_b)
                  buff[9-misalign_word+length_a+i] = *(uint8_t *)word_tmp;
              }
              /* word write */
              if(flash_Write(word_tmp, buff + 9-misalign_word+length_a, 4) == 0) {
                event_buffer[EVT_BUFF_OFFSET+6] = BLE_STATUS_SUCCESS;                
              }
              else {
                event_buffer[EVT_BUFF_OFFSET+6] = FLASH_WRITE_FAILED;
              }
            }
          }
          break;
          
        case CMD_UPDATER_READ_DATA_BLOCK:
          
          /* Verify the address is inside the Flash range */
          /* Removed the check of _MEMORY_FLASH_END_ to be compatible with BlueNRG-2 (that has an increased flash size
           * Otherwise, the project should be splitted BlueNRG-1 / BlueNRG-2. So, 6 configuration.
           * 6 different c files will be generated, and the DTM should include the related Updater file according to the device used. */
//          if(word_tmp>=DTM_APP_ADDR && word_tmp<=_MEMORY_FLASH_END_) {
          if(word_tmp>=DTM_APP_ADDR) {
            
            uint32_t length;
            length = (uint16_t)buff[7];
            length |= (((uint16_t)buff[8])<<8);
            
            for (uint32_t i = 0; i < length; i ++) {
              event_buffer[EVT_BUFF_OFFSET+7+i] = *((uint8_t *)word_tmp);
              word_tmp++;
            }
            event_buffer[EVT_BUFF_OFFSET+2] = 0x04 + length;
            event_buffer[EVT_BUFF_OFFSET+6] = BLE_STATUS_SUCCESS;
          }
          else {
            event_buffer[EVT_BUFF_OFFSET+6] = FLASH_READ_FAILED;
          }
          break;
          
        case CMD_UPDATER_CALC_CRC:
          event_buffer[EVT_BUFF_OFFSET+6] = FLASH_READ_FAILED;
          event_buffer[EVT_BUFF_OFFSET+2] = 0x08;
          event_buffer[EVT_BUFF_OFFSET+7] = 0xFF;
          event_buffer[EVT_BUFF_OFFSET+8] = 0xFF;
          event_buffer[EVT_BUFF_OFFSET+9] = 0xFF;
          event_buffer[EVT_BUFF_OFFSET+10] = 0xFF;
          
          /* Removed the check of _MEMORY_FLASH_END_ to be compatible with BlueNRG-2 (that has an increased flash size
           * Otherwise, the project should be splitted BlueNRG-1 / BlueNRG-2. So, 6 configuration.
           * 6 different c files will be generated, and the DTM should include the related Updater file according to the device used. */
//          if(word_tmp>=DTM_APP_ADDR && word_tmp<=_MEMORY_FLASH_END_) {
          if(word_tmp>=DTM_APP_ADDR) {
            event_buffer[EVT_BUFF_OFFSET+6] = BLE_STATUS_SUCCESS;
            
            uint32_t crc;
            uint32_t length;            
            length = (uint32_t)buff[7];            
            crc = updater_calc_crc(word_tmp, length);
            
            event_buffer[EVT_BUFF_OFFSET+7] = (uint8_t)crc;
            event_buffer[EVT_BUFF_OFFSET+8] = (uint8_t)(crc >> 8);
            event_buffer[EVT_BUFF_OFFSET+9] = (uint8_t)(crc >> 16);
            event_buffer[EVT_BUFF_OFFSET+10] =(uint8_t)(crc >> 24);
          }
          break;
          
        default:
          event_buffer[EVT_BUFF_OFFSET+1] = COMMAND_STATUS_EVENT;          
          event_buffer[EVT_BUFF_OFFSET+3] = BLE_ERROR_UNKNOWN_HCI_COMMAND;
          event_buffer[EVT_BUFF_OFFSET+4] = 0x01;     // 1 HCI_CMD_SLOT 
          event_buffer[EVT_BUFF_OFFSET+5] = buff[0];     // OPCODE_LB
          event_buffer[EVT_BUFF_OFFSET+6] = buff[1];     // OPCODE_HB
          break;
        }
      }
      else {
        event_buffer[EVT_BUFF_OFFSET+1] = COMMAND_STATUS_EVENT;          
        event_buffer[EVT_BUFF_OFFSET+3] = BLE_ERROR_UNKNOWN_HCI_COMMAND;
        event_buffer[EVT_BUFF_OFFSET+4] = 0x01;     // 1 HCI_CMD_SLOT 
        event_buffer[EVT_BUFF_OFFSET+5] = buff[0];     // OPCODE_LB
        event_buffer[EVT_BUFF_OFFSET+6] = buff[1];     // OPCODE_HB
      }
      
      if(event_pending == 1) {
        if(event_buffer[EVT_BUFF_OFFSET+2]>4)
          event_buffer_len = (7 + event_buffer[EVT_BUFF_OFFSET+2] - 4);
        else
          event_buffer_len = 7;
      }
      command_pending = 0;
    }
    
#ifdef UART_INTERFACE
    /* Event queue */
    if (event_pending!=0 && (dma_state == DMA_IDLE)) {
      if (dma_state == DMA_IDLE) {
        dma_state = DMA_IN_PROGRESS;
        DMA_Rearm(DMA_CH_UART_TX, (uint32_t)event_buffer, event_buffer_len);
        event_pending = 0;
      }
    }
    
    if((DMA->ISR & DMA_FLAG_TC_UART_TX) != (uint32_t)RESET) {
      DMA->IFCR = DMA_FLAG_TC_UART_TX;
      
      /* DMA finished the transfer */
      dma_state = DMA_IDLE;
      
      /* DMA_CH disable */
      CLEAR_BIT(DMA_CH_UART_TX->CCR, SET);
      
    }
    
#endif
    
  }
}


/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/
