#include "bsp_usart.h"

/* 定义每个串口结构体变量 */
static UART_T g_tUart3;
static uint8_t g_TxBuf3[UART3_TX_BUF_SIZE]; /* 发送缓冲区 */
static uint8_t g_RxBuf3[UART3_RX_BUF_SIZE]; /* 接收缓冲区 */

static void UART_VAR_Init(void);
static void UART_HARD_Init(void);
static void UART_WRITE_Fifo(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen);
static uint8_t UART_READ_Fifo(UART_T *_pUart, uint8_t *_pByte);
static void UART_IRQ_Hand(UART_T *_pUart);
void UART_RS485_Init(void);

/*
*********************************************************************************************************
*  函 数 名: BSP_UART_Init
*  功能说明: 初始化串口硬件，并对全局变量赋初值.
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
void BSP_UART_Init(void)
{
  UART_VAR_Init();      /* 必须先初始化全局变量,再配置硬件 */
  UART_HARD_Init();     /* 配置串口的硬件参数(波特率等) */
#if UART3_RS485_EN == 1
  UART_RS485_Init();    /* 配置RS485芯片的发送使能硬件，配置为推挽输出 */
#endif
}

/*
*********************************************************************************************************
*  函 数 名: COM_TO_UARTT
*  功能说明: 将COM端口号转换为UART指针
*  形    参: _ucPort: 端口号(COM1 - COM8)
*  返 回 值: uart指针
*********************************************************************************************************
*/
UART_T *COM_TO_UARTT(COM_PORT_E _ucPort)
{
  if (_ucPort == COM3)
  {
    return &g_tUart3;
  }
  else
  {
    return 0;
  }
}

/*
*********************************************************************************************************
*  函 数 名: COM_TO_UARTT
*  功能说明: 将COM端口号转换为 USART_TypeDef* USARTx
*  形    参: _ucPort: 端口号(COM1 - COM8)
*  返 回 值: USART_TypeDef*,  USART1, USART2, USART3, UART4, UART5，USART6，UART7，UART8。
*********************************************************************************************************
*/
USART_TypeDef *ComToUSARTx(COM_PORT_E _ucPort)
{
  if (_ucPort == COM3)
  {
    return USART3;
  }
  else
  {
    /* 不做任何处理 */
    return 0;
  }
}

/*
*********************************************************************************************************
*  函 数 名: UART_SEND_Buf
*  功能说明: 向串口发送一组数据。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
*  形    参: _ucPort: 端口号(COM1 - COM8)
*        _ucaBuf: 待发送的数据缓冲区
*        _usLen : 数据长度
*  返 回 值: 无
*********************************************************************************************************
*/
void UART_SEND_Buf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen)
{
  UART_T *pUart;

  pUart = COM_TO_UARTT(_ucPort);
  if (pUart == 0)
  {
    return;
  }
#if UART3_RS485_EN == 1
  if (pUart->SendBefor != 0)
  {
    pUart->SendBefor(); /* 如果是RS485通信，可以在这个函数中将RS485设置为发送模式 */
  }
#endif
  UART_WRITE_Fifo(pUart, _ucaBuf, _usLen);
}

/*
*********************************************************************************************************
*  函 数 名: UART_SEND_Char
*  功能说明: 向串口发送1个字节。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
*  形    参: _ucPort: 端口号(COM1 - COM8)
*        _ucByte: 待发送的数据
*  返 回 值: 无
*********************************************************************************************************
*/
void UART_SEND_Char(COM_PORT_E _ucPort, uint8_t _ucByte)
{
  UART_SEND_Buf(_ucPort, &_ucByte, 1);
}

/*
*********************************************************************************************************
*  函 数 名: UART_GET_Char
*  功能说明: 从接收缓冲区读取1字节，非阻塞。无论有无数据均立即返回。
*  形    参: _ucPort: 端口号(COM1 - COM8)
*        _pByte: 接收到的数据存放在这个地址
*  返 回 值: 0 表示无数据, 1 表示读取到有效字节
*********************************************************************************************************
*/
uint8_t UART_GET_Char(COM_PORT_E _ucPort, uint8_t *_pByte)
{
  UART_T *pUart;

  pUart = COM_TO_UARTT(_ucPort);
  if (pUart == 0)
  {
    return 0;
  }

  return UART_READ_Fifo(pUart, _pByte);
}

