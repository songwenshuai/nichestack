/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IFEC_H
#define __IFEC_H

/* Includes ------------------------------------------------------------------*/
#include "ipport.h"
#include "tcpport.h"


/*  If DHCP will not be used, select valid static IP addresses here, for example:
 *           IP: 169.254.134.169
 *      Gateway: 192.168.155.254
 *  Subnet Mask: 255.255.255.0
 *          MAC: 00.07.ED.AA.BB.CC
 */

#define IPADDR0 169
#define IPADDR1 254
#define IPADDR2 134
#define IPADDR3 169

#define GWADDR0 192
#define GWADDR1 168
#define GWADDR2 155
#define GWADDR3 254

#define MSKADDR0 255
#define MSKADDR1 255
#define MSKADDR2 255
#define MSKADDR3 0

#define MACADDR0 0x00
#define MACADDR1 0x07
#define MACADDR2 0xED
#define MACADDR3 0xAA
#define MACADDR4 0xBB
#define MACADDR5 0xCC

#define IP4_ADDR(ipaddr, a, b, c, d) ipaddr =                                           \
        htonl((((unsigned long)(a & 0xff) << 24) | ((unsigned long)(b & 0xff) << 16) |  \
        ((unsigned long)(c & 0xff) << 8) | (unsigned long)(d & 0xff)))

/**
 * @brief MAC address
 **/

typedef __start_packed struct
{
   __start_packed union
   {
      uint8_t b[6];
      uint16_t w[3];
   };
} __end_packed MacAddr;

/*******************************************************************************
 *
 * InterNiche device services prototypes.
 *
 ******************************************************************************/

int eth_dev_init(int iface);
int close (int fd);
int fcntl (int file, int cmd, ...);
int read (int fd, void *ptr, size_t len);
int write (int fd, const void *ptr, size_t len);

#endif /* __ETHER_NETIF_H */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
