/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_PB_H
#define __BSP_PB_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/**
  * @}
  */

/** @defgroup BSP BSP Nucleo 144
  * @{
  */

typedef enum
{
  BUTTON_USER = 0,
  /* Alias */
  BUTTON_KEY = BUTTON_USER
}Button_TypeDef;

typedef enum
{
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
}ButtonMode_TypeDef;

/**
  * @}
  */

/** @defgroup STM32H7xx_NUCLEO_144_BUTTON NUCLEO_144 BUTTON
  * @{
  */
#define BUTTONn                                 1

/**
 * @brief Key push-button
 */
#define USER_BUTTON_PIN                          GPIO_PIN_13
#define USER_BUTTON_GPIO_PORT                    GPIOC
#define USER_BUTTON_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define USER_BUTTON_GPIO_CLK_DISABLE()           __HAL_RCC_GPIOC_CLK_DISABLE()
#define USER_BUTTON_EXTI_LINE                    GPIO_PIN_13
#define USER_BUTTON_EXTI_IRQn                    EXTI15_10_IRQn

#define BUTTONx_GPIO_CLK_ENABLE(__INDEX__)      USER_BUTTON_GPIO_CLK_ENABLE()
#define BUTTONx_GPIO_CLK_DISABLE(__INDEX__)     USER_BUTTON_GPIO_CLK_DISABLE()

/* Aliases */
#define KEY_BUTTON_PIN                       USER_BUTTON_PIN
#define KEY_BUTTON_GPIO_PORT                 USER_BUTTON_GPIO_PORT
#define KEY_BUTTON_GPIO_CLK_ENABLE()         USER_BUTTON_GPIO_CLK_ENABLE()
#define KEY_BUTTON_GPIO_CLK_DISABLE()        USER_BUTTON_GPIO_CLK_DISABLE()
#define KEY_BUTTON_EXTI_LINE                 USER_BUTTON_EXTI_LINE
#define KEY_BUTTON_EXTI_IRQn                 USER_BUTTON_EXTI_IRQn

/**
  * @}
  */

/** @defgroup STM32H7xx_NUCLEO_144_Exported_Functions Exported Functions
  * @{
  */
void             BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
void             BSP_PB_DeInit(Button_TypeDef Button);
uint32_t         BSP_PB_GetState(Button_TypeDef Button);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __BSP_PB_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