/*
*********************************************************************************************************
*  函 数 名: UART_CLR_TX_Fifo
*  功能说明: 清零串口发送缓冲区
*  形    参: _ucPort: 端口号(COM1 - COM8)
*  返 回 值: 无
*********************************************************************************************************
*/
void UART_CLR_TX_Fifo(COM_PORT_E _ucPort)
{
  UART_T *pUart;

  pUart = COM_TO_UARTT(_ucPort);
  if (pUart == 0)
  {
    return;
  }

  pUart->usTxWrite = 0;
  pUart->usTxRead = 0;
  pUart->usTxCount = 0;
}

/*
*********************************************************************************************************
*  函 数 名: UART_CLR_RX_Fifo
*  功能说明: 清零串口接收缓冲区
*  形    参: _ucPort: 端口号(COM1 - COM8)
*  返 回 值: 无
*********************************************************************************************************
*/
void UART_CLR_RX_Fifo(COM_PORT_E _ucPort)
{
  UART_T *pUart;

  pUart = COM_TO_UARTT(_ucPort);
  if (pUart == 0)
  {
    return;
  }

  pUart->usRxWrite = 0;
  pUart->usRxRead = 0;
  pUart->usRxCount = 0;
}

/*
*********************************************************************************************************
*  函 数 名: UART_SET_Baud
*  功能说明: 设置串口的波特率. 本函数固定设置为无校验，收发都使能模式
*  形    参: _ucPort: 端口号(COM1 - COM8)
*        _BaudRate: 波特率，8倍过采样  波特率.0-12.5Mbps
*                                16倍过采样 波特率.0-6.25Mbps
*  返 回 值: 无
*********************************************************************************************************
*/
void UART_SET_Baud(COM_PORT_E _ucPort, uint32_t _BaudRate)
{
  USART_TypeDef *USARTx;

  USARTx = ComToUSARTx(_ucPort);
  if (USARTx == 0)
  {
    return;
  }

  UART_SET_Param(USARTx, _BaudRate, UART_PARITY_NONE, UART_MODE_TX_RX);
}

#if UART3_RS485_EN == 1
/* 如果是RS485通信，请按如下格式编写函数， 我们仅举了 USART3作为RS485的例子 */

/*
*********************************************************************************************************
*  函 数 名: UART_RS485_Init
*  功能说明: 配置RS485发送使能口线 TXE
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
void UART_RS485_Init(void)
{
  GPIO_InitTypeDef gpio_init;

  /* 打开GPIO时钟 */
  RS485_TXEN_GPIO_CLK_ENABLE();

  /* 配置引脚为推挽输出 */
  gpio_init.Mode = GPIO_MODE_OUTPUT_PP;        /* 推挽输出 */
  gpio_init.Pull = GPIO_NOPULL;                /* 上下拉电阻不使能 */
  gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH; /* GPIO速度等级 */
  gpio_init.Pin = RS485_TXEN_PIN;
  HAL_GPIO_Init(RS485_TXEN_GPIO_PORT, &gpio_init);
}

/*
*********************************************************************************************************
*  函 数 名: RS485_SetBaud
*  功能说明: 修改485串口的波特率。
*  形    参: _baud : 8倍过采样  波特率.0-12.5Mbps
*                     16倍过采样 波特率.0-6.25Mbps
*  返 回 值: 无
*********************************************************************************************************
*/
void RS485_SetBaud(uint32_t _baud)
{
  UART_SET_Baud(COM3, _baud);
}

/*
*********************************************************************************************************
*  函 数 名: RS485_SendBefor
*  功能说明: 发送数据前的准备工作。对于RS485通信，请设置RS485芯片为发送状态，
*        并修改 UART_VAR_Init()中的函数指针等于本函数名，比如 g_tUart2.SendBefor = RS485_SendBefor
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
void RS485_SendBefor(void)
{
  RS485_TX_EN(); /* 切换RS485收发芯片为发送模式 */
}

