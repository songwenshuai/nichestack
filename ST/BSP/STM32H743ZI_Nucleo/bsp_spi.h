/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32H7xx_NUCLEO_144_H
#define __STM32H7xx_NUCLEO_144_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/*############################### SPI_A #######################################*/
#ifdef HAL_SPI_MODULE_ENABLED

#define NUCLEO_SPIx                                     SPI1
#define NUCLEO_SPIx_CLK_ENABLE()                        __HAL_RCC_SPI1_CLK_ENABLE()

#define NUCLEO_SPIx_SCK_AF                              GPIO_AF5_SPI1
#define NUCLEO_SPIx_SCK_GPIO_PORT                       GPIOA
#define NUCLEO_SPIx_SCK_PIN                             GPIO_PIN_5
#define NUCLEO_SPIx_SCK_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOA_CLK_ENABLE()
#define NUCLEO_SPIx_SCK_GPIO_CLK_DISABLE()              __HAL_RCC_GPIOA_CLK_DISABLE()

#define NUCLEO_SPIx_MISO_AF                             GPIO_AF5_SPI1
#define NUCLEO_SPIx_MISO_GPIO_PORT                      GPIOA
#define NUCLEO_SPIx_MISO_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOA_CLK_ENABLE()
#define NUCLEO_SPIx_MISO_GPIO_CLK_DISABLE()             __HAL_RCC_GPIOA_CLK_DISABLE()
#define NUCLEO_SPIx_MISO_PIN                            GPIO_PIN_6

#define NUCLEO_SPIx_MOSI_AF                             GPIO_AF5_SPI1
#define NUCLEO_SPIx_MOSI_GPIO_PORT                      GPIOB
#define NUCLEO_SPIx_MOSI_PIN                            GPIO_PIN_5
#define NUCLEO_SPIx_MOSI_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOB_CLK_ENABLE()
#define NUCLEO_SPIx_MOSI_GPIO_CLK_DISABLE()             __HAL_RCC_GPIOB_CLK_DISABLE()

#define SD_CS_GPIO_PORT                                 GPIOD
#define SD_CS_PIN                                       GPIO_PIN_14
#define SD_CS_GPIO_CLK_ENABLE()                         __HAL_RCC_GPIOF_CLK_ENABLE()
#define SD_CS_GPIO_CLK_DISABLE()                        __HAL_RCC_GPIOF_CLK_DISABLE()

/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the SPI communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */
#define NUCLEO_SPIx_TIMEOUT_MAX                   1000

/**
  * @brief  SD Control Lines management
  */
#define SD_CS_LOW()       HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_RESET)
#define SD_CS_HIGH()      HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_SET)

#endif /* HAL_SPI_MODULE_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* __STM32H7xx_NUCLEO_144_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
