/*
*********************************************************************************************************
*
*                                              uC/OS-II
*                                            EXAMPLE CODE
*
* Filename : main.c
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <cpu.h>
#include  <lib_mem.h>
#include  <os.h>
#include  <bsp_os.h>
#include  <bsp_clk.h>
#include  <bsp_led.h>
#include  <bsp_int.h>
#include  <stm32h7xx_hal.h>

#if (OS_TRACE_EN > 0u)
/* SEEGER */
#include  <os_trace.h>
#endif

#include  "app_cfg.h"
#include  "bsp_usart.h"

/* Nichestack definitions */
#include  "ipport.h"
#include  "libport.h"
#include  "osport.h"
#include  "tcpport.h"
#include  "net.h"

#include "fftest.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_STK  StartupTaskStk[TK_STARTUP_SSIZE];
static  INT8U  *StartupTaskName  = "Startup Task";

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/


static  void  StartupTask (void  *p_arg);
static  void  MPU_Config(void);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*
* Notes       : none
*********************************************************************************************************
*/

int  main (void)
{
#if OS_TASK_NAME_EN > 0u
    INT8U  os_err;
#endif

    MPU_Config();
    HAL_Init();                                                 /* Initialize STM32Cube HAL Library                     */
    BSP_ClkInit();                                              /* Initialize the main clock                            */
    BSP_IntInit();                                              /* Initialize RAM interrupt vector table.               */
    BSP_OS_TickInit();                                          /* Initialize kernel tick timer                         */
    BSP_LED_Init();                                             /* Initialize LEDs                                      */
    BSP_UART_Init();                                            /* Initialize UART                                      */
    FS_Init();
#if (LIB_MEM_CFG_HEAP_SIZE > 0u)
    Mem_Init();                                                 /* Initialize Memory Managment Module                   */
#endif
    CPU_IntDis();                                               /* Disable all Interrupts                               */
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */
    osInitKernel();                                             /* Initialize uC/OS-II                                  */

    os_err = OSTaskCreateExt( StartupTask,                      /* Create the startup task                              */
                              0,
                              &StartupTaskStk[TK_STARTUP_SSIZE - 1u],
                              TK_STARTUP_TPRIO,
                              TK_STARTUP_TPRIO,
                              &StartupTaskStk[0u],
                              TK_STARTUP_SSIZE,
                              0u,
                              (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
    if (os_err != OS_ERR_NONE)
    {
        /* All other errors are fatal */
        TRACE_ERROR("Task create error on %s\n", StartupTaskName);
    }
#if OS_TASK_NAME_EN > 0u
    OSTaskNameSet( TK_STARTUP_TPRIO,
                  (INT8U *)StartupTaskName,
                  &os_err);
#endif
    if (os_err != OS_ERR_NONE)
    {
        /* All other errors are fatal */
        TRACE_ERROR("Task Name Set error on %s\n", StartupTaskName);
    }

    osStartKernel();                                                  /* Start multitasking (i.e. give control to uC/OS-II)   */

    while (DEF_TRUE) {                                          /* Should Never Get Here.                               */
        ;
    }
}


/*
*********************************************************************************************************
*                                            STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'StartupTask()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  StartupTask (void *p_arg)
{
    (void)p_arg;

    OS_TRACE_INIT();                                            /* Initialize the OS Trace recorder                     */

    BSP_OS_TickEnable();                                        /* Enable the tick timer and interrupt                  */

#if (OS_TRACE_EN > 0u)
    SEGGER_SYSVIEW_PrintfHostEx("Start SystemView",SEGGER_SYSVIEW_WARNING);
#endif

#if (OS_TASK_STAT_EN > 0u)
    OSStatInit();                                               /* Determine CPU capacity                               */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

    /*
    * Initialize Altera NicheStack TCP/IP Stack - Nios II Edition specific code.
    * NicheStack is initialized from a task, so that RTOS will have started, and 
    * I/O drivers are available.  Two tasks are created:
    *    "Inet main"  task with priority 2
    *    "clock tick" task with priority 3
    */
    iniche_init();

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */    
    osDeleteTask(&Os_Prio_Self);
  }
}


