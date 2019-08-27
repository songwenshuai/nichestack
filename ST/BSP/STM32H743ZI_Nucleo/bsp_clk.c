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
*                                         STM32H743ZI NUCLEO
*
* Filename : bsp_clk.c
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <lib_def.h>
#include  "bsp_clk.h"

#ifdef  CLK_MODULE_PRESENT
#include  <Source/clk.h>
#include  <clk_cfg.h>
#endif

#include  <stm32h7xx_hal.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                          GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            BSP_ClkInit()
*
* Description : Initializes the system clock for the kernel.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Note(s)     : (1) Should be called before OSStart() in main.
*
*               (2) The voltage scaling allows optimizing the power consumption when the device is
*                   clocked below the maximum system frequency, to update the voltage scaling value
*                   regarding system frequency refer to product datasheet.
*********************************************************************************************************
*/

void  BSP_ClkInit (void)
{
    RCC_ClkInitTypeDef  RCC_ClkInit;
    RCC_OscInitTypeDef  RCC_OscInit;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    PWR->CR3 &= ~PWR_CR3_SCUEN;                                 /* Supply configuration updated locked.                 */

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);  /* See Note 2.                                      */

    while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
        ;
    }
    /* Enable D2 domain SRAM1 Clock (0x30000000 AXI)*/
    __HAL_RCC_D2SRAM1_CLK_ENABLE(); 
    /* Enable D2 domain SRAM3 Clock (0x30040000 AXI)*/
    __HAL_RCC_D2SRAM3_CLK_ENABLE();
                                                                /* Enable HSE Osc and activate PLL with HSE as source   */
    RCC_OscInit.OscillatorType = RCC_OSCILLATORTYPE_HSE;        /* HSE = 8 MHz                                          */
    RCC_OscInit.HSEState       = RCC_HSE_ON;
    RCC_OscInit.HSIState       = RCC_HSI_OFF;
    RCC_OscInit.CSIState       = RCC_CSI_OFF;
    RCC_OscInit.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInit.PLL.PLLSource  = RCC_PLLSOURCE_HSE;

    RCC_OscInit.PLL.PLLM       = 4u;                            /* f(REF_CK)    = HSE / PLLM          = 2MHz            */
    RCC_OscInit.PLL.PLLN       = 400u;                          /* f(VCO clock) = f(REF_CK) * PLLN    = 800MHz          */
    RCC_OscInit.PLL.PLLP       = 2u;                            /* PLL(P CLK)   = f(VCO clock) / PLLR = 400MHz          */
    RCC_OscInit.PLL.PLLR       = 2u;                            /* PLL(R CLK)   = f(VCO clock) / PLLR = 400MHz          */
    RCC_OscInit.PLL.PLLQ       = 4u;                            /* PLL(Q CLK)   = f(VCO clock) / PLLQ = 200MHz          */

    RCC_OscInit.PLL.PLLVCOSEL  = RCC_PLL1VCOWIDE;
    RCC_OscInit.PLL.PLLRGE     = RCC_PLL1VCIRANGE_2;

    if (HAL_RCC_OscConfig(&RCC_OscInit) != HAL_OK) {
        while(1u);                                              /* STOP if error                                        */
    }

                                                                /* ----- CFG SYSCLK SOURCE AND BUS CLOCK DIVIDERS ----- */
    RCC_ClkInit.ClockType      = (RCC_CLOCKTYPE_SYSCLK   |
                                  RCC_CLOCKTYPE_HCLK     |
                                  RCC_CLOCKTYPE_D1PCLK1  |
                                  RCC_CLOCKTYPE_PCLK1    |
                                  RCC_CLOCKTYPE_PCLK2    |
                                  RCC_CLOCKTYPE_D3PCLK1);

    RCC_ClkInit.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;       /* Select PLL as system clock source.                   */
    RCC_ClkInit.SYSCLKDivider  = RCC_SYSCLK_DIV1;               /* SYSCLK  = PLL(P CLK) = 400MHz                        */
    RCC_ClkInit.AHBCLKDivider  = RCC_HCLK_DIV2;                 /* HCLK    = AHBxCLK = SYSCLK / AHBPRES(2) = 200MHz.    */
    RCC_ClkInit.APB3CLKDivider = RCC_APB3_DIV2;                 /* APB3CLK = AHB3CLK / APB3DIV(2)          = 100MHz.    */
    RCC_ClkInit.APB1CLKDivider = RCC_APB1_DIV2;                 /* APB1CLK = AHB1CLK / APB1DIV(2)          = 100MHz.    */
    RCC_ClkInit.APB2CLKDivider = RCC_APB2_DIV2;                 /* APB2CLK = AHB2CLK / APB2DIV(2)          = 100MHz.    */
    RCC_ClkInit.APB4CLKDivider = RCC_APB4_DIV2;                 /* APB2CLK = AHB4CLK / APB4DIV(2)          = 100MHz.    */

    if (HAL_RCC_ClockConfig(&RCC_ClkInit, FLASH_LATENCY_4) != HAL_OK) {
        while(1u);                                              /* STOP if error                                        */
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3;
    PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        while(1u);                                              /* STOP if error                                        */
    }
    __HAL_RCC_CSI_ENABLE();                                     /* activate CSI clk mondatory for I/O Compensation Cell */
    __HAL_RCC_SYSCFG_CLK_ENABLE();                              /* Enable SYSCFG clk mondatory for I/O Compensation Cell*/
    HAL_EnableCompensationCell();                               /* Enables the I/O Compensation Cell                    */
}


