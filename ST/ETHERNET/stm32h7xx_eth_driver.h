#ifndef _STM32H7XX_ETH_DRIVER_H
#define _STM32H7XX_ETH_DRIVER_H

//Dependencies
#include "stdint.h"
#include "string.h"
#include "compiler_port.h"
#include "error.h"

//TCP/IP stack tick interval
#define NET_TICK_INTERVAL 100

//Maximum duration a write operation may block
#define NIC_MAX_BLOCKING_TIME INFINITE_DELAY

//Number of TX buffers
#define STM32H7XX_ETH_TX_BUFFER_COUNT        8

//TX buffer size
#define STM32H7XX_ETH_TX_BUFFER_SIZE         1536

//Number of RX buffers
#define STM32H7XX_ETH_RX_BUFFER_COUNT        8

//RX buffer size
#define STM32H7XX_ETH_RX_BUFFER_SIZE         1536

//Interrupt priority grouping
#define STM32H7XX_ETH_IRQ_PRIORITY_GROUPING  3

//Ethernet interrupt group priority
#define STM32H7XX_ETH_IRQ_GROUP_PRIORITY     12

//Ethernet interrupt subpriority
#define STM32H7XX_ETH_IRQ_SUB_PRIORITY       0

//MMCRIMR register
#define ETH_MMCRIMR_RXLPITRCIM  0x08000000
#define ETH_MMCRIMR_RXLPIUSCIM  0x04000000
#define ETH_MMCRIMR_RXUCGPIM    0x00020000
#define ETH_MMCRIMR_RXALGNERPIM 0x00000040
#define ETH_MMCRIMR_RXCRCERPIM  0x00000020

//MMCTIMR register
#define ETH_MMCTIMR_TXLPITRCIM  0x08000000
#define ETH_MMCTIMR_TXLPIUSCIM  0x04000000
#define ETH_MMCTIMR_TXGPKTIM    0x00200000
#define ETH_MMCTIMR_TXMCOLGPIM  0x00008000
#define ETH_MMCTIMR_TXSCOLGPIM  0x00004000

//Transmit normal descriptor (read format)
#define ETH_TDES0_BUF1AP        0xFFFFFFFF
#define ETH_TDES1_BUF2AP        0xFFFFFFFF
#define ETH_TDES2_IOC           0x80000000
#define ETH_TDES2_TTSE          0x40000000
#define ETH_TDES2_B2L           0x3FFF0000
#define ETH_TDES2_VTIR          0x0000C000
#define ETH_TDES2_B1L           0x00003FFF
#define ETH_TDES3_OWN           0x80000000
#define ETH_TDES3_CTXT          0x40000000
#define ETH_TDES3_FD            0x20000000
#define ETH_TDES3_LD            0x10000000
#define ETH_TDES3_CPC           0x0C000000
#define ETH_TDES3_SAIC          0x03800000
#define ETH_TDES3_THL           0x00780000
#define ETH_TDES3_TSE           0x00040000
#define ETH_TDES3_CIC           0x00030000
#define ETH_TDES3_FL            0x00007FFF

//Transmit normal descriptor (write-back format)
#define ETH_TDES0_TTSL          0xFFFFFFFF
#define ETH_TDES1_TTSH          0xFFFFFFFF
#define ETH_TDES3_OWN           0x80000000
#define ETH_TDES3_CTXT          0x40000000
#define ETH_TDES3_FD            0x20000000
#define ETH_TDES3_LD            0x10000000
#define ETH_TDES3_TTSS          0x00020000
#define ETH_TDES3_ES            0x00008000
#define ETH_TDES3_JT            0x00004000
#define ETH_TDES3_FF            0x00002000
#define ETH_TDES3_PCE           0x00001000
#define ETH_TDES3_LOC           0x00000800
#define ETH_TDES3_NC            0x00000400
#define ETH_TDES3_LC            0x00000200
#define ETH_TDES3_EC            0x00000100
#define ETH_TDES3_CC            0x000000F0
#define ETH_TDES3_ED            0x00000008
#define ETH_TDES3_UF            0x00000004
#define ETH_TDES3_DB            0x00000002
#define ETH_TDES3_IHE           0x00000001

