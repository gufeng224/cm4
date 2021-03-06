/*
    FreeRTOS V9.0.0 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/******************************************************************************
 * See http://www.freertos.org/Microchip_CEC1302_ARM_Cortex-M4F_Low_Power_Demo.html
 *
 * This project provides two demo applications.  A simple blinky style project
 * that demonstrates low power tickless functionality, and a more comprehensive
 * test and demo application.  The configCREATE_LOW_POWER_DEMO setting, which is
 * defined in FreeRTOSConfig.h, is used to select between the two.  The simply
 * blinky low power demo is implemented and described in main_low_power.c.  The
 * more comprehensive test and demo application is implemented and described in
 * main_full.c.
 *
 * The simple blinky demo uses aggregated interrupts.  The full demo uses
 * disaggregated interrupts.
 *
 * This file implements the code that is not demo specific, including the
 * hardware setup and standard FreeRTOS hook functions.
 *
 * ENSURE TO READ THE DOCUMENTATION PAGE FOR THIS PORT AND DEMO APPLICATION ON
 * THE http://www.FreeRTOS.org WEB SITE FOR FULL INFORMATION ON USING THIS DEMO
 * APPLICATION, AND ITS ASSOCIATE FreeRTOS ARCHITECTURE PORT!
 *
 */

/* Scheduler include files. */
//#include "FreeRTOS.h"
//#include "task.h"
#include <math.h>
#include "mosWrapper.h"

#include "drvINTC.h"
#include "halUART.h"
#include "halCPUINT.h"
#include "halCPU.h"
#include "halBDMA.h"
#include "halTimer.h"

#include "voc_task.h"
#include "mbx_task.h"

/* Hardware register addresses. */
#define mainVTOR 					( * ( uint32_t * ) 0xE000ED08 )
#define mainNVIC_AUX_ACTLR			( * ( uint32_t * ) 0xE000E008 )
#define mainEC_INTERRUPT_CONTROL	( * ( volatile uint32_t * ) 0x4000FC18 )

/*-----------------------------------------------------------*/

/*
 * Configure the hardware as necessary to run this demo.
 */
static void prvSetupHardware( void );

/*
 * main_low_power() is used when configCREATE_LOW_POWER_DEMO is set to 1.
 * main_full() is used when configCREATE_LOW_POWER_DEMO is set to 0.
 */
#if 0
#if( configCREATE_LOW_POWER_DEMO == 1 )

	extern void main_low_power( void );

#else

	extern void main_full( void );

	/* Some of the tests and examples executed as part of the full demo make use
	of the tick hook to call API functions from an interrupt context. */
	extern void vFullDemoTickHook( void );

#endif /* #if configCREATE_LOW_POWER_DEMO == 1 */
#endif


/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file. */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );

/*-----------------------------------------------------------*/

double squareroot(double x)
{
    double it = x;
    while (fabs(it*it - x) > 1e-13) {
        it = it - (it*it-x)/(2*it);
    }
    return it;
}

void benchmark(void)
{
    const int num_iter = 1000;
    int i;
    TickType_t t = xTaskGetTickCount();
    volatile double sum_real = 0;
    for (i = 0; i < num_iter; i++) {
        sum_real += squareroot(10000.0);
    }
    TickType_t ttot = xTaskGetTickCount() - t;
    //UartSendTrace("%d milliseconds\n\r", ttot);
}

void BenchmarkTask(void *pvParameters)
{
    UartSendTrace("\r\nBenchmark!\n\n\r");

    while(1)
    {
	    benchmark();
    }
}

/*-----------------------------------------------------------*/

