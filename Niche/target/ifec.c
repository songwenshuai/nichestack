/* Includes ------------------------------------------------------------------*/
#include "ethernetif.h"
#include "ifec.h"

#include "ipport.h"
#include "tcpport.h"
#include "netbuf.h"
#include "net.h"
#include "q.h"
#include "ether.h"

#ifdef IP_MULTICAST
#include "igmp_cmn.h"
extern int mcastlist(struct in_multi *);
#endif

/***********************statistic values*******************/
uint32_t totInts        = 0;
uint32_t rxInts         = 0;
uint32_t txInts         = 0;
uint32_t txNoInts       = 0;
uint32_t dropped_pkts   = 0;
uint32_t bad_plen       = 0;
uint32_t ifacess        = 0;


extern void ENET_Configuration(void);
extern uint32_t ETH_SendTxPkt(uint8_t *addr, uint16_t FrameLength);
extern char mac_addr[6];
extern struct net netstatic[STATIC_NETS];

int ENET_Init(int iface)
{
  NET ifp;

  ifp = &netstatic[iface];
  ifp->n_mib->ifAdminStatus = NI_UP; /* status = UP */
  ifp->n_mib->ifOperStatus = NI_UP;
  ifp->n_mib->ifLastChange = cticks * (100/TPS);

  ENET_Configuration();

  return (0);
}

/*******************************************************************************
* Function Name  : ENET_TxPkt
* Description    : Transmit a packet
* Input          : ppkt: pointer to application packet Buffer
*                : size: Tx Packet size
* Output         : None
* Return         : None
*******************************************************************************/
int ENET_TxPkt(PACKET pkt)
{
  unsigned int rv, slen, addr;
  NET ifp;

  ifp = &netstatic[ifacess];

  addr = (uint32_t)pkt->nb_prot + ETHHDR_BIAS;
  slen = pkt->nb_plen - ETHHDR_BIAS;

  totInts++; //Inc nb of frame
  ENTER_CRIT_SECTION(ptk);

  rv = ETH_SendTxPkt((uint8_t *)addr, slen);
  if (rv == ETH_SUCCESS)
  {
    txInts++; //Inc nb of sent frame

    /* update packet statistics */
    ifp->n_mib->ifOutOctets += (u_long)pkt->nb_plen;
    if (*pkt->nb_prot & 0x01)/* 疑问 0x80 */
      ifp->n_mib->ifOutNUcastPkts++;
    else
      ifp->n_mib->ifOutUcastPkts++;
  }
  else
  {
    txNoInts++;
  }
  EXIT_CRIT_SECTION(ptk);

  if (pkt)
    pk_free(pkt);

  return (0); /* alloc done interrupt will start xmit */
}

/*
 * ENET_Close
 *
 *   --> iface               Number of interfaces before init.
 *
 *   <--                        Number of interfaces after init.
 *
 *   This function initializes the InterNiche devices.  The number of interfaces
 * before initialization is specified by iface.  This function returns the
 * total number of interfaces after initialization.
 */
int ENET_Close(int iface)
{
  NET ifp;

  ifp = &netstatic[iface];
  printf("ENET_Close\n");

  ifp->n_mib->ifAdminStatus = NI_DOWN; /* status = down */
  ifp->n_mib->ifOperStatus = NI_DOWN;  /* status = down */
  ifp->n_mib->ifLastChange = cticks * (100 / TPS);
  return 0;
}

/* FUNCTION: ENET_stats()
 *
 * Print Ethernet controller statistics
 *
 * PARAM1: void *             PIO handle
 * PARAM2: int                network interface index
 *
 * RETURN: none
 */
void ENET_Stats(void *pio, int iface)
{
  ns_printf(pio, "dm9ka_stats\n");
  ns_printf(pio, "Interrupts: Rx: %lu, Tx: %lu, Total: %lu\n", rxInts, txInts, totInts);
  ns_printf(pio, "Dropped pkts: %d, Bad length: %d, Not Send: %d\n", dropped_pkts, bad_plen, txNoInts);
}

/*
 * input_ippkt
 *
 *   --> iface               Number of interfaces before init.
 *
 *   <--                        Number of interfaces after init.
 *
 *   This function initializes the InterNiche devices.  The number of interfaces
 * before initialization is specified by iface.  This function returns the
 * total number of interfaces after initialization.
 */
