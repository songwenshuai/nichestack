/**
  ******************************************************************************
  * @file    LwIP/LwIP_HTTP_Server_Netconn_RTOS/Src/ethernetif.c
  * @author  MCD Application Team
  * @brief   This file implements Ethernet network interface drivers for lwIP
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include  <cpu.h>
#include  <lib_mem.h>
#include  <os.h>
#include <string.h>
#include "stm32h7xx_hal.h"
#include "lan8742.h"
#include "printf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define ETH_RX_BUFFER_SIZE                                (1536UL)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* 
@Note: This interface is implemented to operate in zero-copy mode only:
        - Rx buffers are allocated statically and passed directly to the LwIP stack,
          they will return back to ETH DMA after been processed by the stack.
        - Tx Buffers will be allocated from LwIP stack memory heap, 
          then passed to ETH HAL driver.

@Notes: 
  1.a. ETH DMA Rx descriptors must be contiguous, the default count is 4, 
       to customize it please redefine ETH_RX_DESC_CNT in stm32xxxx_hal_conf.h
  1.b. ETH DMA Tx descriptors must be contiguous, the default count is 4, 
       to customize it please redefine ETH_TX_DESC_CNT in stm32xxxx_hal_conf.h

  2.a. Rx Buffers number must be between ETH_RX_DESC_CNT and 2*ETH_RX_DESC_CNT
  2.b. Rx Buffers must have the same size: ETH_RX_BUFFER_SIZE, this value must
       passed to ETH DMA in the init field (EthHandle.Init.RxBuffLen)
*/

#pragma location=0x30040000
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
#pragma location=0x30040060
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */
#pragma location=0x30040200
uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_RX_BUFFER_SIZE]; /* Ethernet Receive Buffers */

uint32_t current_pbuf_idx =0;

ETH_HandleTypeDef EthHandle;
ETH_TxPacketConfig TxConfig; 

lan8742_Object_t LAN8742;

uint32_t netif_is_link_up = 0;

/* Private function prototypes -----------------------------------------------*/

int32_t ETH_PHY_IO_Init(void);
int32_t ETH_PHY_IO_DeInit (void);
int32_t ETH_PHY_IO_WriteReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t RegVal);
int32_t ETH_PHY_IO_ReadReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t *pRegVal);
int32_t ETH_PHY_IO_GetTick(void);

lan8742_IOCtx_t  LAN8742_IOCtx = {ETH_PHY_IO_Init,
                                  ETH_PHY_IO_DeInit,
                                  ETH_PHY_IO_WriteReg,
                                  ETH_PHY_IO_ReadReg,
                                  ETH_PHY_IO_GetTick};

/* Private functions ---------------------------------------------------------*/
void ethernet_link_thread( void const * argument );


extern void ETH_DMATxDescListInit(ETH_HandleTypeDef *heth);
extern void ETH_DMARxDescListInit(ETH_HandleTypeDef *heth);

#ifdef NICHE
extern void hexdump(void * pio, void * buffer, unsigned len);
extern int input_ippkt(uint8_t *addr, int RxLen);
#endif

char mac_addr[6] = {01,02,55,66,88,99};
#define EETH_DEBUG 1

#ifdef NICHE
extern char mac_addr[6];
extern uint32_t ifacess;
extern struct net netstatic[STATIC_NETS];
#endif

#define APP_CFG_ETHIF_TASK_PRIO                           4u
#define APP_CFG_ETHLINK_TASK_PRIO                         5u

OS_EVENT * RxPktSemaphore;
static  OS_STK  EthifTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE * 2];
static  OS_STK  EthLinkTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE * 2];
static  void  EthifTask (void *p_arg);
static  void  EthLinkTask (void *p_arg);

/* FUNCTION: hexdump()
 *
 * hexdump() - does a hex dump to console of a passed buffer. The 
 * buffer is declared as void so structs can be passed with the 
 * Compiler fussing. 
 *
 * 
 * PARAM1: void * pio
 * PARAM2: void * buffer
 * PARAM3: unsigned len
 *
 * RETURNS: void
 */
#ifdef EETH_DEBUG

#define  HEX_BYTES_PER_LINE   16