//Transmit context descriptor
#define ETH_TDES0_TTSL          0xFFFFFFFF
#define ETH_TDES1_TTSH          0xFFFFFFFF
#define ETH_TDES2_IVT           0xFFFF0000
#define ETH_TDES2_MSS           0x00003FFF
#define ETH_TDES3_OWN           0x80000000
#define ETH_TDES3_CTXT          0x40000000
#define ETH_TDES3_OSTC          0x08000000
#define ETH_TDES3_TCMSSV        0x04000000
#define ETH_TDES3_CDE           0x00800000
#define ETH_TDES3_IVLTV         0x00020000
#define ETH_TDES3_VLTV          0x00010000
#define ETH_TDES3_VT            0x0000FFFF

//Receive normal descriptor (read format)
#define ETH_RDES0_BUF1AP        0xFFFFFFFF
#define ETH_RDES2_BUF2AP        0xFFFFFFFF
#define ETH_RDES3_OWN           0x80000000
#define ETH_RDES3_IOC           0x40000000
#define ETH_RDES3_BUF2V         0x02000000
#define ETH_RDES3_BUF1V         0x01000000

//Receive normal descriptor (write-back format)
#define ETH_RDES0_IVT           0xFFFF0000
#define ETH_RDES0_OVT           0x0000FFFF
#define ETH_RDES1_OPC           0xFFFF0000
#define ETH_RDES1_TD            0x00008000
#define ETH_RDES1_TSA           0x00004000
#define ETH_RDES1_PV            0x00002000
#define ETH_RDES1_PFT           0x00001000
#define ETH_RDES1_PMT           0x00000F00
#define ETH_RDES1_IPCE          0x00000080
#define ETH_RDES1_IPCB          0x00000040
#define ETH_RDES1_IPV6          0x00000020
#define ETH_RDES1_IPV4          0x00000010
#define ETH_RDES1_IPHE          0x00000008
#define ETH_RDES1_PT            0x00000007
#define ETH_RDES2_L3L4FM        0xE0000000
#define ETH_RDES2_L4FM          0x10000000
#define ETH_RDES2_L3FM          0x08000000
#define ETH_RDES2_MADRM         0x07F80000
#define ETH_RDES2_HF            0x00040000
#define ETH_RDES2_DAF           0x00020000
#define ETH_RDES2_SAF           0x00010000
#define ETH_RDES2_VF            0x00008000
#define ETH_RDES2_ARPRN         0x00000400
#define ETH_RDES3_OWN           0x80000000
#define ETH_RDES3_CTXT          0x40000000
#define ETH_RDES3_FD            0x20000000
#define ETH_RDES3_LD            0x10000000
#define ETH_RDES3_RS2V          0x08000000
#define ETH_RDES3_RS1V          0x04000000
#define ETH_RDES3_RS0V          0x02000000
#define ETH_RDES3_CE            0x01000000
#define ETH_RDES3_GP            0x00800000
#define ETH_RDES3_RWT           0x00400000
#define ETH_RDES3_OE            0x00200000
#define ETH_RDES3_RE            0x00100000
#define ETH_RDES3_DE            0x00080000
#define ETH_RDES3_LT            0x00070000
#define ETH_RDES3_ES            0x00008000
#define ETH_RDES3_PL            0x00007FFF

//Receive context descriptor
#define ETH_RDES0_RTSL          0xFFFFFFFF
#define ETH_RDES1_RTSH          0xFFFFFFFF
#define ETH_RDES3_OWN           0x80000000
#define ETH_RDES3_CTXT          0x40000000

//PHY address
#define LAN8742_PHY_ADDR 0

//LAN8742 registers
#define LAN8742_PHY_REG_BMCR        0x00
#define LAN8742_PHY_REG_BMSR        0x01
#define LAN8742_PHY_REG_PHYIDR1     0x02
#define LAN8742_PHY_REG_PHYIDR2     0x03
#define LAN8742_PHY_REG_ANAR        0x04
#define LAN8742_PHY_REG_ANLPAR      0x05
#define LAN8742_PHY_REG_ANER        0x06
#define LAN8742_PHY_REG_ANNPTR      0x07
#define LAN8742_PHY_REG_ANNPRR      0x08
#define LAN8742_PHY_REG_MMDACR      0x0D
#define LAN8742_PHY_REG_MMDAADR     0x0E
#define LAN8742_PHY_REG_ENCTR       0x10
#define LAN8742_PHY_REG_MCSR        0x11
#define LAN8742_PHY_REG_SMR         0x12
#define LAN8742_PHY_REG_TDRPDCR     0x18
#define LAN8742_PHY_REG_TDRCSR      0x19
#define LAN8742_PHY_REG_SECR        0x1A
#define LAN8742_PHY_REG_SCSIR       0x1B
#define LAN8742_PHY_REG_CLR         0x1C
#define LAN8742_PHY_REG_ISR         0x1D
#define LAN8742_PHY_REG_IMR         0x1E
#define LAN8742_PHY_REG_PSCSR       0x1F

