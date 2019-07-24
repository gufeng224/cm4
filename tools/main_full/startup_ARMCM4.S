/* File: startup_ARMCM4.S
 * Purpose: startup file for Cortex-M4 devices. Should use with
 *   GCC for ARM Embedded Processors
 * Version: V2.0
 * Date: 16 August 2013
 *
/* Copyright (c) 2011 - 2013 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/
	.syntax	unified
	.arch	armv7e-m

	.extern __SRAM_segment_end__

	.section .isr_vector,"a",%progbits
	.align	4
	.globl	__isr_vector
	.global __Vectors

__Vectors:
__isr_vector:
	.long	__SRAM_segment_end__ - 4  /* Top of Stack at top of RAM*/
	.long	Reset_Handler         /* Reset Handler */
	.long	NMI_Handler           /* NMI Handler */
	.long	HardFault_Handler     /* Hard Fault Handler */
	.long	MemManage_Handler     /* MPU Fault Handler */
	.long	BusFault_Handler      /* Bus Fault Handler */
	.long	UsageFault_Handler    /* Usage Fault Handler */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	SVC_Handler           /* SVCall Handler */
	.long	DebugMon_Handler      /* Debug Monitor Handler */
	.long	0                     /* Reserved */
	.long	PendSV_Handler        /* PendSV Handler */
	.long	SysTick_Handler       /* SysTick Handler */

	/* External interrupts */
	.long     NVIC_Handler_WDT
	.long     NVIC_Handler_001
	.long     NVIC_Handler_002
	.long     NVIC_Handler_003
	.long     NVIC_Handler_004
	.long     NVIC_Handler_005
	.long     NVIC_Handler_006
	.long     NVIC_Handler_007
	.long     NVIC_Handler_008
	.long     NVIC_Handler_009
	.long     NVIC_Handler_010
	.long     NVIC_Handler_011
	.long     NVIC_Handler_012
	.long     NVIC_Handler_013
	.long     NVIC_Handler_014
	.long     NVIC_Handler_015
	.long     NVIC_Handler_016
	.long     NVIC_Handler_017
	.long     NVIC_Handler_018
	.long     NVIC_Handler_019
	.long     NVIC_Handler_020
	.long     NVIC_Handler_021
	.long     NVIC_Handler_022
	.long     NVIC_Handler_023
	.long     NVIC_Handler_024
	.long     NVIC_Handler_025
	.long     NVIC_Handler_026
	.long     NVIC_Handler_027
	.long     NVIC_Handler_028
	.long     NVIC_Handler_029
	.long     NVIC_Handler_030
	.long     NVIC_Handler_031
	.long     NVIC_Handler_032
	.long     NVIC_Handler_033
	.long     NVIC_Handler_034
	.long     NVIC_Handler_035
	.long     NVIC_Handler_036
	.long     NVIC_Handler_037
	.long     NVIC_Handler_038
	.long     NVIC_Handler_039
	.long     NVIC_Handler_040
	.long     NVIC_Handler_041
	.long     NVIC_Handler_042
	.long     NVIC_Handler_043
	.long     NVIC_Handler_044
	.long     NVIC_Handler_045
	.long     NVIC_Handler_046
	.long     NVIC_Handler_047
	.long     NVIC_Handler_048
	.long     NVIC_Handler_049
	.long     NVIC_Handler_050
	.long     NVIC_Handler_051
	.long     NVIC_Handler_052
	.long     NVIC_Handler_053
	.long     NVIC_Handler_054
	.long     NVIC_Handler_055
	.long     NVIC_Handler_056
	.long     NVIC_Handler_057
	.long     NVIC_Handler_058
	.long     NVIC_Handler_059
	.long     NVIC_Handler_060
	.long     NVIC_Handler_061
	.long     NVIC_Handler_062
	.long     NVIC_Handler_063
	.long     NVIC_Handler_064
	.long     NVIC_Handler_065
	.long     NVIC_Handler_066
	.long     NVIC_Handler_067
	.long     NVIC_Handler_068
	.long     NVIC_Handler_069
	.long     NVIC_Handler_070
	.long     NVIC_Handler_071
	.long     NVIC_Handler_072
	.long     NVIC_Handler_073
	.long     NVIC_Handler_074
	.long     NVIC_Handler_075
	.long     NVIC_Handler_076
	.long     NVIC_Handler_077
	.long     NVIC_Handler_078
	.long     NVIC_Handler_079
	.long     NVIC_Handler_080
	.long     NVIC_Handler_081
	.long     NVIC_Handler_082
	.long     NVIC_Handler_083
	.long     NVIC_Handler_084
	.long     NVIC_Handler_085
	.long     NVIC_Handler_086
	.long     NVIC_Handler_087
	.long     NVIC_Handler_088
	.long     NVIC_Handler_089
	.long     NVIC_Handler_090
	.long     NVIC_Handler_091
	.long     NVIC_Handler_092
	.long     NVIC_Handler_093
	.long     NVIC_Handler_094
	.long     NVIC_Handler_095
	.long     NVIC_Handler_096
	.long     NVIC_Handler_097
	.long     NVIC_Handler_098
	.long     NVIC_Handler_099
	.long     NVIC_Handler_100
	.long     NVIC_Handler_101
	.long     NVIC_Handler_102
	.long     NVIC_Handler_103
	.long     NVIC_Handler_104
	.long     NVIC_Handler_105
	.long     NVIC_Handler_106
	.long     NVIC_Handler_107
	.long     NVIC_Handler_108
	.long     NVIC_Handler_109
	.long     NVIC_Handler_110
	.long     NVIC_Handler_111
	.long     NVIC_Handler_112
	.long     NVIC_Handler_113
	.long     NVIC_Handler_114
	.long     NVIC_Handler_115
	.long     NVIC_Handler_116
	.long     NVIC_Handler_117
	.long     NVIC_Handler_118
	.long     NVIC_Handler_119
	.long     NVIC_Handler_120
	.long     NVIC_Handler_121
	.long     NVIC_Handler_122
	.long     NVIC_Handler_123
	.long     NVIC_Handler_124
	.long     NVIC_Handler_125
	.long     NVIC_Handler_126
	.long     NVIC_Handler_127
	.long     NVIC_Handler_128
	.long     NVIC_Handler_129
	.long     NVIC_Handler_130