/*
*********************************************************************************************************
*                                           BSP_ClkEnable()
*
* Description : Enables a specific peripheral clock.
*
* Argument(s) : clk_id    Clock identifier
*
* Return(s)   : none
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_ClkEnable (BSP_CLK_ID  clk_id)
{
    (void)clk_id;
}


/*
*********************************************************************************************************
*                                          BSP_ClkDisable()
*
* Description : Disables a specific peripheral clock.
*
* Argument(s) : clk_id    Clock identifier
*
* Return(s)   : none
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_ClkDisable (BSP_CLK_ID  clk_id)
{
    (void)clk_id;
}


/*
*********************************************************************************************************
*                                          BSP_ClkFreqGet()
*
* Description : Gets the current frequency of the specified clock.
*
* Argument(s) : clk_id    Clock identifier
*
* Return(s)   : The specific clock frequency, in Hz.
*
* Note(s)     : (1) This function will return 0 if the clock is not available. The caller should check
*                   the return value before using it.
*********************************************************************************************************
*/

CPU_INT32U  BSP_ClkFreqGet (BSP_CLK_ID  clk_id)
{
    CPU_INT32U  clk_freq;


    switch (clk_id) {
        case CLK_ID_SYSCLK:
             clk_freq = HAL_RCC_GetSysClockFreq();
             break;


        case CLK_ID_HCLK:
             clk_freq = HAL_RCC_GetHCLKFreq();
             break;


        case CLK_ID_PCLK1:
             clk_freq = HAL_RCC_GetPCLK1Freq();
             break;


        case CLK_ID_PCLK2:
             clk_freq = HAL_RCC_GetPCLK2Freq();
             break;


        default:
             clk_freq = 0u;                                     /* not a valid clock frequency.                         */
             break;
    }

    return (clk_freq);
}


/*
*********************************************************************************************************
*                                          BSP_ClkFreqSet()
*
* Description : Sets the frequency of a specific clock module.
*
* Argument(s) : clk_id        Clock identifier
*
*               freq          The frequency in Hz. we want clk_id to run at.
*
* Return(s)   : DEF_VALID,    The specified clock and/or frequency are valid arguments.
*
*               DEF_INVALID,  The specified clock and/or frequency are invalid arguments.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_ClkFreqSet (BSP_CLK_ID  clk_id,
                             CPU_INT32U  freq)
{
   (void)clk_id;
   (void)freq;

    return (DEF_VALID);
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            EXTENDED API
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      EXTERNAL TIMESTAMP TIMER
*********************************************************************************************************
*/

#ifdef  CLK_MODULE_PRESENT


/*
*********************************************************************************************************
*                                          Clk_ExtTS_Init()
*
* Description : Initialize & start External timestamp timer.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Clk_Init().
*
*               This function is an INTERNAL Clock module function & MUST be implemented by application/
*               BSP function(s) [see Note #1] but MUST NOT be called by application function(s).
*
* Note(s)     : (1) CLK_ExtTS_Init() is an application/BSP function that MUST be defined by the developer
*                   if External timestamp is enabled.
*
*                   See 'clk_cfg.h  CLK CONFIGURATION  Note #1'.
*
*               (2) (a) External timestamp values MUST be returned via 'CLK_TS_SEC' data type.
*
*                   (b) External timestamp values SHOULD be returned on the epoch of Clock module and
*                       include the time zone offset.
*
*               (3) (a) External timestamp SHOULD be an 'up' counter whose values increase at each second.
*                       It's possible to use a 'down' counter, but a conversion MUST be applied when setting
*                       and getting timestamp.
*
*                   (b) External timestamp COULD come from another application (e.g. by SNTPc).
*********************************************************************************************************
*/

