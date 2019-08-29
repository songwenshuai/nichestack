/*
*********************************************************************************************************
*
*  模块名称 : 串口中断+FIFO驱动模块
*  文件名称 : bsp_usart.h
*  说    明 : 头文件
*
*
*********************************************************************************************************
*/

#ifndef _BSP_USART_H_
#define _BSP_USART_H_

#include "stm32h7xx_hal.h"

/* 串口3的GPIO --- PC10 PC11  RS485 */
#define USART3_CLK_ENABLE() __HAL_RCC_USART3_CLK_ENABLE()

#define USART3_TX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define USART3_TX_GPIO_PORT GPIOC
#define USART3_TX_PIN GPIO_PIN_10
#define USART3_TX_AF GPIO_AF7_USART3

#define USART3_RX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define USART3_RX_GPIO_PORT GPIOC
#define USART3_RX_PIN GPIO_PIN_11
#define USART3_RX_AF GPIO_AF7_USART3

#define UART3_RS485_EN 0

/* 开关全局中断的宏 */
#define ENABLE_INT() __set_PRIMASK(0)  /* 使能全局中断 */
#define DISABLE_INT() __set_PRIMASK(1) /* 禁止全局中断 */

#if UART3_RS485_EN == 1
/* PB2 控制RS485芯片的发送使能 */
#define RS485_TXEN_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define RS485_TXEN_GPIO_PORT GPIOB
#define RS485_TXEN_PIN GPIO_PIN_2

#define RS485_RX_EN() RS485_TXEN_GPIO_PORT->BSRRH = RS485_TXEN_PIN
#define RS485_TX_EN() RS485_TXEN_GPIO_PORT->BSRRL = RS485_TXEN_PIN
#endif

/* 定义端口号 */
typedef enum
{
  COM1 = 0, /* USART1 */
  COM2 = 1, /* USART2 */
  COM3 = 2, /* USART3 */
  COM4 = 3, /* UART4 */
  COM5 = 4, /* UART5 */
  COM6 = 5, /* USART6 */
  COM7 = 6, /* UART7 */
  COM8 = 7  /* UART8 */
} COM_PORT_E;

/* 定义串口波特率和FIFO缓冲区大小，分为发送缓冲区和接收缓冲区, 支持全双工 */
#define UART3_BAUD 115200
#define UART3_TX_BUF_SIZE 1 * 1024
#define UART3_RX_BUF_SIZE 1 * 1024

/* 串口设备结构体 */
typedef struct
{
  USART_TypeDef  *uart;             /* STM32内部串口设备指针 */
  uint8_t        *pTxBuf;           /* 发送缓冲区 */
  uint8_t        *pRxBuf;           /* 接收缓冲区 */
  uint16_t       usTxBufSize;       /* 发送缓冲区大小 */
  uint16_t       usRxBufSize;       /* 接收缓冲区大小 */
  __IO uint16_t  usTxWrite;         /* 发送缓冲区写指针 */
  __IO uint16_t  usTxRead;          /* 发送缓冲区读指针 */
  __IO uint16_t  usTxCount;         /* 等待发送的数据个数 */

  __IO uint16_t  usRxWrite;         /* 接收缓冲区写指针 */
  __IO uint16_t  usRxRead;          /* 接收缓冲区读指针 */
  __IO uint16_t  usRxCount;         /* 还未读取的新数据个数 */
#if UART3_RS485_EN == 1
  void (*SendBefor)(void);          /* 开始发送之前的回调函数指针（主要用于RS485切换到发送模式） */
  void (*SendOver)(void);           /* 发送完毕的回调函数指针（主要用于RS485将发送模式切换为接收模式） */
  void (*ReciveNew)(uint8_t _byte); /* 串口收到数据的回调函数指针 */
#endif
  uint8_t Sending; /* 正在发送中 */
} UART_T;

void BSP_UART_Init(void);

void UART_SEND_Buf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void UART_SEND_Char(COM_PORT_E _ucPort, uint8_t _ucByte);

uint8_t UART_GET_Char(COM_PORT_E _ucPort, uint8_t *_pByte);

void UART_CLR_TX_Fifo(COM_PORT_E _ucPort);
void UART_CLR_RX_Fifo(COM_PORT_E _ucPort);

void UART_SET_Baud(COM_PORT_E _ucPort, uint32_t _BaudRate);

void UART_SET_Param(USART_TypeDef *Instance, uint32_t BaudRate, uint32_t Parity, uint32_t Mode);

uint8_t UART_TX_Empty(COM_PORT_E _ucPort);
uint8_t UART_RX_Empty(COM_PORT_E _ucPort);


#if UART3_RS485_EN == 1
void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen);
void RS485_SendStr(char *_pBuf);
void RS485_SetBaud(uint32_t _baud);
#endif

#endif

/***************************** (END OF FILE) *********************************/