/* ToDo:  Add here the vectors for the device specific external interrupts handler */
	//.long	<DeviceInterrupt>_IRQHandler              /*  0: Default                  */

	.size	__Vectors, . - __Vectors

	.text
	.thumb
	.thumb_func
	.align	2
	.globl _start
	.extern main
	.globl	Reset_Handler
	.type	Reset_Handler, %function
_start:
Reset_Handler:
/*  Firstly it copies data from read only memory to RAM. There are two schemes
 *  to copy. One can copy more than one sections. Another can only copy
 *  one section.  The former scheme needs more instructions and read-only
 *  data to implement than the latter.
 *  Macro __STARTUP_COPY_MULTIPLE is used to choose between two schemes.  */

/*  Single section scheme.
 *
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
	ldr sp, =__SRAM_segment_end__
	sub sp, sp, #4

/*
	ldr	r1, =__etext
	ldr	r2, =__data_start__
	ldr	r3, =__data_end__

.L_loop1:
	cmp	r2, r3
	ittt	lt
	ldrlt	r0, [r1], #4
	strlt	r0, [r2], #4
	blt	.L_loop1
*/

/*  This part of work usually is done in C library startup code. Otherwise,
 *  define this macro to enable it in this startup.
 *
 *  There are two schemes too. One can clear multiple BSS sections. Another
 *  can only clear one section. The former is more size expensive than the
 *  latter.
 *
 *  Define macro __STARTUP_CLEAR_BSS_MULTIPLE to choose the former.
 *  Otherwise efine macro __STARTUP_CLEAR_BSS to choose the later.
 */

 /*  Single BSS section scheme.
 *
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
	ldr	r1, =__bss_start__
	ldr	r2, =__bss_end__

	movs	r0, 0
.L_loop3:
	cmp	r1, r2
	itt	lt
	strlt	r0, [r1], #4
	blt	.L_loop3

#ifndef __NO_SYSTEM_INIT
/*	bl	SystemInit */
#endif

	bl	main

	.pool
	.size	Reset_Handler, . - Reset_Handler

	.align	1
	.thumb_func
	.weak	Default_Handler
	.type	Default_Handler, %function
