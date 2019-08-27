/* Includes ------------------------------------------------------------------*/
#include "bsp_spi.h"

/**
  * @brief LINK SD Card
  */
#define SD_DUMMY_BYTE            0xFF
#define SD_NO_RESPONSE_EXPECTED  0x80

/**
 * @brief BUS variables
 */
#ifdef HAL_SPI_MODULE_ENABLED
uint32_t SpixTimeout = NUCLEO_SPIx_TIMEOUT_MAX; /*<! Value of Timeout when SPI communication fails */
static SPI_HandleTypeDef hnucleo_Spi;
#endif /* HAL_SPI_MODULE_ENABLED */

/** @defgroup STM32H7xx_NUCLEO_144_LOW_LEVEL_Private_Functions LOW LEVEL Private Functions
  * @{
  */
#ifdef HAL_SPI_MODULE_ENABLED
static void       SPIx_Init(void);
static void       SPIx_MspInit(SPI_HandleTypeDef *hspi);
static void       SPIx_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength);
static void       SPIx_Error(void);

/* SD IO functions */
void              SD_IO_Init(void);
void              SD_IO_CSState(uint8_t state);
void              SD_IO_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength);
uint8_t           SD_IO_WriteByte(uint8_t Data);

#endif /* HAL_SPI_MODULE_ENABLED */

/******************************************************************************
                            BUS OPERATIONS
*******************************************************************************/

/******************************* SPI ********************************/
#ifdef HAL_SPI_MODULE_ENABLED

/**
  * @brief  Initializes SPI MSP.
  * @retval None
  */
static void SPIx_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  RCC_PeriphCLKInitTypeDef  SPI_PeriphClkInit;

  /*** Configure the GPIOs ***/
  /* Enable GPIO clock */
  NUCLEO_SPIx_SCK_GPIO_CLK_ENABLE();
  NUCLEO_SPIx_MISO_GPIO_CLK_ENABLE();
  NUCLEO_SPIx_MOSI_GPIO_CLK_ENABLE();

  /* Configure SPI SCK */
  GPIO_InitStruct.Pin = NUCLEO_SPIx_SCK_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = NUCLEO_SPIx_SCK_AF;
  HAL_GPIO_Init(NUCLEO_SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);

  /* Configure SPI MISO and MOSI */
  GPIO_InitStruct.Pin = NUCLEO_SPIx_MOSI_PIN;
  GPIO_InitStruct.Alternate = NUCLEO_SPIx_MOSI_AF;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(NUCLEO_SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = NUCLEO_SPIx_MISO_PIN;
  GPIO_InitStruct.Alternate = NUCLEO_SPIx_MISO_AF;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  HAL_GPIO_Init(NUCLEO_SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);

  SPI_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SPI123;
  SPI_PeriphClkInit.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;
  HAL_RCCEx_PeriphCLKConfig(&SPI_PeriphClkInit);

  /*** Configure the SPI peripheral ***/
  /* Enable SPI clock */
  NUCLEO_SPIx_CLK_ENABLE();
}


/**
  * @brief  Initializes SPI HAL.
  * @retval None
  */