#if (CLK_CFG_EXT_EN == DEF_ENABLED)
void  Clk_ExtTS_Init (void)
{

}
#endif


/*
*********************************************************************************************************
*                                           Clk_ExtTS_Get()
*
* Description : Get Clock module's timestamp from converted External timestamp.
*
* Argument(s) : none.
*
* Return(s)   : Current Clock timestamp (in seconds, UTC+00).
*
* Caller(s)   : Clk_GetTS().
*
*               This function is an INTERNAL Clock module function & MUST be implemented by application/
*               BSP function(s) [see Note #1] but SHOULD NOT be called by application function(s).
*
* Note(s)     : (1) Clk_ExtTS_Get() is an application/BSP function that MUST be defined by the developer
*                   if External timestamp is enabled.
*
*                   See 'clk_cfg.h  CLK CONFIGURATION  Note #1'
*
*
*               (2) (a) Clock timestamp values MUST be returned via 'CLK_TS_SEC' data type.
*
*                   (b) (1) If the External timestamp has more bits than the 'CLK_TS_SEC' data type,
*                           Clk_ExtTS_Get() MUST truncate the External timestamp's higher order bits
*                           greater than the 'CLK_TS_SEC' data type.
*
*                       (2) If the External timestamp has less bits than the 'CLK_TS_SEC' data type,
*                           Clk_ExtTS_Get() MUST pad the Clock timestamp's higher order bits with
*                           0 bits.
*
*
*               (3) (a) External timestamp values MUST be returned from the reference of the Clock
*                       epoch start date/time.
*
*                   (b) External timestamp SHOULD start on midnight of January 1st of its epoch start
*                       year. Otherwise, the equations to convert between External timestamp & Clock
*                       timestamp MUST also include the External timestamp's epoch Day-of-Year, Hour,
*                       Minute, & Second (see Note #4).
*
*                   (c) Returned Clock timestamp MUST be representable in Clock epoch. Thus equivalent
*                       date of the External timestamp MUST be between :
*
*                       (1) >= CLK_EPOCH_YR_START
*                       (2) <  CLK_EPOCH_YR_END
*
*                   (d) If the External timestamp includes an (optional) external time zone,
*                       Clk_ExtTS_Get() MUST subtract the external time zone offset from the
*                       converted External timestamp.
*
*
*               (4) The Clock timestamp is calculated by one of the following equations (assuming
*                   Note #3b) :
*
*                   (a) When External epoch start year is less than Clock epoch start year
*                       ('CLK_EPOCH_YR_START') :
*
*                       Clock TS = External TS
*                                - [(((Clock start year - External start year) * 365) + leap day count)
*                                    * seconds per day]
*                                - External TZ
*
*                       Examples with a 32-bit External timestamp :
*
*                       (1)   Valid equivalent date to convert is after  Clock epoch start year :
*
*                             2010 Oct 8, 11:11:11 UTC-05:00
*                           External TS (in seconds)                                 = 1286536271
*                           External start year                                      =       1970
*                           Clock    start year                                      =       2000
*                           Leap day count between External & Clock epoch start year =          7
*                           External TZ (in seconds)                                 =     -18000
*                           Clock    TS (in seconds)                                 =  339869471
*                             2010 Oct 8, 16:11:11 UTC
*
*                           This example successfully converts an External timestamp into a
*                           representable Clock timestamp without underflowing.
*
*                       (2) Invalid equivalent date to convert is before Clock epoch start year :
*
*                             1984 Oct 8, 11:11:11 UTC-05:00
*                           External TS (in seconds)                                 =  466081871
*                           External start year                                      =       1970
*                           Clock    start year                                      =       2000
*                           Leap day count between External & Clock epoch start year =          7
*                           External TZ (in seconds)                                 =     -18000
*                           Clock    TS (in seconds)                                 = -480584929
*
*                           This example underflows to a negative Clock timestamp since the
*                           equivalent date to convert is incorrectly less than the Clock epoch
*                           start year ('CLK_EPOCH_YR_START').
*
*
*                   (b) When External epoch start year is greater than Clock epoch start year
*                       ('CLK_EPOCH_YR_START') :
*
*                       Clock TS = External TS
*                                + [(((External start year - Clock start year) * 365) + leap day count)
*                                    * seconds per day]
*                                - External TZ
*
*
*                       Examples with a 32-bit External timestamp :
*
*                       (1)   Valid equivalent date to convert is before Clock epoch end year :
*
*                             2010 Oct 8, 11:11:11 UTC-05:00
*                           External TS (in seconds)                                 =   24232271
*                           External start year                                      =       2010
*                           Clock    end   year                                      =       2136
*                           Leap day count between External & Clock epoch start year =          3
*                           External TZ (in seconds)                                 =     -18000
*                           Clock    TS (in seconds)                                 =  339869471
*                             2010 Oct 8, 16:11:11 UTC-05:00
*
*                           This example successfully converts an External timestamp into a
*                           representable Clock timestamp without overflowing.
*
*                       (2) Invalid equivalent date to convert is after  Clock epoch end year :
*
*                             2140 Oct 8, 11:11:11 UTC-05:00
*                           External TS (in seconds)                                 = 4126677071
*                           External start year                                      =       2010
*                           Clock    end   year                                      =       2136
*                           Leap day count between External & Clock epoch start year =          3
*                           External TZ (in seconds)                                 =     -18000
*                           Clock    TS (in seconds)                                 = 4442314271
*
*                           This example overflows the Clock timestamp (32-bit) 'CLK_TS_SEC' data
*                           type with an equivalent date incorrectly greater than or equal to the
*                           Clock epoch end year ('CLK_EPOCH_YR_END').
*
*
*                   (c) Where
*
*                       (1) Clock    TS             Converted Clock timestamp (in seconds,
*                                                       from UTC+00)
*                       (2) External TS             External timestamp to convert (in seconds)
*                       (3) Clock    start year     Clock epoch start year ('CLK_EPOCH_YR_START')
*                       (4) Clock    end   year     Clock epoch end   year ('CLK_EPOCH_YR_END')
*                       (5) External start year     External timestamp epoch start year
*                       (6) Leap day count          Number of leap days between Clock epoch
*                                                       start year & External epoch start year
*                       (7) Seconds per day         Number of seconds per day (86400)
*                       (8) External TZ             Time zone offset applied to External TS
*                                                       (in seconds, from UTC+00)
*********************************************************************************************************
*/