int main( void )
{
	/* See http://www.freertos.org/Microchip_CEC1302_ARM_Cortex-M4F_Low_Power_Demo.html */

	/* Configure the hardware ready to run the demo. */
	prvSetupHardware();

  UartSendTrace("main....\n");

#if 0
	/* The configCREATE_LOW_POWER_DEMO setting is described at the top
	of this file. */
	#if( configCREATE_LOW_POWER_DEMO == 1 )
	{
		/* The low power demo also demonstrated aggregated interrupts, so clear
		the interrupt control register to disable the alternative NVIC vectors. */
		mainEC_INTERRUPT_CONTROL = pdFALSE;

		main_low_power();
	}
	#else
	{
		/* The full demo also demonstrated disaggregated interrupts, so set the
		interrupt control register to enable the alternative NVIC vectors. */
		mainEC_INTERRUPT_CONTROL = pdTRUE;

		main_full();
	}
	#endif
#endif

  //OUTREG8(REG_ADDR_BASE_MAILBOX, 0x44);

  vRegisterSampleCLICommands();
  vUARTCommandConsoleStart(512, ( tskIDLE_PRIORITY + 1 ));

  xTaskCreate(BenchmarkTask, "Benchmark Task", configMINIMAL_STACK_SIZE,
                  NULL, tskIDLE_PRIORITY+1, NULL);

  vTaskStartScheduler();
  for(;;);

	//return 0;
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
//extern void system_set_ec_clock( void );
extern unsigned long __Vectors[];

	/* Disable M4 write buffer: fix CEC1302 hardware bug. */
	//mainNVIC_AUX_ACTLR |= 0x07;

	//system_set_ec_clock();

	/* Assuming downloading code via the debugger - so ensure the hardware
	is using the vector table downloaded with the application. */
	mainVTOR = ( uint32_t ) __Vectors;

  MHal_UART_Init();

  MHal_Timer_Reset();

  MDrv_INTC_Init();

  MHal_BDMA_Init();

  MDrv_CPU_SetFreq(E_CPU_FREQ_HIGH);
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

	/* Force an assert. */
	configASSERT( ( volatile void * ) NULL );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	//( void ) pcTaskName;
	//( void ) pxTask;

  UartSendTrace("[vApplicationStackOverflowHook] pxTask = 0x%x, pcTaskName = %s\n", pxTask, pcTaskName);

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */

	/* Force an assert. */
	configASSERT( ( volatile void * ) NULL );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeHeapSpace;

	/* This is just a trivial example of an idle hook.  It is called on each
	cycle of the idle task.  It must *NOT* attempt to block.  In this case the
	idle task just queries the amount of FreeRTOS heap that remains.  See the
	memory management section on the http://www.FreeRTOS.org web site for memory
	management options.  If there is a lot of heap memory free then the
	configTOTAL_HEAP_SIZE value in FreeRTOSConfig.h can be reduced to free up
	RAM. */
	xFreeHeapSpace = xPortGetFreeHeapSize();

	/* Remove compiler warning about xFreeHeapSpace being set but never used. */
	//( void ) xFreeHeapSpace;
  UartSendTrace("[vApplicationIdleHook] xFreeHeapSpace = 0x%x\n", xFreeHeapSpace);
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* The full demo includes tests that run from the tick hook. */
	#if( configCREATE_LOW_POWER_DEMO == 0 )
	{
		/* Some of the tests and demo tasks executed by the full demo include
		interaction from an interrupt - for which the tick interrupt is used
		via the tick hook function. */
		vFullDemoTickHook();
	}
	#endif
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

	/* Pass out a pointer to the StaticTask_t structure in which the Idle task's
	state will be stored. */
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	/* Pass out the array that will be used as the Idle task's stack. */
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	/* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;

  UartSendTrace("[vApplicationGetIdleTaskMemory] size = %d\n", *pulIdleTaskStackSize);
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

	/* Pass out a pointer to the StaticTask_t structure in which the Timer
	task's state will be stored. */
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

	/* Pass out the array that will be used as the Timer task's stack. */
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;

	/* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;

  UartSendTrace("[vApplicationGetTimerTaskMemory] size = %d\n", *pulTimerTaskStackSize);
}


