/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                    MICRIUM BOARD SUPPORT PACKAGE
*
* Filename : bsp_int_armv7m.c
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <cpu.h>
#include  <os.h>

#include  "bsp_int.h"


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/

CPU_FNCT_VOID  VectorTbl_RAM[INT_VTOR_TBL_SIZE] __attribute__(( aligned (INT_VTOR_TBL_ALIGNMENT) ));


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                         GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              BSP_IntInit()
*
* Description : Initialize interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : (1) The Cortex-M "Vector Table Offset Register" section states the following:
*
*                   You must align the offset to the number of exception entries in the vector table.
*                   The minimum alignment is 32 words, enough for up to 16 interrupts. For more
*                   interrupts, adjust the alignment by rounding up to the next power of two. For example,
*                   if you require 21 interrupts, the alignment must be on a 64-word boundary because the
*                   required table size is 37 words, and the next power of two is 64. SEE YOUR VENDOR
*                   DOCUMENTATION FOR THE ALIGNMENT DEATILS FOR YOUR DEVICE.
*********************************************************************************************************
*/

void  BSP_IntInit (void)
{
    CPU_INT16U      int_id;
    CPU_FNCT_VOID  *p_vect_tbl;
    CPU_SR_ALLOC();


    p_vect_tbl = (CPU_FNCT_VOID *)CPU_REG_NVIC_VTOR;
    for (int_id = 0u; int_id <= (INT_VTOR_TBL_SIZE - 1u); int_id++) {
        VectorTbl_RAM[int_id] = p_vect_tbl[int_id];             /* Copy current table into RAM table                    */
    }

    CPU_CRITICAL_ENTER();
    CPU_REG_NVIC_VTOR = (CPU_INT32U)&VectorTbl_RAM[0u];         /* See note 1.                                          */
    CPU_CRITICAL_EXIT();
}


/*
*********************************************************************************************************
*                                             BSP_IntClear()
*
* Description : Clear interrupt.
*
* Argument(s) : int_id    Interrupt to clear.
*
* Return(s)   : none.
*
* Note(s)     : (1) An interrupt does not need to be cleared within the interrupt controller.
*********************************************************************************************************
*/

void  BSP_IntClear (BSP_INT_ID  int_id)
{

}


/*
*********************************************************************************************************
*                                            BSP_IntDisable()
*
* Description : Disable interrupt.
*
* Argument(s) : int_id    Interrupt to disable.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntDisable (BSP_INT_ID  int_id)
{
    if (int_id < INT_ID_MAX_NBR) {
        CPU_IntSrcDis(int_id + ARMV7M_CORE_EXCS);
    }
}


/*
*********************************************************************************************************
*                                             BSP_IntEnable()
*
* Description : Enable interrupt.
*
* Argument(s) : int_id    Interrupt to enable.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntEnable (BSP_INT_ID  int_id)
{
    if (int_id < INT_ID_MAX_NBR) {
        CPU_IntSrcEn(int_id + ARMV7M_CORE_EXCS);
    }
}


/*
*********************************************************************************************************
*                                            BSP_IntVectSet()
*
* Description : Assign ISR handler.
*
* Argument(s) : int_id         Interrupt for which vector will be set.
*
*               int_prio       Priority of interrupt to be set.
*
*               int_type       Type of interrupt(Depends on CPU Architecture).
*
*               isr_handler    Handler to assign
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntVectSet (BSP_INT_ID     int_id,
                      CPU_INT08U     int_prio,
                      BSP_INT_TYPE   int_type,
                      CPU_FNCT_VOID  isr_handler)
{
   (void)int_prio;
   (void)int_type;

    if (int_id < INT_ID_MAX_NBR) {
        VectorTbl_RAM[int_id + ARMV7M_CORE_EXCS] = isr_handler;
    }
}
