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

#include  "../app_cfg.h"
#include  "bsp_usart.h"
#include  "printf.h"

extern UART_HandleTypeDef huart3;
extern void ENET_Configuration(void);
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

static  OS_STK  StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  StartupTask (void  *p_arg);
static  void  MPU_Config(void);
char  UART_read(void);
void  UART_write(char ch);
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
    CPU_INT08U  os_err;
#endif
    MPU_Config();
    HAL_Init();                                                 /* Initialize STM32Cube HAL Library                     */
    BSP_ClkInit();                                              /* Initialize the main clock                            */
    BSP_IntInit();                                              /* Initialize RAM interrupt vector table.               */
    BSP_OS_TickInit();                                          /* Initialize kernel tick timer                         */

    Mem_Init();                                                 /* Initialize Memory Managment Module                   */
    CPU_IntDis();                                               /* Disable all Interrupts                               */
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */

    OSInit();                                                   /* Initialize uC/OS-II                                  */

    OSTaskCreateExt( StartupTask,                               /* Create the startup task                              */
                     0,
                    &StartupTaskStk[APP_CFG_STARTUP_TASK_STK_SIZE - 1u],
                     APP_CFG_STARTUP_TASK_PRIO,
                     APP_CFG_STARTUP_TASK_PRIO,
                    &StartupTaskStk[0u],
                     APP_CFG_STARTUP_TASK_STK_SIZE,
                     0u,
                    (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if OS_TASK_NAME_EN > 0u
    OSTaskNameSet(         APP_CFG_STARTUP_TASK_PRIO,
                  (INT8U *)"Startup Task",
                           &os_err);
#endif
    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)   */

    while (DEF_TRUE) {                                            /* Should Never Get Here.                               */
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

    MX_USART3_UART_Init();

    BSP_LED_Init();                                             /* Initialize LEDs                                      */
    ENET_Configuration();
#if (OS_TASK_STAT_EN > 0u)
    OSStatInit();                                               /* Determine CPU capacity                               */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

    OSTaskDel(OS_PRIO_SELF);

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
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
  
  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU attributes as Device not cacheable 
     for ETH DMA descriptors */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;                      //区域使能/禁止 
  MPU_InitStruct.BaseAddress = 0x30040000;                        //配置区域基地址 
  MPU_InitStruct.Size = MPU_REGION_SIZE_256B;                     //区域容量 
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;       //设置访问权限
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;            //禁止/允许缓冲
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;          //禁止/允许缓存
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;          //禁止/允许共享
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;                     //区域编号 
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;                   //类型扩展级别 
  MPU_InitStruct.SubRegionDisable = 0x00;                         //子 region 除能位段设置
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;     //允许/禁止取指

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
* @brief  CPU L1-Cache enable.
* @param  None
* @retval None
*/
void CPU_Cache_Init(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

/**
  * @brief  Retargets the C library printf function to the USART1.
  * @param  ch: character to send
  * @param  f: pointer to file (not used)
  * @retval The character transmitted
  */
void UART_write(char ch)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);
}

/**
  * @brief  Retargets the C library scanf function to the USART1.
  * @param  f: pointer to file (not used)
  * @retval The character received
  */
char UART_read(void)
{
  uint8_t ch = 0;
  /* We received the charracter on the handler of the USART1 */
  /* The handler must be initialise before */
  HAL_UART_Receive(&huart3, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}
