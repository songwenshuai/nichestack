/*
 * FILENAME: brdutils.c
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
#include  <stm32h7xx_hal.h>

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
#include "bsp_usart.h"

OsSemaphore mheap_sem_ptr;
OsSemaphore rcvdq_sem_ptr;
#ifdef OS_PREEMPTIVE
OsSemaphore resid_semaphore[MAX_RESID + 1];
OsSemaphore app_semaphore[MAX_SEMID + 1];
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
 * Create Sem
 */
void sem_create(void)
{
   int i;
   INT8U SEM_NAME[20] = {0};
   
   /* initialize the npalloc() heap semaphore */
   if(!osCreateSemaphore(&mheap_sem_ptr, 1))
   {
      TRACE_ERROR("mheap_sem_ptr create err\r\n");      /* SYS_DEBUG MESSAGE */
   }
   OSEventNameSet(mheap_sem_ptr.p,  (INT8U *)"mheap sem",   NULL);

   if(!osCreateSemaphore(&rcvdq_sem_ptr, 0))
   {
      TRACE_ERROR("rcvdq_sem_ptr create err\r\n");      /* SYS_DEBUG MESSAGE */
   }
   OSEventNameSet(rcvdq_sem_ptr.p,  (INT8U *)"rcvdq sem",   NULL);

#ifdef OS_PREEMPTIVE

   for (i = 0; i <= MAX_RESID; i++)
   {
      if(!osCreateSemaphore(&resid_semaphore[i], 1))
      {
         TRACE_ERROR("resid_semaphore create err\r\n");  /* SYS_DEBUG MESSAGE */
      }
      sprintf((char *)SEM_NAME, "resid sem[%d]", i);
      OSEventNameSet(resid_semaphore[i].p, SEM_NAME, NULL);
   }
   
   for (i = 0; i <= MAX_SEMID; i++)
   {
      if(!osCreateSemaphore(&app_semaphore[i], 1))
      {
         TRACE_ERROR("app_semaphore create err\r\n"); /* SYS_DEBUG MESSAGE */
      }
      sprintf((char *)SEM_NAME, "app sem[%d]", i);
      OSEventNameSet(app_semaphore[i].p, SEM_NAME, NULL);
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
    if(!osCreateSemaphore(&global_TCPwakeup_set[i].semaphore, 0))
      {
         TRACE_ERROR("globwake_semaphore create err\r\n");  /* SYS_DEBUG MESSAGE */
      }
      sprintf((char *)SEM_NAME, "tcp wake sem[%d]", i);
      OSEventNameSet(global_TCPwakeup_set[i].semaphore.p, SEM_NAME, NULL);
   }
  global_TCPwakeup_setIndx = 0;
#endif /* TCPWAKE_RTOS */
}

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
   iniche_net_ready = 0;

   /* Get HEAP definitions */
   mheap_init(HEAP_START, HEAP_SIZE);
   
   /* Created Sem */
   sem_create();

   /* Start the Iniche-specific network tasks and initialize the network
     * devices.
     */
   osSuspendAllTasks();
   netmain(); /* Create net tasks */
   osResumeAllTasks();

   /* Wait for the network stack to be ready before proceeding. */
   while (!iniche_net_ready)
      TK_SLEEP(1);
}
#endif /* !SUPERLOOP */


char *npalloc(unsigned size)
{
  char *ptr;
  char *(*alloc_rtn)(size_t size) = calloc1;

#ifdef RTOS
   int status;
#endif

#ifdef RTOS
   status = osWaitForSemaphore(&mheap_sem_ptr, INFINITE_DELAY);
   if (!status)
   {
      TRACE_ERROR("alloc mheap sem pend err\r\n");  /* SYS_DEBUG MESSAGE */
   }
#endif

#ifdef MEM_WRAPPERS
  ptr = wrap_alloc(size, alloc_rtn);
#else
  ptr = (*alloc_rtn)(size);
#endif

#ifdef RTOS 
   osReleaseSemaphore(&mheap_sem_ptr);
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
   int status;
#endif

#ifdef RTOS
   status = osWaitForSemaphore(&mheap_sem_ptr, INFINITE_DELAY);
   if (!status)
   {
      TRACE_ERROR("free mheap sem pend err\r\n");  /* SYS_DEBUG MESSAGE */
   }
#endif

#ifdef MEM_WRAPPERS
  wrap_free((char *)ptr, free_rtn);
#else
  (*free_rtn)((char *)ptr);
#endif

#ifdef RTOS
   osReleaseSemaphore(&mheap_sem_ptr);
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
int getch(void)
{
#if 1 /* 从串口接收FIFO中取1个数据, 只有取到数据才返回 */
   unsigned char chr;

   while (UART_GET_Char(COM3, &chr) == 0)
   ;

   return chr;
#else
  /* 等待接收到数据 */
  while ((USART3->ISR & USART_ISR_RXNE) == 0)
  {
  }

  return (int)USART3->RDR;
#endif
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
#if 1 /* 将需要printf的字符通过串口中断FIFO发送出去，printf函数会立即返回 */
   /* Convert LF in to CRLF */
   if (chr == '\n')
   {
      UART_SEND_Char(COM3,'\r');
   }

   UART_SEND_Char(COM3,chr);
   //return chr;

#else /* 采用阻塞方式发送每个字符,等待数据发送完毕 */
   /* 写一个字节到USART1 */
   USART3->TDR = chr;
 
   /* 等待发送结束 */
   while ((USART3->ISR & USART_ISR_TC) == 0)
   ;

   return chr;
#endif
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
  if(UART_RX_Empty(COM3))
  {
    return TRUE;
  }
  return FALSE;
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
unsigned int irq_level = 0;

/* Latch on to Altera's NIOS2 BSP */
static OS_CPU_SR _cpu_statusreg;

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
   OS_CPU_SR cpu_statusreg_sr;

   cpu_statusreg_sr = OS_CPU_SR_Save();

   if (++irq_level == 1)
   {
      _cpu_statusreg = cpu_statusreg_sr;
   }
}


/* Re-Enable Interrupts */
void irq_Unmask(void)
{
   if (--irq_level == 0)
   {
      OS_CPU_SR_Restore(_cpu_statusreg);
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
   return ((u_long)osGetSystemTime());
}

#endif  /* USE_PROFILER */