#if (CLK_CFG_EXT_EN == DEF_ENABLED)
CLK_TS_SEC  Clk_ExtTS_Get (void)
{
    return (0u);
}
#endif


/*
*********************************************************************************************************
*                                           Clk_ExtTS_Set()
*
* Description : Set External timestamp.
*
* Argument(s) : ts_sec      Timestamp value to set (in seconds, UTC+00).
*
* Return(s)   : DEF_OK,     if External timestamp succesfully set.
*
*               DEF_FAIL,   otherwise.
*
* Caller(s)   : Clk_SetTS().
*
*               This function is an INTERNAL Clock module function & MUST be implemented by application/
*               BSP function(s) [see Note #1] but SHOULD NOT be called by application function(s).
*
* Note(s)     : (1) CLK_ExtTS_Set() is an application/BSP function that MUST be defined by the developer
*                   if External timestamp is enabled.
*
*                       See 'clk_cfg.h  CLK CONFIGURATION  Note #1'.
*
*                   (a) If External timestamp is provided by another application, it's possible that the
*                       External timestamp may NOT be set (e.g. by SNTPc) in which case CLK_ExtTS_Set()
*                       MUST ALWAYS return 'DEF_FAIL'.
*
*
*               (2) (a) External timestamp values are converted from Clock timestamp's 'CLK_TS_SEC'
*                       data type.
*
*                   (b) (1) If the External timestamp has more bits than the 'CLK_TS_SEC' data type,
*                           Clk_ExtTS_Set() MUST pad the External timestamp's higher order bits with
*                           0 bits.
*
*                       (2) If the External timestamp has less bits than the 'CLK_TS_SEC' data type,
*                           Clk_ExtTS_Set() MUST truncate the Clock timestamp's higher order bits
*                           greater than the External timestamp.
*
*
*               (3) (a) External timestamp values MUST be converted from the reference of the Clock
*                       epoch start date/time.
*
*                   (b) External timestamp SHOULD start on midnight of January 1st of its epoch start
*                       year. Otherwise, the equations to convert between External timestamp & Clock
*                       timestamp MUST also include the External timestamp's epoch Day-of-Year, Hour,
*                       Minute, & Second (see Note #4).
*
*                   (c) Converted External timestamp MUST be representable in External epoch. Thus
*                       equivalent date of the External timestamp MUST be between :
*
*                       (1) External epoch start year
*                       (2) External epoch end   year
*
*                   (d) If the External timestamp includes an (optional) external time zone,
*                       Clk_ExtTS_Set() MUST add the external time zone offset to the converted
*                       External timestamp.
*
*
*               (4) The External timestamp is calculated by one of the following equations (assuming
*                   Note #3b) :
*
*                   (a) When External epoch start year is less than Clock epoch start year
*                       ('CLK_EPOCH_YR_START') :
*
*                       External TS = Clock TS
*                                   + [(((Clock start year - External start year) * 365) + leap day count)
*                                       * seconds per day]
*                                   + External TZ
*
*                       Examples with a 32-bit External timestamp :
*
*                       (1)   Valid equivalent date to convert is before External epoch end year :
*
*                             2010 Oct 8, 16:11:11 UTC
*                           Clock    TS (in seconds)                                 =  339869471
*                           External start year                                      =       1970
*                           External end   year                                      =       2106
*                           Leap day count between External & Clock epoch start year =          7
*                           External TZ (in seconds)                                 =     -18000
*                           External TS (in seconds)                                 = 1286536271
*                             2010 Oct 8, 11:11:11 UTC-05:00
*
*                           This example successfully converts an External timestamp into a
*                           representable Clock timestamp without overflowing.
*
*                       (2) Invalid equivalent date to convert is after  External epoch end year :
*
*                             2120 Oct 8, 11:11:11 UTC
*                           Clock    TS (in seconds)                                 = 3811144271
*                           External start year                                      =       1970
*                           External end   year                                      =       2106
*                           Leap day count between External & Clock epoch start year =          7
*                           External TZ (in seconds)                                 =     -18000
*                           External TS (in seconds)                                 = 4757811071
*
*                           This example overflows the External (32-bit) timestamp with an equivalent
*                           date incorrectly greater than or equal to the External epoch end year.
*
*
*                   (b) When External epoch start year is greater than Clock epoch start year
*                       ('CLK_EPOCH_YR_START') :
*
*                       External TS = Clock TS
*                                   - [(((External start year - Clock start year) * 365) + leap day count)
*                                       * seconds per day]
*                                   + External TZ
*
*
*                       Examples with a 32-bit External timestamp :
*
*                       (1)   Valid equivalent date to convert is after  External epoch start year :
*
*                             2010 Oct 8, 16:11:11 UTC
*                           Clock    TS (in seconds)                                 =  339869471
*                           External start year                                      =       2010
*                           Leap day count between External & Clock epoch start year =          3
*                           External TZ (in seconds)                                 =     -18000
*                           External TS (in seconds)                                 =   24232271
*                             2010 Oct 8, 11:11:11 UTC-05:00
*
*                           This example successfully converts an External timestamp into a
*                           representable Clock timestamp without underflowing.
*
*                       (2) Invalid equivalent date to convert is before External epoch start year :
*
*                             2005 Oct 8, 11:11:11 UTC
*                           Clock    TS (in seconds)                                 =  182085071
*                           External start year                                      =       2010
*                           Leap day count between External & Clock epoch start year =          3
*                           External TZ (in seconds)                                 =     -18000
*                           External TS (in seconds)                                 = -133552129
*
*                           This example underflows to a negative External timestamp since the
*                           equivalent date to convert is incorrectly less than the External
*                           epoch start year.
*
*
*                   (c) where
*
*                       (1) Clock    TS             Clock timestamp (in seconds, from UTC+00)
*                       (2) External TS             Converted External timestamp (in seconds)
*                       (3) Clock    start year     Clock epoch start year ('CLK_EPOCH_YR_START')
*                       (4) External start year     External timestamp epoch start year
*                       (5) External end   year     External timestamp epoch end   year
*                       (6) Leap day count          Number of leap days between Clock epoch
*                                                       start year & External epoch start year
*                       (7) Seconds per day         Number of seconds per day (86400)
*                       (8) External TZ             Time zone offset applied to External TS
*                                                       (in seconds, from UTC+00)
*********************************************************************************************************
*/

#if (CLK_CFG_EXT_EN == DEF_ENABLED)
CPU_BOOLEAN  Clk_ExtTS_Set (CLK_TS_SEC  ts_sec)
{
    return (DEF_FAIL);
}
#endif


/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of uC/Clk module include.       */
