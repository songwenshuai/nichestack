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
*                                              STM32H7x3
*
* Filename : bsp_int.h
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP present pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  BSP_INT_PRESENT
#define  BSP_INT_PRESENT


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                     EXTERNAL C LANGUAGE LINKAGE
*
* Note(s) : (1) C++ compilers MUST 'extern'ally declare ALL C function prototypes & variable/object
*               declarations for correct C language linkage.
*********************************************************************************************************
*/

#ifdef __cplusplus
extern  "C" {                                                   /* See Note #1.                                         */
#endif


/*
*********************************************************************************************************
*                                               DEFINES
*
* Note(s) : (1) The Cortex-M "Vector Table Offset Register" section states the following:
*
*               You must align the offset to the number of exception entries in the vector table. The
*               minimum alignment is 32 words, enough for up to 16 interrupts. For more interrupts,
*               adjust the alignment by rounding up to the next power of two. For example, if you require
*               21 interrupts, the alignment must be on a 64-word boundary because the required table
*               size is 37 words, and the next power of two is 64. SEE YOUR VENDOR DOCUMENTATION FOR THE
*               ALIGNMENT DETAILS FOR YOUR DEVICE.
*********************************************************************************************************
*/

#define  ARMV7M_CORE_EXCS             16u

#define  INT_ID_MAX_NBR              150u                       /* Max. number of ext. interrupt sources. (Check MCU RM)*/

                                                                /* 166 VTOR entries; next power of 2 is 256             */
#define  INT_VTOR_TBL_SIZE         (INT_ID_MAX_NBR + ARMV7M_CORE_EXCS)
#define  INT_VTOR_TBL_ALIGNMENT    (0x400uL)                    /* 256 * 4 = 1024 words. See note 1                     */


/*
*********************************************************************************************************
*                                              DATA TYPES
*********************************************************************************************************
*/
                                                                /* ------------- STM32H7x3 Specific Intr. ------------- */
typedef  enum  bsp_int_id {
    INT_ID_WWDG               = 0u,                             /* Window WatchDog Interrupt                            */
    INT_ID_PVD_AVD            = 1u,                             /* PVD/AVD through EXTI Line detection Interrupt        */
    INT_ID_TAMP_STAMP         = 2u,                             /* Tamper and TimeStamp intr. through the EXTI line     */
    INT_ID_RTC_WKUP           = 3u,                             /* RTC Wakeup interrupt through the EXTI line           */
    INT_ID_FLASH              = 4u,                             /* FLASH global Interrupt                               */
    INT_ID_RCC                = 5u,                             /* RCC global Interrupt                                 */
    INT_ID_EXTI0              = 6u,                             /* EXTI Line0 Interrupt                                 */
    INT_ID_EXTI1              = 7u,                             /* EXTI Line1 Interrupt                                 */
    INT_ID_EXTI2              = 8u,                             /* EXTI Line2 Interrupt                                 */
    INT_ID_EXTI3              = 9u,                             /* EXTI Line3 Interrupt                                 */
    INT_ID_EXTI4              = 10u,                            /* EXTI Line4 Interrupt                                 */
    INT_ID_DMA1_Stream0       = 11u,                            /* DMA1 Stream 0 global Interrupt                       */
    INT_ID_DMA1_Stream1       = 12u,                            /* DMA1 Stream 1 global Interrupt                       */
    INT_ID_DMA1_Stream2       = 13u,                            /* DMA1 Stream 2 global Interrupt                       */
    INT_ID_DMA1_Stream3       = 14u,                            /* DMA1 Stream 3 global Interrupt                       */
    INT_ID_DMA1_Stream4       = 15u,                            /* DMA1 Stream 4 global Interrupt                       */
    INT_ID_DMA1_Stream5       = 16u,                            /* DMA1 Stream 5 global Interrupt                       */
    INT_ID_DMA1_Stream6       = 17u,                            /* DMA1 Stream 6 global Interrupt                       */
    INT_ID_ADC                = 18u,                            /* ADC1 and  ADC2 global Interrupts                     */
    INT_ID_FDCAN1_IT0         = 19u,                            /* FDCAN1 Interrupt line 0                              */
    INT_ID_FDCAN2_IT0         = 20u,                            /* FDCAN2 Interrupt line 0                              */
    INT_ID_FDCAN1_IT1         = 21u,                            /* FDCAN1 Interrupt line 1                              */
    INT_ID_FDCAN2_IT1         = 22u,                            /* FDCAN2 Interrupt line 1                              */
    INT_ID_EXTI9_5            = 23u,                            /* External Line[9:5] Interrupts                        */
    INT_ID_TIM1_BRK           = 24u,                            /* TIM1 Break Interrupt                                 */
    INT_ID_TIM1_UP            = 25u,                            /* TIM1 Update Interrupt                                */
    INT_ID_TIM1_TRG_COM       = 26u,                            /* TIM1 Trigger and Commutation Interrupt               */
    INT_ID_TIM1_CC            = 27u,                            /* TIM1 Capture Compare Interrupt                       */
    INT_ID_TIM2               = 28u,                            /* TIM2 global Interrupt                                */
    INT_ID_TIM3               = 29u,                            /* TIM3 global Interrupt                                */
    INT_ID_TIM4               = 30u,                            /* TIM4 global Interrupt                                */
    INT_ID_I2C1_EV            = 31u,                            /* I2C1 Event Interrupt                                 */
    INT_ID_I2C1_ER            = 32u,                            /* I2C1 Error Interrupt                                 */
    INT_ID_I2C2_EV            = 33u,                            /* I2C2 Event Interrupt                                 */
    INT_ID_I2C2_ER            = 34u,                            /* I2C2 Error Interrupt                                 */
    INT_ID_SPI1               = 35u,                            /* SPI1 global Interrupt                                */
    INT_ID_SPI2               = 36u,                            /* SPI2 global Interrupt                                */
    INT_ID_USART1             = 37u,                            /* USART1 global Interrupt                              */
    INT_ID_USART2             = 38u,                            /* USART2 global Interrupt                              */
    INT_ID_USART3             = 39u,                            /* USART3 global Interrupt                              */
    INT_ID_EXTI15_10          = 40u,                            /* External Line[15:10] Interrupts                      */
    INT_ID_RTC_Alarm          = 41u,                            /* RTC Alarm (A and B) through EXTI Line Interrupt      */
    INT_ID_TIM8_BRK_TIM12     = 43u,                            /* TIM8 Break Interrupt and TIM12 global interrupt      */
    INT_ID_TIM8_UP_TIM13      = 44u,                            /* TIM8 Update Interrupt & TIM13 global interrupt       */
    INT_ID_TIM8_TRG_COM_TIM14 = 45u,                            /* TIM8 Trigger & Comm Intr and TIM14 global intr       */
    INT_ID_TIM8_CC            = 46u,                            /* TIM8 Capture Compare Interrupt                       */
    INT_ID_DMA1_Stream7       = 47u,                            /* DMA1 Stream7 Interrupt                               */
    INT_ID_FMC                = 48u,                            /* FMC global Interrupt                                 */
    INT_ID_SDMMC1             = 49u,                            /* SDMMC1 global Interrupt                              */
    INT_ID_TIM5               = 50u,                            /* TIM5 global Interrupt                                */
    INT_ID_SPI3               = 51u,                            /* SPI3 global Interrupt                                */
    INT_ID_UART4              = 52u,                            /* UART4 global Interrupt                               */
    INT_ID_UART5              = 53u,                            /* UART5 global Interrupt                               */
    INT_ID_TIM6_DAC           = 54u,                            /* TIM6 global and DAC1&2 underrun error interrupts     */
    INT_ID_TIM7               = 55u,                            /* TIM7 global interrupt                                */
    INT_ID_DMA2_Stream0       = 56u,                            /* DMA2 Stream 0 global Interrupt                       */
    INT_ID_DMA2_Stream1       = 57u,                            /* DMA2 Stream 1 global Interrupt                       */
    INT_ID_DMA2_Stream2       = 58u,                            /* DMA2 Stream 2 global Interrupt                       */
    INT_ID_DMA2_Stream3       = 59u,                            /* DMA2 Stream 3 global Interrupt                       */
    INT_ID_DMA2_Stream4       = 60u,                            /* DMA2 Stream 4 global Interrupt                       */
    INT_ID_ETH                = 61u,                            /* Ethernet global Interrupt                            */
    INT_ID_ETH_WKUP           = 62u,                            /* Ethernet Wakeup through EXTI line Interrupt          */
    INT_ID_FDCAN_CAL          = 63u,                            /* FDCAN Calibration unit Interrupt                     */
    INT_ID_RSVD0              = 64u,
    INT_ID_RSVD1              = 65u,
    INT_ID_RSVD2              = 66u,
    INT_ID_RSVD3              = 67u,
    INT_ID_DMA2_Stream5       = 68u,                            /* DMA2 Stream 5 global interrupt                       */
    INT_ID_DMA2_Stream6       = 69u,                            /* DMA2 Stream 6 global interrupt                       */
    INT_ID_DMA2_Stream7       = 70u,                            /* DMA2 Stream 7 global interrupt                       */
    INT_ID_USART6             = 71u,                            /* USART6 global interrupt                              */
    INT_ID_I2C3_EV            = 72u,                            /* I2C3 event interrupt                                 */
    INT_ID_I2C3_ER            = 73u,                            /* I2C3 error interrupt                                 */
    INT_ID_OTG_HS_EP1_OUT     = 74u,                            /* USB OTG HS End Point 1 Out global interrupt          */
    INT_ID_OTG_HS_EP1_IN      = 75u,                            /* USB OTG HS End Point 1 In global interrupt           */
    INT_ID_OTG_HS_WKUP        = 76u,                            /* USB OTG HS Wakeup through EXTI interrupt             */
    INT_ID_OTG_HS             = 77u,                            /* USB OTG HS global interrupt                          */
    INT_ID_DCMI               = 78u,                            /* DCMI global interrupt                                */
    INT_ID_RNG                = 80u,                            /* RNG global interrupt                                 */
    INT_ID_FPU                = 81u,                            /* FPU global interrupt                                 */
    INT_ID_UART7              = 82u,                            /* UART7 global interrupt                               */
    INT_ID_UART8              = 83u,                            /* UART8 global interrupt                               */
    INT_ID_SPI4               = 84u,                            /* SPI4 global Interrupt                                */
    INT_ID_SPI5               = 85u,                            /* SPI5 global Interrupt                                */
    INT_ID_SPI6               = 86u,                            /* SPI6 global Interrupt                                */
    INT_ID_SAI1               = 87u,                            /* SAI1 global Interrupt                                */
    INT_ID_LTDC               = 88u,                            /* LTDC global Interrupt                                */
    INT_ID_LTDC_ER            = 89u,                            /* LTDC Error global Interrupt                          */
    INT_ID_DMA2D              = 90u,                            /* DMA2D global Interrupt                               */
    INT_ID_SAI2               = 91u,                            /* SAI2 global Interrupt                                */
    INT_ID_QUADSPI            = 92u,                            /* Quad SPI global interrupt                            */
    INT_ID_LPTIM1             = 93u,                            /* LP TIM1 interrupt                                    */
    INT_ID_CEC                = 94u,                            /* HDMI-CEC global Interrupt                            */
    INT_ID_I2C4_EV            = 95u,                            /* I2C4 Event Interrupt                                 */
    INT_ID_I2C4_ER            = 96u,                            /* I2C4 Error Interrupt                                 */
    INT_ID_SPDIF_RX           = 97u,                            /* SPDIF-RX global Interrupt                            */
    INT_ID_OTG_FS_EP1_OUT     = 98u,                            /* USB OTG HS2 global interrupt                         */
    INT_ID_OTG_FS_EP1_IN      = 99u,                            /* USB OTG HS2 End Point 1 Out global interrupt         */
    INT_ID_OTG_FS_WKUP        = 100u,                           /* USB OTG HS2 End Point 1 In global interrupt          */
    INT_ID_OTG_FS             = 101u,                           /* USB OTG HS2 Wakeup through EXTI interrupt            */
    INT_ID_DMAMUX1_OVR	      = 102u,                           /* DMAMUX1 Overrun interrupt                            */
    INT_ID_HRTIM1_Master      = 103u,                           /* HRTIM Master Timer global Interrupts                 */
    INT_ID_HRTIM1_TIMA        = 104u,                           /* HRTIM Timer A global Interrupt                       */
    INT_ID_HRTIM1_TIMB        = 105u,                           /* HRTIM Timer B global Interrupt                       */
    INT_ID_HRTIM1_TIMC        = 106u,                           /* HRTIM Timer C global Interrupt                       */
    INT_ID_HRTIM1_TIMD        = 107u,                           /* HRTIM Timer D global Interrupt                       */
    INT_ID_HRTIM1_TIME        = 108u,                           /* HRTIM Timer E global Interrupt                       */
    INT_ID_HRTIM1_FLT         = 109u,                           /* HRTIM Fault global Interrupt                         */
    INT_ID_DFSDM1_FLT0        = 110u,                           /* DFSDM Filter1 Interrupt                              */
    INT_ID_DFSDM1_FLT1        = 111u,                           /* DFSDM Filter2 Interrupt                              */
    INT_ID_DFSDM1_FLT2        = 112u,                           /* DFSDM Filter3 Interrupt                              */
    INT_ID_DFSDM1_FLT3        = 113u,                           /* DFSDM Filter4 Interrupt                              */
    INT_ID_SAI3               = 114u,                           /* SAI3 global Interrupt                                */
    INT_ID_SWPMI1             = 115u,                           /* Serial Wire Interface 1 global interrupt             */
    INT_ID_TIM15              = 116u,                           /* TIM15 global Interrupt                               */
    INT_ID_TIM16              = 117u,                           /* TIM16 global Interrupt                               */
    INT_ID_TIM17              = 118u,                           /* TIM17 global Interrupt                               */
    INT_ID_MDIOS_WKUP         = 119u,                           /* MDIOS Wakeup  Interrupt                              */
    INT_ID_MDIOS              = 120u,                           /* MDIOS global Interrupt                               */
    INT_ID_JPEG               = 121u,                           /* JPEG global Interrupt                                */
    INT_ID_MDMA               = 122u,                           /* MDMA global Interrupt                                */
    INT_ID_SDMMC2             = 124u,                           /* SDMMC2 global Interrupt                              */
    INT_ID_HSEM1              = 125u,                           /* HSEM1 global Interrupt                               */
    INT_ID_RSVD4              = 126u,
    INT_ID_ADC3               = 127u,                           /* ADC3 global Interrupt                                */
    INT_ID_DMAMUX2_OVR        = 128u,                           /* DMAMUX2 Overrun interrupt                            */
    INT_ID_BDMA_Channel0      = 129u,                           /* BDMA Channel 0 global Interrupt                      */
    INT_ID_BDMA_Channel1      = 130u,                           /* BDMA Channel 1 global Interrupt                      */
    INT_ID_BDMA_Channel2      = 131u,                           /* BDMA Channel 2 global Interrupt                      */
    INT_ID_BDMA_Channel3      = 132u,                           /* BDMA Channel 3 global Interrupt                      */
    INT_ID_BDMA_Channel4      = 133u,                           /* BDMA Channel 4 global Interrupt                      */
    INT_ID_BDMA_Channel5      = 134u,                           /* BDMA Channel 5 global Interrupt                      */
    INT_ID_BDMA_Channel6      = 135u,                           /* BDMA Channel 6 global Interrupt                      */
    INT_ID_BDMA_Channel7      = 136u,                           /* BDMA Channel 7 global Interrupt                      */
    INT_ID_COMP               = 137u,                           /* COMP global Interrupt                                */
    INT_ID_LPTIM2             = 138u,                           /* LP TIM2 global interrupt                             */
    INT_ID_LPTIM3             = 139u,                           /* LP TIM3 global interrupt                             */
    INT_ID_LPTIM4             = 140u,                           /* LP TIM4 global interrupt                             */
    INT_ID_LPTIM5             = 141u,                           /* LP TIM5 global interrupt                             */
    INT_ID_LPUART1            = 142u,                           /* LP UART1 interrupt                                   */
    INT_ID_CRS                = 144u,                           /* Clock Recovery Global Interrupt                      */
    INT_ID_SAI4               = 146u,                           /* SAI4 global interrupt                                */
    INT_ID_WAKEUP_PIN         = 149u,                           /* Interrupt for all 6 wake-up pins                     */
} BSP_INT_ID;


typedef enum bsp_int_type {                                     /* Types of Interrupt.                                  */
    INT_IRQ,                                                    /* Normal interrupt request.                            */
} BSP_INT_TYPE;


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void  BSP_IntInit   (void);

void  BSP_IntEnable (BSP_INT_ID     int_id);

void  BSP_IntDisable(BSP_INT_ID     int_id);

void  BSP_IntClear  (BSP_INT_ID     int_id);

void  BSP_IntVectSet(BSP_INT_ID     int_id,
                     CPU_INT08U     int_prio,
                     BSP_INT_TYPE   int_type,
                     CPU_FNCT_VOID  isr_handler);


/*
*********************************************************************************************************
*                                   EXTERNAL C LANGUAGE LINKAGE END
*********************************************************************************************************
*/

#ifdef __cplusplus
}                                                               /* End of 'extern'al C lang linkage.                    */
#endif


/*
*********************************************************************************************************
*                                              MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
