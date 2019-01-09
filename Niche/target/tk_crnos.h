/*
 * FILENAME: tk_crnos.h
 *
 * Copyright  2001-2002 By InterNiche Technologies Inc. All rights reserved
 *
 * Definitions to map NicheTask "TK_" macros to ChronOS/uCOS RTOS
 *
 * MODULE: MISCLIB
 *
 * PORTABLE: yes (within uCOS systems)
 */

#ifndef TK_CRNOS_H
#define TK_CRNOS_H

#ifndef CHRONOS
#error - must define CHRONOS in ipport.h to use this file
#endif


/* Define the CHRONOS object control TK_ macros...  */
typedef OS_EVENT * 				os_sema;						/* 信号量 */
typedef u_char     			 	os_object;						/* 任务句柄 */
#define OS_TimeDly(cnt)		  	OSTimeDly(cnt)					/* 延时函数 */
#define OS_SEM_TIMEOUT        	OS_ERR_TIMEOUT
#define OS_NONE_ERR           	OS_ERR_NONE

/* macros for task type, entry, and name */
#define  TK_ENTRY(name)       	void  name(void * parm)
#define  TK_OBJECT(name)      	os_object   name
#define  TK_OBJECT_PTR(name)  	os_object * name
#define  TK_OBJECT_REF(name)  	TK_OBJECT(name)
#define  TK_ENTRY_PTR(name)   	void(*name)(void*)

#define  TK_THIS              	TK_OSTaskQuery()

#ifndef  TK_RETURN_ERROR
#define  TK_RETURN_ERROR()    	return
#endif
#ifndef  TK_RETURN_OK
#define  TK_RETURN_OK()       	return
#endif


/*
 * synchronization primitives
 * 
 * Need one or the other of LOCK_NET_RESOURCE/UNLOCK_NET_RESOURCE and
 * ENTER_CRIT_SECTION/EXIT_CRIT_SECTION.  These should be implementable
 * as either functions or macros, so we allow e.g. ipport.h_h to 
 * override these definitions by defining the names as macros.
 * CHRONOS targets generally use LOCK_NET_RESOURCE/UNLOCK_NET_RESOURCE
 * so we default to function declaractions for them.
 */
extern void LOCK_NET_RESOURCE(int res);
extern void UNLOCK_NET_RESOURCE(int res);

extern void irq_Mask(void);
extern void irq_Unmask(void);

#define  ENTER_CRIT_SECTION(p)      irq_Mask()
#define  EXIT_CRIT_SECTION(p)       irq_Unmask()


/* declare tasks which may need to be accessed by system code */
extern   TK_OBJECT(to_pingcheck);
extern   TK_OBJECT(to_netmain);
extern   TK_OBJECT(to_ftpclnt);


/* map TK_ macros to CHRONOS: */
#define  TK_BLOCK()          OSTaskSuspend(OS_PRIO_SELF)

extern os_sema rcvdq_sem_ptr;
#define  TK_NETRX_BLOCK()    { \
	                       		uint8_t err; \
	                       		OSSemPend(rcvdq_sem_ptr, TPS, &err); \
	                       		if ((err != OS_NONE_ERR) && (err != OS_SEM_TIMEOUT)) \
                                  dtrap(); \
	                     	}

#define  TK_SLEEP(count)     OS_TimeDly(count + 1)

/* (Id) is always of type TK_OBJECT_PTR */
#define  TK_WAKE(Id)         OSTaskResume(*(os_object*)(Id))

/* (ev) is always of type TK_OBJECT_REF */
#define  TK_WAKE_EVENT(ev)   OSTaskResume((os_object)(ev))

/* Do tk_yield() as a function to avoid pointless delays */
void tk_yield(void);
#define TK_YIELD()           tk_yield()


/* For ChronOS SignalPktDemux() is a routine which manually sets the net
 * task to runnable. This is so we can call it from an ISR and not have
 * ChronOS enable interrupts on us.
 */
#define  SignalPktDemux()     OSSemPost(rcvdq_sem_ptr)
#define  SignalFtpClient()    OSTaskResume(to_ftpclnt)

#endif  /* TK_CRNOS_H */

