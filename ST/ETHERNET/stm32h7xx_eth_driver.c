//Dependencies
#include <stm32h7xx_hal.h>
#include "stm32h7xx_eth_driver.h"

#include "os_port.h"
#include "debug.h"

#include "osport.h"
#include "bsp_led.h"
#include "ifec.h"

//Transmit buffer
#pragma data_alignment = 4
#pragma location = ".ram_no_cache"
static uint8_t txBuffer[STM32H7XX_ETH_TX_BUFFER_COUNT][STM32H7XX_ETH_TX_BUFFER_SIZE];

//Receive buffer
#pragma data_alignment = 4
#pragma location = ".ram_no_cache"
static uint8_t rxBuffer[STM32H7XX_ETH_RX_BUFFER_COUNT][STM32H7XX_ETH_RX_BUFFER_SIZE];

//Transmit DMA descriptors
#pragma data_alignment = 4
#pragma location = ".ram_no_cache"
static Stm32h7xxTxDmaDesc txDmaDesc[STM32H7XX_ETH_TX_BUFFER_COUNT];

//Receive DMA descriptors
#pragma data_alignment = 4
#pragma location = ".ram_no_cache"
static Stm32h7xxRxDmaDesc rxDmaDesc[STM32H7XX_ETH_RX_BUFFER_COUNT];

//Current transmit descriptor
static unsigned int txIndex;
//Current receive descriptor
static unsigned int rxIndex;

uint32_t ifduplexMode = 0, iflinkspeed = 0, iflinkState = 0;


OsEvent TxEvent; ///<Network controller TX event
//Event object to receive notifications from device drivers
OsEvent netEvent;

int ethpEvent; ///<A PHY event is pending
int netpEvent; ///<A NIC event is pending

//Timestamp
uint32_t netTimestamp;

#ifdef ALT_INICHE
extern MacAddr Addrmac;
extern uint32_t ifacess;
extern uint32_t rxInts;
extern struct net netstatic[STATIC_NETS];
extern int input_ippkt(uint8_t *addr, int RxLen);
#endif

/**
 * @brief Write PHY register
 * @param[in] phyAddr PHY address
 * @param[in] regAddr Register address
 * @param[in] data Register value
 **/

static void stm32h7xxEthWritePhyReg(uint8_t phyAddr, uint8_t regAddr, uint16_t data)
{
    uint32_t value;

    //Take care not to alter MDC clock configuration
    value = ETH->MACMDIOAR & ETH_MACMDIOAR_CR;
    //Set up a write operation
    value |= ETH_MACMDIOAR_MOC_WR | ETH_MACMDIOAR_MB;
    //PHY address
    value |= (phyAddr << 21) & ETH_MACMDIOAR_PA;
    //Register address
    value |= (regAddr << 16) & ETH_MACMDIOAR_RDA;

    //Data to be written in the PHY register
    ETH->MACMDIODR = data & ETH_MACMDIODR_MD;

    //Start a write operation
    ETH->MACMDIOAR = value;
    //Wait for the write to complete
    while (ETH->MACMDIOAR & ETH_MACMDIOAR_MB)
        ;
}

/**
 * @brief Read PHY register
 * @param[in] phyAddr PHY address
 * @param[in] regAddr Register address
 * @return Register value
 **/

static uint16_t stm32h7xxEthReadPhyReg(uint8_t phyAddr, uint8_t regAddr)
{
    uint32_t value;

    //Take care not to alter MDC clock configuration
    value = ETH->MACMDIOAR & ETH_MACMDIOAR_CR;
    //Set up a read operation
    value |= ETH_MACMDIOAR_MOC_RD | ETH_MACMDIOAR_MB;
    //PHY address
    value |= (phyAddr << 21) & ETH_MACMDIOAR_PA;
    //Register address
    value |= (regAddr << 16) & ETH_MACMDIOAR_RDA;

    //Start a read operation
    ETH->MACMDIOAR = value;
    //Wait for the read to complete
    while (ETH->MACMDIOAR & ETH_MACMDIOAR_MB)
        ;

    //Return PHY register contents
    return ETH->MACMDIODR & ETH_MACMDIODR_MD;
}

/**
 * @brief GPIO configuration
 **/

