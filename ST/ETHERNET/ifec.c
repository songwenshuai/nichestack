
/* Includes ------------------------------------------------------------------*/
#include  <stm32h7xx_hal.h>
#include "ifec.h"

#include "ipport.h"
#include "tcpport.h"
#include "netbuf.h"
#include "net.h"
#include "q.h"
#include "ether.h"
#include "nvparms.h"

#ifdef IP_MULTICAST
#include "igmp_cmn.h"
extern int mcastlist(struct in_multi *);
#endif

#if (OS_TRACE_EN > 0u)
/* SEEGER */
#include  <os_trace.h>
#endif

#include "stm32h7xx_eth_driver.h"
#include "error.h"

extern struct net netstatic[STATIC_NETS];

/***********************statistic values*******************/
uint32_t rxInts         = 0; /* 接收中断计数 */

uint32_t rxPnts         = 0; /* 接收包计数   */
uint32_t rxNoPnts       = 0; /* 接收包失败计数 */

uint32_t txInts         = 0; /* 发送计数     */

uint32_t txPnts         = 0; /* 发送包计数     */
uint32_t txNoPnts       = 0; /* 发送包失败计数 */

uint32_t ifacess        = 0;

MacAddr Addrmac; ///<Link-layer address

/* 
 * The IP, gateway, and subnet mask address below are used as a last resort 
 * if no network settings can be found, and DHCP (if enabled) fails. You can
 * edit these as a quick-and-dirty way of changing network settings if desired.
 * 
 * Default IP addresses are set to all zeros so that DHCP server packets will
 * penetrate secure routers. They are NOT intended to be valid static IPs, 
 * these values are only a valid default on networks with DHCP server. 
 */

/*
 * get_mac_addr
 *
 *   --> net                    Network interface for which to get MAC address.
 *   <-- Addrmac               MAC address.
 *
 *   This function returns in Addrmac a MAC address to be used with the network
 * interface specified by net.
 */
int get_mac_addr(MacAddr *addrmac)
{
  int error = -1;

  /* This is the Altera Vendor ID */
  addrmac->b[0] = MACADDR0;
  addrmac->b[1] = MACADDR1;
  addrmac->b[2] = MACADDR2;

  /* Reserverd Board identifier */
  addrmac->b[3] = MACADDR3;
  addrmac->b[4] = MACADDR4;
  addrmac->b[5] = MACADDR5;

  error = 0;

  return error;
}

/*
 * get_ip_addr
 *
 *   --> p_dev                  Device for which to get IP address.
 *   <-- p_addr                 IP address for device.
 *   <-- p_netmask              IP netmask for device.
 *   <-- p_gw_addr              IP gateway address for device.
 *   <-- p_use_dhcp             TRUE if DHCP should be used to obtain an IP
 *                              address.
 *
 *   This function provides IP address information for the InterNiche network
 * interface device specified by p_dev.  If a static IP address is to be used,
 * the IP address, netmask, and default gateway address are returned in p_addr,
 * p_netmask, and p_gw_addr, and FALSE is returned in p_use_dhcp.  If the DHCP
 * protocol is to be used to obtain an IP address, TRUE is returned in
 * p_use_dhcp.
 */
