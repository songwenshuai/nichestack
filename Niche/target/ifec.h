/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IFEC_H
#define __IFEC_H

/* Includes ------------------------------------------------------------------*/
#include "ipport.h"
#include "tcpport.h"

/*******************************************************************************
 *
 * InterNiche device services prototypes.
 *
 ******************************************************************************/

int eth_devices_init(int iface);

/*
 * get_mac_addr
 *
 *   --> net                    Network interface for which to get MAC address.
 *   <-- mac_addr               MAC address.
 *
 *   This function returns in mac_addr a MAC address to be used with the network
 * interface specified by net.
 */

int get_mac_addr(unsigned char mac_addr[6]);


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

int get_ip_addr( ip_addr *p_addr, ip_addr *p_netmask, ip_addr *p_gw_addr, int *p_use_dhcp);


#endif /* __ETHER_NETIF_H */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