/*
*********************************************************************************************************
*  函 数 名: RS485_SendOver
*  功能说明: 发送一串数据结束后的善后处理。对于RS485通信，请设置RS485芯片为接收状态，
*        并修改 UART_VAR_Init()中的函数指针等于本函数名，比如 g_tUart2.SendOver = RS485_SendOver
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
void RS485_SendOver(void)
{
  RS485_RX_EN(); /* 切换RS485收发芯片为接收模式 */
}

/*
*********************************************************************************************************
*  函 数 名: RS485_SendBuf
*  功能说明: 通过RS485芯片发送一串数据。注意，本函数不等待发送完毕。
*  形    参: _ucaBuf : 数据缓冲区
*        _usLen : 数据长度
*  返 回 值: 无
*********************************************************************************************************
*/
void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen)
{
  UART_SEND_Buf(COM3, _ucaBuf, _usLen);
}

/*
*********************************************************************************************************
*  函 数 名: RS485_SendStr
*  功能说明: 向485总线发送一个字符串，0结束。
*  形    参: _pBuf 字符串，0结束
*  返 回 值: 无
*********************************************************************************************************
*/
void RS485_SendStr(char *_pBuf)
{
  RS485_SendBuf((uint8_t *)_pBuf, strlen(_pBuf));
}

/*
*********************************************************************************************************
*  函 数 名: RS485_ReciveNew
*  功能说明: 接收到新的数据
*  形    参: _byte 接收到的新数据
*  返 回 值: 无
*********************************************************************************************************
*/
//extern void MODH_ReciveNew(uint8_t _byte);
void RS485_ReciveNew(uint8_t _byte)
{
  //  MODH_ReciveNew(_byte);
}
#endif
/*
*********************************************************************************************************
*  函 数 名: UART_VAR_Init
*  功能说明: 初始化串口相关的变量
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
static void UART_VAR_Init(void)
{
  g_tUart3.uart = USART3;                   /* STM32 串口设备 */
  g_tUart3.pTxBuf = g_TxBuf3;               /* 发送缓冲区指针 */
  g_tUart3.pRxBuf = g_RxBuf3;               /* 接收缓冲区指针 */
  g_tUart3.usTxBufSize = UART3_TX_BUF_SIZE; /* 发送缓冲区大小 */
  g_tUart3.usRxBufSize = UART3_RX_BUF_SIZE; /* 接收缓冲区大小 */
  g_tUart3.usTxWrite = 0;                   /* 发送FIFO写索引 */
  g_tUart3.usTxRead = 0;                    /* 发送FIFO读索引 */
  g_tUart3.usRxWrite = 0;                   /* 接收FIFO写索引 */
  g_tUart3.usRxRead = 0;                    /* 接收FIFO读索引 */
  g_tUart3.usRxCount = 0;                   /* 接收到的新数据个数 */
  g_tUart3.usTxCount = 0;                   /* 待发送的数据个数 */
#if UART3_RS485_EN == 1
  g_tUart3.SendBefor = RS485_SendBefor;     /* 发送数据前的回调函数 */
  g_tUart3.SendOver = RS485_SendOver;       /* 发送完毕后的回调函数 */
  g_tUart3.ReciveNew = RS485_ReciveNew;     /* 接收到新数据后的回调函数 */
#endif
  g_tUart3.Sending = 0; /* 正在发送中标志 */
}

/*
*********************************************************************************************************
*  函 数 名: UART_SET_Param
*  功能说明: 配置串口的硬件参数（波特率，数据位，停止位，起始位，校验位，中断使能）适合于STM32- H7开发板
*  形    参: Instance   USART_TypeDef类型结构体
*             BaudRate   波特率
*             Parity     校验类型，奇校验或者偶校验
*             Mode       发送和接收模式使能
*  返 回 值: 无
*********************************************************************************************************
*/
void UART_SET_Param(USART_TypeDef *Instance, uint32_t BaudRate, uint32_t Parity, uint32_t Mode)
{
  UART_HandleTypeDef UartHandle;

  /*##-1- 配置串口硬件参数 ######################################*/
  /* 异步串口模式 (UART Mode) */
  /* 配置如下:
    - 字长    = 8 位
    - 停止位  = 1 个停止位
    - 校验    = 参数Parity
    - 波特率  = 参数BaudRate
    - 硬件流控制关闭 (RTS and CTS signals) */

  UartHandle.Instance = Instance;

  UartHandle.Init.BaudRate = BaudRate;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits = UART_STOPBITS_1;
  UartHandle.Init.Parity = Parity;
  UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode = Mode;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
  UartHandle.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  UartHandle.Init.Prescaler = UART_PRESCALER_DIV1;
  UartHandle.Init.FIFOMode = UART_FIFOMODE_DISABLE;
  UartHandle.Init.TXFIFOThreshold = UART_TXFIFO_THRESHOLD_1_8;
  UartHandle.Init.RXFIFOThreshold = UART_RXFIFO_THRESHOLD_1_8;
  UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  if (HAL_UART_Init(&UartHandle) != HAL_OK)
  {
    return;
  }
}