void myhexdump(void * buffer, unsigned len)
{
   uint8_t * data  =  (uint8_t *)buffer;
   unsigned int count;
   char  c;

   while (len)
   {
      /* display data in hex */
      for (count = 0; (count < HEX_BYTES_PER_LINE) && (count < len); ++count)
         printf("%02x ", *(data + count));
      /* display data in ascii */
      for (count = 0; (count < HEX_BYTES_PER_LINE) && (count < len); ++count)
      {
         c = *(data + count);
         printf("%c", ((c >= 0x20) && (c < 0x7f)) ? c : '.');
      }
      printf("\n");
      len -= count;
      data += count;
   }
}
#endif

/*******************************************************************************
                       LL Driver Interface ( LwIP stack --> ETH) 
*******************************************************************************/
/**
  * @brief In this function, the hardware should be initialized.
  * Called from ethernetif_init().
  *
  * @param netif the already initialized lwip network interface structure
  *        for this ethernetif
  */
void ENET_Configuration(void)
{
  uint32_t idx, duplex, speed = 0;
  int32_t PHYLinkState;
  ETH_MACConfigTypeDef MACConf;
  CPU_INT08U  os_err;

#ifdef NICHE
  NET ifp;
  ifp = &netstatic[ifacess];
#endif

  EthHandle.Instance = ETH;  
  EthHandle.Init.MACAddr = (uint8_t *)mac_addr;
  EthHandle.Init.MediaInterface = HAL_ETH_RMII_MODE;
  EthHandle.Init.RxDesc = DMARxDscrTab;
  EthHandle.Init.TxDesc = DMATxDscrTab;
  EthHandle.Init.RxBuffLen = ETH_RX_BUFFER_SIZE;
  
  /* configure ethernet peripheral (GPIOs, clocks, MAC, DMA) */
  HAL_ETH_Init(&EthHandle);
  
  for(idx = 0; idx < ETH_RX_DESC_CNT; idx ++)
  {
    HAL_ETH_DescAssignMemory(&EthHandle, idx, Rx_Buff[idx], NULL);
  }
  
  memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfig));  
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
  
  RxPktSemaphore = OSSemCreate(0);
    OSTaskCreateExt( EthifTask,                               /* Create the Ethif task                              */
                     0,
                    &EthifTaskStk[(APP_CFG_STARTUP_TASK_STK_SIZE * 2) - 1u],
                     APP_CFG_ETHIF_TASK_PRIO,
                     APP_CFG_ETHIF_TASK_PRIO,
                    &EthifTaskStk[0u],
                     APP_CFG_STARTUP_TASK_STK_SIZE * 2,
                     0u,
                    (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if OS_TASK_NAME_EN > 0u
    OSTaskNameSet(         APP_CFG_ETHIF_TASK_PRIO,
                  (INT8U *)"Ethif Task",
                           &os_err);
#endif
    OSTaskCreateExt( EthLinkTask,                               /* Create the EthLink task                              */
                     0,
                    &EthLinkTaskStk[(APP_CFG_STARTUP_TASK_STK_SIZE * 2) - 1u],
                     APP_CFG_ETHLINK_TASK_PRIO,
                     APP_CFG_ETHLINK_TASK_PRIO,
                    &EthLinkTaskStk[0u],
                     (APP_CFG_STARTUP_TASK_STK_SIZE * 2),
                     0u,
                    (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if OS_TASK_NAME_EN > 0u
    OSTaskNameSet(         APP_CFG_ETHLINK_TASK_PRIO,
                  (INT8U *)"EthLink Task",
                           &os_err);
#endif
  /* Set PHY IO functions */
  LAN8742_RegisterBusIO(&LAN8742, &LAN8742_IOCtx);
  
  /* Initialize the LAN8742 ETH PHY */
  LAN8742_Init(&LAN8742);

  PHYLinkState = LAN8742_GetLinkState(&LAN8742);
  
  /* Get link state */  
  if(PHYLinkState <= LAN8742_STATUS_LINK_DOWN)
  {
    netif_is_link_up = 0;
#ifdef NICHE
    ifp->n_mib->ifAdminStatus = NI_DOWN; /* status = down */
    ifp->n_mib->ifOperStatus = NI_DOWN;  /* status = down */
    ifp->n_mib->ifLastChange = cticks * (100 / TPS);
#endif
    printf("Init ENET Close\n");
  }
  else 
  {
    switch (PHYLinkState)
    {
    case LAN8742_STATUS_100MBITS_FULLDUPLEX:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      break;
    case LAN8742_STATUS_100MBITS_HALFDUPLEX:
      duplex = ETH_HALFDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      break;
    case LAN8742_STATUS_10MBITS_FULLDUPLEX:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_10M;
      break;
    case LAN8742_STATUS_10MBITS_HALFDUPLEX:
      duplex = ETH_HALFDUPLEX_MODE;
      speed = ETH_SPEED_10M;
      break;
    default:
      duplex = ETH_FULLDUPLEX_MODE;
      speed = ETH_SPEED_100M;
      break;      
    }
    
    /* Get MAC Config MAC */
    HAL_ETH_GetMACConfig(&EthHandle, &MACConf); 
    MACConf.DuplexMode = duplex;
    MACConf.Speed = speed;
    HAL_ETH_SetMACConfig(&EthHandle, &MACConf);
    HAL_ETH_Start_IT(&EthHandle);
    netif_is_link_up = 1;
#ifdef NICHE
    ifp->n_mib->ifAdminStatus = NI_UP; /* status = UP */
    ifp->n_mib->ifOperStatus = NI_UP;
    ifp->n_mib->ifLastChange = cticks * (100/TPS);
#endif
    printf("Init ENET Open\n");
  }
}

/**
  * @brief This function should do the actual transmission of the packet. The packet is
  * contained in the pbuf that is passed to the function. This pbuf
  * might be chained.
  *
  * @param netif the lwip network interface structure for this ethernetif
  * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
  * @return 0 if the packet could be sent
  *         an int8_t value if the packet couldn't be sent
  *
  * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
  *       strange results. You might consider waiting for space in the DMA queue
  *       to become available since the stack doesn't retry to send a packet
  *       dropped because of memory failure (except for the TCP timers).
  */
int8_t ETH_SendTxPkt(uint8_t *buffer, uint32_t len)
{
  int8_t errval = HAL_OK;
  ETH_BufferTypeDef *Txbuffer = NULL;
  
  memset(Txbuffer, 0, sizeof(ETH_BufferTypeDef));
  
  Txbuffer->buffer  = buffer;
  Txbuffer->len     = len;
  Txbuffer->next    = NULL;
  
  TxConfig.Length   = len;
  TxConfig.TxBuffer = Txbuffer;

  /* Clean and Invalidate data cache */
  SCB_CleanInvalidateDCache();

  errval = HAL_ETH_Transmit(&EthHandle, &TxConfig, 0xFFFF);
  
  return errval;
}

/**
  * @brief This function is the ethernetif_input task, it is processed when a packet 
  * is ready to be read from the interface. It uses the function low_level_input() 
  * that should handle the actual reception of bytes from the network
  * interface. Then the type of the received packet is determined and
  * the appropriate input function is called.
  *
  * @param netif the lwip network interface structure for this ethernetif
  */
static  void  EthifTask (void *p_arg)
{
  (void)p_arg;
  ETH_BufferTypeDef RxBuff;
  uint32_t framelength = 0;
  CPU_INT08U err;
  
  for (;;)
  {
    OSSemPend(RxPktSemaphore, 0, &err);
    if (err == OS_ERR_NONE)
    {

      /* Clean and Invalidate data cache */
      SCB_CleanInvalidateDCache();

      if (HAL_ETH_GetRxDataBuffer(&EthHandle, &RxBuff) == HAL_OK)
      {
        HAL_ETH_GetRxDataLength(&EthHandle, &framelength);

        /* Invalidate data cache for ETH Rx Buffers */
        SCB_InvalidateDCache_by_Addr((uint32_t *)Rx_Buff, (ETH_RX_DESC_CNT * ETH_RX_BUFFER_SIZE));

        if (current_pbuf_idx < (ETH_RX_DESC_CNT - 1))
        {
          current_pbuf_idx++;
        }
        else
        {
          current_pbuf_idx = 0;
        }
      }
#ifdef EETH_DEBUG
      printf("RX\r\n");
      myhexdump(RxBuff.buffer, framelength);
      printf("\r\n");
#endif
      
#ifdef NICHE
      input_ippkt(RxBuff.buffer, framelength);
#endif
      /* Build Rx descriptor to be ready for next data reception */
      HAL_ETH_BuildRxDescriptors(&EthHandle);
    }
  }
}

/*******************************************************************************
                       Ethernet MSP Routines
*******************************************************************************/
/**
  * @brief  Initializes the ETH MSP.
  * @param  heth: ETH handle
  * @retval None
*/
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Ethernett MSP init: RMII Mode */
  
  /* Enable GPIOs clocks */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

/* Ethernet pins configuration ************************************************/
  /*
        RMII_REF_CLK ----------------------> PA1
        RMII_MDIO -------------------------> PA2
        RMII_MDC --------------------------> PC1
        RMII_MII_CRS_DV -------------------> PA7
        RMII_MII_RXD0 ---------------------> PC4
        RMII_MII_RXD1 ---------------------> PC5
        RMII_MII_RXER ---------------------> PG2
        RMII_MII_TX_EN --------------------> PG11
        RMII_MII_TXD0 ---------------------> PG13
        RMII_MII_TXD1 ---------------------> PB13
  */

  /* Configure PA1, PA2 and PA7 */
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL; 
  GPIO_InitStructure.Alternate = GPIO_AF11_ETH;
  GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Configure PB13 */
  GPIO_InitStructure.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* Configure PC1, PC4 and PC5 */
  GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Configure PG2, PG11, PG13 and PG14 */
  GPIO_InitStructure.Pin =  GPIO_PIN_2 | GPIO_PIN_11 | GPIO_PIN_13;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);	
  
  /* Enable the Ethernet global Interrupt */
  HAL_NVIC_SetPriority(ETH_IRQn, 0x7, 0);
  HAL_NVIC_EnableIRQ(ETH_IRQn);
  
  /* Enable Ethernet clocks */
  __HAL_RCC_ETH1MAC_CLK_ENABLE();
  __HAL_RCC_ETH1TX_CLK_ENABLE();
  __HAL_RCC_ETH1RX_CLK_ENABLE();
}

/**
  * @brief  Ethernet Rx Transfer completed callback
  * @param  heth: ETH handle
  * @retval None
  */
void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{
  CPU_INT08U err;

  err = OSSemPost(RxPktSemaphore);

  if (err != OS_ERR_NONE)
  {
    printf("Post RxPkt Semaphore Error !\r\n");
  }
}

/*******************************************************************************
                       PHI IO Functions
*******************************************************************************/
/**
  * @brief  Initializes the MDIO interface GPIO and clocks.
  * @param  None
  * @retval 0 if OK, -1 if ERROR
  */
int32_t ETH_PHY_IO_Init(void)
{  
  /* We assume that MDIO GPIO configuration is already done
     in the ETH_MspInit() else it should be done here 
  */
  
  /* Configure the MDIO Clock */
  HAL_ETH_SetMDIOClockRange(&EthHandle);
  
  return 0;
}

/**
  * @brief  De-Initializes the MDIO interface .
  * @param  None
  * @retval 0 if OK, -1 if ERROR
  */
int32_t ETH_PHY_IO_DeInit (void)
{
  return 0;
}

/**
  * @brief  Read a PHY register through the MDIO interface.
  * @param  DevAddr: PHY port address
  * @param  RegAddr: PHY register address
  * @param  pRegVal: pointer to hold the register value 
  * @retval 0 if OK -1 if Error
  */
int32_t ETH_PHY_IO_ReadReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t *pRegVal)
{
  if(HAL_ETH_ReadPHYRegister(&EthHandle, DevAddr, RegAddr, pRegVal) != HAL_OK)
  {
    return -1;
  }
  
  return 0;
}