//BMCR register
#define BMCR_RESET                  (1 << 15)
#define BMCR_LOOPBACK               (1 << 14)
#define BMCR_SPEED_SEL              (1 << 13)
#define BMCR_AN_EN                  (1 << 12)
#define BMCR_POWER_DOWN             (1 << 11)
#define BMCR_ISOLATE                (1 << 10)
#define BMCR_RESTART_AN             (1 << 9)
#define BMCR_DUPLEX_MODE            (1 << 8)
#define BMCR_COL_TEST               (1 << 7)

//BMSR register
#define BMSR_100BT4                 (1 << 15)
#define BMSR_100BTX_FD              (1 << 14)
#define BMSR_100BTX                 (1 << 13)
#define BMSR_10BT_FD                (1 << 12)
#define BMSR_10BT                   (1 << 11)
#define BMSR_100BT2_FD              (1 << 10)
#define BMSR_100BT2                 (1 << 9)
#define BMSR_EXTENTED_STATUS        (1 << 8)
#define BMSR_AN_COMPLETE            (1 << 5)
#define BMSR_REMOTE_FAULT           (1 << 4)
#define BMSR_AN_ABLE                (1 << 3)
#define BMSR_LINK_STATUS            (1 << 2)
#define BMSR_JABBER_DETECT          (1 << 1)
#define BMSR_EXTENDED_CAP           (1 << 0)

//ANAR register
#define ANAR_NP                     (1 << 15)
#define ANAR_RF                     (1 << 13)
#define ANAR_PAUSE1                 (1 << 11)
#define ANAR_PAUSE0                 (1 << 10)
#define ANAR_100BTX_FD              (1 << 8)
#define ANAR_100BTX                 (1 << 7)
#define ANAR_10BT_FD                (1 << 6)
#define ANAR_10BT                   (1 << 5)
#define ANAR_SELECTOR4              (1 << 4)
#define ANAR_SELECTOR3              (1 << 3)
#define ANAR_SELECTOR2              (1 << 2)
#define ANAR_SELECTOR1              (1 << 1)
#define ANAR_SELECTOR0              (1 << 0)

//ANLPAR register
#define ANLPAR_NP                   (1 << 15)
#define ANLPAR_ACK                  (1 << 14)
#define ANLPAR_RF                   (1 << 13)
#define ANLPAR_PAUSE1               (1 << 11)
#define ANLPAR_PAUSE0               (1 << 10)
#define ANLPAR_100BT4               (1 << 9)
#define ANLPAR_100BTX_FD            (1 << 8)
#define ANLPAR_100BTX               (1 << 7)
#define ANLPAR_10BT_FD              (1 << 6)
#define ANLPAR_10BT                 (1 << 5)
#define ANLPAR_SELECTOR4            (1 << 4)
#define ANLPAR_SELECTOR3            (1 << 3)
#define ANLPAR_SELECTOR2            (1 << 2)
#define ANLPAR_SELECTOR1            (1 << 1)
#define ANLPAR_SELECTOR0            (1 << 0)

//ANER register
#define ANER_RX_NP_LOC_ABLE         (1 << 6)
#define ANER_RX_NP_STOR_LOC         (1 << 5)
#define ANER_PDF                    (1 << 4)
#define ANER_LP_NP_ABLE             (1 << 3)
#define ANER_NP_ABLE                (1 << 2)
#define ANER_PAGE_RX                (1 << 1)
#define ANER_LP_AN_ABLE             (1 << 0)

//ANNPTR register
#define ANNPTR_NEXT_PAGE            (1 << 15)
#define ANNPTR_MSG_PAGE             (1 << 13)
#define ANNPTR_ACK2                 (1 << 12)
#define ANNPTR_TOGGLE               (1 << 11)
#define ANNPTR_MESSAGE10            (1 << 10)
#define ANNPTR_MESSAGE9             (1 << 9)
#define ANNPTR_MESSAGE8             (1 << 8)
#define ANNPTR_MESSAGE7             (1 << 7)
#define ANNPTR_MESSAGE6             (1 << 6)
#define ANNPTR_MESSAGE5             (1 << 5)
#define ANNPTR_MESSAGE4             (1 << 4)
#define ANNPTR_MESSAGE3             (1 << 3)
#define ANNPTR_MESSAGE2             (1 << 2)
#define ANNPTR_MESSAGE1             (1 << 1)
#define ANNPTR_MESSAGE0             (1 << 0)

