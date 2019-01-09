Introduction
============

The 3.1 release of the InterNiche TCP/IP stack adds support for
IGMPv2 [RFC 2236].  The new release also decouples the use of
multicasting on a local LAN from the use of the IGMP protocol.

Usage of various multicast-related #defines in ipport.h_h
=========================================================

Applications that intend to restrict multicasting to the local 
network need only enable IP_MULTICAST in ipport.h_h.  This will 
allow the applications to register (for reception) one (or more) 
multicast address(es) of interest (on a particular link) with 
the corresponding device driver (for that link).  The same 
#define also provides the user with the capability to send out 
packets to a IPv4 multicast destination address.

If the application wants to be able to send and receive IPv4
multicast packets to and from off-link destinations (in addition
to hosts on the local network), it must enable one or both of 
the IGMP protocols.  Each link can only run one IGMP protocol 
(IGMPv1 or IGMPv2) at any given time.

If the user intends to run IGMPv1 on all links in the system, 
he must enable IP_MULTICAST and IGMP_V1 in ipport.h_h.

If the user intends to run IGMPv2 on all links in the system, 
he must enable IP_MULTICAST and IGMP_V2 in ipport.h_h.

If the user intends to run IGMPv1 on some links in the system
and IGMPv2 on other links in the system, he must enable 
IP_MULTICAST, IGMP_V1, and IGMP_V2 in ipport.h_h.

The user can configure the IGMP operating mode for a given
link by setting the 'igmp_oper_mode' field in the struct net
data structure.  This field is typically set at initialization
time based on the corresponding "IGMP mode:" configuration line
in the webport.nv configuration file.  If the IGMP mode is not 
specified in webport.nv, it defaults to IGMP_V1.

If the user would like to map an IPv4 multicast destination
address to its corresponding Ethernet multicast address
(01-00-5e-xx-xx-xx) [RFC 1112], he must enable ETHMCAST in the 
stack's configuration header file.  Otherwise, packets with an 
IPv4 multicast destination address are transmitted with the
Ethernet destination address set to the broadcast address 
(ff-ff-ff-ff-ff-ff).

Data Structure
==============

With v3.1, the struct net data structure in h\net.h  includes
IGMP-related fields.

struct net
{
   ...
#ifdef  IP_MULTICAST
   ...
#if defined (IGMP_V1) || defined (IGMP_V2)
   /* boolean variable that indicates our belief in whether the 
    * network currently "has" a  v1 router.  This variable is 
    * set to false (0) at startup for IGMPv2, and is always true
    * for IGMPv1.
    */
   u_char igmpv1_rtr_present; /* 1 (IGMP_TRUE) or 0 (IGMP_FALSE) */
   /* this variable keeps track of when (in ticks) the last IGMPv1
    * Host Membership Query message was received (the contents of
    * this variable are only valid when we believe that the network
    * has a v1 router).  This is only valid for a link that is
    * configured in IGMPv2 mode.
    */
   u_long igmpv1_query_rcvd_time;
   /* operating mode for IGMP on this link - IGMPv1 or IGMPv2 */
   u_char igmp_oper_mode;   
#endif /* IGMPv1 or IGMPv2 */
#endif   /* IP_MULTICAST */
};

CLI
===

The "igmp" CLI command provides information on various IGMP
v1- and IGMPv2-related statistics.

INET> igmp
nd0: mode: 2 [v2] v1 rtr: 0 [absent] v1 last query: 0 [now 52]
lo0: mode: 2 [v2] v1 rtr: 0 [absent] v1 last query: 0 [now 53]
[Rx ] IGMP messages rcvd: 0, timers running: 0
[Rx ] IGMPv1 Host Membership Queries rcvd (by v1-mode links): 0
[Rx ] IGMPv1 Host Membership Reports rcvd: 0
[Rx ] IGMP Host Membership Reports rcvd causing timer cancellation: 0
[Rx ] IGMPv1 Host Membership Queries rcvd (by v2-mode links): 0
[Rx ] IGMPv2 General Queries rcvd: 0, Group-Specific Queries rcvd: 0
[Rx ] IGMP Host Membership Reports rcvd causing timer cancellation: 0
[Rx ] IGMP Host Membership Reports rcvd with no local timer: 0
[Rx ] IGMPv2 Leave Group messages rcvd: 0
[Tx ] IGMPv2 Leave Group messages sent: 0, Membership Reports sent: 0
[Tx ] IGMPv1 Host Membership Reports sent: 0
[Err] IGMP packets rcvd with bad length: 0
[Err] IGMP packets rcvd with bad checksum: 0
[Err] Packet buffer allocation failures: 0, Bad IGMP Oper Mode config: 0
[Err] Bad IGMP Queries rcvd: 0, Bad IGMP Reports rcvd: 0
[Err] Bad IGMPv2 Group-Specific Queries rcvd: 0
[Err] IGMPv2 Group-Specific Queries rcvd with unknown Group Address: 0
[Err] IGMP Membership Reports rcvd with unknown Group Address: 0
[Err] Number of IGMPv2 messages rcvd without Router Alert option: 0
[Err] IGMP packets of unknown type rcvd by v1-mode links: 0
[Err] IGMP packets of unknown type rcvd by v2-mode links: 0

Limitations
===========

There are currently no mechanisms to dynamically switch a
link from one version of the IGMP protocol to another.  The
IGMP operating mode is set at initialization time, and is
expected to remain unchanged for the lifetime of the
device running the InterNiche TCP/IP stack.  However, it is 
possible to add code to switch operating modes at run-time.