/**
  * @brief  Write a value to a PHY register through the MDIO interface.
  * @param  DevAddr: PHY port address
  * @param  RegAddr: PHY register address
  * @param  RegVal: Value to be written 
  * @retval 0 if OK -1 if Error
  */
int32_t ETH_PHY_IO_WriteReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t RegVal)
{
  if(HAL_ETH_WritePHYRegister(&EthHandle, DevAddr, RegAddr, RegVal) != HAL_OK)
  {
    return -1;
  }
  
  return 0;
}

/**
  * @brief  Get the time in millisecons used for internal PHY driver process.
  * @retval Time value
  */
int32_t ETH_PHY_IO_GetTick(void)
{
  return HAL_GetTick();
}

/**
  * @brief  Check the ETH link state and update netif accordingly.
  * @param  argument: netif
  * @retval None
  */
static  void  EthLinkTask (void *p_arg)
{
  (void)p_arg;
  ETH_MACConfigTypeDef MACConf;
  int32_t PHYLinkState;
  uint32_t linkchanged = 0, speed = 0, duplex =0;
#ifdef NICHE
  NET ifp;
  ifp = &netstatic[ifacess];
#endif
  for(;;)
  {
    
    PHYLinkState = LAN8742_GetLinkState(&LAN8742);
    
    if((netif_is_link_up) && (PHYLinkState <= LAN8742_STATUS_LINK_DOWN))
    {
      HAL_ETH_Stop_IT(&EthHandle);
      netif_is_link_up = 0;
#ifdef NICHE
      ifp->n_mib->ifAdminStatus = NI_DOWN; /* status = down */
      ifp->n_mib->ifOperStatus = NI_DOWN;  /* status = down */
      ifp->n_mib->ifLastChange = cticks * (100 / TPS);
#endif
      printf("Check ENET Close\n");
    }
    else if((!netif_is_link_up) && (PHYLinkState > LAN8742_STATUS_LINK_DOWN))
    {
      switch (PHYLinkState)
      {
      case LAN8742_STATUS_100MBITS_FULLDUPLEX:
        duplex = ETH_FULLDUPLEX_MODE;
        speed = ETH_SPEED_100M;
        linkchanged = 1;
        break;
      case LAN8742_STATUS_100MBITS_HALFDUPLEX:
        duplex = ETH_HALFDUPLEX_MODE;
        speed = ETH_SPEED_100M;
        linkchanged = 1;
        break;
      case LAN8742_STATUS_10MBITS_FULLDUPLEX:
        duplex = ETH_FULLDUPLEX_MODE;
        speed = ETH_SPEED_10M;
        linkchanged = 1;
        break;
      case LAN8742_STATUS_10MBITS_HALFDUPLEX:
        duplex = ETH_HALFDUPLEX_MODE;
        speed = ETH_SPEED_10M;
        linkchanged = 1;
        break;
      default:
        break;      
      }
      
      if(linkchanged)
      {
        /*------------------ DMA Tx Descriptors Configuration ----------------------*/
        ETH_DMATxDescListInit(&EthHandle);
        /*------------------ DMA Rx Descriptors Configuration ----------------------*/
        ETH_DMARxDescListInit(&EthHandle);
        
        for(uint32_t idx = 0; idx < ETH_RX_DESC_CNT; idx ++)
        {
          HAL_ETH_DescAssignMemory(&EthHandle, idx, Rx_Buff[idx], NULL);
        }
        
        /* Get MAC Config MAC */
        HAL_ETH_GetMACConfig(&EthHandle, &MACConf); 
        MACConf.DuplexMode = duplex;
        MACConf.Speed = speed;
        HAL_ETH_SetMACConfig(&EthHandle, &MACConf);
        HAL_ETH_Start_IT(&EthHandle);
#ifdef NICHE
        ifp->n_mib->ifAdminStatus = NI_UP; /* status = UP */
        ifp->n_mib->ifOperStatus = NI_UP;
        ifp->n_mib->ifLastChange = cticks * (100/TPS);
#endif
        netif_is_link_up = 1;
        printf("Check ENET Open\r\n");
      }
    }
    
    OSTimeDly(100);
  }
}

/**
  * @brief  This function handles Ethernet interrupt request.
  * @param  None
  * @retval None
  */
void ETH_IRQHandler(void)
{
  HAL_ETH_IRQHandler(&EthHandle);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