/*
*********************************************************************************************************
*  函 数 名: UART_HARD_Init
*  功能说明: 配置串口的硬件参数（波特率，数据位，停止位，起始位，校验位，中断使能）适合于STM32-H7开发板
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
static void UART_HARD_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;

  /*
      下面这个配置可以注释掉，预留下来是为了方便以后选择其它时钟使用
      默认情况下，USART1和USART6选择的PCLK2，时钟100MHz。
      USART2，USART3，UART4，UART5，UART6，UART7和UART8选择的时钟是PLCK1，时钟100MHz。
    */
  RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART16;
  RCC_PeriphClkInit.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
  HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);

  /* 使能 GPIO TX/RX 时钟 */
  USART3_TX_GPIO_CLK_ENABLE();
  USART3_RX_GPIO_CLK_ENABLE();

  /* 使能 USARTx 时钟 */
  USART3_CLK_ENABLE();

  /* 配置TX引脚 */
  GPIO_InitStruct.Pin = USART3_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = USART3_TX_AF;
  HAL_GPIO_Init(USART3_TX_GPIO_PORT, &GPIO_InitStruct);

  /* 配置RX引脚 */
  GPIO_InitStruct.Pin = USART3_RX_PIN;
  GPIO_InitStruct.Alternate = USART3_RX_AF;
  HAL_GPIO_Init(USART3_RX_GPIO_PORT, &GPIO_InitStruct);

  /* 配置NVIC the NVIC for UART */
  HAL_NVIC_SetPriority(USART3_IRQn, 0, 3);
  HAL_NVIC_EnableIRQ(USART3_IRQn);

  /* 配置波特率、奇偶校验 */
  UART_SET_Param(USART3, UART3_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);

  SET_BIT(USART3->ICR, USART_ICR_TCCF);   /* 清除TC发送完成标志 */
  SET_BIT(USART3->RQR, USART_RQR_RXFRQ);  /* 清除RXNE接收标志 */
  SET_BIT(USART3->CR1, USART_CR1_RXNEIE); /* 使能PE. RX接受中断 */
}

/*
*********************************************************************************************************
*  函 数 名: UART_WRITE_Fifo
*  功能说明: 填写数据到UART发送缓冲区,并启动发送中断。中断处理函数发送完毕后，自动关闭发送中断
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/
static void UART_WRITE_Fifo(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
  uint16_t i;

  for (i = 0; i < _usLen; i++)
  {
    /* 如果发送缓冲区已经满了，则等待缓冲区空 */
    while (1)
    {
      __IO uint16_t usCount;

      DISABLE_INT();
      usCount = _pUart->usTxCount;
      ENABLE_INT();

      if (usCount < _pUart->usTxBufSize)
      {
        break;
      }
      else if (usCount == _pUart->usTxBufSize) /* 数据已填满缓冲区 */
      {
        if ((_pUart->uart->CR1 & USART_CR1_TXEIE) == 0)
        {
          SET_BIT(_pUart->uart->CR1, USART_CR1_TXEIE);
        }
      }
    }

    /* 将新数据填入发送缓冲区 */
    _pUart->pTxBuf[_pUart->usTxWrite] = _ucaBuf[i];

    DISABLE_INT();
    if (++_pUart->usTxWrite >= _pUart->usTxBufSize)
    {
      _pUart->usTxWrite = 0;
    }
    _pUart->usTxCount++;
    ENABLE_INT();
  }

  SET_BIT(_pUart->uart->CR1, USART_CR1_TXEIE); /* 使能发送中断（缓冲区空） */
}