//ANNPRR register
#define ANNPRR_NEXT_PAGE            (1 << 15)
#define ANNPRR_ACK                  (1 << 14)
#define ANNPRR_MSG_PAGE             (1 << 13)
#define ANNPRR_ACK2                 (1 << 12)
#define ANNPRR_TOGGLE               (1 << 11)
#define ANNPRR_MESSAGE10            (1 << 10)
#define ANNPRR_MESSAGE9             (1 << 9)
#define ANNPRR_MESSAGE8             (1 << 8)
#define ANNPRR_MESSAGE7             (1 << 7)
#define ANNPRR_MESSAGE6             (1 << 6)
#define ANNPRR_MESSAGE5             (1 << 5)
#define ANNPRR_MESSAGE4             (1 << 4)
#define ANNPRR_MESSAGE3             (1 << 3)
#define ANNPRR_MESSAGE2             (1 << 2)
#define ANNPRR_MESSAGE1             (1 << 1)
#define ANNPRR_MESSAGE0             (1 << 0)

//MMDACR register
#define MMDACR_FUNCTION1            (1 << 15)
#define MMDACR_FUNCTION0            (1 << 14)
#define MMDACR_DEVAD4               (1 << 4)
#define MMDACR_DEVAD3               (1 << 3)
#define MMDACR_DEVAD2               (1 << 2)
#define MMDACR_DEVAD1               (1 << 1)
#define MMDACR_DEVAD0               (1 << 0)

//ENCTR register
#define ENCTR_EDPD_TX_NLP_EN        (1 << 15)
#define ENCTR_EDPD_TX_NLP_ITS1      (1 << 14)
#define ENCTR_EDPD_TX_NLP_ITS0      (1 << 13)
#define ENCTR_EDPD_RX_NLP_WAKE_EN   (1 << 12)
#define ENCTR_EDPD_RX_NLP_MIDS1     (1 << 11)
#define ENCTR_EDPD_RX_NLP_MIDS0     (1 << 10)
#define ENCTR_EDPD_EXT_CROSSOVER    (1 << 1)
#define ENCTR_EXT_CROSSOVER_TIME    (1 << 0)

//MCSR register
#define MCSR_EDPWRDOWN              (1 << 13)
#define MCSR_FARLOOPBACK            (1 << 9)
#define MCSR_ALTINT                 (1 << 6)
#define MCSR_ENERGYON               (1 << 1)

//SMR register
#define SMR_MODE2                   (1 << 7)
#define SMR_MODE1                   (1 << 6)
#define SMR_MODE0                   (1 << 5)
#define SMR_PHYAD4                  (1 << 4)
#define SMR_PHYAD3                  (1 << 3)
#define SMR_PHYAD2                  (1 << 2)
#define SMR_PHYAD1                  (1 << 1)
#define SMR_PHYAD0                  (1 << 0)

//TDRPDCR register
#define TDRPDCR_DELAY_IN            (1 << 15)
#define TDRPDCR_LINE_BREAK_COUNTER2 (1 << 14)
#define TDRPDCR_LINE_BREAK_COUNTER1 (1 << 13)
#define TDRPDCR_LINE_BREAK_COUNTER0 (1 << 12)
#define TDRPDCR_PATTERN_HIGH5       (1 << 11)
#define TDRPDCR_PATTERN_HIGH4       (1 << 10)
#define TDRPDCR_PATTERN_HIGH3       (1 << 9)
#define TDRPDCR_PATTERN_HIGH2       (1 << 8)
#define TDRPDCR_PATTERN_HIGH1       (1 << 7)
#define TDRPDCR_PATTERN_HIGH0       (1 << 6)
#define TDRPDCR_PATTERN_LOW5        (1 << 5)
#define TDRPDCR_PATTERN_LOW4        (1 << 4)
#define TDRPDCR_PATTERN_LOW3        (1 << 3)
#define TDRPDCR_PATTERN_LOW2        (1 << 2)
#define TDRPDCR_PATTERN_LOW1        (1 << 1)
#define TDRPDCR_PATTERN_LOW0        (1 << 0)

