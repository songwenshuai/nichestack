/* 
 * File name : osport.h
 *
 * Map InterNiche tasking primitives to ChronOS or set up NicheTask.
 *
 * This file for:
 *   ALTERA Cyclone Dev board with the ALTERA Nios2 Core.
 *   SMSC91C11 10/100 Ethernet Controller
 *   GNU C Compiler provided by ALTERA Quartus Toolset.
 *   Quartus HAL BSP
 *   uCOS-II RTOS Rel 2.76 as distributed by Altera/NIOS2
 *
 * 06/21/2004
 * 
 */

/* We don't guard osport.h with OSPORT_H on purpose -AK- */

#ifndef osport_h
#define osport_h

#include "ipport.h"

#include "q.h"
#include "netbuf.h"
#include "net.h"
#include "arp.h"
#include "ip.h"
#include "icmp.h"
#include "udp.h"
#include "nvparms.h"
#include "nvfsio.h"
#include "menu.h"
#include "app_ping.h"

/* table with an entry for each internet task/thread. This is filled
in the netmain.c, so it should have the same values in the same order 
in all ports */

struct inet_taskinfo {
   TK_OBJECT_PTR(tk_ptr);  /* pointer to static task object */
   char * name;            /* name of task */
   TK_ENTRY_PTR(entry);    /* pointer to code to start task at */
   int   priority;         /* priority of task */
   int   stacksize;        /* size (bytes) of task's stack */
};

int TK_NEWTASK(struct inet_taskinfo * nettask);

extern char * pre_task_setup(void);
extern char * post_task_setup(void);

extern int netmain(void);
extern TK_OBJECT(to_netmain);

extern int num_net_tasks;
extern struct inet_taskinfo nettasks[];

extern void dtrap(void);

/* RTOS pend/post */
#define  GLOBWAKE_SZ             20

struct wake_event
{
   void *wake_sem;
   void *soc_event;        
   char *name;
};

struct TCP_PendPost
{
   u_long   ctick;           /* time entry was added */
   void     *soc_event;      /* wakeup event */	
   os_sema  semaphore;      /* semaphore to wait on */
};
 
#ifdef MINI_IP
#define net_system_exit (FALSE)
#else
extern int net_system_exit;   /* TRUE if system is shutting down */
#endif /* MINI_IP */

extern int iniche_net_ready;

#endif /* osport_h */