/*
*********************************************************************************************************
*   函 数 名: UART_RX_Empty
*   功能说明: 判断发送缓冲区是否为空。
*   形    参:  _pUart : 串口设备
*   返 回 值: 1为空。0为不空。
*********************************************************************************************************
*/
uint8_t UART_RX_Empty(COM_PORT_E _ucPort)
{
  UART_T *pUart;
  uint8_t Receive;

  pUart = COM_TO_UARTT(_ucPort);
  if (pUart == 0)
  {
    return 0;
  }

  Receive = pUart->usRxCount;

  if (Receive == 0)
  {
    return 0;
  }
  return 1;
}

/*
*********************************************************************************************************
*  函 数 名: UART_READ_Fifo
*  功能说明: 从串口接收缓冲区读取1字节数据 （用于主程序调用）
*  形    参: _pUart : 串口设备
*        _pByte : 存放读取数据的指针
*  返 回 值: 0 表示无数据  1表示读取到数据
*********************************************************************************************************
*/
static uint8_t UART_READ_Fifo(UART_T *_pUart, uint8_t *_pByte)
{
  uint16_t usCount;

  /* usRxWrite 变量在中断函数中被改写，主程序读取该变量时，必须进行临界区保护 */
  DISABLE_INT();
  usCount = _pUart->usRxCount;
  ENABLE_INT();

  /* 如果读和写索引相同，则返回0 */
  //if (_pUart->usRxRead == usRxWrite)
  if (usCount == 0) /* 已经没有数据 */
  {
    return 0;
  }
  else
  {
    *_pByte = _pUart->pRxBuf[_pUart->usRxRead]; /* 从串口接收FIFO取1个数据 */

    /* 改写FIFO读索引 */
    DISABLE_INT();
    if (++_pUart->usRxRead >= _pUart->usRxBufSize)
    {
      _pUart->usRxRead = 0;
    }
    _pUart->usRxCount--;
    ENABLE_INT();
    return 1;
  }
}

/*
*********************************************************************************************************
*   函 数 名: UART_TX_Empty
*   功能说明: 判断发送缓冲区是否为空。
*   形    参:  _pUart : 串口设备
*   返 回 值: 1为空。0为不空。
*********************************************************************************************************
*/
uint8_t UART_TX_Empty(COM_PORT_E _ucPort)
{
  UART_T *pUart;
  uint8_t Sending;

  pUart = COM_TO_UARTT(_ucPort);
  if (pUart == 0)
  {
    return 0;
  }

  Sending = pUart->Sending;

  if (Sending != 0)
  {
    return 0;
  }
  return 1;
}