static void SPIx_Init(void)
{
  if(HAL_SPI_GetState(&hnucleo_Spi) == HAL_SPI_STATE_RESET)
  {
    /* SPI Config */
    hnucleo_Spi.Instance = NUCLEO_SPIx;
    /* SPI baudrate is set to 12,5 MHz maximum (PLL1_Q/SPI_BaudRatePrescaler = 200/16 = 12,5 MHz)
    to verify these constraints:
      - ST7735 LCD SPI interface max baudrate is 15MHz for write and 6.66MHz for read
        Since the provided driver doesn't use read capability from LCD, only constraint
        on write baudrate is considered.
      - SD card SPI interface max baudrate is 25MHz for write/read
    */
    hnucleo_Spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    hnucleo_Spi.Init.Direction = SPI_DIRECTION_2LINES;
    hnucleo_Spi.Init.CLKPhase = SPI_PHASE_2EDGE;
    hnucleo_Spi.Init.CLKPolarity = SPI_POLARITY_HIGH;
    hnucleo_Spi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hnucleo_Spi.Init.CRCPolynomial = 7;
    hnucleo_Spi.Init.DataSize = SPI_DATASIZE_8BIT;
    hnucleo_Spi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hnucleo_Spi.Init.NSS = SPI_NSS_SOFT;
    hnucleo_Spi.Init.TIMode = SPI_TIMODE_DISABLE;
    hnucleo_Spi.Init.Mode = SPI_MODE_MASTER;
    hnucleo_Spi.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;  /* Recommanded setting to avoid glitches */

    hnucleo_Spi.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
    hnucleo_Spi.Init.CRCLength = SPI_CRC_LENGTH_8BIT;
    hnucleo_Spi.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
    hnucleo_Spi.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
    hnucleo_Spi.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
    hnucleo_Spi.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    hnucleo_Spi.Init.MasterSSIdleness = 0x00000000;
    hnucleo_Spi.Init.MasterInterDataIdleness = 0x00000000;
    hnucleo_Spi.Init.MasterReceiverAutoSusp = 0x00000000;

    SPIx_MspInit(&hnucleo_Spi);
    HAL_SPI_Init(&hnucleo_Spi);
  }
}

/**
  * @brief  SPI Write and Read data
  * @param  DataIn: Data to write
  * @param  DataOut: Data to read
  * @param  DataLength: Data size
  * @retval None
  */
static void SPIx_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_SPI_TransmitReceive(&hnucleo_Spi, (uint8_t*) DataIn, DataOut, DataLength, SpixTimeout);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Execute user timeout callback */
    SPIx_Error();
  }
}

/**
  * @brief  SPI error treatment function
  * @retval None
  */
static void SPIx_Error (void)
{
  /* De-initialize the SPI communication BUS */
  HAL_SPI_DeInit(&hnucleo_Spi);

  /* Re-Initiaize the SPI communication BUS */
  SPIx_Init();
}

/******************************************************************************
                            LINK OPERATIONS
*******************************************************************************/

/********************************* LINK SD ************************************/
/**
  * @brief  Initializes the SD Card and put it into StandBy State (Ready for
  *         data transfer).
  * @retval None
  */
void SD_IO_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  uint8_t counter;

  /* SD_CS_GPIO Periph clock enable */
  SD_CS_GPIO_CLK_ENABLE();

  /* Configure SD_CS_PIN pin: SD Card CS pin */
  GPIO_InitStruct.Pin = SD_CS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SD_CS_GPIO_PORT, &GPIO_InitStruct);

  /*------------Put SD in SPI mode--------------*/
  /* SD SPI Config */
  SPIx_Init();

  /* SD chip select high */
  SD_IO_CSState(1);

  /* Send dummy byte 0xFF, 10 times with CS high */
  /* Rise CS and MOSI for 80 clocks cycles */
  for (counter = 0; counter <= 9; counter++)
  {
    /* Send dummy byte 0xFF */
    SD_IO_WriteByte(SD_DUMMY_BYTE);
  }
}

/**
  * @brief  Set the SD_CS pin.
  * @param  val: SD CS value.
  * @retval None
  */
void SD_IO_CSState(uint8_t val)
{
  if(val == 1)
  {
    SD_CS_HIGH();
  }
  else
  {
    SD_CS_LOW();
  }
}

/**
  * @brief  SD Write and Read data
  * @param  DataIn: Data to write
  * @param  DataOut: Data to read
  * @param  DataLength: Data size
  * @retval None
  */
void SD_IO_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength)
{
  /* Send the byte */
  SPIx_WriteReadData(DataIn, DataOut, DataLength);
}

/**
  * @brief  Writes a byte on the SD.
  * @param  Data: byte to send.
  * @retval None
  */
uint8_t SD_IO_WriteByte(uint8_t Data)
{
  uint8_t tmp;
  /* Send the byte */
  SPIx_WriteReadData(&Data,&tmp,1);
  return tmp;
}

#endif /* HAL_SPI_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