static void stm32h7xxEthInitGpio(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    //Enable SYSCFG clock
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    //Enable GPIO clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    //Select RMII mode
    HAL_SYSCFG_ETHInterfaceSelect(SYSCFG_ETH_RMII);

    //Configure RMII pins
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AF11_ETH;

    //Configure ETH_RMII_REF_CLK (PA1), ETH_MDIO (PA2) and ETH_RMII_CRS_DV (PA7)
    GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    //Configure ETH_RMII_TXD1 (PB13)
    GPIO_InitStructure.Pin = GPIO_PIN_13;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

    //Configure ETH_MDC (PC1), ETH_RMII_RXD0 (PC4) and ETH_RMII_RXD1 (PC5)
    GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

    //Configure RMII_TX_EN (PG11), ETH_RMII_TXD0 (PG13)
    GPIO_InitStructure.Pin = GPIO_PIN_11 | GPIO_PIN_13;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/**
 * @brief Initialize DMA descriptor lists
 **/

static void stm32h7xxEthInitDmaDesc(void)
{
    unsigned int i;

    //Initialize TX DMA descriptor list
    for (i = 0; i < STM32H7XX_ETH_TX_BUFFER_COUNT; i++)
    {
        //The descriptor is initially owned by the application
        txDmaDesc[i].tdes0 = 0;
        txDmaDesc[i].tdes1 = 0;
        txDmaDesc[i].tdes2 = 0;
        txDmaDesc[i].tdes3 = 0;
    }

    //Initialize TX descriptor index
    txIndex = 0;

    //Initialize RX DMA descriptor list
    for (i = 0; i < STM32H7XX_ETH_RX_BUFFER_COUNT; i++)
    {
        //The descriptor is initially owned by the DMA
        rxDmaDesc[i].rdes0 = (uint32_t)rxBuffer[i];
        rxDmaDesc[i].rdes1 = 0;
        rxDmaDesc[i].rdes2 = 0;
        rxDmaDesc[i].rdes3 = ETH_RDES3_OWN | ETH_RDES3_IOC | ETH_RDES3_BUF1V;
    }

    //Initialize RX descriptor index
    rxIndex = 0;

    //Start location of the TX descriptor list
    ETH->DMACTDLAR = (uint32_t)&txDmaDesc[0];
    //Length of the transmit descriptor ring
    ETH->DMACTDRLR = STM32H7XX_ETH_TX_BUFFER_COUNT - 1;

    //Start location of the RX descriptor list
    ETH->DMACRDLAR = (uint32_t)&rxDmaDesc[0];
    //Length of the receive descriptor ring
    ETH->DMACRDRLR = STM32H7XX_ETH_RX_BUFFER_COUNT - 1;
}

/**
 * @brief Adjust MAC configuration parameters for proper operation
 * @return Error code
 **/

static error_t stm32h7xxEthUpdateMacConfig(void)
{
    uint32_t config;

    //Read current MAC configuration
    config = ETH->MACCR;

    //10BASE-T or 100BASE-TX operation mode?
    if (iflinkspeed == NIC_LINK_SPEED_100MBPS)
        config |= ETH_MACCR_FES;
    else
        config &= ~ETH_MACCR_FES;

    //Half-duplex or full-duplex mode?
    if (ifduplexMode == NIC_FULL_DUPLEX_MODE)
        config |= ETH_MACCR_DM;
    else
        config &= ~ETH_MACCR_DM;

    //Update MAC configuration register
    ETH->MACCR = config;

    //Successful processing
    return NO_ERROR;
}

/**
 * @brief LAN8742 event handler
 **/

static void lan8742EventHandler(void)
{
    uint16_t value;
#ifdef ALT_INICHE
    NET ifp;
    ifp = &netstatic[ifacess];
#endif
    //Read status register to acknowledge the interrupt
    value = stm32h7xxEthReadPhyReg(LAN8742_PHY_ADDR, LAN8742_PHY_REG_ISR);

    //Link status change?
    if (value & (IMR_AN_COMPLETE | IMR_LINK_DOWN))
    {
        //Any link failure condition is latched in the BMSR register... Reading
        //the register twice will always return the actual link status
        value = stm32h7xxEthReadPhyReg(LAN8742_PHY_ADDR, LAN8742_PHY_REG_BMSR);
        value = stm32h7xxEthReadPhyReg(LAN8742_PHY_ADDR, LAN8742_PHY_REG_BMSR);

        //Link is up?
        if (value & BMSR_LINK_STATUS)
        {
#ifdef ALT_INICHE
            ifp->n_mib->ifAdminStatus = NI_UP; /* status = UP */
            ifp->n_mib->ifOperStatus = NI_UP;
            ifp->n_mib->ifLastChange = cticks * (100 / TPS);
#endif
            //Display link state
            TRACE_INFO("Link is up ...\r\n");
            //Read PHY special control/status register
            value = stm32h7xxEthReadPhyReg(LAN8742_PHY_ADDR, LAN8742_PHY_REG_PSCSR);

            //Check current operation mode
            switch (value & PSCSR_HCDSPEED_MASK)
            {
            //10BASE-T
            case PSCSR_HCDSPEED_10BT:
                iflinkspeed = NIC_LINK_SPEED_10MBPS;
                ifduplexMode = NIC_HALF_DUPLEX_MODE;
                //100BASE-TX
                TRACE_INFO("Duplex mode = Half-Duplex\r\n");
                //10BASE-T
                TRACE_INFO("Link speed = 10 Mbps\r\n");
                break;
            //10BASE-T full-duplex
            case PSCSR_HCDSPEED_10BT_FD:
                iflinkspeed = NIC_LINK_SPEED_10MBPS;
                ifduplexMode = NIC_FULL_DUPLEX_MODE;
                //1000BASE-T
                TRACE_INFO("Duplex mode = Full-Duplex\r\n");
                //10BASE-T
                TRACE_INFO("Link speed = 10 Mbps\r\n");
                break;
            //100BASE-TX
            case PSCSR_HCDSPEED_100BTX:
                iflinkspeed = NIC_LINK_SPEED_100MBPS;
                ifduplexMode = NIC_HALF_DUPLEX_MODE;
                //100BASE-TX
                TRACE_INFO("Duplex mode = Half-Duplex\r\n");
                //100BASE-TX
                TRACE_INFO("Link speed = 100 Mbps\r\n");
                break;
            //100BASE-TX full-duplex
            case PSCSR_HCDSPEED_100BTX_FD:
                iflinkspeed = NIC_LINK_SPEED_100MBPS;
                ifduplexMode = NIC_FULL_DUPLEX_MODE;
                //1000BASE-T
                TRACE_INFO("Duplex mode = Full-Duplex\r\n");
                //100BASE-TX
                TRACE_INFO("Link speed = 100 Mbps\r\n");
                break;
            //Unknown operation mode
            default:
                //10BASE-T
                TRACE_INFO("Link speed = %" PRIu32 " bps\r\n", iflinkspeed);
                //Debug message
                TRACE_INFO("Invalid Duplex mode\r\n");
                break;
            }

            //Update link state
            iflinkState = TRUE;

            //Adjust MAC configuration parameters for proper operation
            stm32h7xxEthUpdateMacConfig();
        }
        else
        {
#ifdef ALT_INICHE
            ifp->n_mib->ifAdminStatus = NI_DOWN; /* status = down */
            ifp->n_mib->ifOperStatus = NI_DOWN;  /* status = down */
            ifp->n_mib->ifLastChange = cticks * (100 / TPS);
#endif
            //Update link state
            iflinkState = FALSE;
            //Display link state
            TRACE_INFO("Link is down ...\r\n");
        }
    }
}

/**
 * @brief STM32H743/753 Ethernet MAC initialization
 * @param[in] interface Underlying network interface
 **/

void ENET_Configuration(void)
{
    //Debug message
    TRACE_INFO("Initializing STM32H7xx Ethernet MAC...\r\n");

    //GPIO configuration
    stm32h7xxEthInitGpio();

    //Enable Ethernet MAC clock
    __HAL_RCC_ETH1MAC_CLK_ENABLE();
    __HAL_RCC_ETH1TX_CLK_ENABLE();
    __HAL_RCC_ETH1RX_CLK_ENABLE();

    //Reset Ethernet MAC peripheral
    __HAL_RCC_ETH1MAC_FORCE_RESET();
    __HAL_RCC_ETH1MAC_RELEASE_RESET();

    //Perform a software reset
    ETH->DMAMR |= ETH_DMAMR_SWR;
    //Wait for the reset to complete
    while (ETH->DMAMR & ETH_DMAMR_SWR)
        ;

    //Adjust MDC clock range depending on HCLK frequency
    ETH->MACMDIOAR = ETH_MACMDIOAR_CR_DIV124;

    //PHY transceiver initialization
    //Debug message
    TRACE_INFO("Initializing LAN8742...\r\n");

    //Reset PHY transceiver (soft reset)
    stm32h7xxEthWritePhyReg(LAN8742_PHY_ADDR, LAN8742_PHY_REG_BMCR, BMCR_RESET);
    //Wait for the reset to complete
    while (stm32h7xxEthReadPhyReg(LAN8742_PHY_ADDR, LAN8742_PHY_REG_BMCR) & BMCR_RESET)
        ;

    //Restore default auto-negotiation advertisement parameters
    stm32h7xxEthWritePhyReg(LAN8742_PHY_ADDR, LAN8742_PHY_REG_ANAR, ANAR_100BTX_FD | ANAR_100BTX | ANAR_10BT_FD | ANAR_10BT | ANAR_SELECTOR0);

    //Enable auto-negotiation
    stm32h7xxEthWritePhyReg(LAN8742_PHY_ADDR, LAN8742_PHY_REG_BMCR, BMCR_AN_EN);

    //The PHY will generate interrupts when link status changes are detected
    stm32h7xxEthWritePhyReg(LAN8742_PHY_ADDR, LAN8742_PHY_REG_IMR, IMR_AN_COMPLETE | IMR_LINK_DOWN);

    //Force the TCP/IP stack to poll the link state at startup
    ethpEvent = TRUE;
    //Notify the TCP/IP stack of the event
    osSetEvent(&netEvent);

    //Use default MAC configuration
    ETH->MACCR = ETH_MACCR_DO;

    //Set the MAC address of the station
    ETH->MACA0LR = Addrmac.w[0] | (Addrmac.w[1] << 16);
    ETH->MACA0HR = Addrmac.w[2];

    //The MAC supports 3 additional addresses for unicast perfect filtering
    ETH->MACA1LR = 0;
    ETH->MACA1HR = 0;
    ETH->MACA2LR = 0;
    ETH->MACA2HR = 0;
    ETH->MACA3LR = 0;
    ETH->MACA3HR = 0;

    //Initialize hash table
    ETH->MACHT0R = 0;
    ETH->MACHT1R = 0;

    //Configure the receive filter
    ETH->MACPFR = ETH_MACPFR_HPF | ETH_MACPFR_HMC;

    //Disable flow control
    ETH->MACTFCR = 0;
    ETH->MACRFCR = 0;

    //Configure DMA operating mode
    ETH->DMAMR = ETH_DMAMR_INTM_0 | ETH_DMAMR_PR_1_1;
    //Configure system bus mode
    ETH->DMASBMR |= ETH_DMASBMR_AAL;
    //The DMA takes the descriptor table as contiguous
    ETH->DMACCR = ETH_DMACCR_DSL_0BIT;

    //Configure TX features
    ETH->DMACTCR = ETH_DMACTCR_TPBL_1PBL;

    //Configure RX features
    ETH->DMACRCR = ETH_DMACRCR_RPBL_1PBL;
    ETH->DMACRCR |= (STM32H7XX_ETH_RX_BUFFER_SIZE << 1) & ETH_DMACRCR_RBSZ;

    //Enable store and forward mode
    ETH->MTLTQOMR |= ETH_MTLTQOMR_TSF;
    ETH->MTLRQOMR |= ETH_MTLRQOMR_RSF;

    //Initialize DMA descriptor lists
    stm32h7xxEthInitDmaDesc();

    //Prevent interrupts from being generated when the transmit statistic
    //counters reach half their maximum value
    ETH->MMCTIMR = ETH_MMCTIMR_TXLPITRCIM | ETH_MMCTIMR_TXLPIUSCIM | ETH_MMCTIMR_TXGPKTIM | ETH_MMCTIMR_TXMCOLGPIM | ETH_MMCTIMR_TXSCOLGPIM;

    //Prevent interrupts from being generated when the receive statistic
    //counters reach half their maximum value
    ETH->MMCRIMR = ETH_MMCRIMR_RXLPITRCIM | ETH_MMCRIMR_RXLPIUSCIM | ETH_MMCRIMR_RXUCGPIM | ETH_MMCRIMR_RXALGNERPIM | ETH_MMCRIMR_RXCRCERPIM;

    //Disable MAC interrupts
    ETH->MACIER = 0;
    //Enable the desired DMA interrupts
    ETH->DMACIER = ETH_DMACIER_NIE | ETH_DMACIER_RIE | ETH_DMACIER_TIE;

    //Set priority grouping (4 bits for pre-emption priority, no bits for subpriority)
    NVIC_SetPriorityGrouping(STM32H7XX_ETH_IRQ_PRIORITY_GROUPING);

    //Configure Ethernet interrupt priority
    NVIC_SetPriority(ETH_IRQn, NVIC_EncodePriority(STM32H7XX_ETH_IRQ_PRIORITY_GROUPING, STM32H7XX_ETH_IRQ_GROUP_PRIORITY, STM32H7XX_ETH_IRQ_SUB_PRIORITY));

    //Enable MAC transmission and reception
    ETH->MACCR |= ETH_MACCR_TE | ETH_MACCR_RE;

    //Enable DMA transmission and reception
    ETH->DMACTCR |= ETH_DMACTCR_ST;
    ETH->DMACRCR |= ETH_DMACRCR_SR;

    //Accept any packets from the upper layer
    osSetEvent(&TxEvent);
}

/**
 * @brief STM32H743/753 Ethernet MAC timer handler
 *
 * This routine is periodically called by the TCP/IP stack to
 * handle periodic operations such as polling the link state
 *
 **/

static void stm32h7xxEthTick(void)
{
    //Handle periodic operations
    uint16_t value;
    int linkState;

    //Read basic status register
    value = stm32h7xxEthReadPhyReg(LAN8742_PHY_ADDR, LAN8742_PHY_REG_BMSR);
    //Retrieve current link state
    linkState = (value & BMSR_LINK_STATUS) ? TRUE : FALSE;

    //Link up event?
    if (linkState && !iflinkState)
    {
        //Set event flag
        ethpEvent = TRUE;
        //Notify the TCP/IP stack of the event
        osSetEvent(&netEvent);
    }
    //Link down event?
    else if (!linkState && iflinkState)
    {
        //Set event flag
        ethpEvent = TRUE;
        //Notify the TCP/IP stack of the event
        osSetEvent(&netEvent);
    }
}

/**
 * @brief STM32H743/753 Ethernet MAC event handler
 **/

static void stm32h7xxEthEventHandler(void)
{
    error_t error;
    unsigned int RxLen;

    //Packet received?
    if (ETH->DMACSR & ETH_DMACSR_RI)
    {
        //Clear interrupt flag
        ETH->DMACSR = ETH_DMACSR_RI;

        //Process all pending packets
        do
        {
            //The current buffer is available for reading?
            if (!(rxDmaDesc[rxIndex].rdes3 & ETH_RDES3_OWN))
            {
                //FD and LD flags should be set
                if ((rxDmaDesc[rxIndex].rdes3 & ETH_RDES3_FD) && (rxDmaDesc[rxIndex].rdes3 & ETH_RDES3_LD))
                {
                    //Make sure no error occurred
                    if (!(rxDmaDesc[rxIndex].rdes3 & ETH_RDES3_ES))
                    {
                        //Retrieve the length of the frame
                        RxLen = rxDmaDesc[rxIndex].rdes3 & ETH_RDES3_PL;
                        //Limit the number of data to read
                        RxLen = MIN(RxLen, STM32H7XX_ETH_RX_BUFFER_SIZE);
#ifdef ALT_INICHE
                        BSP_LED_Toggle(USER_LD2);
                        //Pass the packet to the upper layer
                        input_ippkt((uint8_t *)rxBuffer[rxIndex], RxLen);
#endif
                        //Valid packet received
                        error = NO_ERROR;
                    }
                    else
                    {
                        //The received packet contains an error
                        error = ERROR_INVALID_PACKET;
                    }
                }
                else
                {
                    //The packet is not valid
                    error = ERROR_INVALID_PACKET;
                }

                //Set the start address of the buffer
                rxDmaDesc[rxIndex].rdes0 = (uint32_t)rxBuffer[rxIndex];
                //Give the ownership of the descriptor back to the DMA
                rxDmaDesc[rxIndex].rdes3 = ETH_RDES3_OWN | ETH_RDES3_IOC | ETH_RDES3_BUF1V;

                //Increment index and wrap around if necessary
                if (++rxIndex >= STM32H7XX_ETH_RX_BUFFER_COUNT)
                    rxIndex = 0;
            }
            else
            {
                //No more data in the receive buffer
                error = ERROR_BUFFER_EMPTY;
            }

            //Clear RBU flag to resume processing
            ETH->DMACSR = ETH_DMACSR_RBU;
            //Instruct the DMA to poll the receive descriptor list
            ETH->DMACRDTPR = 0;

            //No more data in the receive buffer?
        } while (error != ERROR_BUFFER_EMPTY);
    }

    //Re-enable DMA interrupts
    ETH->DMACIER = ETH_DMACIER_NIE | ETH_DMACIER_RIE | ETH_DMACIER_TIE;
}

/**
 * @brief Send a packet to the network controller
 * @param[in] interface Underlying network interface
 * @param[in] buffer Multi-part buffer containing the data to send
 * @param[in] offset Offset to the first data byte
 * @return Error code
 **/
error_t nicSendPacket(uint8_t *buffer, uint32_t length)
{
    int status;

    //Wait for the transmitter to be ready to send
    status = osWaitForEvent(&TxEvent, NIC_MAX_BLOCKING_TIME);

    //Check whether the specified event is in signaled state
    if (status)
    {
        //Disable interrupts
        stm32h7xxEthDisableIrq();

        //Check the frame length
        if (length > STM32H7XX_ETH_TX_BUFFER_SIZE)
        {
            //The transmitter can accept another packet
            osSetEvent(&TxEvent);
            //Report an error
            return ERROR_INVALID_LENGTH;
        }

        //Make sure the current buffer is available for writing
        if (txDmaDesc[txIndex].tdes3 & ETH_TDES3_OWN)
            return ERROR_FAILURE;

        //Copy user data to the transmit buffer
        memcpy(txBuffer[txIndex], buffer, (length + 3) & ~3UL);

        //Set the start address of the buffer
        txDmaDesc[txIndex].tdes0 = (uint32_t)txBuffer[txIndex];
        //Write the number of bytes to send
        txDmaDesc[txIndex].tdes2 = ETH_TDES2_IOC | (length & ETH_TDES2_B1L);
        //Give the ownership of the descriptor to the DMA
        txDmaDesc[txIndex].tdes3 = ETH_TDES3_OWN | ETH_TDES3_FD | ETH_TDES3_LD;

        //Data synchronization barrier
        __DSB();

        //Clear TBU flag to resume processing
        ETH->DMACSR = ETH_DMACSR_TBU;
        //Instruct the DMA to poll the transmit descriptor list
        ETH->DMACTDTPR = 0;

        //Increment index and wrap around if necessary
        if (++txIndex >= STM32H7XX_ETH_TX_BUFFER_COUNT)
            txIndex = 0;

        //Check whether the next buffer is available for writing
        if (!(txDmaDesc[txIndex].tdes3 & ETH_TDES3_OWN))
        {
            //The transmitter can accept another packet
            osSetEvent(&TxEvent);
        }
        //Re-enable interrupts if necessary
        stm32h7xxEthEnableIrq();
#ifdef ALT_INICHE
        BSP_LED_Toggle(USER_LD1);
#endif
        //Data successfully written
        return NO_ERROR;
    }
    else
    {
        //The transmitter is busy
        return ERROR_TRANSMITTER_BUSY;
    }
}

/**
 * @brief Enable interrupts
 **/

void stm32h7xxEthEnableIrq(void)
{
    //Enable Ethernet MAC interrupts
    NVIC_EnableIRQ(ETH_IRQn);
}

/**
 * @brief Disable interrupts
 **/

void stm32h7xxEthDisableIrq(void)
{
    //Disable Ethernet MAC interrupts
    NVIC_DisableIRQ(ETH_IRQn);
}

/**
 * @brief STM32H743/753 Ethernet MAC interrupt service routine
 **/

void ETH_IRQHandler(void)
{
    int flag;
    uint32_t status;

    //Enter interrupt service routine
    osEnterIsr();

    //This flag will be set if a higher priority task must be woken
    flag = FALSE;

    //Read DMA status register
    status = ETH->DMACSR;

    //A packet has been transmitted?
    if (status & ETH_DMACSR_TI)
    {
        //Clear TI interrupt flag
        ETH->DMACSR = ETH_DMACSR_TI;

        //Check whether the TX buffer is available for writing
        if (!(txDmaDesc[txIndex].tdes3 & ETH_TDES3_OWN))
        {
            //Notify the TCP/IP stack that the transmitter is ready to send
            flag |= osSetEventFromIsr(&TxEvent);
        }
    }

    //A packet has been received?
    if (status & ETH_DMACSR_RI)
    {
#ifdef ALT_INICHE
        rxInts++;
#endif
        //Disable RIE interrupt
        ETH->DMACIER &= ~ETH_DMACIER_RIE;

        //Set event flag
        netpEvent = TRUE;
        //Notify the TCP/IP stack of the event
        flag |= osSetEventFromIsr(&netEvent);
    }

    //Clear NIS interrupt flag
    ETH->DMACSR = ETH_DMACSR_NIS;

    //Leave interrupt service routine
    osExitIsr(flag);
}

/**
 * @brief TCP/IP events handling
 **/
void EthifTask(void *p_arg)
{
    (void)p_arg;
    int status;
    uint32_t time;
    uint32_t timeout;

    //Get current time
    netTimestamp = osGetSystemTime();

    //Receive notifications when the transmitter is ready to send
    if (!osCreateEvent(&TxEvent))
    {
        //Failed to create event object
        TRACE_INFO("ERROR_OUT_OF_RESOURCES\r\n");
    }
    //Create a event object to receive notifications from device drivers
    if (!osCreateEvent(&netEvent))
    {
        //Failed to create mutex
        TRACE_INFO("ERROR_OUT_OF_RESOURCES\r\n");
    }

    //Interrupts can be safely enabled
    stm32h7xxEthEnableIrq();

    //Main loop
    while (1)
    {
        //Get current time
        time = osGetSystemTime();

        //Compute the maximum blocking time when waiting for an event
        if (timeCompare(time, netTimestamp) < 0)
            timeout = netTimestamp - time;
        else
            timeout = 0;

        //Receive notifications when a frame has been received, or the
        //link state of any network interfaces has changed
        status = osWaitForEvent(&netEvent, timeout);

        //Check whether the specified event is in signaled state
        if (status)
        {
            //Check whether a NIC event is pending
            if (netpEvent)
            {
                //Acknowledge the event by clearing the flag
                netpEvent = FALSE;

                //Valid NIC driver?
                //Disable hardware interrupts
                stm32h7xxEthDisableIrq();
                //Handle NIC events
                stm32h7xxEthEventHandler();
                //Re-enable hardware interrupts
                stm32h7xxEthEnableIrq();
            }

            //Check whether a PHY event is pending
            if (ethpEvent)
            {
                //Acknowledge the event by clearing the flag
                ethpEvent = FALSE;

                //Valid NIC driver?
                //Disable hardware interrupts
                stm32h7xxEthDisableIrq();
                //Handle PHY events
                lan8742EventHandler();
                //Re-enable hardware interrupts
                stm32h7xxEthEnableIrq();
            }
        }

        //Get current time
        time = osGetSystemTime();

        //Check current time
        if (timeCompare(time, netTimestamp) >= 0)
        {
            //Disable interrupts
            stm32h7xxEthDisableIrq();
            //Handle periodic operations
            stm32h7xxEthTick();
            //Re-enable interrupts if necessary
            stm32h7xxEthEnableIrq();

            //Next event
            netTimestamp = time + NET_TICK_INTERVAL;
        }
    }
}