/*
*********************************************************************************************************
*  函 数 名: UART_IRQ_Hand
*  功能说明: 供中断服务程序调用，通用串口中断处理函数
*  形    参: _pUart : 串口设备
*  返 回 值: 无
*********************************************************************************************************
*/
static void UART_IRQ_Hand(UART_T *_pUart)
{
  uint32_t isrflags = READ_REG(_pUart->uart->ISR);
  uint32_t cr1its = READ_REG(_pUart->uart->CR1);
  uint32_t cr3its = READ_REG(_pUart->uart->CR3);

  /* 处理接收中断  */
  if ((isrflags & USART_ISR_RXNE) != RESET)
  {
    /* 从串口接收数据寄存器读取数据存放到接收FIFO */
    uint8_t ch;

    ch = READ_REG(_pUart->uart->RDR);
    _pUart->pRxBuf[_pUart->usRxWrite] = ch;
    if (++_pUart->usRxWrite >= _pUart->usRxBufSize)
    {
      _pUart->usRxWrite = 0;
    }
    if (_pUart->usRxCount < _pUart->usRxBufSize)
    {
      _pUart->usRxCount++;
    }

    /* 回调函数,通知应用程序收到新数据,一般是发送1个消息或者设置一个标记 */
#if UART3_RS485_EN == 1
    if (_pUart->ReciveNew)
    {
      _pUart->ReciveNew(ch); /* 比如，交给MODBUS解码程序处理字节流 */
    }
#endif
  }

  /* 处理发送缓冲区空中断 */
  if (((isrflags & USART_ISR_TXE) != RESET) && (cr1its & USART_CR1_TXEIE) != RESET)
  {
    if (_pUart->usTxCount == 0)
    {
      /* 发送缓冲区的数据已取完时， 禁止发送缓冲区空中断 （注意：此时最后1个数据还未真正发送完毕）*/
      //USART_ITConfig(_pUart->uart, USART_IT_TXE, DISABLE);
      CLEAR_BIT(_pUart->uart->CR1, USART_CR1_TXEIE);

      /* 使能数据发送完毕中断 */
      SET_BIT(_pUart->uart->CR1, USART_CR1_TCIE);
    }
    else
    {
      _pUart->Sending = 1;

      /* 从发送FIFO取1个字节写入串口发送数据寄存器 */
      _pUart->uart->TDR = _pUart->pTxBuf[_pUart->usTxRead];
      if (++_pUart->usTxRead >= _pUart->usTxBufSize)
      {
        _pUart->usTxRead = 0;
      }
      _pUart->usTxCount--;
    }
  }
  /* 数据bit位全部发送完毕的中断 */
  if (((isrflags & USART_ISR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
  {
    if (_pUart->usTxCount == 0)
    {
      /* 如果发送FIFO的数据全部发送完毕，禁止数据发送完毕中断 */
      CLEAR_BIT(_pUart->uart->CR1, USART_CR1_TCIE);
#if UART3_RS485_EN == 1
      /* 回调函数, 一般用来处理RS485通信，将RS485芯片设置为接收模式，避免抢占总线 */
      if (_pUart->SendOver)
      {
        _pUart->SendOver();
      }
#endif
      _pUart->Sending = 0;
    }
    else
    {
      /* 正常情况下，不会进入此分支 */
      /* 如果发送FIFO的数据还未完毕，则从发送FIFO取1个数据写入发送数据寄存器 */
      _pUart->uart->TDR = _pUart->pTxBuf[_pUart->usTxRead];
      if (++_pUart->usTxRead >= _pUart->usTxBufSize)
      {
        _pUart->usTxRead = 0;
      }
      _pUart->usTxCount--;
    }
  }

  /* 清除中断标志 */
  SET_BIT(_pUart->uart->ICR, UART_CLEAR_PEF);    /*  Framing Error Clear Flag          */
  SET_BIT(_pUart->uart->ICR, UART_CLEAR_FEF);    /*  Noise detected Clear Flag         */
  SET_BIT(_pUart->uart->ICR, UART_CLEAR_NEF);    /*  OverRun Error Clear Flag          */
  SET_BIT(_pUart->uart->ICR, UART_CLEAR_OREF);   /*  IDLE line detected Clear Flag     */
  SET_BIT(_pUart->uart->ICR, UART_CLEAR_IDLEF);  /*  Transmission Complete Clear Flag  */
  SET_BIT(_pUart->uart->ICR, UART_CLEAR_TCF);    /*  LIN Break Detection Clear Flag    */
  SET_BIT(_pUart->uart->ICR, UART_CLEAR_LBDF);   /*  CTS Interrupt Clear Flag          */
  SET_BIT(_pUart->uart->ICR, UART_CLEAR_CTSF);   /*  Receiver Time Out Clear Flag      */
  SET_BIT(_pUart->uart->ICR, UART_CLEAR_CMF);    /*  Character Match Clear Flag        */
  SET_BIT(_pUart->uart->ICR, UART_CLEAR_WUF);    /*  Wake Up from stop mode Clear Flag */
  SET_BIT(_pUart->uart->ICR, UART_CLEAR_TXFECF); /*  TXFIFO empty Clear Flag           */
}

/*
*********************************************************************************************************
*  函 数 名: USART1_IRQHandler  USART2_IRQHandler USART3_IRQHandler UART4_IRQHandler UART5_IRQHandler等
*  功能说明: USART中断服务程序
*  形    参: 无
*  返 回 值: 无
*********************************************************************************************************
*/

void USART3_IRQHandler(void)
{
  UART_IRQ_Hand(&g_tUart3);
}

/***************************** (END OF FILE) *********************************/