/**
  * @brief  Configure the MPU attributes 
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{
   MPU_Region_InitTypeDef MPU_InitStruct;

   //Disable MPU
   HAL_MPU_Disable();

   //DTCM RAM
   MPU_InitStruct.Enable            = MPU_REGION_ENABLE;             //区域使能/禁止
   MPU_InitStruct.Number            = MPU_REGION_NUMBER0;            //区域编号
   MPU_InitStruct.BaseAddress       = 0x20000000;                    //配置区域基地址
   MPU_InitStruct.Size              = MPU_REGION_SIZE_128KB;         //区域容量
   MPU_InitStruct.SubRegionDisable  = 0;                             //子 region 除能位段设置
   MPU_InitStruct.TypeExtField      = MPU_TEX_LEVEL0;                //类型扩展级别
   MPU_InitStruct.AccessPermission  = MPU_REGION_FULL_ACCESS;        //设置访问权限
   MPU_InitStruct.DisableExec       = MPU_INSTRUCTION_ACCESS_ENABLE; //允许/禁止取指
   MPU_InitStruct.IsShareable       = MPU_ACCESS_SHAREABLE;          //禁止/允许共享
   MPU_InitStruct.IsCacheable       = MPU_ACCESS_CACHEABLE;          //禁止/允许缓存
   MPU_InitStruct.IsBufferable      = MPU_ACCESS_NOT_BUFFERABLE;     //禁止/允许缓冲
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   //AXI SRAM
   MPU_InitStruct.Enable            = MPU_REGION_ENABLE;             //区域使能/禁止
   MPU_InitStruct.Number            = MPU_REGION_NUMBER1;            //区域编号
   MPU_InitStruct.BaseAddress       = 0x24000000;                    //配置区域基地址
   MPU_InitStruct.Size              = MPU_REGION_SIZE_512KB;         //区域容量
   MPU_InitStruct.SubRegionDisable  = 0x00;                          //子 region 除能位段设置
   MPU_InitStruct.TypeExtField      = MPU_TEX_LEVEL0;                //类型扩展级别
   MPU_InitStruct.AccessPermission  = MPU_REGION_FULL_ACCESS;        //设置访问权限
   MPU_InitStruct.DisableExec       = MPU_INSTRUCTION_ACCESS_ENABLE; //允许/禁止取指
   MPU_InitStruct.IsShareable       = MPU_ACCESS_SHAREABLE;          //禁止/允许共享
   MPU_InitStruct.IsCacheable       = MPU_ACCESS_CACHEABLE;          //禁止/允许缓存
   MPU_InitStruct.IsBufferable      = MPU_ACCESS_NOT_BUFFERABLE;     //禁止/允许缓冲
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   //AHB SRAM1(malloc)
    /* Configure the MPU attributes as Cacheable write through */
    MPU_InitStruct.Enable           = MPU_REGION_ENABLE;             //区域使能/禁止
    MPU_InitStruct.Number           = MPU_REGION_NUMBER2;            //区域编号
    MPU_InitStruct.BaseAddress      = 0x30000000;                    //配置区域基地址
    MPU_InitStruct.Size             = MPU_REGION_SIZE_256KB;         //区域容量
    MPU_InitStruct.SubRegionDisable = 0x00;                          //子 region 除能位段设置
    MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;                //类型扩展级别
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;        //设置访问权限
    MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE; //允许/禁止取指
    MPU_InitStruct.IsShareable      = MPU_ACCESS_SHAREABLE;          //禁止/允许共享
    MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;          //禁止/允许缓存
    MPU_InitStruct.IsBufferable     = MPU_ACCESS_BUFFERABLE;         //禁止/允许缓冲
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

   //AHB SRAM3 (no cache)(ETH Buffer)
    /* Configure the MPU attributes as Device not cacheable 
       for ETH DMA descriptors */
    MPU_InitStruct.Enable           = MPU_REGION_ENABLE;             //区域使能/禁止
    MPU_InitStruct.Number           = MPU_REGION_NUMBER3;            //区域编号
    MPU_InitStruct.BaseAddress      = 0x30040000;                    //配置区域基地址
    MPU_InitStruct.Size             = MPU_REGION_SIZE_32KB;          //区域容量
    MPU_InitStruct.SubRegionDisable = 0x00;                          //子 region 除能位段设置
    MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;                //类型扩展级别
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;        //设置访问权限
    MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE; //允许/禁止取指
    MPU_InitStruct.IsShareable      = MPU_ACCESS_SHAREABLE;          //禁止/允许共享
    MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;      //禁止/允许缓存
    MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;     //禁止/允许缓冲
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Enable the MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
* @brief  CPU L1-Cache enable.
* @param  None
* @retval None
*/
#if (CPU_CFG_CACHE_MGMT_EN == DEF_ENABLED)
void CPU_Cache_Init(void)
{
    /* Enable I-Cache */
    SCB_EnableICache();

    /* Enable D-Cache */
    SCB_EnableDCache();
}
#endif
