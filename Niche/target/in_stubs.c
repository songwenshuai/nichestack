/*
 * FILENAME: in_stubs.c
 *
 * Copyright 2004 InterNiche Technologies Inc. All rights reserved.
 *
 * Target Board utility functions for InterNiche TCP/IP.
 * This one for the ALTERA Cyclone board with the ALTERA Nios2 Core running 
 * with the uCOS-II RTOS.
 *
 * This file for:
 *   ALTERA Cyclone Dev board with the ALTERA Nios2 Core.
 *   SMSC91C11 10/100 Ethernet Controller
 *   GNU C Compiler provided by ALTERA Quartus Toolset.
 *   Quartus HAL BSP
 *   uCOS-II RTOS Rel 2.76 as distributed by Altera/NIOS2
 *
 * MODULE  : NIOS2GCC
 * ROUTINES: dtrap(), clock_init(), clock_c(), kbhit() getch().
 * PORTABLE: no
 *
 * 06/21/2004
 * 
 */

/* Nichestack definitions */
#include "stm32f10x.h"

/* Includes ------------------------------------------------------------------*/
#include "cpu.h"

#include "ipport.h"
#include "libport.h"
#include "osport.h"
#include "tcpport.h"
#include "net.h"
#include "in_utils.h"
#include "memwrap.h"
#include "ifec.h"

char mac_addr[6]; /* MAC address */

os_sema mheap_sem_ptr;
os_sema rcvdq_sem_ptr;
#ifdef OS_PREEMPTIVE
os_sema resid_semaphore[MAX_RESID + 1];
os_sema app_semaphore[MAX_SEMID + 1];
#endif

#ifndef TCPWAKE_RTOS
/*
 * Q and Mutex used by tcp_sleep/wakeup
 */
extern struct TCP_PendPost global_TCPwakeup_set[GLOBWAKE_SZ];
extern int global_TCPwakeup_setIndx;
#endif

/* NicheStack network structure. */
extern struct net netstatic[STATIC_NETS];

/*
 * Altera Niche Stack Nios port modification:
 * Provide init routine to call after multi-tasking
 * has started to create uc/OS resources.
 *
 * Do not build this portion with SUPERLOOP
 */
#ifndef SUPERLOOP

void iniche_init(void)
{
  int i;

  iniche_net_ready = 0;

   /* initialize the npalloc() heap semaphore */
   mheap_sem_ptr = OSSemCreate(1);
   if (!mheap_sem_ptr)
      panic("mheap_sem_ptr create err"); 

   rcvdq_sem_ptr = OSSemCreate(0);
   if (!rcvdq_sem_ptr)
      panic("rcvdq_sem_ptr create err"); 

#ifdef OS_PREEMPTIVE
  for (i = 0; i <= MAX_RESID; i++)
  {
    resid_semaphore[i] = OSSemCreate(1);
    if (!resid_semaphore[i])
      panic("resid_semaphore create err");
  }
  for (i = 0; i <= MAX_SEMID; i++)
  {
    app_semaphore[i] = OSSemCreate(1);
    if (!app_semaphore[i])
      panic("app_semaphore create err");
  }
#endif /* OS_PREEMPTIVE */

#ifndef TCPWAKE_RTOS
  /* 
    * clear global_TCPwakeup_set
    */
  for (i = 0; i < GLOBWAKE_SZ; i++)
  {
    global_TCPwakeup_set[i].ctick = 0;
    global_TCPwakeup_set[i].soc_event = NULL;
    global_TCPwakeup_set[i].semaphore = OSSemCreate(0);
    if (!global_TCPwakeup_set[i].semaphore)
      panic("globwake_semaphore create err");
  }
  global_TCPwakeup_setIndx = 0;
#endif /* TCPWAKE_RTOS */
}
#endif /* !SUPERLOOP */


char *npalloc(unsigned size)
{
  char *ptr;
  char *(*alloc_rtn)(size_t size) = calloc1;

#ifdef RTOS
   uint8_t err;
#endif

#ifdef RTOS
   OSSemPend(mheap_sem_ptr, 0, &err);
   if(err)
   {
      int errct = 0;

      /* sometimes we get a "timeout" error even though we passed a zero
       * to indicate we'll wait forever. When this happens, try again:
       */
      while(err == OS_SEM_TIMEOUT)
      {
         if(errct++ > 1000)
         {
            panic("npalloc");    /* fatal? */
            return NULL;
         }
         OSSemPend(mheap_sem_ptr, 0, &err);
      }
   }
#endif

#ifdef MEM_WRAPPERS
  ptr = wrap_alloc(size, alloc_rtn);
#else
  ptr = (*alloc_rtn)(size);
#endif

#ifdef RTOS 
   err = OSSemPost(mheap_sem_ptr);
#endif

  if (!ptr)
    return NULL;

  MEMSET(ptr, 0, size);
  return ptr;
}