int input_ippkt(uint8_t *addr, int RxLen)
{
  NET ifp;
  PACKET frame;
  struct ethhdr *et;
  uint8_t *des_sp, *pck_dp;

  /* frame = pointer to the packet */
  /* ifacess = network interface index */
  ifp = &netstatic[ifacess];
  frame = pk_alloc(RxLen - ETH_HDR_LEN + ETHHDR_SIZE); /* Ethernet MAC header is 14 bytes */

  /* if 'alloc_mem()' has failed, ignore this packet. */
  if (frame == NULL)
  {
    ifp->n_mib->ifInDiscards++;
    return (0);
  }

  //Inc nb of received frame
  des_sp = (uint8_t *)addr;
  pck_dp = (uint8_t *)(uint32_t)((frame->nb_buff) + ETHHDR_BIAS);
  memcpy(pck_dp, des_sp, RxLen);

  // update packet header
  /* pointer to data to send. */
  frame->nb_prot = frame->nb_buff + ETHHDR_SIZE; /* point to start of IP header */
  frame->nb_tstamp = cticks;
  et = (struct ethhdr *)(frame->nb_buff + ETHHDR_BIAS);
  frame->type = et->e_type;

  /* nets[ ]structure for posting statistics */
  frame->net = ifp;

  /* length of data to send */
  frame->nb_plen = RxLen - ETH_HDR_LEN - ETH_CRC_LEN; /* length of MAC layer header */
  frame->net->n_mib->ifInOctets += frame->nb_plen;

  /* rcvdq is protected by Interupt disabling when it's being accessed
   * by the network task, so it's safe for us to call putq().
   */
  /* -AK- later this should have been done as a part of the ISR. Chk. */
  LOCK_NET_RESOURCE(RXQ_RESID);
  putq(&rcvdq, frame); /* give received pkt to stack */
  UNLOCK_NET_RESOURCE(RXQ_RESID);
  SignalPktDemux(); /* wake Interniche net task */

  return (0);
}

/*
 * eth_devices_init
 *
 *   --> iface               Number of interfaces before init.
 *
 *   <--                        Number of interfaces after init.
 *
 *   This function initializes the InterNiche devices.  The number of interfaces
 * before initialization is specified by iface.  This function returns the
 * total number of interfaces after initialization.
 */
int eth_devices_init(int iface)
{
  int use_dhcp;
  NET p_net;
  ip_addr ipaddr, netmask, gw;

  get_mac_addr((unsigned char *)mac_addr);

  get_ip_addr(&ipaddr, &netmask, &gw, &use_dhcp);

  p_net = &netstatic[iface];
  /*
  * ifAdminStatus is 1, 表示管理员设置为 UP，启用端口
  * ifOperStatus  is 2 ，物理连接为 DOWN，查原因，是不是没接线，或没开机。
  */
  p_net->n_mib->ifSpeed = 100000000;
  p_net->n_mib->ifAdminStatus = NI_DOWN; /* status = down */
  p_net->n_mib->ifOperStatus = NI_DOWN;  /* will be set up in init()  */
  p_net->n_mib->ifLastChange = cticks * (100 / TPS);
  p_net->n_mib->ifPhysAddress = (u_char *)mac_addr;
  p_net->n_mib->ifDescr = (char *)"ENETH series ethernet";
  p_net->n_lnh = ETHHDR_SIZE;      /* ethernet header size */
  p_net->n_hal = 6;                /* hardware address length */
  p_net->n_mib->ifType = ETHERNET; /* device type */
  p_net->n_mtu = MTU;              /* max frame size */

  /* install our hardware driver routines */
  p_net->n_init = ENET_Init;
  p_net->pkt_send = ENET_TxPkt;
  p_net->raw_send = NULL;
  p_net->n_close = ENET_Close;
  p_net->n_stats = ENET_Stats;

#ifdef IP_V6
  p_net->n_flags |= (NF_NBPROT | NF_BCAST | NF_MCAST | NF_IEEE48 | NF_IPV6);
#else
  p_net->n_flags |= (NF_NBPROT | NF_BCAST | NF_MCAST | NF_IEEE48 );
#endif

#ifdef DHCP_CLIENT
  /* 
   * OR in the DHCP flag, if enabled. This will allow any
   * application-specific flag setting in get_ip_addr(), such 
   * as enabling AUTOIP, to occur 
   */
  if (use_dhcp)
  {
    p_net->n_flags |= NF_DHCPC;
  }
#endif
  p_net->n_ipaddr = ipaddr;
  p_net->snmask = netmask;
  p_net->n_defgw = gw;
#ifdef IP_MULTICAST
  p_net->n_mcastlist = mcastlist;
#if defined(IGMP_V1) || defined(IGMP_V2)
  p_net->igmp_oper_mode = IGMP_MODE_DEFAULT;
#endif /* IGMPv1 or IGMPv2 */
#endif /* IP_MULTICAST */

  ifacess = iface;

  /* Initialize next device. */
  iface++;

  return (iface);
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