//TDRCSR register
#define TDRCSR_EN                   (1 << 15)
#define TDRCSR_AD_FILTER_EN         (1 << 14)
#define TDRCSR_CH_CABLE_TYPE1       (1 << 10)
#define TDRCSR_CH_CABLE_TYPE0       (1 << 9)
#define TDRCSR_CH_STATUS            (1 << 8)
#define TDRCSR_CH_LENGTH7           (1 << 7)
#define TDRCSR_CH_LENGTH6           (1 << 6)
#define TDRCSR_CH_LENGTH5           (1 << 5)
#define TDRCSR_CH_LENGTH4           (1 << 4)
#define TDRCSR_CH_LENGTH3           (1 << 3)
#define TDRCSR_CH_LENGTH2           (1 << 2)
#define TDRCSR_CH_LENGTH1           (1 << 1)
#define TDRCSR_CH_LENGTH0           (1 << 0)

//SCSIR register
#define SCSIR_AMDIXCTRL             (1 << 15)
#define SCSIR_CH_SELECT             (1 << 13)
#define SCSIR_SQEOFF                (1 << 11)
#define SCSIR_XPOL                  (1 << 4)

//CLR register
#define CLR_CBLN3                   (1 << 15)
#define CLR_CBLN2                   (1 << 14)
#define CLR_CBLN1                   (1 << 13)
#define CLR_CBLN0                   (1 << 12)

//ISR register
#define ISR_WOL                     (1 << 8)
#define ISR_ENERGYON                (1 << 7)
#define ISR_AN_COMPLETE             (1 << 6)
#define ISR_REMOTE_FAULT            (1 << 5)
#define ISR_LINK_DOWN               (1 << 4)
#define ISR_AN_LP_ACK               (1 << 3)
#define ISR_PD_FAULT                (1 << 2)
#define ISR_AN_PAGE_RECEIVED        (1 << 1)

//IMR register
#define IMR_WOL                     (1 << 8)
#define IMR_ENERGYON                (1 << 7)
#define IMR_AN_COMPLETE             (1 << 6)
#define IMR_REMOTE_FAULT            (1 << 5)
#define IMR_LINK_DOWN               (1 << 4)
#define IMR_AN_LP_ACK               (1 << 3)
#define IMR_PD_FAULT                (1 << 2)
#define IMR_AN_PAGE_RECEIVED        (1 << 1)

//PSCSR register
#define PSCSR_AUTODONE              (1 << 12)
#define PSCSR_HCDSPEED2             (1 << 4)
#define PSCSR_HCDSPEED1             (1 << 3)
#define PSCSR_HCDSPEED0             (1 << 2)

//Speed indication
#define PSCSR_HCDSPEED_MASK         (7 << 2)
#define PSCSR_HCDSPEED_10BT         (1 << 2)
#define PSCSR_HCDSPEED_100BTX       (2 << 2)
#define PSCSR_HCDSPEED_10BT_FD      (5 << 2)
#define PSCSR_HCDSPEED_100BTX_FD    (6 << 2)

//C++ guard
#ifdef __cplusplus
   extern "C" {
#endif


/**
 * @brief Transmit descriptor
 **/

typedef struct
{
   uint32_t tdes0;
   uint32_t tdes1;
   uint32_t tdes2;
   uint32_t tdes3;
} Stm32h7xxTxDmaDesc;


/**
 * @brief Receive descriptor
 **/

typedef struct
{
   uint32_t rdes0;
   uint32_t rdes1;
   uint32_t rdes2;
   uint32_t rdes3;
} Stm32h7xxRxDmaDesc;

/**
 * @brief Link speed
 **/

typedef enum
{
   NIC_LINK_SPEED_UNKNOWN = 0,
   NIC_LINK_SPEED_10MBPS  = 10000000,
   NIC_LINK_SPEED_100MBPS = 100000000,
   NIC_LINK_SPEED_1GBPS   = 1000000000
} NicLinkSpeed;

/**
 * @brief Duplex mode
 **/

typedef enum
{
   NIC_UNKNOWN_DUPLEX_MODE = 0,
   NIC_HALF_DUPLEX_MODE    = 1,
   NIC_FULL_DUPLEX_MODE    = 2
} NicDuplexMode;


extern uint32_t ifduplexMode, iflinkspeed;

void ENET_Configuration(void);
void stm32h7xxEthEnableIrq(void);
void stm32h7xxEthDisableIrq(void);
error_t nicSendPacket(uint8_t *buffer, uint32_t length);

//C++ guard
#ifdef __cplusplus
   }
#endif

#endif