void npfree(void *ptr)
{
  void (*free_rtn)(char *ptr) = mem_free;

#ifdef RTOS
   uint8_t err;
#endif

#ifdef RTOS
   OSSemPend(mheap_sem_ptr, 0, &err);
   if (err)
   {
      int errct = 0;

      /* sometimes we get a "timeout" error even though we passed a zero
       * to indicate we'll wait forever. When this happens, try again:
       */
      while (err == OS_SEM_TIMEOUT)
      {
         if (errct++ > 1000)
         {
            panic("npfree");    /* fatal? */
            return;
         }
         OSSemPend(mheap_sem_ptr, 0, &err);
      }
   }
#endif

#ifdef MEM_WRAPPERS
  wrap_free((char *)ptr, free_rtn);
#else
  (*free_rtn)((char *)ptr);
#endif

#ifdef RTOS
   err = OSSemPost(mheap_sem_ptr);
#endif
}

/* FUNCTION: memalign()
 *
 * Allocate memory with a given memory alignment
 *
 * PARAM1: align        alignment factor
 * PARAM2: size         number of bytes to allocate
 *
 * RETURN: char *       pointer to allocated memory,
 *                      or NULL if allocation failed
 *
 * 
 */

char *
memalign(unsigned align, unsigned size)
{
   char *ptr;

   /* align must be a power of 2 */
   if (align & (align - 1))
      return ((void *)NULL);

   ptr = (char *)npalloc(size + align - 1);
   if (ptr != NULL)
   {
      ptr = (char *)((unsigned)ptr & ~(align - 1));
   }

   return (ptr);
}

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
 * generate_mac_addr()
 * 
 * This routine is called when failed to read MAC address from flash (i.e: no 
 * flash on the board). The user is prompted to enter the serial number at the 
 * console. A MAC address is then generated using 0xFF followed by the last 2 
 * bytes of the serial number appended to Altera's Vendor ID, an assigned MAC 
 * address range with the first 3 bytes of 00:07:ED.  For example, if the Nios 
 * Development Board serial number is 040800017, the corresponding ethernet 
 * number generated will be 00:07:ED:FF:8F:11.
 * 
 */