Default_Handler:
	b	.
	.size	Default_Handler, . - Default_Handler

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
	.macro	def_irq_handler	handler_name
	.weak	\handler_name
	.set	\handler_name, Default_Handler
	.endm

	def_irq_handler	NMI_Handler
	def_irq_handler	HardFault_Handler
	def_irq_handler	MemManage_Handler
	def_irq_handler	BusFault_Handler
	def_irq_handler	UsageFault_Handler
/*	def_irq_handler	SVC_Handler */
	def_irq_handler	DebugMon_Handler
/*	def_irq_handler	PendSV_Handler */
/*	def_irq_handler	SysTick_Handler */
	def_irq_handler	DEF_IRQHandler

	def_irq_handler     NVIC_Handler_WDT
	def_irq_handler     NVIC_Handler_001
	def_irq_handler     NVIC_Handler_002
	def_irq_handler     NVIC_Handler_003
	def_irq_handler     NVIC_Handler_004
	def_irq_handler     NVIC_Handler_005
	def_irq_handler     NVIC_Handler_006
	def_irq_handler     NVIC_Handler_007
	def_irq_handler     NVIC_Handler_008
	def_irq_handler     NVIC_Handler_009
	def_irq_handler     NVIC_Handler_010
	def_irq_handler     NVIC_Handler_011
	def_irq_handler     NVIC_Handler_012
	def_irq_handler     NVIC_Handler_013
	def_irq_handler     NVIC_Handler_014
	def_irq_handler     NVIC_Handler_015
	def_irq_handler     NVIC_Handler_016
	def_irq_handler     NVIC_Handler_017
	def_irq_handler     NVIC_Handler_018
	def_irq_handler     NVIC_Handler_019
	def_irq_handler     NVIC_Handler_020
	def_irq_handler     NVIC_Handler_021
	def_irq_handler     NVIC_Handler_022
	def_irq_handler     NVIC_Handler_023
	def_irq_handler     NVIC_Handler_024
	def_irq_handler     NVIC_Handler_025
	def_irq_handler     NVIC_Handler_026
	def_irq_handler     NVIC_Handler_027
	def_irq_handler     NVIC_Handler_028
	def_irq_handler     NVIC_Handler_029
	def_irq_handler     NVIC_Handler_030
	def_irq_handler     NVIC_Handler_031
	def_irq_handler     NVIC_Handler_032
	def_irq_handler     NVIC_Handler_033
	def_irq_handler     NVIC_Handler_034
	def_irq_handler     NVIC_Handler_035
	def_irq_handler     NVIC_Handler_036
	def_irq_handler     NVIC_Handler_037
	def_irq_handler     NVIC_Handler_038
	def_irq_handler     NVIC_Handler_039
	def_irq_handler     NVIC_Handler_040
	def_irq_handler     NVIC_Handler_041
	def_irq_handler     NVIC_Handler_042
	def_irq_handler     NVIC_Handler_043
	def_irq_handler     NVIC_Handler_044
	def_irq_handler     NVIC_Handler_045
	def_irq_handler     NVIC_Handler_046
	def_irq_handler     NVIC_Handler_047
	def_irq_handler     NVIC_Handler_048
	def_irq_handler     NVIC_Handler_049
	def_irq_handler     NVIC_Handler_050
	def_irq_handler     NVIC_Handler_051
	def_irq_handler     NVIC_Handler_052
	def_irq_handler     NVIC_Handler_053
	def_irq_handler     NVIC_Handler_054
	def_irq_handler     NVIC_Handler_055
	def_irq_handler     NVIC_Handler_056
	def_irq_handler     NVIC_Handler_057
	def_irq_handler     NVIC_Handler_058
	def_irq_handler     NVIC_Handler_059
	def_irq_handler     NVIC_Handler_060
	def_irq_handler     NVIC_Handler_061
	def_irq_handler     NVIC_Handler_062
	def_irq_handler     NVIC_Handler_063
	def_irq_handler     NVIC_Handler_064
	def_irq_handler     NVIC_Handler_065
	def_irq_handler     NVIC_Handler_066
	def_irq_handler     NVIC_Handler_067
	def_irq_handler     NVIC_Handler_068
	def_irq_handler     NVIC_Handler_069
	def_irq_handler     NVIC_Handler_070
	def_irq_handler     NVIC_Handler_071
	def_irq_handler     NVIC_Handler_072
	def_irq_handler     NVIC_Handler_073
	def_irq_handler     NVIC_Handler_074
	def_irq_handler     NVIC_Handler_075
	def_irq_handler     NVIC_Handler_076
	def_irq_handler     NVIC_Handler_077
	def_irq_handler     NVIC_Handler_078
	def_irq_handler     NVIC_Handler_079
	def_irq_handler     NVIC_Handler_080
	def_irq_handler     NVIC_Handler_081
	def_irq_handler     NVIC_Handler_082
	def_irq_handler     NVIC_Handler_083
	def_irq_handler     NVIC_Handler_084
	def_irq_handler     NVIC_Handler_085
	def_irq_handler     NVIC_Handler_086
	def_irq_handler     NVIC_Handler_087
	def_irq_handler     NVIC_Handler_088
	def_irq_handler     NVIC_Handler_089
	def_irq_handler     NVIC_Handler_090
	def_irq_handler     NVIC_Handler_091
	def_irq_handler     NVIC_Handler_092
	def_irq_handler     NVIC_Handler_093
	def_irq_handler     NVIC_Handler_094
	def_irq_handler     NVIC_Handler_095
	def_irq_handler     NVIC_Handler_096
	def_irq_handler     NVIC_Handler_097
	def_irq_handler     NVIC_Handler_098
	def_irq_handler     NVIC_Handler_099
	def_irq_handler     NVIC_Handler_100
	def_irq_handler     NVIC_Handler_101
	def_irq_handler     NVIC_Handler_102
	def_irq_handler     NVIC_Handler_103
	def_irq_handler     NVIC_Handler_104
	def_irq_handler     NVIC_Handler_105
	def_irq_handler     NVIC_Handler_106
	def_irq_handler     NVIC_Handler_107
	def_irq_handler     NVIC_Handler_108
	def_irq_handler     NVIC_Handler_109
	def_irq_handler     NVIC_Handler_110
	def_irq_handler     NVIC_Handler_111
	def_irq_handler     NVIC_Handler_112
	def_irq_handler     NVIC_Handler_113
	def_irq_handler     NVIC_Handler_114
	def_irq_handler     NVIC_Handler_115
	def_irq_handler     NVIC_Handler_116
	def_irq_handler     NVIC_Handler_117
	def_irq_handler     NVIC_Handler_118
	def_irq_handler     NVIC_Handler_119
	def_irq_handler     NVIC_Handler_120
	def_irq_handler     NVIC_Handler_121
	def_irq_handler     NVIC_Handler_122
	def_irq_handler     NVIC_Handler_123
	def_irq_handler     NVIC_Handler_124
	def_irq_handler     NVIC_Handler_125
	def_irq_handler     NVIC_Handler_126
	def_irq_handler     NVIC_Handler_127
	def_irq_handler     NVIC_Handler_128
	def_irq_handler     NVIC_Handler_129
	def_irq_handler     NVIC_Handler_130

	.end