int get_ip_addr(ip_addr *ipaddr, ip_addr *netmask, ip_addr *gw, int *use_dhcp)
{
  IP4_ADDR(*ipaddr,  IPADDR0,  IPADDR1,  IPADDR2,  IPADDR3);
  IP4_ADDR(*gw,      GWADDR0,  GWADDR1,  GWADDR2,  GWADDR3);
  IP4_ADDR(*netmask, MSKADDR0, MSKADDR1, MSKADDR2, MSKADDR3);

#ifdef DHCP_CLIENT
  *use_dhcp = 1;
#else  /* not DHCP_CLIENT */
  *use_dhcp = 0;

  printf("Static IP Address is %d.%d.%d.%d\n",
         ip4_addr1(*ipaddr),
         ip4_addr2(*ipaddr),
         ip4_addr3(*ipaddr),
         ip4_addr4(*ipaddr));
#endif /* not DHCP_CLIENT */

  /* Non-standard API: return 1 for success */
  return 1;
}


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
* Function Name  : ENET_RAW
* Description    : Transmit a packet
* Input          : ppkt: pointer to application packet Buffer
*                : net  - NET structure associated with the TSE MAC instance
*                : data - pointer to the data payload
*                : data_bytes - number of bytes of the data payload to be sent to the MAC
* Output         : SUCCESS if success, else a negative value
* Return         : None
*******************************************************************************/
int ENET_RAW(NET net, char * data, unsigned int data_bytes)
{
  error_t rv;

#ifdef NPDEBUG
  /* Sanity check interface pointer */
  if(net->raw_send != ENET_RAW)
  {
    dprintf("ETHIF: bad net\n");
    dtrap();
  }
#endif

  /* Don't send if iface is logically down */
  if(net->n_mib->ifAdminStatus != NI_UP)
  {
    net->n_mib->ifOutDiscards++; /* bump mib counter for these */
    return ENP_LOGIC;    /* right thing to do? */
  }

  rv = nicSendPacket((uint8_t *)data + ETHHDR_BIAS, data_bytes - ETHHDR_BIAS);
  if(rv == NO_ERROR)
  {
#if (OS_TRACE_EN > 0u)
    SEGGER_SYSVIEW_PrintfHostEx("ETH Send Tx Pkt Succ",SEGGER_SYSVIEW_WARNING);
#endif
    txPnts++; //Inc nb of sent frame

    net->n_mib->ifOutOctets += data_bytes;
    /* we dont know whether it was unicast or not, we count both in <ifOutUcastPkts> */
    if (* data & 0x01)
      net->n_mib->ifOutNUcastPkts++;
    else
      net->n_mib->ifOutUcastPkts++;

    return SUCCESS;  /*success */
  }
  else   /* = 0, success */
  {
#if (OS_TRACE_EN > 0u)
    SEGGER_SYSVIEW_PrintfHostEx("ETH Send Tx Pkt Err",SEGGER_SYSVIEW_WARNING);
#endif
    txNoPnts++;
    dprintf("raw_send() MSGDMA not available, ret=%d, data_bytes=%d\n",rv, data_bytes);
    net->n_mib->ifOutDiscards++;

    return SEND_DROPPED;   /* ENP_RESOURCE and SEND_DROPPED have the same value! */
  }
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
  unsigned int slen, addr;
  NET ifp;
  error_t rv;
  ifp = &netstatic[ifacess];

#ifdef NPDEBUG
  /* Sanity check interface pointer */
  if(ifp->pkt_send != ENET_TxPkt)
  {
    dprintf("macloop: bad net\n");
    dtrap();
  }
#endif

  /* Don't send if iface is logically down */
  if(ifp->n_mib->ifAdminStatus != NI_UP)
  {
    ifp->n_mib->ifOutDiscards++; /* bump mib counter for these */
    return ENP_LOGIC;    /* right thing to do? */
  }

  addr = (uint32_t)pkt->nb_prot + ETHHDR_BIAS;
  slen = pkt->nb_plen - ETHHDR_BIAS;

  ENTER_CRIT_SECTION(ptk);

  rv = nicSendPacket((uint8_t *)addr, slen);
  if (rv == NO_ERROR)
  {
#if (OS_TRACE_EN > 0u)
    SEGGER_SYSVIEW_PrintfHostEx("ETH Send Tx Pkt Succ",SEGGER_SYSVIEW_WARNING);
#endif
    txPnts++; //Inc nb of sent frame

    /* update packet statistics */
    ifp->n_mib->ifOutOctets += (u_long)pkt->nb_plen;
    if (*(pkt->nb_prot + ETHHDR_BIAS) & 0x01)
      ifp->n_mib->ifOutNUcastPkts++;
    else
      ifp->n_mib->ifOutUcastPkts++;
  }
  else
  {
#if (OS_TRACE_EN > 0u)
    SEGGER_SYSVIEW_PrintfHostEx("ETH Send Tx Pkt Err",SEGGER_SYSVIEW_WARNING);
#endif
    txNoPnts++;
    dprintf("pkt_send() MSGDMA not available, ret=%d, pkt->nb_plen=%d\n",rv, pkt->nb_plen);
    ifp->n_mib->ifOutDiscards++;
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
  return SUCCESS;
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
  ns_printf(pio, "link stats\n");
  ns_printf(pio, "Interrupts: rxInts: %lu, rxPnts: %lu, txPnts: %lu\n", rxInts, rxPnts, txPnts);
  ns_printf(pio, "Not Send: %d\n", txNoPnts);
  ns_printf(pio, "Not Rcv : %d\n", rxNoPnts);
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

  LOCK_NET_RESOURCE(FREEQ_RESID);
  frame = pk_alloc(RxLen - ETH_HDR_LEN + ETHHDR_SIZE); /* Ethernet MAC header is 14 bytes */
  UNLOCK_NET_RESOURCE(FREEQ_RESID);

  /* if 'alloc_mem()' has failed, ignore this packet. */
  if (frame == NULL)
  {
    ifp->n_mib->ifInDiscards++;
    rxNoPnts ++;
    return ENP_RESOURCE;
  }

  //Inc nb of received frame
  des_sp = (uint8_t *)addr;
  pck_dp = (uint8_t *)(uint32_t)((frame->nb_buff) + ETHHDR_BIAS);
  memcpy(pck_dp, des_sp, (RxLen + 3) & ~3UL);

  // update packet header
  /* pointer to data to send. */
  frame->nb_prot = frame->nb_buff + ETHHDR_SIZE; /* point to start of IP header */
  frame->nb_tstamp = cticks;
  et = (struct ethhdr *)(frame->nb_buff + ETHHDR_BIAS);
  frame->type = et->e_type;

  /* nets[ ]structure for posting statistics */
  frame->net = ifp;

  /* length of data to send */
  frame->nb_plen = RxLen - ETH_HDR_LEN; /* length of MAC layer header */
  frame->net->n_mib->ifInOctets += frame->nb_plen;

#if (OS_TRACE_EN > 0u)
  SEGGER_SYSVIEW_PrintfHostEx("ETH Rcv Rx Pkt",SEGGER_SYSVIEW_LOG);
#endif
  
  rxPnts++;
  
  /* rcvdq is protected by Interupt disabling when it's being accessed
   * by the network task, so it's safe for us to call putq().
   */
  /* -AK- later this should have been done as a part of the ISR. Chk. */
  LOCK_NET_RESOURCE(RXQ_RESID);
  putq(&rcvdq, (q_elt)frame); /* give received pkt to stack */
  UNLOCK_NET_RESOURCE(RXQ_RESID);

  SignalPktDemux(); /* wake Interniche net task */

  return (0);
}

//-----------------------------------------------------------------------------------------------------------
//--------------------------------------------- DEV PORT ----------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

/*
 * iniche_devices_init
 *
 *   --> if_count               Number of interfaces before init.
 *
 *   <--                        Number of interfaces after init.
 *
 *   This function initializes the InterNiche devices.  The number of interfaces
 * before initialization is specified by if_count.  This function returns the
 * total number of interfaces after initialization.
 */
int iniche_devices_init(int if_count)
{
  int use_dhcp;
  NET p_net;
  ip_addr ipaddr, netmask, gw;

  p_net = &netstatic[if_count];

  get_mac_addr(&Addrmac);

  get_ip_addr(&ipaddr, &netmask, &gw, &use_dhcp);

  p_net->n_ipaddr = ipaddr;
  p_net->n_defgw = gw;
  p_net->snmask = netmask;
  p_net->n_lnh = ETHHDR_SIZE;      /* ethernet header size */

#ifdef IEEE_802_3
  p_net->n_lnh += sizeof(struct snap_hdr);   /* The bloated version */
#endif

  p_net->n_hal = 6;                /* hardware address length */
  p_net->n_mib->ifType = ETHERNET; /* device type */

#ifdef IEEE_802_3
  p_net->n_mtu = MTU; /* max frame size */
#else
  p_net->n_mtu = MTU; /* max frame size */
#endif

  if(p_net->name[0] == 0)
  {
    p_net->name[0] = 'i';
    p_net->name[1] = 'f';
    p_net->name[2] = (char)('0' + if_count);
  }

#ifdef IP_V6
  p_net->n_flags |= (NF_NBPROT | NF_IPV6);
#else
  p_net->n_flags |= (NF_NBPROT);
#endif

  /* search the NV parameters for if_count setup for our name. If this
    * fails we just default to what's already in the ifp.
    */
#ifdef INCLUDE_NVPARMS
  if_configbyname(p_net);
#endif

#ifdef DYNAMIC_IFACES   /* dynamic extensions */
  p_net->n_setstate = lb_setstate;
#endif  /* DYNAMIC_IFACES */

  /*
  * ifAdminStatus is 1, 表示管理员设置为 UP，启用端口
  * ifOperStatus  is 2 ，物理连接为 DOWN，未接线，或没开机。
  */
  p_net->n_mib->ifAdminStatus = NI_DOWN; /* status = down */
  p_net->n_mib->ifOperStatus = NI_DOWN;  /* will be set up in init()  */
  p_net->n_mib->ifLastChange = cticks * (100 / TPS);
  p_net->n_mib->ifPhysAddress = (u_char *)Addrmac.b;
  p_net->n_mib->ifDescr = (char *)"ENETH series ethernet";
  p_net->n_mib->ifIndex = if_netnumber(p_net);
  p_net->n_mib->ifSpeed = 100000000;
  p_net->n_mib->ifMtu   = 1500;         /* ethernetish default */

  /* install our hardware driver routines */
  p_net->n_init = ENET_Init;
#ifdef IP_RAW
  p_net->raw_send = ENET_RAW;
#else /* use packet send */
  p_net->pkt_send = ENET_TxPkt;
#endif   /* IP_RAW */
  p_net->n_close = ENET_Close;
  p_net->n_stats = ENET_Stats;

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

#ifdef IP_MULTICAST
  p_net->n_mcastlist = mcastlist;
#if defined(IGMP_V1) || defined(IGMP_V2)
  p_net->igmp_oper_mode = IGMP_MODE_DEFAULT;
#endif /* IGMPv1 or IGMPv2 */
#endif /* IP_MULTICAST */

  ifacess = if_count;

  /* Initialize next device. */
  if_count++;

  return (if_count);
}


/*
 * The write() system call is used to write a block of data to a file or device.
 * This implementation vectors requests to either the HAL alt_write() function 
 * (for files and device drivers) or the InterNiche send() function for sockets.
 */

int write (int fd, const void *ptr, size_t len)
{
  return send (fd, (void*) ptr, len, 0);
}

/*
 * The read() system call is used to read a block of data from a file or device.
 * This implementation vectors requests to either the HAL alt_read() function 
 * (for files and device drivers) or the InterNiche recvfrom() function for
 * sockets.
 */

int read (int fd, void *ptr, size_t len)
{
  return recvfrom(fd, ptr, len, 0, NULL, NULL);
}

/*
 * fcntl() is called by an application to release a file descriptor. This 
 * implementation duplicates the code of the HAL alt_fcntl() function 
 * (for files and device drivers) or calls the InterNiche bsd_ioctl for 
 * sockets.
 */

int fcntl (int file, int cmd, ...)
{
  long      flags;
  va_list  argp;

  va_start(argp, cmd);
  flags = va_arg(argp, long);
  va_end(argp);
  return bsd_ioctl(file, cmd, flags);
}

/*
 * close() is called by an application to release a file descriptor. This 
 * implementation vectors requests to either the HAL alt_close() function 
 * (for files and device drivers) or the InterNiche soclose() function for
 * sockets.
 */

int close (int fd)
{
  return t_socketclose ((long) fd);
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