int generate_mac_addr(unsigned char mac_addr[6])
{
    int error = -1;

    /* This is the Altera Vendor ID */
    mac_addr[0] = MACADDR0;
    mac_addr[1] = MACADDR1;
    mac_addr[2] = MACADDR2;

    /* Reserverd Board identifier */
    mac_addr[3] = MACADDR3;
    mac_addr[4] = MACADDR4;
    mac_addr[5] = MACADDR5;

    printf("Your Ethernet MAC address is %02x:%02x:%02x:%02x:%02x:%02x\n",
    mac_addr[0], mac_addr[1],mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    error = 0;

    return error;
}

/*
* get_mac_addr
*
* Read the MAC address in a board specific way
*
*/
int get_mac_addr(unsigned char mac_addr[6])
{
    int error = 0;

    error = generate_mac_addr(mac_addr);

    return error;
}

/*
 * get_ip_addr()
 * 
 * This routine is called by InterNiche to obtain an IP address for the
 * specified network adapter. Like the MAC address, obtaining an IP address is
 * very system-dependant and therefore this function is exported for the
 * developer to control.
 * 
 * In our system, we are either attempting DHCP auto-negotiation of IP address,
 * or we are setting our own static IP, Gateway, and Subnet Mask addresses our
 * self. This routine is where that happens.
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

/* FUNCTION: dump_pkt()
 *
 * Print information about a packet
 *
 * PARAM1: pkt;         PACKET to dump
 *
 * RETURN: none
 */

void dump_pkt(PACKET pkt)
{
  int i;
  if (pkt == NULL)
  {
    printf("dump_pkt(): NULL pkt pointer\n");
    return;
  }
  printf("nb_plen = %d\n", pkt->nb_plen);
  for (i = 0; i < pkt->nb_plen; i++)
  {
    if ((i % 16) == 0)
    {
      printf("\n");
    }
    printf("%x ", (unsigned char)(pkt->nb_prot)[i]);
  }
  printf("\n");
}


/* FUNCTION: exit()
 *
 * Program exit
 *
 * PARAM1: code;     program exit code
 *
 * RETURNS: none
 */

void exit(int code)
{
    printf("Exit, code %d. Push RESET button now.\n", code);
    while (1)
        ;
}

/* dtrap() - function to trap to debugger */
void dtrap(void)
{
   printf("dtrap - needs breakpoint\n");
}

/* FUNCTION: getch()
 *
 * Read a chatacter from the Console
 *
 * PARAMS: none
 *
 * RETURNS: int              value of character read or -1 if no character
 */
int getch()
{
   return (uint16_t)(USART2->DR & (uint16_t)0x01FF);
}

/* FUNCTION: dputchar()
 *
 * Output a character to the Console device
 *
 * PARAM1: int            character to output
 *
 * RETURNS: none
 *
 * Converts <CR> to <CR><LF>
 */
void dputchar(int chr)
{
    /* Convert LF in to CRLF */
    if (chr == '\n')
    {
      while ((USART2->SR & USART_FLAG_TXE) == RESET)
        /* null */;
      USART2->DR = ('\r' & (uint16_t)0x01FF);
    }

    /* mask out any high bits */
    while ((USART2->SR & USART_FLAG_TXE) == RESET)
      /* null */;
    USART2->DR = (chr & (uint16_t)0x01FF);
}

/* FUNCTION: kbhit()
 *
 * Tests if there is a character available from the keyboard
 *
 * PARAMS: none
 *
 * RETURN: TRUE if a character is available, otherwise FALSE
 */
int kbhit(void)
{
  if ((USART2->SR & USART_FLAG_RXNE) != (uint16_t)RESET)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*
 * Altera Niche Stack Nios port modification:
 * Add clock_init and clock_c as empty routines to support superloop
 */
#ifdef SUPERLOOP
void clock_init(void)
{
   /* null */ ;
}

void clock_c(void)
{
   /* null */ ;
}

#else /* !SUPERLOOP */

/*
 * system clock : NO OP - start clock tick counting;
 * called at init time.
 * 
 */
int OS_TPS;
int cticks_factor;
int cticks_initialized = 0;

void clock_init(void)
{
   OS_TPS = OS_TICKS_PER_SEC;
   cticks_factor = 0;
   cticks = 0;
   cticks_initialized = 1;
}

/* undo effects of clock_init (i.e. restore ISR vector) 
 * NO OP since using RTOS's timer.
 */
void clock_c(void)
{
   /* null */ ;
}


/* This function is called from the uCOS-II OSTimeTickHook() routine.
 * Use the uCOS-II/Altera HAL BSP's timer and scale cticks as per TPS.
 */

void cticks_hook(void)
{
   if (cticks_initialized) 
   {
      cticks_factor += TPS;
      if (cticks_factor >= OS_TPS)
      {
         cticks++;
         cticks_factor -= OS_TPS;
      }
   }
}

#endif /* SUPERLOOP */

/* Level of Nesting in irq_Mask() and irq_Unmask() */
int irq_level = 0;

/* Latch on to Altera's NIOS2 BSP */
static int cpu_statusreg;

#ifdef NOT_DEF
#define IRQ_PRINTF   1
#endif

/* Disable Interrupts */

/*
 * Altera Niche Stack Nios port modification:
 * The old implementation of the irq_Mask and irq_unMask functions
 * was incorrect because it didn't implement nesting of the interrupts!
 * The InterNiche handbook specification implies that nesting is
 * something that is supported, and the code seems to need it as 
 * well.
 *
 * From Section 2.2.3.1 on the NicheStack Handbook:
 * "Note that it is not sufficient to simply disable interrupts in 
 * ENTER_CRIT_SECTION() and enable them in EXIT_CRIT_SECTION()
 * because calls to ENTER_CRIT_SECTION() can be  nested."
 */
void irq_Mask(void)
{
   if (++irq_level == 1)
   {
      cpu_statusreg = CPU_SR_Save();
   }
}


/* Re-Enable Interrupts */
void irq_Unmask(void)
{
   if (--irq_level == 0)
   {
      CPU_SR_Restore(cpu_statusreg);
   }
}


#ifdef USE_PROFILER

/* FUNCTION: get_ptick
 * 
 * Read the hig-resolution timer
 * 
 * PARAMS: none
 * 
 * RETURN: current high-res timer value
 */
u_long
get_ptick(void)
{
   return (OSTimeGet());
}

#endif  /* USE_PROFILER */

