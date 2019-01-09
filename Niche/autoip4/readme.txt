

Readme.txt for autoip4

Release 1.91, August 2002


	This directory contains the source to implement the "auto IP" 
portion of Universal Plug & Play" (UPnP). This feature is the part of 
UPnP that allows an embedded Internet device to acquire an IP address 
without manual configuration or a DHCP server. As of this writing there 
is an official RFC underway for this, but it is not yet a ratified 
standard. The code in this directory is in compliance with the proposed 
standard.

Auto IP is the only part of the UPnP specification that directly 
affects the IP stack. This code base is integrated with InterNiche’s 
IPv4 "NicheStack" and is NOT designed for use with other stacks.



Files:

	The Auto IP code is implemented in the following files:

autoip.c  - Core Auto IP state machine
autoip.h  - Auto IP definitions
ds_app.h  - default database definitions and macros
makefile  - nmake compatible makefile
upnp.c    - The UPnP state machine - driver the Auto IP state machine
upnp.h    - UPnP definitions
upnpmenu.c - the menu routines & structures.



Auto IP description

	The theory behind Auto IP is that an IP host can obtain an IP 
address by picking a random address in a private address space, and 
send ARPs looking for the address. If the address is not found (i.e. 
there is no reply to the ARPs) then the host may used the address. 
Before attempting this "Auto IP" configuration, hosts are supposed 
to first try to obtain an address via DHCP, and only use Auto IP of 
DHCP fails. This source supports this as the default of several 
optional modes. Since Auto IP uses ARP, it only works of Ethernet & 
other devices that support ARP.



Structure

	This code maintains an array of "upnp" structures, one for 
each interface  in the nets[] array. The name of the UPnP array is 
upnp[], and it’s size is MAXNETS. A timer routine gets called about 
10 times a second, and scans this array for pending Auto IP work. 
Each of the upnp[] entries contains a state variable and several 
timers to control state changes. States are changes when we receive 
DHCP packets, receive ARP packets, or a timer expires.

	To enable this code in an InterNiche build, you must define 
both USE_AUTOIP and USE_UPNP in your ipport.h file. As explained 
below ARP and possible other modules need this defined for Auto IP 
to work. 

	The system startup code starts Auto IP by calling the routine 
Upnp_init(). This should be done after the nets structures are 
initialized and the IP stack is started, but before any applications 
start using the network. Once this call returns, no further i
ntervention by the application is needed. The timer is provided by an 
Interniche interval timer, so INICHE_TIMERS should be enabled in 
ipport.h



External hooks

	The Auto IP code uses the dhc_callback() feature of the DHCP 
client to monitor the state of DHCP configuration. If DHCP fails to 
provide an address in a set amount of time (default is one minute, 
see below for information on how to change this), then the Auto IP 
code selects an IP address from the free pool and commences sending 
ARPs. If  no ARP reply to that address is received, then it assumes 
the address is available an assigned it to the interface.  The ARP 
code needs to be compiled with USE_AUTOIP set in ipport.h for this 
to work.



Configurable options:

The Auto IP code can work in several modes. The possible modes are 
described by an ENUM value, which maybe one of the following:

eIP_METHOD_DHCP_AUTO_FIXED - Try DHCP, then AuoIP, then fixed address
eIP_METHOD_DHCP_FIXED  - Try DHCP, then fixed address
eIP_METHOD_AUTO_FIXED,   -Try AutoIP, then fixed address
eIP_METHOD_FIXED              - Used fixed (NV) address

	The default more is eIP_METHOD_DHCP_AUTO_FIXED. You can 
change this by setting the global variable "addressing_method".

	The pool of IP addresses from which Auto IP will select it’s 
address is defined by two variables. The Base address (the lowest) 
is given by dBASE_AUTO_IP_ADDRESS, and the highest is dMAX_AUTO_IP_ADDRESS. 
These are both global IP addresses, which may be changed by the 
application code before calling Upnp_init(). 

	The period of time to wait for DHCP to complete before 
switching to Auto IP is controlled by the global variable DHCP_WAIT_TIME. 
This is set in 1/10 second units. The default is 600 (60 seconds). 
This may be changed by the system before calling Upnp_init().



Database:

	The UPnP system maintains a small database of IP addressing 
information. This in implemented in an array of structures, defined 
on the file ds_app.h. There is an array item for each interface. The 
database contains (by default) copies of the "fixed" IP info (address, 
subnet mask, and gateway) and the current IP info. If valid fixed info 
id provided by setting it in the nets[] structures prior to calling 
Upnp_init() then this information will be used if Auto IP fails. 

	The simple default database may be replaced with something 
larger by setting the #define USER_DS_SUPPORT in ipport.h. The porting 
engineer will then need to provide the primitives defined in the 
ds_app.h file. Most uses should not need to do this.



UI and MENUS

	In the INICHE_MENUS define is set in ipport.h, then Auto IP 
will provide a set of console options for monitoring and fine-tuning 
the Auto IP and UPnP system. These menu items are:

upstats	      -       status of UPnP Interfaces
updisable   -       disable UPnP on an interface
uprestart    -       restart UPnP process on interface
upbase       -       Set new base for UPnP address pool
updbase     -       Dump UPnP database for interface



