/*
 * ARM Limited (ARM) is supplying this software for use with Cortex-M
 * processor based microcontroller, but can be equally used for other
 * suitable processor architectures. This file can be freely distributed.
 * Modifications to this file shall be clearly marked.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * @file     irq.h
 * @brief    CMSIS HeaderFile
 * @version  1.2
 * @date     02. June 2017
 * @note     Generated by SVDConv V3.3.9 on Friday, 02.06.2017 20:26:27
 *           from File 'Cleveland2.svd',
 *           last modified on Friday, 02.06.2017 12:25:41
 */



/** @addtogroup Mstarsemi
  * @{
  */


/** @addtogroup Murphy
  * @{
  */


#ifndef IRQ_H
#define IRQ_H

#ifdef __cplusplus
extern "C" {
#endif


/** @addtogroup Configuration_of_CMSIS
  * @{
  */



/* =========================================================================================================================== */
/* ================                                Interrupt Number Definition                                ================ */
/* =========================================================================================================================== */

typedef enum {
/* =======================================  ARM Cortex-M4 Specific Interrupt Numbers  ======================================== */
  Reset_IRQn                = -15,              /*!< -15  Reset Vector, invoked on Power up and warm reset                     */
  NonMaskableInt_IRQn       = -14,              /*!< -14  Non maskable Interrupt, cannot be stopped or preempted               */
  HardFault_IRQn            = -13,              /*!< -13  Hard Fault, all classes of Fault                                     */
  MemoryManagement_IRQn     = -12,              /*!< -12  Memory Management, MPU mismatch, including Access Violation
                                                     and No Match                                                              */
  BusFault_IRQn             = -11,              /*!< -11  Bus Fault, Pre-Fetch-, Memory Access Fault, other address/memory
                                                     related Fault                                                             */
  UsageFault_IRQn           = -10,              /*!< -10  Usage Fault, i.e. Undef Instruction, Illegal State Transition        */
  SVCall_IRQn               =  -5,              /*!< -5 System Service Call via SVC instruction                                */
  DebugMonitor_IRQn         =  -4,              /*!< -4 Debug Monitor                                                          */
  PendSV_IRQn               =  -2,              /*!< -2 Pendable request for system service                                    */
  SysTick_IRQn              =  -1,              /*!< -1 System Tick Timer                                                      */
/* =========================================  Cleveland Specific Interrupt Numbers  ========================================== */
  UART0_IRQn                =  24,              /*!< 16 PM UART                                                                 */
  VBDMA_IRQn                =  39,              /*!< 23 V_BDMA                                                                  */
  AUDIO_IRQn                =  2,               /*!< 38 Audio                                                                   */
  NOPMIRQ_IRQn              =  0,
  NOPMFIQ_IRQn              =  1,
  MAX_IRQn                  =  239,
  PMToCP_IRQn               =  239+64+36+1,   //reg_hst0to3_int//noPM FIQ 36
  HK0ToCP_IRQn              =  239+64+40+1,   //reg_hst1to3_int//noPM FIQ 40
  HK1ToCP_IRQn              =  239+64+44+1,   //reg_hst2to3_int//noPM FIQ 44
  CPToHK0_IRQn              =  239+64+49+1,   //reg_hst3to1_int//noPM FIQ 49
  
  NONE_IRQn                 = 0xFFFF,
  AUDIO_VAD_IRQn            = NONE_IRQn,
  UART1_IRQn                = NONE_IRQn,
  UART2_IRQn                = NONE_IRQn,

} IRQn_Type;



/* =========================================================================================================================== */
/* ================                           Processor and Core Peripheral Section                           ================ */
/* =========================================================================================================================== */

/* ===========================  Configuration of the ARM Cortex-M4 Processor and Core Peripherals  =========================== */
#define __CM4_REV                 0x0100U       /*!< CM4 Core Revision                                                         */
#define __NVIC_PRIO_BITS               3        /*!< Number of Bits used for Priority Levels                                   */
#define __Vendor_SysTickConfig         0        /*!< Set to 1 if different SysTick Config is used                              */
#define __MPU_PRESENT                  1        /*!< MPU present or not                                                        */
#define __FPU_PRESENT                  1        /*!< FPU present or not                                                        */


/** @} */ /* End of group Configuration_of_CMSIS */

#include "core_cm4.h"                           /*!< ARM Cortex-M4 processor and core peripherals                              */
//#include "system_Cleveland.h"                   /*!< Cleveland System                                                          */



#ifdef __cplusplus
}
#endif

#endif /* IRQ_H */


/** @} */ /* End of group Murphy */

/** @} */ /* End of group Mstarsemi */